var SysTrayX = {
  debugAccounts: false,

  pollTiming: {
    pollStartupDelay: "30",
    pollInterval: "30"
  },

  platformInfo: undefined
};

SysTrayX.Messaging = {
  unreadFiltersTest: [
    { unread: true },
    { unread: true, folder: { accountId: "account1", path: "/INBOX" } }
  ],

  init: function() {
    //  Get the accounts from the storage
    SysTrayX.Messaging.getAccounts();

    // Lookout for storage changes
    browser.storage.onChanged.addListener(SysTrayX.Messaging.storageChanged);

    //  Send the window title to app
    SysTrayX.Messaging.sendTitle();

    //  Send preferences to app
    SysTrayX.Messaging.sendPreferences();

    //    this.unReadMessages(this.unreadFiltersTest).then(this.unreadCb);
    //    window.setInterval(SysTrayX.Messaging.pollAccounts, 1000);
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
  storageChanged: function(changes, area) {
    //  Get the new preferences
    SysTrayX.Messaging.getAccounts();

    if ("pollStartupDelay" in changes && changes["pollStartupDelay"].newValue) {
      SysTrayX.pollTiming = {
        ...SysTrayX.pollTiming,
        pollStartupDelay: changes["pollStartupDelay"].newValue
      };
    }

    if ("pollInterval" in changes && changes["pollInterval"].newValue) {
      SysTrayX.pollTiming = {
        ...SysTrayX.pollTiming,
        pollInterval: changes["pollInterval"].newValue
      };
    }

    if ("addonprefchanged" in changes && changes["addonprefchanged"].newValue) {
      //
      //  Send new preferences to the app
      //
      SysTrayX.Messaging.sendPreferences();

      //  Reset flag
      browser.storage.sync.set({
        addonprefchanged: false
      });
    }
  },

  //
  //  Poll the accounts
  //
  pollAccounts: function() {
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
      SysTrayX.Messaging.unReadMessages([{ unread: true }]).then(
        SysTrayX.Messaging.unreadCb
      );
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
  unReadMessages: async function(filters) {
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
  unreadCb: function(count) {
    SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
  },

  sendTitle: function() {
    const title = "-" + SysTrayX.Window.startWindow.title.split("-").pop();
    SysTrayX.Link.postSysTrayXMessage({ title: title });
  },

  sendPreferences: function() {
    const getter = browser.storage.sync.get([
      "debug",
      "pollStartupDelay",
      "pollInterval",
      "hideOnMinimize",
      "startMinimized",
      "iconType",
      "iconMime",
      "icon"
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function(result) {
    const debug = result.debug || "false";
    const pollStartupDelay = result.pollStartupDelay || "30";
    const pollInterval = result.pollInterval || "30";
    const hideOnMinimize = result.hideOnMinimize || "true";
    const startMinimized = result.startMinimized || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
        pollStartupDelay: pollStartupDelay,
        pollInterval: pollInterval,
        hideOnMinimize: hideOnMinimize,
        startMinimized: startMinimized,
        iconType: iconType,
        iconMime: iconMime,
        icon: icon
      }
    });
  },

  onSendIconStorageError: function(error) {
    console.log(`GetIcon Error: ${error}`);
  },

  //
  //  Get the accounts from the storage
  //
  getAccounts: function() {
    const getter = browser.storage.sync.get(["accounts", "filters"]);
    getter.then(this.getAccountsStorage, this.onGetAccountsStorageError);

    if (SysTrayX.debugAccounts) {
      const accountsDiv = document.getElementById("accounts");

      const accountsAttr = accountsDiv.getAttribute("data-accounts");
      console.debug(`Accounts attr: ${accountsAttr}`);

      const accounts = JSON.parse(accountsAttr);
      console.debug(`Accounts poll: ${accounts.length}`);
    }
  },

  //
  //  Get the accounts from the storage and
  //  make them available in the background HTML
  //
  getAccountsStorage: function(result) {
    const accounts = result.accounts || undefined;

    //  Store them in the background HTML
    const accountsDiv = document.getElementById("accounts");
    accountsDiv.setAttribute("data-accounts", JSON.stringify(accounts));

    const filters = result.filters || undefined;

    //  Store them in the background HTML
    const filtersDiv = document.getElementById("filters");
    filtersDiv.setAttribute("data-filters", JSON.stringify(filters));
  },

  onGetAccountsStorageError: function(error) {
    console.log(`GetAccounts Error: ${error}`);
  }
};

//
//  Link object. Handles the native messaging to the system tray app
//
SysTrayX.Link = {
  portSysTrayX: undefined,

  init: function() {
    //  Connect to the app
    this.portSysTrayX = browser.runtime.connectNative("SysTray_X");

    //  Listen for messages from the app.
    this.portSysTrayX.onMessage.addListener(
      SysTrayX.Link.receiveSysTrayXMessage
    );
  },

  postSysTrayXMessage: function(object) {
    //  Send object (will be stringified by postMessage)
    SysTrayX.Link.portSysTrayX.postMessage(object);
  },

  receiveSysTrayXMessage: function(response) {
    if (response["window"]) {
      if (response["window"] === "minimized") {
        browser.windows.update(SysTrayX.Window.startWindow.id, {
          state: "minimized"
        });
      }

      if (response["window"] === "normal") {
        browser.windows.update(SysTrayX.Window.startWindow.id, {
          state: "normal",
          focused: true
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
          iconMime: iconMime
        });
      }

      const icon = response["preferences"].icon;
      if (icon) {
        browser.storage.sync.set({
          icon: icon
        });
      }

      const iconType = response["preferences"].iconType;
      if (iconType) {
        browser.storage.sync.set({
          iconType: iconType
        });
      }

      const hideOnMinimize = response["preferences"].hideOnMinimize;
      if (hideOnMinimize) {
        browser.storage.sync.set({
          hideOnMinimize: hideOnMinimize
        });
      }

      const startMinimized = response["preferences"].startMinimized;
      if (startMinimized) {
        browser.storage.sync.set({
          startMinimized: startMinimized
        });
      }

      const pollStartupDelay = response["preferences"].pollStartupDelay;
      if (pollStartupDelay) {
        browser.storage.sync.set({
          pollStartupDelay: pollStartupDelay
        });
      }

      const pollInterval = response["preferences"].pollInterval;
      if (pollInterval) {
        browser.storage.sync.set({
          pollInterval: pollInterval
        });
      }

      const debug = response["preferences"].debug;
      if (debug) {
        browser.storage.sync.set({
          debug: debug
        });
      }
    }
  }
};

SysTrayX.Window = {
  startWindow: undefined,

  focusChanged: function(windowId) {
    browser.windows.getCurrent().then(win => {
      SysTrayX.Link.postSysTrayXMessage({ window: win.state });
    });
  }
};

async function start() {
  //  Get the prefered start state
  const state = await getStartupState();

  if (state == "minimized") {
    browser.windows.update(browser.windows.WINDOW_ID_CURRENT, {
      state: "minimized"
    });
  }

  // Get the poll timing
  SysTrayX.pollTiming = await getPollTiming();

  //  Set platform
  SysTrayX.platformInfo = await browser.runtime
    .getPlatformInfo()
    .then(info => info);
  console.log("OS: " + SysTrayX.platformInfo.os);
  console.log("Arch: " + SysTrayX.platformInfo.arch);
  console.log("Nack-Arch: " + SysTrayX.platformInfo.nacl_arch);

  //  Init defaults before everything
  await getDefaultIcon();

  SysTrayX.Window.startWindow = await browser.windows
    .getCurrent()
    .then(currentWindow => currentWindow);

  //  Setup the link first
  SysTrayX.Link.init();

  //  Send current state
  SysTrayX.Link.postSysTrayXMessage({ window: state });

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();
