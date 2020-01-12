console.log("Starting background.js");

var SysTrayX = {
  debugAccounts: false
};

SysTrayX.Messaging = {
  initialized: false,

  unreadFiltersTest: [
    { unread: true },
    { unread: true, folder: { accountId: "account1", path: "/INBOX" } }
  ],

  init: function() {
    if (this.initialized) {
      console.log("Messaging already initialized");
      return;
    }
    console.log("Enabling Messaging");

    //    this.unReadMessages(this.unreadFiltersTest).then(this.unreadCb);

    this.initialized = true;
  },

  /*
   * Use the messages API to get the unread messages (Promise)
   * Be aware that the data is only avaiable inside the callback
   */
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

    let filters = result.filters || [];

    // Store them in the background HTML
    let filtersDiv = document.getElementById("filters");
    filtersDiv.setAttribute("data-filters", JSON.stringify(filters));
  },

  onGetAccountsStoageError: function(error) {
    console.log(`GetAccounts Error: ${error}`);
  },

  getAccounts: function() {
    console.debug("Get accounts");

    let getter = browser.storage.sync.get(["accounts", "filters"]);
    getter.then(this.getAccountsStorage, this.onGetAccountsStoageError);

    if (SysTrayX.debugAccounts) {
      let accountsDiv = document.getElementById("accounts");

      let accountsAttr = accountsDiv.getAttribute("data-accounts");
      console.debug("Accounts attr: " + accountsAttr);

      let accounts = JSON.parse(accountsAttr);
      console.debug("Accounts poll: " + accounts.length);
    }
  },  
};

console.log("Starting SysTray-X");

SysTrayX.Messaging.init();

function pollAccounts() {
  console.debug("Polling");

  SysTrayX.Messaging.getAccounts();

  /*
   *  Get the unread nessages of the selected accounts
   */
  let filtersDiv = document.getElementById("filters");
  let filtersAttr = filtersDiv.getAttribute("data-filters");
  let filters = JSON.parse(filtersAttr);

  SysTrayX.Messaging.unReadMessages(filters).then(SysTrayX.Messaging.unreadCb);
}

window.setInterval(pollAccounts, 1000);

console.log("Done");
