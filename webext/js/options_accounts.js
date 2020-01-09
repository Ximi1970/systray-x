var SysTrayX = {};

SysTrayX.Accounts = {
  initialized: false,

  init: function() {
    if (this.initialized) {
      console.log("Accounts already initialized");
      return;
    }
    console.log("Enabling Accounts");

    this.getAccounts().then(this.getAccountsCb);

    this.initialized = true;
  },

  /*
   * Use the messages API to get the unread messages (Promise)
   * Be aware that the data is only avaiable inside the callback
   */
  getAccounts: async function() {
    return await browser.accounts.list();
  },

  /*
   * Callback for getAccounts
   */
  getAccountsCb: function(mailAccount) {
    let accounts = new Object();
    let i;

    for (i = 0; i < mailAccount.length; i++) {
      console.debug("SysTrayX accounts id: " + mailAccount[i].id);
      console.debug("SysTrayX accounts name: " + mailAccount[i].name);
      console.debug("SysTrayX accounts type: " + mailAccount[i].type);

      accounts[mailAccount[i].type] = [];
      accounts[mailAccount[i].type].push({
        id: mailAccount[i].id,
        name: mailAccount[i].name
      });
    }

    console.debug("SysTrayX Storage: " + accounts.imap[0].name);

    for (let prop in accounts) {
      console.debug(prop + ": " + accounts[prop][0].name);
    }

    let p = document.getElementsByClassName("account");
    p[0].innerHTML = accounts.imap[0].name;

    // Get base
    let treeBase = document.getElementsByClassName("accountstree");

    for (let prop in accounts) {
      console.debug("Tree: " + prop + ": " + accounts[prop][0].name);

      let typeLi = document.createElement("li");

      let typeSpan = document.createElement("span");
      typeSpan.setAttribute("class", "caret");
      let typeText = document.createTextNode(prop);
      typeSpan.appendChild(typeText);

      typeLi.appendChild(typeSpan);

      treeBase[0].appendChild(typeLi);
    }
  }
};

SysTrayX.Accounts.init();
