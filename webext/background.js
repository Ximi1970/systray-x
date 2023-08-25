var SysTrayX = {
  startupState: undefined,

  restorePositions: false,
  startupWindowPositions: [],

  hideDefaultIcon: false,

  startApp: "",
  startAppArgs: "",
};

SysTrayX.Info = {
  version: "0",
  platformInfo: {},
  browserInfo: {},

  displayInfo: function () {
    console.debug("Info Addon version: " + this.version);
    console.debug("Info Platform: " + JSON.stringify(this.platformInfo));
    console.debug("Info Browser: " + JSON.stringify(this.browserInfo));
  },
};

SysTrayX.Messaging = {
  accounts: [],
  folderTree: {},
  countType: "0",
  closeType: "1",
  filters: undefined,
  unread: {},
  new: {},
  displayedFolder: undefined,

  init: async function () {
    // Send the startup positions?
    if (SysTrayX.restorePositions) {
      SysTrayX.Link.postSysTrayXMessage({
        positions: SysTrayX.startupWindowPositions,
      });
    }

    // Minimize on startup handled by Companion app as backup
    if (SysTrayX.startupState === "minimized") {
      SysTrayX.Link.postSysTrayXMessage({ window: "minimized_all_startup" });
    }

    // Lookout for storage changes
    browser.storage.onChanged.addListener(SysTrayX.Messaging.storageChanged);

    //  Send the platform info to app
    SysTrayX.Messaging.sendLocale();

    //  Send the platform info to app
    SysTrayX.Messaging.sendPlatformInfo();

    //  Send the browser info to app
    SysTrayX.Messaging.sendBrowserInfo();

    //  Send version to app
    SysTrayX.Messaging.sendVersion();

    //  Send hide default icon preference
    SysTrayX.Messaging.sendHideDefaultIcon();

    //  Send preferences to app
    SysTrayX.Messaging.sendPreferences();

    //  Send start app trigger
    SysTrayX.Messaging.sendStartApp();

    //  Let us wait until TB is ready, needed for TB91 and higher?
    const startupDelay = await storage()
      .get("startupDelay")
      .then((result) => result.startupDelay || "5");
    const delay = (ms) => new Promise((res) => setTimeout(res, ms));
    await delay(startupDelay * 1000);

    //  Get all accounts
    if (SysTrayX.Info.browserInfo.majorVersion < 91) {
      SysTrayX.Messaging.accounts = await browser.accounts.list();

      //  Get folder tree
      SysTrayX.Messaging.folderTree = getFolderTree(
        SysTrayX.Messaging.accounts,
        SysTrayX.Info.browserInfo
      );
    } else {
      SysTrayX.Messaging.accounts = await browser.accounts.list(false);

      // Fill the sub folders using the folders API, they are not same...
      for (let i = 0; i < SysTrayX.Messaging.accounts.length; ++i) {
        const subFolders = await browser.folders.getSubFolders(
          SysTrayX.Messaging.accounts[i],
          true
        );
        SysTrayX.Messaging.accounts[i].folders = subFolders;
      }

      console.debug("Accounts: " + JSON.stringify(SysTrayX.Messaging.accounts));
    }

    // Get the filters (needs the accounts)
    const getFiltersPromise = () => new Promise((res) => res(getFilters()));
    SysTrayX.Messaging.filters = await getFiltersPromise();

    // Get the count type
    const getCountTypePromise = () => new Promise((res) => res(getCountType()));
    SysTrayX.Messaging.countType = await getCountTypePromise();

    if (SysTrayX.Info.browserInfo.majorVersion < 91) {
      //  Set TB versionn
      browser.folderChange.setVersion(
        Number(SysTrayX.Info.browserInfo.majorVersion)
      );

      //  Catch the unread / new mails
      browser.folderChange.onUnreadMailChange.addListener(function (unread) {
        SysTrayX.Messaging.unreadCb(unread);
      });

      browser.folderChange.onFolderChange.addListener(function (
        rootFolder,
        parentFolder,
        folder,
        added
      ) {
        SysTrayX.Messaging.updateFilters(
          rootFolder,
          parentFolder,
          folder,
          added
        );
      });

      //  Set the count type in the folderChange listener
      browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));

      //  Set the filters in the folderChange listener
      browser.folderChange.setFilters(SysTrayX.Messaging.filters);
    } else {
      browser.messages.onNewMailReceived.addListener(
        SysTrayX.Messaging.listenerNewMail
      );

      browser.folders.onCreated.addListener(
        SysTrayX.Messaging.listenerFolderCreated
      );
      browser.folders.onRenamed.addListener(
        SysTrayX.Messaging.listenerFolderRenamed
      );
      browser.folders.onDeleted.addListener(
        SysTrayX.Messaging.listenerFolderDeleted
      );

      if (SysTrayX.Messaging.countType === "0") {
        // Get the unread count
        const getCountUnreadPromise = () =>
          new Promise((res) => res(SysTrayX.Messaging.countUnread()));
        await getCountUnreadPromise();
      } else {
        // Update new count
        const getCountNewPromise = () =>
          new Promise((res) => res(SysTrayX.Messaging.countNew()));
        await getCountNewPromise();
      }

      //  Set count listener
      browser.folders.onFolderInfoChanged.addListener(
        SysTrayX.Messaging.listenerFolderInfoChanged
      );
    }

    //  Catch a folder change to reset the new counter
    browser.mailTabs.onDisplayedFolderChanged.addListener(
      SysTrayX.Window.folderChanged
    );

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);
  },

  listenerNewMail: function (folder, messages) {
    /*
    console.debug("New mail in: " + folder.accountId + ", " + folder.path);
    console.debug("New messages: " + JSON.stringify(messages));
    */

    if (messages.messages.length > 0) {
      if (SysTrayX.Messaging.new[folder.accountId] === undefined) {
        SysTrayX.Messaging.new[folder.accountId] = {};
      }

      if (SysTrayX.Messaging.new[folder.accountId][folder.path] === undefined) {
        SysTrayX.Messaging.new[folder.accountId][folder.path] = [];
      }

      SysTrayX.Messaging.new[folder.accountId][folder.path] =
        SysTrayX.Messaging.new[folder.accountId][folder.path].concat(
          messages.messages
        );

      //      console.debug(
      //        "Messages: " +
      //          JSON.stringify(SysTrayX.Messaging.new[folder.accountId][folder.path])
      //      );
    }

    if (SysTrayX.Messaging.countType === "1") {
      getNewMailCount();
      //      console.debug("listenerNewMail: New count");
    }
  },

  listenerFolderCreated: function (createdFolder) {
    const found = isParentFolderInFilters(createdFolder);
    if (found) {
      addFolderToFilters(createdFolder);
    }
  },

  listenerFolderRenamed: function (originalFolder, renameFolder) {
    deleteFolderFromFilters(originalFolder);

    const found = isParentFolderInFilters(renameFolder);
    if (found) {
      addFolderToFilters(renameFolder);
    }
  },

  listenerFolderDeleted: function (deletedFolder) {
    deleteFolderFromFilters(deletedFolder);
  },

  listenerFolderInfoChanged: async function (folder, folderInfo) {
    /*
    console.debug("FolderInfoChanged: " + JSON.stringify(folder));
    console.debug("FolderInfoChanged: " + JSON.stringify(folderInfo));
    */

    if (folderInfo.unreadMessageCount !== undefined) {
      if (SysTrayX.Messaging.unread[folder.accountId] === undefined) {
        SysTrayX.Messaging.unread[folder.accountId] = {};
      }

      if (SysTrayX.Messaging.new[folder.accountId] === undefined) {
        SysTrayX.Messaging.new[folder.accountId] = {};
      }

      if (SysTrayX.Messaging.new[folder.accountId][folder.path] === undefined) {
        SysTrayX.Messaging.new[folder.accountId][folder.path] = [];
      }

      SysTrayX.Messaging.unread[folder.accountId][folder.path] =
        folderInfo.unreadMessageCount;

      if (SysTrayX.Messaging.countType === "0") {
        getUnreadMailCount();
        //        console.debug("listenerFolderInfoChanged: Unread count");
      } else {
        // Check if the new mails have been read, remove from new storage
        const messages = SysTrayX.Messaging.new[folder.accountId][folder.path];

        if (messages.length > 0) {
          const newMessages = [];
          for (let i = 0; i < messages.length; ++i) {
            const message = messages[i];

            const getHeaderPromise = (messageId) =>
              new Promise((res) => res(messenger.messages.get(messageId)));
            const header = await getHeaderPromise(message.id);

            if (!header.read) {
              newMessages.push(message);
            }
          }
          SysTrayX.Messaging.new[folder.accountId][folder.path] = [
            ...newMessages,
          ];

          getNewMailCount();
          //          console.debug("listenerFolderInfoChanged: New count");
        }
      }
    }
  },

  countUnread: async function () {
    for (const filter of SysTrayX.Messaging.filters) {
      for (const path of filter.folders) {
        const folder = {
          accountId: filter.accountId,
          path: path,
        };

        let mailFolderInfo = {};
        try {
          mailFolderInfo = await browser.folders.getFolderInfo(folder);
        } catch (err) {
          console.debug("Filter error: " + err);
          console.debug("Filter error: " + JSON.stringify(folder));
        }

        if (mailFolderInfo.unreadMessageCount !== undefined) {
          if (SysTrayX.Messaging.unread[folder.accountId] === undefined) {
            SysTrayX.Messaging.unread[folder.accountId] = {};
          }

          SysTrayX.Messaging.unread[folder.accountId][folder.path] =
            mailFolderInfo.unreadMessageCount;
        }
      }
    }

    let count = 0;
    Object.keys(SysTrayX.Messaging.unread).forEach((key) =>
      Object.keys(SysTrayX.Messaging.unread[key]).forEach(
        (path) => (count = count + SysTrayX.Messaging.unread[key][path])
      )
    );

    //console.debug("countUnread: " + count);
    SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
  },

  countNew: async function () {
    for (const filter of SysTrayX.Messaging.filters) {
      for (const path of filter.folders) {
        const folder = {
          accountId: filter.accountId,
          path: path,
        };

        let mailFolderInfo = {};
        try {
          mailFolderInfo = await browser.folders.getFolderInfo(folder);
        } catch (err) {
          console.debug("Filter error: " + err);
          console.debug("Filter error: " + JSON.stringify(folder));
        }

        if (mailFolderInfo.unreadMessageCount !== undefined) {
          if (SysTrayX.Messaging.new[folder.accountId] === undefined) {
            SysTrayX.Messaging.new[folder.accountId] = {};
          }

          if (
            SysTrayX.Messaging.new[folder.accountId][folder.path] === undefined
          ) {
            SysTrayX.Messaging.new[folder.accountId][folder.path] = [];
          }

          //          SysTrayX.Messaging.unread[folder.accountId][folder.path] =
          //            mailFolderInfo.unreadMessageCount;
        }
      }
    }

    let count = 0;
    Object.keys(SysTrayX.Messaging.new).forEach((key) =>
      Object.keys(SysTrayX.Messaging.new[key]).forEach(
        (path) => (count = count + SysTrayX.Messaging.new[key][path].length)
      )
    );

    //console.debug("countNew: " + count);
    SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
  },

  onCloseButton: function () {
    //console.debug("Minimize all")

    SysTrayX.Link.postSysTrayXMessage({ window: "minimized_all" });
    /*
    browser.windows.update(browser.windows.WINDOW_ID_CURRENT, {
      state: "minimized",
    });
    */
  },

  //
  //  Handle a storage change
  //
  storageChanged: async function (changes, area) {
    //  Get the new preferences

    if ("filters" in changes && changes["filters"].newValue) {
      SysTrayX.Messaging.filters = changes["filters"].newValue;

      if (SysTrayX.Info.browserInfo.majorVersion < 91) {
        browser.folderChange.setFilters(SysTrayX.Messaging.filters);
      } else {
        if (SysTrayX.Messaging.countType === "0") {
          getUnreadMailCount();
        } else {
          getNewMailCount();
        }
      }
    }

    if ("closeType" in changes && changes["closeType"].newValue) {
      SysTrayX.Messaging.closeType = changes["closeType"].newValue;

      browser.windowEvent.setCloseType(Number(SysTrayX.Messaging.closeType));

      if (SysTrayX.Messaging.closeType !== "0") {
        browser.windowEvent.onCloseButtonClick.addListener(
          SysTrayX.Messaging.onCloseButton
        );
      } else {
        browser.windowEvent.onCloseButtonClick.removeListener(
          SysTrayX.Messaging.onCloseButton
        );
      }
    }

    if ("countType" in changes && changes["countType"].newValue) {
      SysTrayX.Messaging.countType = changes["countType"].newValue;

      if (SysTrayX.Info.browserInfo.majorVersion < 91) {
        browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));
      } else {
        if (SysTrayX.Messaging.countType === "0") {
          getUnreadMailCount();
        } else {
          getNewMailCount();
        }
      }
    }

    if ("addonprefchanged" in changes && changes["addonprefchanged"].newValue) {
      //
      //  Send new preferences to the app
      //
      SysTrayX.Messaging.sendPreferences();

      //  Reset flag
      await storage().set({
        addonprefchanged: false,
      });
    }
  },

  //
  //  Callback for unReadMessages
  //
  unreadCb: function (count) {
    //console.debug("unreadCb: " + count);
    SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
  },

  //
  //  Callback for folder changes
  //
  updateFilters: function (rootFolder, parentFolder, folder, added) {
    const oldFolders = SysTrayX.Messaging.folderTree[rootFolder];
    const oldPaths = getFolderPaths(oldFolders);

    browser.accounts.list().then(
      (accounts) => {
        this.updateFiltersCallback(
          rootFolder,
          parentFolder,
          folder,
          added,
          oldFolders,
          oldPaths,
          accounts
        );
      },
      () => console.log("Failed to get the accounts")
    );
  },

  updateFiltersCallback: async function (
    rootFolder,
    parentFolder,
    folder,
    added,
    oldFolders,
    oldPaths,
    accounts
  ) {
    //  Get new folder tree
    const folderTree = getFolderTree(accounts, SysTrayX.Info.browserInfo);
    const newFolders = folderTree[rootFolder];
    const newPaths = getFolderPaths(newFolders);
    const changes = findFolderPathsDiff(oldPaths, newPaths).filter((change) =>
      change.endsWith(parentFolder + "/" + folder)
    );

    if (changes.length === 1 && added) {
      //  Folder has beeen added

      const addedFolder = changes[0];

      //  Is parent selected?
      const parentAddedFolder = addedFolder.substring(
        0,
        addedFolder.lastIndexOf("/")
      );

      const parentSelected = findFolderPath(
        SysTrayX.Messaging.filters,
        parentAddedFolder
      );

      if (parentSelected) {
        //  Add the new folder to the filters

        const newFilter = {
          ...parentSelected,
          folder: {
            ...parentSelected.folder,
            path: changes[0],
            name: changes[0].substring(changes[0].lastIndexOf("/") + 1),
          },
        };

        if (
          SysTrayX.Messaging.filters.filter(
            (filter) => filter.folder.path === newFilter.folder.path
          ).length === 0
        ) {
          SysTrayX.Messaging.filters.push(newFilter);

          //  Store the new filters
          await storage().set({
            filters: SysTrayX.Messaging.filters,
          });
        }
      }
    } else if (changes.length === 1 && !added) {
      //  Folder has been removed, remove also all children

      const filters = SysTrayX.Messaging.filters.filter(
        (filter) => !filter.folder.path.startsWith(changes[0])
      );

      if (filters.length !== SysTrayX.Messaging.filters.length) {
        //  Remove found filters (and children) from the filters
        SysTrayX.Messaging.filters = filters;

        //  Store the new filters
        await storage().set({
          filters: SysTrayX.Messaging.filters,
        });
      }
    }

    //  Store the new accounts and folder tree
    SysTrayX.Messaging.accounts = accounts;
    SysTrayX.Messaging.folderTree = folderTree;
  },

  sendBrowserInfo: function () {
    const info = SysTrayX.Info.browserInfo;
    SysTrayX.Link.postSysTrayXMessage({ browserInfo: info });
  },

  sendPlatformInfo: function () {
    const info = SysTrayX.Info.platformInfo;
    SysTrayX.Link.postSysTrayXMessage({ platformInfo: info });
  },

  sendVersion: function () {
    SysTrayX.Link.postSysTrayXMessage({ version: SysTrayX.Info.version });
  },

  sendHideDefaultIcon: function () {
    SysTrayX.Link.postSysTrayXMessage({
      hideDefaultIcon: SysTrayX.hideDefaultIcon,
    });
  },

  sendLocale: function () {
    const locale = browser.i18n.getUILanguage();

    SysTrayX.Link.postSysTrayXMessage({
      locale: locale,
    });
  },

  sendStartApp: function () {
    SysTrayX.Link.postSysTrayXMessage({
      startApp: SysTrayX.startApp,
      startAppArgs: SysTrayX.startAppArgs,
    });
  },

  sendCloseApp: function () {
    SysTrayX.Link.postSysTrayXMessage({
      closeApp: "closeApp",
    });
  },

  sendPreferences: async function () {
    await storage()
      .get([
        "debug",
        "minimizeType",
        "minimizeIconType",
        "closeType",
        "startMinimized",
        "restorePositions",
        "defaultIconType",
        "defaultIconMime",
        "defaultIcon",
        "hideDefaultIcon",
        "iconType",
        "iconMime",
        "icon",
        "showNumber",
        "numberColor",
        "numberSize",
        "numberAlignment",
        "numberMargins",
        "countType",
        "startupDelay",
        "theme",
        "startApp",
        "startAppArgs",
        "closeApp",
        "closeAppArgs",
      ])
      .then(
        SysTrayX.Messaging.sendPreferencesStorage,
        SysTrayX.Messaging.onSendPreferecesStorageError
      );
  },

  sendPreferencesStorage: function (result) {
    const debug = result.debug || "false";
    const minimizeType = result.minimizeType || "1";
    const minimizeIconType = result.minimizeIconType || "1";
    const closeType = result.closeType || "1";
    const startMinimized = result.startMinimized || "false";
    const restorePositions = result.restorePositions || "false";
    const defaultIconType = result.defaultIconType || "0";
    const defaultIconMime = result.defaultIconMime || "image/png";
    const defaultIcon = result.defaultIcon || [];
    const hideDefaultIcon = result.hideDefaultIcon || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];
    const showNumber = result.showNumber || "true";
    let numberColor = result.numberColor || "#000000";
    const numberSize = result.numberSize || "10";
    const numberAlignment = result.numberAlignment || "4";
    const numberMargins = result.numberMargins || {
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
    };
    const countType = result.countType || "0";
    const startupDelay = result.startupDelay || "5";
    const theme = result.theme || "0";
    const startApp = result.startApp || "";
    const startAppArgs = result.startAppArgs || "";
    const closeApp = result.closeApp || "";
    const closeAppArgs = result.closeAppArgs || "";

    if (theme == "0" && numberColor == "#ffffff") {
      numberColor = "#000000";
    } else if (theme == "1" && numberColor == "#000000") {
      numberColor = "#ffffff";
    }

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug,
        minimizeType,
        minimizeIconType,
        closeType,
        startMinimized,
        restorePositions,
        defaultIconType,
        defaultIconMime,
        defaultIcon,
        hideDefaultIcon,
        iconType,
        iconMime,
        icon,
        showNumber,
        numberColor,
        numberSize,
        numberAlignment,
        numberMargins,
        countType,
        startupDelay,
        theme,
        startApp,
        startAppArgs,
        closeApp,
        closeAppArgs,
      },
    });

    if (SysTrayX.startupState) {
      //  Send startup state after the prefs
      //  so the hide is handled conform the prefs
      if (SysTrayX.startupState === "minimized") {
        SysTrayX.Link.postSysTrayXMessage({ window: "minimized_all" });
        //SysTrayX.Link.postSysTrayXMessage({ window: SysTrayX.startupState });
      }

      SysTrayX.startupState = undefined;
    }
  },

  onSendIconStorageError: function (error) {
    console.log(`GetIcon Error: ${error}`);
  },

  onGetAccountsStorageError: function (error) {
    console.log(`GetAccounts Error: ${error}`);
  },
};

//
//  Link object. Handles the native messaging to the system tray app
//
SysTrayX.Link = {
  portSysTrayX: undefined,

  init: function () {
    //  Connect to the app
    this.portSysTrayX = browser.runtime.connectNative("SysTray_X");

    //  Listen for messages from the app.
    this.portSysTrayX.onMessage.addListener(
      SysTrayX.Link.receiveSysTrayXMessage
    );
  },

  postSysTrayXMessage: function (object) {
    //  Send object (will be stringified by postMessage)

    //    console.debug("postSysTrayXMessage: " + JSON.stringify(object));
    SysTrayX.Link.portSysTrayX.postMessage(object);
  },

  receiveSysTrayXMessage: async function (response) {
    if (response["shutdown"]) {
      browser.windowEvent.onCloseButtonClick.removeListener(
        SysTrayX.Messaging.onCloseButton
      );

      SysTrayX.Link.postSysTrayXMessage({ shutdown: "true" });
    }

    const kdeIntegration = response["kdeIntegration"];
    if (kdeIntegration !== undefined) {
      await storage().set({
        kdeIntegration: kdeIntegration,
      });
    }

    const positions = response["positions"];
    if (positions) {
      await storage().set({
        windowPositions: positions,
      });
    }

    if (response["preferences"]) {
      //  Store the preferences from the app
      const defaultIconMime = response["preferences"].defaultIconMime;
      if (defaultIconMime) {
        await storage().set({
          defaultIconMime: defaultIconMime,
        });
      }

      const defaultIcon = response["preferences"].defaultIcon;
      if (defaultIcon) {
        await storage().set({
          defaultIcon: defaultIcon,
        });
      }

      const defaultIconType = response["preferences"].defaultIconType;
      if (defaultIconType) {
        await storage().set({
          defaultIconType: defaultIconType,
        });
      }

      const hideDefaultIcon = response["preferences"].hideDefaultIcon;
      if (hideDefaultIcon) {
        await storage().set({
          hideDefaultIcon: hideDefaultIcon,
        });
      }

      const iconMime = response["preferences"].iconMime;
      if (iconMime) {
        await storage().set({
          iconMime: iconMime,
        });
      }

      const icon = response["preferences"].icon;
      if (icon) {
        await storage().set({
          icon: icon,
        });
      }

      const iconType = response["preferences"].iconType;
      if (iconType) {
        await storage().set({
          iconType: iconType,
        });
      }

      const showNumber = response["preferences"].showNumber;
      if (showNumber) {
        await storage().set({
          showNumber: showNumber,
        });
      }

      const numberColor = response["preferences"].numberColor;
      if (numberColor) {
        await storage().set({
          numberColor: numberColor,
        });
      }

      const numberSize = response["preferences"].numberSize;
      if (numberSize) {
        await storage().set({
          numberSize: numberSize,
        });
      }

      const numberAlignment = response["preferences"].numberAlignment;
      if (numberAlignment) {
        await storage().set({
          numberAlignment: numberAlignment,
        });
      }

      const numberMargins = response["preferences"].numberMargins;
      if (numberMargins) {
        await storage().set({
          numberMargins: numberMargins,
        });
      }

      const countType = response["preferences"].countType;
      if (countType) {
        await storage().set({
          countType: countType,
        });
      }

      const startupDelay = response["preferences"].startupDelay;
      if (startupDelay) {
        await storage().set({
          startupDelay: startupDelay,
        });
      }

      const minimizeType = response["preferences"].minimizeType;
      if (minimizeType) {
        await storage().set({
          minimizeType: minimizeType,
        });
      }

      const minimizeIconType = response["preferences"].minimizeIconType;
      if (minimizeIconType) {
        await storage().set({
          minimizeIconType: minimizeIconType,
        });
      }

      const closeType = response["preferences"].closeType;
      if (closeType) {
        await storage().set({
          closeType: closeType,
        });
      }

      const startMinimized = response["preferences"].startMinimized;
      if (startMinimized) {
        await storage().set({
          startMinimized: startMinimized,
        });
      }

      const restorePositions = response["preferences"].restorePositions;
      if (restorePositions) {
        await storage().set({
          restorePositions: restorePositions,
        });
      }

      const theme = response["preferences"].theme;
      if (theme) {
        await storage().set({
          theme: theme,
        });
      }

      const startApp = response["preferences"].startApp;
      if (startApp !== undefined) {
        await storage().set({
          startApp: startApp,
        });
      }

      const startAppArgs = response["preferences"].startAppArgs;
      if (startAppArgs !== undefined) {
        await storage().set({
          startAppArgs: startAppArgs,
        });
      }

      const closeApp = response["preferences"]. closeApp;
      if (closeApp !== undefined) {
        await storage().set({
          closeApp: closeApp,
        });
      }

      const closeAppArgs = response["preferences"].closeAppArgs;
      if (closeAppArgs !== undefined) {
        await storage().set({
          closeAppArgs: closeAppArgs,
        });
      }

      const debug = response["preferences"].debug;
      if (debug) {
        await storage().set({
          debug: debug,
        });
      }
    }
  },
};

SysTrayX.Window = {
  focusChanged: function (windowId) {
    browser.windows.getCurrent().then((win) => {
      SysTrayX.Link.postSysTrayXMessage({ window: win.state });
    });
  },

  folderChanged: function (tab, displayedFolder) {
    /*
    console.debug("Folder changed tab: " + JSON.stringify(tab));
    console.debug(
      "Folder changed displayedFolder: " + JSON.stringify(displayedFolder)
    );
    */

    const oldDisplayedFolder = SysTrayX.Messaging.displayedFolder;
    if (oldDisplayedFolder !== undefined) {
      if (
        SysTrayX.Messaging.new[oldDisplayedFolder.accountId] !== undefined &&
        SysTrayX.Messaging.new[oldDisplayedFolder.accountId][
          oldDisplayedFolder.path
        ] !== undefined
      ) {
        SysTrayX.Messaging.new[oldDisplayedFolder.accountId][
          oldDisplayedFolder.path
        ] = [];
      }

      if (SysTrayX.Messaging.countType === "1") {
        getNewMailCount();
      }
    }

    SysTrayX.Messaging.displayedFolder = displayedFolder;
  },
};

async function start() {
  //  Set platform
  SysTrayX.Info.platformInfo = await browser.runtime
    .getPlatformInfo()
    .then((info) => info);

  //  Set browser
  SysTrayX.Info.browserInfo = await browser.runtime
    .getBrowserInfo()
    .then((info) => info);

  const version = SysTrayX.Info.browserInfo.version.split(".");
  SysTrayX.Info.browserInfo.majorVersion = version[0];
  SysTrayX.Info.browserInfo.minorVersion = version[1];

  //  Get addon version
  SysTrayX.Info.version = browser.runtime.getManifest().version;

  SysTrayX.Info.displayInfo();

  //  Get the prefered start state
  const state = await getStartupState();
  SysTrayX.startupState = state;

  //  Restore window positions
  const restorePositions = await getRestorePositionsState();

  if (restorePositions === "true") {
    SysTrayX.restorePositions = true;

    // Get the start window positions
    SysTrayX.startupWindowPositions = await getStartupWindowPositions();
  }

  // Get the close type
  SysTrayX.Messaging.closeType = await getCloseType();
  browser.windowEvent.setCloseType(Number(SysTrayX.Messaging.closeType));

  //  Intercept close button?
  if (SysTrayX.Messaging.closeType !== "0") {
    browser.windowEvent.onCloseButtonClick.addListener(
      SysTrayX.Messaging.onCloseButton
    );
  }

  //  Hide the default icon
  const hideDefaultIcon = await getHideDefaultIcon();
  SysTrayX.hideDefaultIcon = hideDefaultIcon;

  //  Get start app launch parameters
  const {startApp, startAppArgs} = await getStartAppParam();
  SysTrayX.startApp = startApp;
  SysTrayX.startAppArgs = startAppArgs;

  //   Used sync storage
  //  const inUse = await browser.storage.sync.getBytesInUse();
  //  console.log("Storage in use: " + inUse);

  //  Init defaults before everything

  //  Reset KDE integration
  await storage().set({
    kdeIntegration: true,
  });

  // Get the default icons
  const getDefaultIconPromise = () =>
    new Promise((res) => res(getDefaultIcon()));
  await getDefaultIconPromise();

  const getIconPromise = () => new Promise((res) => res(getIcon()));
  await getIconPromise();

  //  Setup the link first
  SysTrayX.Link.init();

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
