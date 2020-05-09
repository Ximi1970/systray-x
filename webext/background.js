var SysTrayX = {
  startupState: undefined,

  platformInfo: undefined,

  browserInfo: undefined,

  version: "0",
};

SysTrayX.Messaging = {
  accounts: [],
  countType: 0,
  filtersExt: undefined,
  filters: undefined,

  init: function () {
    // Lookout for storage changes
    browser.storage.onChanged.addListener(SysTrayX.Messaging.storageChanged);

    //  Send the platform info to app
    SysTrayX.Messaging.sendPlatformInfo();

    //  Send the browser info to app
    SysTrayX.Messaging.sendBrowserInfo();

    //  Send the window title to app
    SysTrayX.Messaging.sendTitle();

    //  Send version to app
    SysTrayX.Messaging.sendVersion();

    //  Send preferences to app
    SysTrayX.Messaging.sendPreferences();

    /*
    //  New mail listener (TB76+)
    if (SysTrayX.browserInfo.majorVersion > 75) {
      //
      //  Mixed results, forgets accounts?, double events?
      //
      browser.messages.onNewMailReceived.addListener(
        SysTrayX.Messaging.newMail
      );
    }
    */

    //  Set the count type in the folderChange listener
    browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));

    //  Set the filters in the folderChange listener
    browser.folderChange.setFilters(SysTrayX.Messaging.filtersExt);

    browser.folderChange.onUnreadMailChange.addListener(function (unread) {
      console.debug("folderChangeListener: " + unread);

      SysTrayX.Messaging.unreadCb(unread)
    });

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);
  },

  /*
  newMail: async function (folder, messages) {
    console.debug(
      "New mail: " + folder.accountId + ", " + messages.messages.length
    );

    let unread = messages.messages.length;
    while (messages.id) {
      page = await browser.messages.continueList(messages.id);

      unread = unread + page.messages.length;
    }
    console.debug("Unread: " + unread);
  },
  */

  //
  //  Handle a storage change
  //
  storageChanged: function (changes, area) {
    //  Get the new preferences

    if ("filtersExt" in changes && changes["filtersExt"].newValue) {
      SysTrayX.Messaging.filtersExt = changes["filtersExt"].newValue;

      browser.folderChange.setFilters(SysTrayX.Messaging.filtersExt);
    }

    if ("filters" in changes && changes["filters"].newValue) {
      SysTrayX.Messaging.filters = changes["filters"].newValue;
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

  sendBrowserInfo: function () {
    const info = SysTrayX.browserInfo;
    SysTrayX.Link.postSysTrayXMessage({ browserInfo: info });
  },

  sendPlatformInfo: function () {
    const info = SysTrayX.platformInfo;
    SysTrayX.Link.postSysTrayXMessage({ platformInfo: info });
  },

  sendTitle: function () {
    const title = "-" + SysTrayX.Window.startWindow.title.split("-").pop();
    SysTrayX.Link.postSysTrayXMessage({ title: title });
  },

  sendVersion: function () {
    SysTrayX.Link.postSysTrayXMessage({ version: SysTrayX.version });
  },

  sendPreferences: function () {
    const getter = browser.storage.sync.get([
      "debug",
      "minimizeType",
      "startMinimized",
      "iconType",
      "iconMime",
      "icon",
      "showNumber",
      "numberColor",
      "countType",
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function (result) {
    const debug = result.debug || "false";
    const minimizeType = result.minimizeType || "1";
    const startMinimized = result.startMinimized || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];
    const showNumber = result.showNumber || "true";
    const numberColor = result.numberColor || "#000000";
    const countType = result.countType || "0";

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
        minimizeType: minimizeType,
        startMinimized: startMinimized,
        iconType: iconType,
        iconMime: iconMime,
        icon: icon,
        showNumber: showNumber,
        numberColor: numberColor,
        countType: countType,
      },
    });

    if (SysTrayX.startupState) {
      //  Send startup state after the prefs
      //  so the hide is handled conform the prefs
      SysTrayX.Link.postSysTrayXMessage({ window: SysTrayX.startupState });

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
    if (response["window"]) {
      if (response["window"] === "minimized") {
        browser.windows.update(SysTrayX.Window.startWindow.id, {
          state: "minimized",
        });
      }

      if (response["window"] === "normal") {
        browser.windows.update(SysTrayX.Window.startWindow.id, {
          state: "normal",
          focused: true,
        });
      }
    }

    if (response["shutdown"]) {
      console.log("Shutdown received: " + response["shutdown"]);
    }

    if (response["preferences"]) {
      //  Store the preferences from the app
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

      const startMinimized = response["preferences"].startMinimized;
      if (startMinimized) {
        browser.storage.sync.set({
          startMinimized: startMinimized,
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
  startWindow: undefined,

  focusChanged: function (windowId) {
    browser.windows.getCurrent().then((win) => {
      SysTrayX.Link.postSysTrayXMessage({ window: win.state });
    });
  },
};

async function start() {
  //  Get the prefered start state
  const state = await getStartupState();

  if (state == "minimized") {
    browser.windows.update(browser.windows.WINDOW_ID_CURRENT, {
      state: "minimized",
    });
  }

  SysTrayX.startupState = state;

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

  //  Get addon version
  SysTrayX.version = browser.runtime.getManifest().version;
  console.log("Addon version: " + SysTrayX.version);

  //  Init defaults before everything
  await getDefaultIcon();

  SysTrayX.Window.startWindow = await browser.windows
    .getCurrent()
    .then((currentWindow) => currentWindow);

  //  Get all accounts
  SysTrayX.Messaging.accounts = await browser.accounts.list();

  // Get the extended filters
  SysTrayX.Messaging.filtersExt = await getFiltersExt();

  // Get the filters
  SysTrayX.Messaging.filters = await getFilters();

  // Get the count type
  SysTrayX.Messaging.countType = await getCountType();

  //  Setup the link first
  SysTrayX.Link.init();

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
