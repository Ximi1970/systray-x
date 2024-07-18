var SysTrayX = {
  Info: {},
  Window: {},
  Messaging: {},

  mainWindowId: undefined,
  windows: undefined,

  startupType: undefined,

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

  options: {},

  displayInfo: function () {
    console.debug("Info Addon version: " + this.version);
    console.debug("Info Platform: " + JSON.stringify(this.platformInfo));
    console.debug("Info Browser: " + JSON.stringify(this.browserInfo));
  },
};

SysTrayX.Messaging = {
  startupDelayFinished: false,
  accounts: [],
  countType: "0",
  showNewIndicator: "false",
  closeType: "1",
  apiCountMethod: "false",
  filters: undefined,
  newMailCache: [],
  folderInfoChangeCache: [],
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

    // Minimize on startup handled by Companion app
    if (SysTrayX.startupType === "1") {
        SysTrayX.Link.postSysTrayXMessage({ startup: "minimized" });
    }
    if (SysTrayX.startupType === "2") {
        SysTrayX.Link.postSysTrayXMessage({ startup: "docked" });
    }

    // Lookout for storage changes
    browser.storage.onChanged.addListener(SysTrayX.Messaging.storageChanged);

    //  Request the options from app
    SysTrayX.Messaging.requestOptions();

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

    //  Let us wait until TB is ready, needed for TB91 (no accounts found, cannot create filters) and higher?
    const startupDelay = await storage()
      .get("startupDelay")
      .then((result) => result.startupDelay || "5");
    const delay = (ms) => new Promise((res) => setTimeout(res, ms));
    await delay(startupDelay * 1000);

    //  Keep trace of account changes
    if (SysTrayX.Info.browserInfo.majorVersion >= 98) {
      browser.accounts.onCreated.addListener(SysTrayX.Messaging.accountAdded);
      browser.accounts.onDeleted.addListener(SysTrayX.Messaging.accountDeleted);
    }

    //  Get the accounts and subfolders
    SysTrayX.Messaging.accounts = await browser.accounts.list();

    console.debug("Accounts: " + JSON.stringify(SysTrayX.Messaging.accounts));

    // Get the filters (needs the accounts)
    const getFiltersPromise = () => new Promise((res) => res(getFilters()));
    SysTrayX.Messaging.filters = await getFiltersPromise();

    // Get the count type
    const getCountTypePromise = () => new Promise((res) => res(getCountType()));
    SysTrayX.Messaging.countType = await getCountTypePromise();

    // Get the show new indicator
    const getShowNewIndicatorPromise = () => new Promise((res) => res(getShowNewIndicator()));
    SysTrayX.Messaging.showNewIndicator = await getShowNewIndicatorPromise();

    // Check the filters for the accounts
    SysTrayX.Messaging.accountFilterCheck();

    // Handle cached mail changes on startup
    SysTrayX.Messaging.startupDelayFinished = true;
    if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false" ) {
      SysTrayX.Messaging.listenerNewMail();
    }
    SysTrayX.Messaging.listenerFolderInfoChanged();

    // Catch folder changes
    browser.folders.onCreated.addListener(
      SysTrayX.Messaging.listenerFolderCreated
    );
    browser.folders.onRenamed.addListener(
      SysTrayX.Messaging.listenerFolderRenamed
    );
    browser.folders.onDeleted.addListener(
      SysTrayX.Messaging.listenerFolderDeleted
    );

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);
  },

  accountAdded: function (id, account) {
    //console.debug("accountAdded: Id: " + id);
    //console.debug("accountAdded: Account: " + JSON.stringify(account));
  },

  accountDeleted: function (id) {
    for (let i = 0; i < SysTrayX.Messaging.accounts.length; ++i) {
      if (SysTrayX.Messaging.accounts[i].id === id) {
        SysTrayX.Messaging.accounts.splice(i,1);
        break;
      }
    }

    SysTrayX.Messaging.accountFilterCheck();
  },

  accountFilterCheck: async function () {
    let ids = [];
    for (let i = 0; i < SysTrayX.Messaging.accounts.length; ++i) {
      ids.push(SysTrayX.Messaging.accounts[i].id);
    }

    let newFilters = [];
    let filtersChanged = false;
    for (let i = 0; i < SysTrayX.Messaging.filters.length; ++i) {
      const id = SysTrayX.Messaging.filters[i].accountId;

      if (ids.includes(id)) {
        newFilters.push(SysTrayX.Messaging.filters[i]);
      } else {
        if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false") {
          if (SysTrayX.Messaging.new[id] != undefined) {
            delete SysTrayX.Messaging.new[id];
          }
          if (SysTrayX.Messaging.unread[id] !== undefined) {
            delete SysTrayX.Messaging.unread[id];
          }
        }

        filtersChanged = true;
      }
    }

    //console.debug("checkFilters: Old: " + JSON.stringify(SysTrayX.Messaging.filters));
    //console.debug("checkFilters: New: " + JSON.stringify(newFilters));

    //console.debug("checkFilters: MesNew: " + JSON.stringify(SysTrayX.Messaging.new));
    //console.debug("checkFilters: MesUnread: " + JSON.stringify(SysTrayX.Messaging.unread));

    if (filtersChanged) {
      SysTrayX.Messaging.filters = [...newFilters];

      await storage().set({
        filters: SysTrayX.Messaging.filters,
      });
    }
  },

  listenerNewMail: function (folder, messages) {

    // Cache the new mail
    if (folder)
    {
      SysTrayX.Messaging.newMailCache.push({ folder, messages });
    }

    //console.debug("listenerNewMailCache: New mail in: " + JSON.stringify(folder));
    //console.debug("listenerNewMailCache: New messages: " + JSON.stringify(messages));
    //console.debug("listenerNewMailCache: Cache: " + SysTrayX.Messaging.newMailCache.length );

    if (SysTrayX.Messaging.startupDelayFinished)
    {
      // Process the received messages
      SysTrayX.Messaging.newMailCache.forEach( (mail) =>
      {
        if (mail.messages.messages.length > 0) {
          if (SysTrayX.Messaging.new[mail.folder.accountId] === undefined) {
            SysTrayX.Messaging.new[mail.folder.accountId] = {};
          }
    
          if (SysTrayX.Messaging.new[mail.folder.accountId][mail.folder.path] === undefined) {
            SysTrayX.Messaging.new[mail.folder.accountId][mail.folder.path] = [];
          }
    
          SysTrayX.Messaging.new[mail.folder.accountId][mail.folder.path] =
            SysTrayX.Messaging.new[mail.folder.accountId][mail.folder.path].concat(
              mail.messages.messages
            );
        }
      });

      // Cache handled
      SysTrayX.Messaging.newMailCache = [];
    }
  },

  listenerFolderInfoChanged: async function (folder, folderInfo) {

    console.debug("folderInfoChanged: folder: " + JSON.stringify(folder));
    console.debug("folderInfoChanged: folderinfo: " + JSON.stringify(folderInfo));
    //console.debug("folderInfoChanged: Cache: " + SysTrayX.Messaging.folderInfoChangeCache.length );

    if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false") {
      if (!SysTrayX.Messaging.startupDelayFinished) {    
        // Cache the folder change
        if (folder)
        {
          SysTrayX.Messaging.folderInfoChangeCache.push({ folder, folderInfo });
        }
      } else {
        if (SysTrayX.Messaging.startupDelayFinished)
        {
          console.debug("folderInfoChanged: delay finished");

          if( SysTrayX.Messaging.folderInfoChangeCache.length > 0 ) {

            console.debug("folderInfoChanged: handle cache");
            console.debug("FolderInfoChanged: cache: " + JSON.stringify(SysTrayX.Messaging.folderInfoChangeCache));

            // Process the received messages
            for (const cache of SysTrayX.Messaging.folderInfoChangeCache) {
              if (cache.folderInfo.unreadMessageCount !== undefined) {
                if (SysTrayX.Messaging.unread[cache.folder.accountId] === undefined) {
                  SysTrayX.Messaging.unread[cache.folder.accountId] = {};
                }

                if (SysTrayX.Messaging.new[cache.folder.accountId] === undefined) {
                  SysTrayX.Messaging.new[cache.folder.accountId] = {};
                }

                if (SysTrayX.Messaging.new[cache.folder.accountId][cache.folder.path] === undefined) {
                  SysTrayX.Messaging.new[cache.folder.accountId][cache.folder.path] = [];
                }

                console.debug("FolderInfoChanged: Unread: " + JSON.stringify(cache.folderInfo.unreadMessageCount));

                SysTrayX.Messaging.unread[cache.folder.accountId][cache.folder.path] =
                  cache.folderInfo.unreadMessageCount;

                // Check if the new mails have been read, removed from new storage
                const messages = SysTrayX.Messaging.new[cache.folder.accountId][cache.folder.path];

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

                  //console.debug("FolderInfoChanged: Clear");
                  //console.debug("FolderInfoChanged: Old: " + JSON.stringify(SysTrayX.Messaging.new[cache.folder.accountId][cache.folder.path]));
                  console.debug("FolderInfoChanged: New: " + JSON.stringify(newMessages));

                  SysTrayX.Messaging.new[cache.folder.accountId][cache.folder.path] = [
                    ...newMessages,
                  ];
                }
              }
            
              // Cache handled
              SysTrayX.Messaging.folderInfoChangeCache = [];
            }
          } else {

            console.debug("folderInfoChanged: handle default");

            // Count the initial unread messages
            for (const filter of SysTrayX.Messaging.filters) {
              const accountId = filter.accountId;
              for (const storedFolder of filter.folders) {
                let path;
                let folderParam;
                if (typeof(storedFolder) === "string") {
                  //  Filters pre TB 121
                  path = storedFolder;
                  folderParam = {
                    accountId: accountId,
                    path: storedFolder,
                  };
                } else {
                  //  Filters TB 121
                  if (storedFolder.mailFolderId === undefined) {
                    //  TB 121 filter setup but older TB
                    path = storedFolder.path;
                    folderParam = {
                      accountId: accountId,
                      path: path,
                    };
                  } else {
                    path = storedFolder.path;
                    folderParam = storedFolder.mailFolderId;  
                  }
                }
          
                // Check unread mails
                let mailFolderInfo = {};
                try {
                  mailFolderInfo = await browser.folders.getFolderInfo(folderParam);
                } catch (err) {
                  console.debug("Filter error: " + err);
                  console.debug("Filter error: " + JSON.stringify(folder));
                }

                if (mailFolderInfo.unreadMessageCount !== undefined) {
                  if (SysTrayX.Messaging.unread[accountId] === undefined) {
                    SysTrayX.Messaging.unread[accountId] = {};
                  }
        
                  SysTrayX.Messaging.unread[accountId][path] =
                    mailFolderInfo.unreadMessageCount;
                }

                // Check if the new mails have been read, removed from new storage
                if (SysTrayX.Messaging.new[accountId] !== undefined &&
                  SysTrayX.Messaging.new[accountId][path] !== undefined ) {
                  const messages = SysTrayX.Messaging.new[accountId][path];

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

                    SysTrayX.Messaging.new[accountId][path] = [
                      ...newMessages,
                    ];
                  }
                }
              }
            }
          }
          sendMailCountPre115();
        }
      }
    } else {
      if (SysTrayX.Messaging.startupDelayFinished)
      {
        sendMailCount();
      }
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

  showHideForceCloseButton: function () {
    if (SysTrayX.Messaging.closeType === "2" || SysTrayX.Messaging.closeType === "4") {
      // Activate the extra close button when all to tray/taskbar is selected
      browser.browserAction.setTitle({title: "Force close"});
      browser.browserAction.setIcon({path: "icons/close.svg"});
      browser.browserAction.enable();
    } else {
      browser.browserAction.setTitle({title: " "});
      browser.browserAction.setIcon({path: "icons/dummy.png"});
      browser.browserAction.disable();
    }
  },

  onNewWindow: async function (id) {
    SysTrayX.Messaging.showHideForceCloseButton();

    SysTrayX.Link.postSysTrayXMessage({ newWindow: id });
  },

  onCloseButton: async function (id, quit) {
    let state = undefined;
    if (SysTrayX.Messaging.closeType === "1" || SysTrayX.Messaging.closeType === "2") {
      // Minimize to tray
      state = "docked";
    } else if (SysTrayX.Messaging.closeType === "3" || SysTrayX.Messaging.closeType === "4") {
      // Minimize
      state = "minimized";
    }

    if (state !== undefined) {
      // Send new state to the companion
      SysTrayX.Link.postSysTrayXMessage({ closeWindow: { id: id, quit: quit } });
    }
  },

  //
  //  Handle a storage change
  //
  storageChanged: async function (changes, area) {
    //  Get the new preferences

    if ("filters" in changes && changes["filters"].newValue) {
      SysTrayX.Messaging.filters = changes["filters"].newValue;

      sendMailCountPre115();
      sendMailCount();
    }

    if ("closeType" in changes && changes["closeType"].newValue) {
      SysTrayX.Messaging.closeType = changes["closeType"].newValue;

      await browser.windowEvent.setCloseType(Number(SysTrayX.Messaging.closeType));

      browser.windowEvent.onNewWindow.removeListener(
        SysTrayX.Messaging.onNewWindow
      );
      browser.windowEvent.onCloseButtonClick.removeListener(
        SysTrayX.Messaging.onCloseButton
      );

      if (SysTrayX.Messaging.closeType !== "0") {
        browser.windowEvent.onNewWindow.addListener(
          SysTrayX.Messaging.onNewWindow
        );
        browser.windowEvent.onCloseButtonClick.addListener(
          SysTrayX.Messaging.onCloseButton
        );
      }

      SysTrayX.Messaging.showHideForceCloseButton();
    }

    if ("countType" in changes && changes["countType"].newValue) {
      SysTrayX.Messaging.countType = changes["countType"].newValue;

      sendMailCountPre115();
      sendMailCount();
    }

    if ("showNewIndicator" in changes && changes["showNewIndicator"].newValue) {
      SysTrayX.Messaging.showNewIndicator = changes["showNewIndicator"].newValue;

      sendMailCountPre115();
      sendMailCount();
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

  requestOptions: function () {
    SysTrayX.Link.postSysTrayXMessage({ optionsRequest: true });
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
        "startupType",
        "windowPosCor",
        "windowPosCorType",
        "restorePositions",
        "defaultIconType",
        "defaultIconMime",
        "defaultIcon",
        "hideDefaultIcon",
        "iconType",
        "iconMime",
        "icon",
        "invertIcon",
        "showNumber",
        "showNewIndicator",
        "countType",
        "startupDelay",
        "apiCountMethod",
        "numberColor",
        "numberSize",
        "numberAlignment",
        "numberMargins",
        "newIndicatorType",
        "newShadeColor",
        "startApp",
        "startAppArgs",
        "closeApp",
        "closeAppArgs",
        "showHideShortcut",
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
    const startupType = result.startupType || "0";
    const windowPosCor = result.windowPosCor || "false";
    const windowPosCorType = result.windowPosCorType || "0";
    const restorePositions = result.restorePositions || "false";
    const defaultIconType = result.defaultIconType || "0";
    const defaultIconMime = result.defaultIconMime || "image/png";
    const defaultIcon = result.defaultIcon || [];
    const hideDefaultIcon = result.hideDefaultIcon || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];
    const invertIcon = result.invertIcon || "false";
    const showNumber = result.showNumber || "true";
    const showNewIndicator = result.showNewIndicator || "false";
    const countType = result.countType || "0";
    const startupDelay = result.startupDelay || "5";
    const apiCountMethod = result.apiCountMethod || "false";
    let numberColor = result.numberColor || "#000000";
    const numberSize = result.numberSize || "10";
    const numberAlignment = result.numberAlignment || "4";
    const numberMargins = result.numberMargins || {
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
    };
    const newIndicatorType = result.newIndicatorType || "2";
    const newShadeColor = result.newShadeColor || "#ff8000";
    const startApp = result.startApp || "";
    const startAppArgs = result.startAppArgs || "";
    const closeApp = result.closeApp || "";
    const closeAppArgs = result.closeAppArgs || "";
    const showHideShortcut = result.showHideShortcut || "";

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug,
        minimizeType,
        minimizeIconType,
        closeType,
        startupType,
        windowPosCor,
        windowPosCorType,
        restorePositions,
        defaultIconType,
        defaultIconMime,
        defaultIcon,
        hideDefaultIcon,
        iconType,
        iconMime,
        icon,
        invertIcon,
        showNumber,
        showNewIndicator,
        countType,
        startupDelay,
        apiCountMethod,
        numberColor,
        numberSize,
        numberAlignment,
        numberMargins,
        newIndicatorType,
        newShadeColor,
        startApp,
        startAppArgs,
        closeApp,
        closeAppArgs,
        showHideShortcut,
      },
    });
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

    //console.debug("postSysTrayXMessage: " + JSON.stringify(object));
    SysTrayX.Link.portSysTrayX.postMessage(object);
  },

  receiveSysTrayXMessage: async function (response) {
    if (response["shutdown"]) {
      browser.windowEvent.onNewWindow.removeListener(
        SysTrayX.Messaging.onNewWindow
      );
      browser.windowEvent.onCloseButtonClick.removeListener(
        SysTrayX.Messaging.onCloseButton
      );

      SysTrayX.Link.postSysTrayXMessage({ shutdown: "true" });
    }

    const options = response["options"];
    if (options !== undefined) {
      SysTrayX.Info.options = options;

      await storage().set({
        options: options,
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

      const startupType = response["preferences"].startupType;
      if (startupType) {
        await storage().set({
          startupType: startupType,
        });
      }

      const windowPosCor = response["preferences"].windowPosCor;
      if (windowPosCor) {
        await storage().set({
          windowPosCor: windowPosCor,
        });
      }

      const windowPosCorType = response["preferences"].windowPosCorType;
      if (windowPosCorType) {
        await storage().set({
          windowPosCorType: windowPosCorType,
        });
      }

      const restorePositions = response["preferences"].restorePositions;
      if (restorePositions) {
        await storage().set({
          restorePositions: restorePositions,
        });
      }

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

      const invertIcon = response["preferences"].invertIcon;
      if (invertIcon) {
        await storage().set({
          invertIcon: invertIcon,
        });
      }

      const showNumber = response["preferences"].showNumber;
      if (showNumber) {
        await storage().set({
          showNumber: showNumber,
        });
      }

      const showNewIndicator = response["preferences"].showNewIndicator;
      if (showNewIndicator) {
        await storage().set({
          showNewIndicator: showNewIndicator,
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

      const apiCountMethod = response["preferences"].apiCountMethod;
      if (apiCountMethod !== undefined) {
        await storage().set({
          apiCountMethod: apiCountMethod,
        });
        SysTrayX.Messaging.apiCountMethod = apiCountMethod;
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

      const newIndicatorType = response["preferences"].newIndicatorType;
      if (newIndicatorType) {
        await storage().set({
          newIndicatorType: newIndicatorType,
        });
      }

      const newShadeColor = response["preferences"].newShadeColor;
      if (newShadeColor) {
        await storage().set({
          newShadeColor: newShadeColor,
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

      const showHideShortcut = response["preferences"].showHideShortcut;
      if (showHideShortcut !== undefined) {
        await storage().set({
          showHideShortcut: showHideShortcut,
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
  focusChanged: async function (windowId) {

    const windows_all = await browser.windows.getAll();
    //console.debug("focusChanged windows: " + JSON.stringify(windows_all));

    const windows = windows_all.map( (win) => { return {id: win.id, state: win.state} });
    //console.debug("focusChanged windows: " + JSON.stringify(windows));

    SysTrayX.Link.postSysTrayXMessage({ windows: windows } );
  },

  folderChanged: function (tab, displayedFolder) {
    
    //console.debug("Folder changed tab: " + JSON.stringify(tab));
    //console.debug("Folder changed displayedFolder: " + JSON.stringify(displayedFolder));

    if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false") {
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

        if (SysTrayX.Messaging.startupDelayFinished) {
          sendMailCountPre115();
        }
      }

      SysTrayX.Messaging.displayedFolder = displayedFolder;
    } else {
      if (SysTrayX.Messaging.startupDelayFinished) {
        sendMailCount();
      }
    }
  },
};

async function start() {
  //  Setup the link first
  SysTrayX.Link.init();
  
  //  Force close a window using a toolbar button
  browser.browserAction.disable();

  browser.browserAction.onClicked.addListener(async () => {
    const window = await browser.windows.getCurrent();
    await browser.windowEvent.forceClose(window.id);

    // Send new state to the companion
    SysTrayX.Link.postSysTrayXMessage({ closeWindow: { id: window.id, quit: true } });
  });
  
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

  //  Get the API count method preference
  const apiCountMethod = await getApiCountMethod();
  SysTrayX.Messaging.apiCountMethod = apiCountMethod;

  // Try to catch the mails
  if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false") {
    // Catch the new incomming mail
    browser.messages.onNewMailReceived.addListener(
      SysTrayX.Messaging.listenerNewMail
    );
  }

  //  Set catch folder changes
  browser.folders.onFolderInfoChanged.addListener(
    SysTrayX.Messaging.listenerFolderInfoChanged
  );

  //  Catch a folder change to reset the new counter
  browser.mailTabs.onDisplayedFolderChanged.addListener(
    SysTrayX.Window.folderChanged
  );

  //  Get the prefered startup type
  const startupType = await getStartupType();
  SysTrayX.startupType = startupType;

  //  Restore window positions
  const restorePositions = await getRestorePositionsState();

  if (restorePositions === "true") {
    SysTrayX.restorePositions = true;

    // Get the start window positions
    SysTrayX.startupWindowPositions = await getStartupWindowPositions();
  }

  // Get main window id
  const window = await browser.windows.getCurrent();
  SysTrayX.mainWindowId = window.id;

  //console.debug("Main window ID: " + SysTrayX.mainWindowId);

  // Get all windows
  const windows = await browser.windows.getAll();
  SysTrayX.windows = windows;

  //console.debug("All window IDs: " + JSON.stringify(windows.map((win) => win.id)));
  //console.debug("Window: " + JSON.stringify(windows));

  // Set the main window id
  await browser.windowEvent.setMainWindowId(Number(SysTrayX.mainWindowId));

  // Set the close type
  SysTrayX.Messaging.closeType = await getCloseType();
  await browser.windowEvent.setCloseType(Number(SysTrayX.Messaging.closeType));

  //  Intercept close button?
  if (SysTrayX.Messaging.closeType !== "0") {
    browser.windowEvent.onNewWindow.addListener(
      SysTrayX.Messaging.onNewWindow
    );
    browser.windowEvent.onCloseButtonClick.addListener(
      SysTrayX.Messaging.onCloseButton
    );

    SysTrayX.Messaging.showHideForceCloseButton();
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

  //  Reset the options
  await storage().set({
    options: {
      kdeIntegration: true,
      shortcuts: true,
    },
  });

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

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
