var SysTrayX = {};

var BrowserInfo = {};

SysTrayX.Accounts = {
  initialized: false,

  init: async function () {
    BrowserInfo = await browser.runtime.getBrowserInfo().then((info) => info);

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
    function createFolderTreePre74(accountName, folders) {
      let result = [];
      let level = { result };

      folders.forEach((folder) => {
        folder.path
          .slice(1)
          .split("/")
          .reduce((r, name, i, a) => {
            if (!r[name]) {
              r[name] = { result: [] };
              r.result.push({
                accountName: accountName,
                accountId: folder.accountId,
                name: folder.name,
                path: folder.path,
                subFolders: r[name].result,
              });
            }

            return r[name];
          }, level);
      });

      return result;
    }

    function createFolderTree(accountName, folders) {
      function traverse(folders) {
        if (!folders) {
          return;
        }
        for (let f of folders) {
          f.accountName = accountName;
          traverse(f.subFolders);
        }
      }

      traverse(folders);

      return folders;
    }

    let accounts = new Object();

    for (let i = 0; i < mailAccount.length; i++) {
      if (false) {
        console.debug("SysTrayX accounts id: " + mailAccount[i].id);
        console.debug("SysTrayX accounts name: " + mailAccount[i].name);
        console.debug("SysTrayX accounts type: " + mailAccount[i].type);
        for (let j = 0; j < mailAccount[i].folders.length; j++) {
          console.debug(
            "SysTrayX accounts folder path: " + mailAccount[i].folders[j].path
          );
          console.debug(
            "SysTrayX accounts folder name: " + mailAccount[i].folders[j].name
          );
          console.debug(
            "SysTrayX accounts folder type: " + mailAccount[i].folders[j].type
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
          typeInput.setAttribute("value", JSON.stringify(accounts[prop][i]));
          typeInput.setAttribute("name", accounts[prop][i].id);

          typeLi.appendChild(typeInput);
          const typeText = document.createTextNode(
            " " + accounts[prop][i].name
          );
          typeLi.appendChild(typeText);

          //  Create a usable folder tree
          let folders = [];
          if (BrowserInfo.version.split(".")[0] < 74) {
            //  Pre TB74 accounts API
            folders = createFolderTreePre74(
              accounts[prop][i].name,
              accounts[prop][i].folders
            );
          } else {
            //  TB74+ accounts API, (this shit never ends...)
            folders = createFolderTree(
              accounts[prop][i].name,
              accounts[prop][i].folders
            );
          }

          console.debug("Folders: " + JSON.stringify(folders));

          //  Recursive list creator
          function createListLevel(level, parent) {
            const typeLevelUl = document.createElement("ul");
            typeLevelUl.setAttribute("class", "nested");

            if (parent) {
              parent.subFolders = [];
              parent.name = "^ Add base folder";

              level.unshift(parent);
            }

            level.forEach((element) => {
              const typeEleLi = document.createElement("li");

              const typeEleSpan = document.createElement("span");
              if (element.subFolders.length > 0) {
                typeEleSpan.setAttribute("class", "caret");
              } else {
                typeEleSpan.setAttribute("class", "caretfiller");
              }
              typeEleLi.appendChild(typeEleSpan);

              const typeEleInput = document.createElement("input");
              typeEleInput.setAttribute("type", "checkbox");
              typeEleInput.setAttribute(
                "value",
                JSON.stringify({
                  accountName: element.accountName,
                  accountId: element.accountId,
                  path: element.path,
                  name: element.name,
                })
              );
              if (element.subFolders.length > 0) {
                typeEleInput.setAttribute("name", "parent-" + element.name);
              } else {
                typeEleInput.setAttribute("name", "child-" + element.name);
              }

              typeEleLi.appendChild(typeEleInput);
              const typeEleText = document.createTextNode(" " + element.name);
              typeEleLi.appendChild(typeEleText);

              if (element.subFolders.length > 0) {
                typeEleLi.appendChild(
                  createListLevel(element.subFolders, element)
                );
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

      //  Setup checkbox control
      let checkboxes = treeBase.querySelectorAll('input[type="checkbox"]');

      for (let x = 0; x < checkboxes.length; x++) {
        checkboxes[x].addEventListener("change", function (e) {
          let parentNode = this.parentNode;

          const cbDescendants = parentNode.querySelectorAll(
            'input[type="checkbox"]'
          );
          for (let y = 0; y < cbDescendants.length; y++) {
            cbDescendants[y].checked = this.checked;
            cbDescendants[y].indeterminate = false;
          }

          while (["ul", "li"].indexOf(parentNode.nodeName.toLowerCase()) >= 0) {
            const mainCb = parentNode.querySelector(
              ':scope > input[type="checkbox"]'
            );

            if (mainCb && mainCb != this) {
              mainCb.checked = this.checked;

              const mainCbChildren = mainCb.parentNode.querySelectorAll(
                'input[type="checkbox"]'
              );
              const numTotal = mainCbChildren.length;

              let numChecked = 0;
              for (let z = 0; z < mainCbChildren.length; z++) {
                numChecked += mainCbChildren[z].checked;
              }

              if (numTotal === numChecked) {
                mainCb.indeterminate = false;
                mainCb.checked = true;
              } else {
                if (numChecked === 0) {
                  mainCb.indeterminate = false;
                  mainCb.checked = false;
                } else {
                  mainCb.indeterminate = true;
                  mainCb.checked = false;
                }
              }
            }

            parentNode = parentNode.parentNode;
          }
        });
      }
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
