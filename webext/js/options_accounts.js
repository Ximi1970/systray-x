var SysTrayX = {};

SysTrayX.Accounts = {
  initialized: false,

  init: function () {
    this.getAccounts().then(this.getAccountsCb);
  },

  /*
   * Use the messages API to get the unread messages (Promise)
   * Be aware that the data is only avaiable inside the callback
   */
  getAccounts: async function () {
    return await browser.accounts.list();
  },

  /*
   * Callback for getAccounts
   */
  getAccountsCb: function (mailAccount) {
    function createFolderTree(folders) {
      let result = [];
      let level = { result };

      folders.forEach((folder) => {
        folder.path
          .slice(1)
          .split("/")
          .reduce((r, name, i, a) => {
            if (!r[name]) {
              r[name] = { result: [] };
              r.result.push({ name: folder.name, children: r[name].result });
            }

            return r[name];
          }, level);
      });

      return result;
    }

    let accounts = new Object();

    for (let i = 0; i < mailAccount.length; i++) {
      if (true) {
        console.debug("SysTrayX accounts id: " + mailAccount[i].id);
        console.debug("SysTrayX accounts name: " + mailAccount[i].name);
        console.debug("SysTrayX accounts type: " + mailAccount[i].type);
        for (let j = 0; j < mailAccount[i].folders.length; j++) {
          console.debug(
            "SysTrayX accounts folder: " +
              mailAccount[i].folders[j].name +
              ", " +
              mailAccount[i].folders[j].type
          );
        }
      }

      if (!accounts[mailAccount[i].type]) {
        accounts[mailAccount[i].type] = [];
      }
      accounts[mailAccount[i].type].push({
        id: mailAccount[i].id,
        name: mailAccount[i].name,
        folders: mailAccount[i].folders,
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

          const typeSubSpan = document.createElement("span");
          typeSubSpan.setAttribute("class", "caret");
          typeLi.appendChild(typeSubSpan);

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

          //  Create a usable folder tree
          const folders = createFolderTree(accounts[prop][i].folders);

          //  Recursive list creator
          function createListLevel(level) {
            const typeLevelUl = document.createElement("ul");
            typeLevelUl.setAttribute("class", "nested");

            level.forEach((element) => {
              console.debug("Name: " + element.name);

              const typeEleLi = document.createElement("li");

              const typeEleSpan = document.createElement("span");
              if (element.children.length > 0) {
                typeEleSpan.setAttribute("class", "caret");
              } else {
                typeEleSpan.setAttribute("class", "noncaret");
              }
              typeEleLi.appendChild(typeEleSpan);

              const typeEleInput = document.createElement("input");
              typeEleInput.setAttribute("type", "checkbox");
              typeEleInput.setAttribute("name", element.name);
//              typeEleInput.setAttribute("value", JSON.stringify(element.name));
              typeEleInput.setAttribute("checked", "true");
              typeEleLi.appendChild(typeEleInput);
              const typeEleText = document.createTextNode(" " + element.name);
              typeEleLi.appendChild(typeEleText);

              if (element.children.length > 0) {
                typeEleLi.appendChild(createListLevel(element.children));
              }

              typeLevelUl.appendChild(typeEleLi);
            });

            return typeLevelUl;
          }

          //  Create a folder list recursively
          typeLi.appendChild(createListLevel(folders));

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
      toggler[i].addEventListener("click", function () {
        this.parentElement.querySelector(".nested").classList.toggle("active");
        this.classList.toggle("caret-down");
      });
    }
  },
};

SysTrayX.Accounts.init();
