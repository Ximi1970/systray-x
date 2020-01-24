console.log("Starting background.js");

var SysTrayX = {
  debugAccounts: false
};

SysTrayX.Messaging = {
  unreadFiltersTest: [
    { unread: true },
    { unread: true, folder: { accountId: "account1", path: "/INBOX" } }
  ],

  init: function() {
    console.log("Enabling Messaging");

    //  Get the accounts from the storage
    SysTrayX.Messaging.getAccounts();
    browser.storage.onChanged.addListener(SysTrayX.Messaging.storageChanged);

    //  Send preferences to app
    SysTrayX.Messaging.sendPreferences();

    //    this.unReadMessages(this.unreadFiltersTest).then(this.unreadCb);
    window.setInterval(SysTrayX.Messaging.pollAccounts, 10000);
  },

  //
  //  Handle a storage change
  //
  storageChanged: function(changes, area) {
    console.debug("Changes in store");

    //  Get the new preferences
    SysTrayX.Messaging.getAccounts();

    if ("addonprefchanged" in changes && changes["addonprefchanged"].newValue) {
      console.debug("Sending preference");

      //
      //  Send new preferences to the app
      //
      SysTrayX.Messaging.sendPreferences();

      //  Reset flag
      browser.storage.sync.set({
        addonprefchanged: false
      });
    }

    /*
    var changedItems = Object.keys(changes);
    for (var item of changedItems) {
      console.log(item + " has changed:");
      console.log("Old value: ");
      console.log(changes[item].oldValue);
      console.log("New value: ");
      console.log(changes[item].newValue);
    }
*/
  },

  //
  //  Poll the accounts
  //
  pollAccounts: function() {
    console.debug("Polling");

    //
    //  Get the unread nessages of the selected accounts
    //
    const filtersDiv = document.getElementById("filters");
    const filtersAttr = filtersDiv.getAttribute("data-filters");
    const filters = JSON.parse(filtersAttr);

    if (filters.length > 0) {
      SysTrayX.Messaging.unReadMessages(filters).then(
        SysTrayX.Messaging.unreadCb
      );
    } else {
      SysTrayX.Messaging.unReadMessages([{ unread: true }]).then(
        SysTrayX.Messaging.unreadCb
      );
    }
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

  sendPreferences: function() {
    console.debug("Send preferences");

    const getter = browser.storage.sync.get([
      "debug",
      "iconType",
      "iconMime",
      "icon"
    ]);
    getter.then(this.sendPreferencesStorage, this.onSendPreferecesStorageError);
  },

  sendPreferencesStorage: function(result) {
    console.debug("Get preferences from storage");

    const debug = result.debug || "false";
    const iconType = result.iconType || "0";
    const iconMime = result.iconMime || "image/png";
    const icon = result.icon || [];

    console.log(`Debug ${debug}`);
    console.log(`Type ${iconType}`);
    console.log(`Mime ${iconMime}`);
    console.log(icon);

    //  Send it to the app
    SysTrayX.Link.postSysTrayXMessage({
      preferences: {
        debug: debug,
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
    console.debug("Get accounts");

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
    console.debug("Get accounts from storage");

    const accounts = result.accounts || [];

    //  Store them in the background HTML
    const accountsDiv = document.getElementById("accounts");
    accountsDiv.setAttribute("data-accounts", JSON.stringify(accounts));

    const filters = result.filters || [];

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
    console.log(`Received: ${response}`);

    if (response["preferences"]) {
      //  Store the preferences from the app
      console.log("Preferences received");

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

      const debug = response["preferences"].debug;
      if (debug) {
        browser.storage.sync.set({
          debug: debug
        });
      }
    }
  }
};

async function start() {
  // Init defaults before everything
  await getDefaultIcon();

  //  Setup the link first
  SysTrayX.Link.init();

  //  Main start
  SysTrayX.Messaging.init();
}

console.log("Starting SysTray-X");

//  Start the add-on
start();

console.log("End SysTray-X");
