var SysTrayX = {};

SysTrayX.Accounts = {
  initialized: false,

  init: function() {
    this.getAccounts().then(this.getAccountsCb);
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

    for (let i = 0; i < mailAccount.length; i++) {
      //      console.debug("SysTrayX accounts id: " + mailAccount[i].id);
      //      console.debug("SysTrayX accounts name: " + mailAccount[i].name);
      //      console.debug("SysTrayX accounts type: " + mailAccount[i].type);

      if (!accounts[mailAccount[i].type]) {
        accounts[mailAccount[i].type] = [];
      }
      accounts[mailAccount[i].type].push({
        id: mailAccount[i].id,
        name: mailAccount[i].name,
        folders: mailAccount[i].folders
      });
    }

    /*
     *  Build tree
     */

    // Get base
    const treeBase = document.getElementById("accountsTree");

    for (let prop in accounts) {
      const typeLi = document.createElement("li");

      const typeSpan = document.createElement("span");
      if (accounts[prop]) {
        typeSpan.setAttribute("class", "caret");
      }
      const typeText = document.createTextNode(prop);
      typeSpan.appendChild(typeText);
      typeLi.appendChild(typeSpan);

      if (accounts[prop]) {
        const typeUl = document.createElement("ul");
        typeUl.setAttribute("class", "nested active");

        for (let i = 0; i < accounts[prop].length; ++i) {
          const typeLi = document.createElement("li");
          const typeInput = document.createElement("input");
          typeInput.setAttribute("type", "checkbox");
          typeInput.setAttribute("name", accounts[prop][i].id);
          typeInput.setAttribute("value", JSON.stringify(accounts[prop][i]));
          typeInput.setAttribute("checked", "true");
          typeLi.appendChild(typeInput);
          const typeText = document.createTextNode(
            " " + accounts[prop][i].name
          );
          typeLi.appendChild(typeText);
          typeUl.appendChild(typeLi);
        }
        typeLi.appendChild(typeUl);
      }

      treeBase.appendChild(typeLi);

      // Restore saved selection
      function setAccounts(result) {
        const treeBase = document.getElementById("accountsTree");
        const accounts = result.accounts || [];
        for (let i = 0; i < accounts.length; ++i) {
          const checkbox = treeBase.querySelector(
            `input[name=${accounts[i].id}]`
          );
          if (checkbox) {
            checkbox.checked = accounts[i].checked;
          }
        }
      }

      function onError(error) {
        console.log(`GetAccounts Error: ${error}`);
      }

      const getAccounts = browser.storage.sync.get("accounts");
      getAccounts.then(setAccounts, onError);
    }

    /*
     *  Activate tree
     */
    const toggler = document.getElementsByClassName("caret");
    for (let i = 0; i < toggler.length; i++) {
      toggler[i].addEventListener("click", function() {
        this.parentElement.querySelector(".nested").classList.toggle("active");
        this.classList.toggle("caret-down");
      });
    }
  }
};

SysTrayX.Accounts.init();
