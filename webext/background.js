console.log("Starting background.js");

var SysTrayX = {};

SysTrayX.Messaging = {
  initialized: false,

  unreadFilter: { unread: true },

  init: function() {
    if (this.initialized) {
      console.log("Messaging already initialized");
      return;
    }
    console.log("Enabling Messaging");

    this.unReadMessages(this.unreadFilter).then(this.unreadCb);

    this.initialized = true;
  },

  /*
   * Use the messages API to get the unread messages (Promise)
   * Be aware that the data is only avaiable inside the callback
   */
  unReadMessages: async function(filter) {
    let page = await browser.messages.query(filter);

    let unread = page.messages.length;

    while (page.id) {
      page = await browser.messages.continueList(page.id);

      unread = unread + page.messages.length;
    }

    return unread;
  },

  /*
   * Callback for unReadMessages
   */
  unreadCb: function(count) {
    console.log("SysTrayX unread " + count);
  },

  /*
   *  Get the accounts from the storage and
   *  make them available in the background HTML
   */
  getAccountsStorage: function(result) {
    console.debug("Get accounts from storage");

    let accounts = result.accounts || [];

    // Store them in the background HTML
    let accountsDiv = document.getElementById("accounts");
    accountsDiv.setAttribute("data-accounts", JSON.stringify(accounts));
  },

  onGetAccountsStoageError: function(error) {
    console.log(`GetAccounts Error: ${error}`);
  },

  getAccounts: function() {
    console.debug("Get accounts");

    let getter = browser.storage.sync.get("accounts");
    getter.then(this.getAccountsStorage, this.onGetAccountsStoageError);
  }
};

console.log("Starting SysTray-X");

SysTrayX.Messaging.init();
SysTrayX.Messaging.getAccounts();




function pollAccounts() {
  let accountsDiv = document.getElementById("accounts");

  accountsAttr = accountsDiv.getAttribute("data-accounts");
  console.debug("Accounts attr: " + accountsAttr);

  accounts = JSON.parse(accountsAttr);
  console.debug("Accounts poll: " + accounts.length);
}

window.setTimeout(pollAccounts, 1000);

console.log("Done");
