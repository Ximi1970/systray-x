var SysTrayX = {
  accounts: [],

  startupState: undefined,

  hideDefaultIcon: false,

  platformInfo: undefined,

  browserInfo: undefined,

  version: "0",
};

SysTrayX.Messaging = {
  accounts: [],
  countType: 0,
  closeType: 1,
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

    //  Send hide default icon preference
    SysTrayX.Messaging.sendHideDefaultIcon();

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

    browser.folderChange.onUnreadMailChange.addListener(function (unread) {
      SysTrayX.Messaging.unreadCb(unread);
    });

    //  Set the count type in the folderChange listener
    browser.folderChange.setCountType(Number(SysTrayX.Messaging.countType));

    //  Set the filters in the folderChange listener
    browser.folderChange.setFilters(SysTrayX.Messaging.filters);

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);
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

  sendHideDefaultIcon: function () {
    SysTrayX.Link.postSysTrayXMessage({
      hideDefaultIcon: SysTrayX.hideDefaultIcon,
    });
  },

  sendPreferences: function () {
    const getter = browser.storage.sync.get([
      "debug",
      "minimizeType",
      "closeType",
      "startMinimized",
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
      "countType",
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function (result) {
    const debug = result.debug || "false";
    const minimizeType = result.minimizeType || "1";
    const closeType = result.closeType || "1";
    const startMinimized = result.startMinimized || "false";
    const defaultIconType = result.defaultIconType || "0";
    const defaultIconMime = result.defaultIconMime || "image/png";
    const defaultIcon = result.defaultIcon || [];
    const hideDefaultIcon = result.hideDefaultIcon || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];
    const showNumber = result.showNumber || "true";
    const numberColor = result.numberColor || "#000000";
    const numberSize = result.numberSize || "10";
    const countType = result.countType || "0";

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
        minimizeType: minimizeType,
        closeType: closeType,
        startMinimized: startMinimized,
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
        countType: countType,
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

  SysTrayX.Window.startWindow = await browser.windows
    .getCurrent()
    .then((currentWindow) => currentWindow);

  //  Get all accounts
  SysTrayX.Messaging.accounts = await browser.accounts.list();

  // Get the filters (needs the accounts)
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
