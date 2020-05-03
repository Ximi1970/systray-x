var SysTrayX = {
  startupState: undefined,

  pollTiming: {
    pollStartupDelay: "60",
    pollInterval: "60",
  },

  platformInfo: undefined,

  browserInfo: undefined,

  version: "0",
};

SysTrayX.Messaging = {
  accounts: [],

  init: function () {
    //  Get the filters from the storage
    SysTrayX.Messaging.getFilters();

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

    //  Start polling the accounts
    window.setTimeout(
      SysTrayX.Messaging.pollAccounts,
      SysTrayX.pollTiming.pollStartupDelay * 1000
    );

    //  Try to catch the window state
    browser.windows.onFocusChanged.addListener(SysTrayX.Window.focusChanged);
  },

  //
  //  Handle a storage change
  //
  storageChanged: function (changes, area) {
    //  Get the new preferences
    SysTrayX.Messaging.getFilters();

    if ("pollStartupDelay" in changes && changes["pollStartupDelay"].newValue) {
      SysTrayX.pollTiming = {
        ...SysTrayX.pollTiming,
        pollStartupDelay: changes["pollStartupDelay"].newValue,
      };
    }

    if ("pollInterval" in changes && changes["pollInterval"].newValue) {
      SysTrayX.pollTiming = {
        ...SysTrayX.pollTiming,
        pollInterval: changes["pollInterval"].newValue,
      };
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
  //  Poll the accounts
  //
  pollAccounts: function () {
    //
    //  Get the unread nessages of the selected accounts
    //
    const filtersDiv = document.getElementById("filters");
    const filtersAttr = filtersDiv.getAttribute("data-filters");

    if (filtersAttr !== "undefined") {
      const filters = JSON.parse(filtersAttr);

      if (filters.length > 0) {
        SysTrayX.Messaging.unReadMessages(filters).then(
          SysTrayX.Messaging.unreadCb
        );
      } else {
        SysTrayX.Link.postSysTrayXMessage({ unreadMail: 0 });
      }
    } else {
      //  Never saved anything, construct temporary filters
      if (SysTrayX.Messaging.accounts.length > 0) {
        //  Construct inbox filters for all accounts
        let filters = [];
        SysTrayX.Messaging.accounts.forEach((account) => {
          const inbox = account.folders.filter(
            (folder) => folder.type == "inbox"
          );

          if (inbox.length > 0) {
            filters.push({
              unread: true,
              folder: inbox[0],
            });
          }
        });

        //  Store them in the background HTML
        const filtersDiv = document.getElementById("filters");
        filtersDiv.setAttribute("data-filters", JSON.stringify(filters));

        SysTrayX.Messaging.unReadMessages(filters).then(
          SysTrayX.Messaging.unreadCb
        );
      } else {
        //  No accounts, no mail
        SysTrayX.Link.postSysTrayXMessage({ unreadMail: 0 });
      }
    }

    // Next round...
    window.setTimeout(
      SysTrayX.Messaging.pollAccounts,
      SysTrayX.pollTiming.pollInterval * 1000
    );
  },

  //
  //  Use the messages API to get the unread messages (Promise)
  //  Be aware that the data is only avaiable inside the callback
  //
  unReadMessages: async function (filters) {
    let unreadMessages = 0;
    for (let i = 0; i < filters.length; ++i) {
      let page = await browser.messages.query(filters[i]);
      let unread = page.messages.length;

      while (page.id) {
        page = await browser.messages.continueList(page.id);

        unread = unread + page.messages.length;
      }

      unreadMessages = unreadMessages + unread;
    }

    return unreadMessages;
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
      "pollStartupDelay",
      "pollInterval",
      "minimizeType",
      "startMinimized",
      "iconType",
      "iconMime",
      "icon",
      "showNumber",
      "numberColor",
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function (result) {
    const debug = result.debug || "false";
    const pollStartupDelay = result.pollStartupDelay || "60";
    const pollInterval = result.pollInterval || "60";
    const minimizeType = result.minimizeType || "1";
    const startMinimized = result.startMinimized || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];
    const showNumber = result.showNumber || "true";
    const numberColor = result.numberColor || "#000000";

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
        pollStartupDelay: pollStartupDelay,
        pollInterval: pollInterval,
        minimizeType: minimizeType,
        startMinimized: startMinimized,
        iconType: iconType,
        iconMime: iconMime,
        icon: icon,
        showNumber: showNumber,
        numberColor: numberColor,
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

  //
  //  Get the filters from the storage
  //
  getFilters: function () {
    const getter = browser.storage.sync.get("filters");
    getter.then(this.getFiltersStorage, this.onGetFiltersStorageError);
  },

  //
  //  Get the filters from the storage and
  //  make them available in the background HTML
  //
  getFiltersStorage: function (result) {
    const filters = result.filters || undefined;

    //  Store them in the background HTML
    const filtersDiv = document.getElementById("filters");
    filtersDiv.setAttribute("data-filters", JSON.stringify(filters));
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

      const pollStartupDelay = response["preferences"].pollStartupDelay;
      if (pollStartupDelay) {
        browser.storage.sync.set({
          pollStartupDelay: pollStartupDelay,
        });
      }

      const pollInterval = response["preferences"].pollInterval;
      if (pollInterval) {
        browser.storage.sync.set({
          pollInterval: pollInterval,
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

  // Get the poll timing
  SysTrayX.pollTiming = await getPollTiming();

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

  //  Setup the link first
  SysTrayX.Link.init();

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
