var SysTrayX = {
  startupState: undefined,

  restorePositions: false,
  startupWindowPositions: [],

  hideDefaultIcon: false,

  platformInfo: undefined,

  browserInfo: undefined,

  version: "0",
};

SysTrayX.Messaging = {
  accounts: [],
  folderTree: {},
  countType: 0,
  closeType: 1,
  filters: undefined,
  unread: {},

  init: function () {
    // Send the startup positions?
    if (SysTrayX.restorePositions) {
      SysTrayX.Link.postSysTrayXMessage({
        positions: SysTrayX.startupWindowPositions,
      });
    }

    // Minimize on startup handled by Companion app as backup
    if (SysTrayX.startupState == "minimized") {
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

    //  Set TB versionn
    const version = SysTrayX.browserInfo.version.split(".")[0];
    browser.folderChange.setVersion(Number(version));

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
      SysTrayX.Messaging.updateFilters(rootFolder, parentFolder, folder, added);
    });

    //  Set the count type in the folderChange listener
    browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));

    //  Set the filters in the folderChange listener
    browser.folderChange.setFilters(SysTrayX.Messaging.filters);

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);

    //  Test new API from TB91
    browser.folders.onFolderInfoChanged.addListener(
      SysTrayX.Messaging.listenerTest
    );
  },

  listenerTest: function (folder, folderInfo) {
    console.debug("Folder: " + JSON.stringify(folder));
    console.debug("FolderInfo: " + JSON.stringify(folderInfo));

    if (folderInfo.unreadMessageCount !== undefined) {
      if (SysTrayX.Messaging.unread[folder.accountId] === undefined) {
        SysTrayX.Messaging.unread[folder.accountId] = {};
      }

      SysTrayX.Messaging.unread[folder.accountId][folder.path] =
        folderInfo.unreadMessageCount;
    }

    console.debug("Unread: " + JSON.stringify(SysTrayX.Messaging.unread));

    let count = 0;
    SysTrayX.Messaging.filters.forEach((filter) => {
      const accountId = filter.folder.accountId;
      const path = filter.folder.path;

      //      console.debug("FilterId: " + accountId);
      //      console.debug("FilterPath: " + path);

      if (SysTrayX.Messaging.unread[accountId] !== undefined) {
        if (SysTrayX.Messaging.unread[accountId][path] !== undefined) {
          console.debug(
            "Match found, count: " + SysTrayX.Messaging.unread[accountId][path]
          );

          count = count + SysTrayX.Messaging.unread[accountId][path];
        } else if (
          SysTrayX.Messaging.unread[accountId][path.toUpperCase()] !== undefined
        ) {
          console.debug(
            "Match found, count: " +
              SysTrayX.Messaging.unread[accountId][path.toUpperCase()]
          );

          count =
            count + SysTrayX.Messaging.unread[accountId][path.toUpperCase()];
        }
      }
    });

    console.debug("Total count: " + count);

    SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
  },

  onCloseButton: function () {
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
  storageChanged: function (changes, area) {
    //  Get the new preferences

    if ("filters" in changes && changes["filters"].newValue) {
      SysTrayX.Messaging.filters = changes["filters"].newValue;

      browser.folderChange.setFilters(SysTrayX.Messaging.filters);
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

      browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));
    }

    if ("addonprefchanged" in changes && changes["addonprefchanged"].newValue) {
      //
      //  Send new preferences to the app
      //
      SysTrayX.Messaging.sendPreferences();

      //  Reset flag
      browser.storage.sync.set({
        addonprefchanged: false,
      });
    }
  },

  //
  //  Callback for unReadMessages
  //
  unreadCb: function (count) {
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

  updateFiltersCallback: function (
    rootFolder,
    parentFolder,
    folder,
    added,
    oldFolders,
    oldPaths,
    accounts
  ) {
    //  Get new folder tree
    const folderTree = getFolderTree(accounts, SysTrayX.browserInfo);
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
          browser.storage.sync.set({
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
        browser.storage.sync.set({
          filters: SysTrayX.Messaging.filters,
        });
      }
    }

    //  Store the new accounts and folder tree
    SysTrayX.Messaging.accounts = accounts;
    SysTrayX.Messaging.folderTree = folderTree;
  },

  sendBrowserInfo: function () {
    const info = SysTrayX.browserInfo;
    SysTrayX.Link.postSysTrayXMessage({ browserInfo: info });
  },

  sendPlatformInfo: function () {
    const info = SysTrayX.platformInfo;
    SysTrayX.Link.postSysTrayXMessage({ platformInfo: info });
  },

  sendVersion: function () {
    SysTrayX.Link.postSysTrayXMessage({ version: SysTrayX.version });
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

  sendPreferences: function () {
    const getter = browser.storage.sync.get([
      "debug",
      "minimizeType",
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
      "theme",
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function (result) {
    const debug = result.debug || "false";
    const minimizeType = result.minimizeType || "1";
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
    const theme = result.theme || "0";

    if (theme == "0" && numberColor == "#ffffff") {
      numberColor = "#000000";
    } else if (theme == "1" && numberColor == "#000000") {
      numberColor = "#ffffff";
    }

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
        minimizeType: minimizeType,
        closeType: closeType,
        startMinimized: startMinimized,
        restorePositions: restorePositions,
        defaultIconType: defaultIconType,
        defaultIconMime: defaultIconMime,
        defaultIcon: defaultIcon,
        hideDefaultIcon: hideDefaultIcon,
        iconType: iconType,
        iconMime: iconMime,
        icon: icon,
        showNumber: showNumber,
        numberColor: numberColor,
        numberSize: numberSize,
        numberAlignment: numberAlignment,
        numberMargins: numberMargins,
        countType: countType,
        theme: theme,
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
    SysTrayX.Link.portSysTrayX.postMessage(object);
  },

  receiveSysTrayXMessage: function (response) {
    if (response["shutdown"]) {
      browser.windowEvent.onCloseButtonClick.removeListener(
        SysTrayX.Messaging.onCloseButton
      );

      SysTrayX.Link.postSysTrayXMessage({ shutdown: "true" });
    }

    const kdeIntegration = response["kdeIntegration"];
    if (kdeIntegration) {
      browser.storage.sync.set({
        kdeIntegration: kdeIntegration,
      });
    }

    const positions = response["positions"];
    if (positions) {
      browser.storage.sync.set({
        windowPositions: positions,
      });
    }

    if (response["preferences"]) {
      //  Store the preferences from the app
      const defaultIconMime = response["preferences"].defaultIconMime;
      if (defaultIconMime) {
        browser.storage.sync.set({
          defaultIconMime: defaultIconMime,
        });
      }

      const defaultIcon = response["preferences"].defaultIcon;
      if (defaultIcon) {
        browser.storage.sync.set({
          defaultIcon: defaultIcon,
        });
      }

      const defaultIconType = response["preferences"].defaultIconType;
      if (defaultIconType) {
        browser.storage.sync.set({
          defaultIconType: defaultIconType,
        });
      }

      const hideDefaultIcon = response["preferences"].hideDefaultIcon;
      if (hideDefaultIcon) {
        browser.storage.sync.set({
          hideDefaultIcon: hideDefaultIcon,
        });
      }

      const iconMime = response["preferences"].iconMime;
      if (iconMime) {
        browser.storage.sync.set({
          iconMime: iconMime,
        });
      }

      const icon = response["preferences"].icon;
      if (icon) {
        browser.storage.sync.set({
          icon: icon,
        });
      }

      const iconType = response["preferences"].iconType;
      if (iconType) {
        browser.storage.sync.set({
          iconType: iconType,
        });
      }

      const showNumber = response["preferences"].showNumber;
      if (showNumber) {
        browser.storage.sync.set({
          showNumber: showNumber,
        });
      }

      const numberColor = response["preferences"].numberColor;
      if (numberColor) {
        browser.storage.sync.set({
          numberColor: numberColor,
        });
      }

      const numberSize = response["preferences"].numberSize;
      if (numberSize) {
        browser.storage.sync.set({
          numberSize: numberSize,
        });
      }

      const numberAlignment = response["preferences"].numberAlignment;
      if (numberAlignment) {
        browser.storage.sync.set({
          numberAlignment: numberAlignment,
        });
      }

      const numberMargins = response["preferences"].numberMargins;
      if (numberMargins) {
        browser.storage.sync.set({
          numberMargins: numberMargins,
        });
      }

      const countType = response["preferences"].countType;
      if (countType) {
        browser.storage.sync.set({
          countType: countType,
        });
      }

      const minimizeType = response["preferences"].minimizeType;
      if (minimizeType) {
        browser.storage.sync.set({
          minimizeType: minimizeType,
        });
      }

      const closeType = response["preferences"].closeType;
      if (closeType) {
        browser.storage.sync.set({
          closeType: closeType,
        });
      }

      const startMinimized = response["preferences"].startMinimized;
      if (startMinimized) {
        browser.storage.sync.set({
          startMinimized: startMinimized,
        });
      }

      const restorePositions = response["preferences"].restorePositions;
      if (restorePositions) {
        browser.storage.sync.set({
          restorePositions: restorePositions,
        });
      }

      const theme = response["preferences"].theme;
      if (theme) {
        browser.storage.sync.set({
          theme: theme,
        });
      }

      const debug = response["preferences"].debug;
      if (debug) {
        browser.storage.sync.set({
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
};

async function start() {
  //  Get the prefered start state
  const state = await getStartupState();

  //  if (state == "minimized") {
  //    browser.windows.update(browser.windows.WINDOW_ID_CURRENT, {
  //      state: "minimized",
  //    });
  //  }

  SysTrayX.startupState = state;

  //  Restore window positions
  const restorePositions = await getRestorePositionsState();

  if (restorePositions == "true") {
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

  //  Set platform
  SysTrayX.platformInfo = await browser.runtime
    .getPlatformInfo()
    .then((info) => info);

  console.log("OS: " + SysTrayX.platformInfo.os);
  console.log("Arch: " + SysTrayX.platformInfo.arch);
  console.log("Nack-Arch: " + SysTrayX.platformInfo.nacl_arch);

  //  Store platform info
  browser.storage.sync.set({
    platformInfo: SysTrayX.platformInfo,
  });

  //  Set browser
  SysTrayX.browserInfo = await browser.runtime
    .getBrowserInfo()
    .then((info) => info);

  const version = SysTrayX.browserInfo.version.split(".");
  SysTrayX.browserInfo.majorVersion = version[0];
  SysTrayX.browserInfo.minorVersion = version[1];

  console.log("Browser: " + SysTrayX.browserInfo.name);
  console.log("Vendor: " + SysTrayX.browserInfo.vendor);
  console.log("Version: " + SysTrayX.browserInfo.version);
  console.log("Build: " + SysTrayX.browserInfo.buildID);

  //  Store browser info
  browser.storage.sync.set({
    browserInfo: SysTrayX.browserInfo,
  });

  //  Reset KDE integration
  browser.storage.sync.set({
    kdeIntegration: true,
  });

  //  Get addon version
  SysTrayX.version = browser.runtime.getManifest().version;
  console.log("Addon version: " + SysTrayX.version);

  //  Init defaults before everything
  await getDefaultIcon();
  await getIcon();

  //  Get all accounts
  if (SysTrayX.browserInfo.majorVersion < 91) {
    SysTrayX.Messaging.accounts = await browser.accounts.list();
  } else {
    console.debug("Getting accounts with folders");

    const includeFolders = true;
    SysTrayX.Messaging.accounts = await browser.accounts.list(includeFolders);
  }

  //  Get folder tree
  SysTrayX.Messaging.folderTree = getFolderTree(
    SysTrayX.Messaging.accounts,
    SysTrayX.browserInfo
  );

  // Get the filters (needs the accounts)
  SysTrayX.Messaging.filters = await getFilters();

  // Get the count type
  SysTrayX.Messaging.countType = await getCountType();

  //  Test new API calls TB91

  /*
  const folder = {
    accountId: "account1",
    path: "/Inbox",
  };
*/

  /*
  const folder = {
    accountId: "account1",
    name: "Inbox",
    path: "/Inbox",
    type: "inbox",
    subFolders: [],
  };
*/

  /*
  const mailFolderInfo = await browser.folders.getFolderInfo(folder);
  console.debug("MailFolderInfo: " + JSON.stringify(mailFolderInfo));
*/

  //  Setup the link first
  SysTrayX.Link.init();

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
