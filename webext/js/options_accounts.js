SysTrayX.Accounts = {
  init: async function () {
    const getAccountsPromise = () =>
      new Promise((res) => res(SysTrayX.Accounts.getAccounts()));
    const accounts = await getAccountsPromise();

    SysTrayX.Accounts.buildTree(accounts);
  },

  getAccounts: async function () {
    if (SysTrayX.Info.browserInfo.majorVersion < 121) {
      let accounts;

      accounts = await browser.accounts.list(false);

      // Fill the sub folders using the folders API, they are not same...
      for (let i = 0; i < accounts.length; ++i) {
        const subFolders = await browser.folders.getSubFolders(
          accounts[i],
          true
        );
        accounts[i].folders = subFolders;
      }

      return accounts;
    } else {
      let accounts = await browser.accounts.list();

      for (let i = 0; i < accounts.length; ++i) {
        const subFolders = await browser.folders.getSubFolders(
          accounts[i].id,
          true
        );
        accounts[i].folders = subFolders;
      }

      return accounts;
    }
  },

  buildTree: function (mailAccount) {
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

          if (prop === "imap" || prop === "pop3") {
            const typeDiv = document.createElement("div");
            typeDiv.setAttribute("id", "accountContainer");

            const typeInputAccount = document.createElement("input");
            typeInputAccount.setAttribute("type", "checkbox");
            typeInputAccount.setAttribute("id", "account");
            typeInputAccount.setAttribute("name", accounts[prop][i].name);

            typeDiv.appendChild(typeInputAccount);

            const typeButton = document.createElement("button");
            typeButton.setAttribute("id", "accountMenu");
            typeButton.setAttribute("name", accounts[prop][i].name);
            typeButton.innerHTML = " &#9776;";

            // Get the divs
            const divAccountsFolders = document.getElementById("accountsFolders");
            const divAccountMenuDialog = document.getElementById("accountMenuDialog");

            // Get the elements
            const accountMenuRepInput = document.getElementById("accountMenuReplyToInput");
            const accountMenuToInput = document.getElementById("accountMenuToInput");
            const accountMenuCcInput = document.getElementById("accountMenuCcInput");
            const accountMenuBccInput = document.getElementById("accountMenuBccInput");
            const accountMenuSubInput = document.getElementById("accountMenuSubjectInput");

            const accountMenuBodyTextArea = document.getElementById("accountMenuBodyTextArea");
            
            const accountMenuPgpKeyCheckbox = document.getElementById("accountMenuPgpKeyChk");
            const accountMenuVCardCheckbox = document.getElementById("accountMenuVCardChk");
            const accountMenuStatNotCheckbox = document.getElementById("accountMenuStatNotChk");
            const accountMenuRetRecCheckbox = document.getElementById("accountMenuReturnReceiptChk");

            const handleClickEvent = (e) => {
              e.preventDefault();

              SysTrayX.Settings.activeFrom = e.target.name;

              // Restore saved data
              const defaults = SysTrayX.Settings.newMessageDefaults[SysTrayX.Settings.activeFrom];

              console.debug("Defaults restore: " + JSON.stringify(defaults));

              if (defaults !== undefined)
              {
                accountMenuRepInput.value = defaults.replyTo ?? "";
                accountMenuToInput.value = defaults.to ?? "";
                accountMenuCcInput.value = defaults.cc ?? "";
                accountMenuBccInput.value = defaults.bcc ?? "";
                accountMenuSubInput.value = defaults.subject ?? "";

                accountMenuBodyTextArea.value = defaults.body ?? "";

                accountMenuPgpKeyCheckbox.checked = defaults.pgpKey ?? false;
                accountMenuVCardCheckbox.checked = defaults.vCard ?? false;
                accountMenuStatNotCheckbox.checked = defaults.statNot ?? false;
                accountMenuRetRecCheckbox.checked = defaults.retRec ?? false;
              } else {
                accountMenuRepInput.value = "";
                accountMenuToInput.value = "";
                accountMenuCcInput.value = "";
                accountMenuBccInput.value = "";
                accountMenuSubInput.value = "";

                accountMenuBodyTextArea.value = "";

                accountMenuPgpKeyCheckbox.checked = false;
                accountMenuVCardCheckbox.checked = false;
                accountMenuStatNotCheckbox.checked = false;
                accountMenuRetRecCheckbox.checked = false;
              }

              // Construct the dialog

              // Not working...
              const accountMenuPgpKey = document.getElementById("accountmenupgpkey");
              accountMenuPgpKey.setAttribute("style", "display: none");
              const accountMenuVCard = document.getElementById("accountmenuvcard");
              accountMenuVCard.setAttribute("style", "display: none");

              // TB version depending
              if (SysTrayX.Info.browserInfo.majorVersion < 102) {
                const accountMenuVCard = document.getElementById("accountmenuvcard");
                accountMenuVCard.setAttribute("style", "display: none");
                const accountMenuStatNot = document.getElementById("accountmenustatnot");
                accountMenuStatNot.setAttribute("style", "display: none");
                const accountMenuRetRec = document.getElementById("accountmenureturnreceipt");
                accountMenuRetRec.setAttribute("style", "display: none");
              }

              const divAccountMenuId = document.getElementById("accountMenuId");
              divAccountMenuId.innerHTML = e.target.name;

              divAccountsFolders.setAttribute("style", "display: none");
              divAccountMenuDialog.removeAttribute("style");
            };

            typeButton.onclick = handleClickEvent;
            typeDiv.appendChild(typeButton);

            typeLi.appendChild(typeDiv);

            // Setup the buttons of the account menu dialog
            const handleClickBackEvent = (e) => {
              e.preventDefault();

              divAccountsFolders.removeAttribute("style");
              divAccountMenuDialog.setAttribute("style", "display: none");

              SysTrayX.Settings.activeFrom = undefined;
            }

            const accountMenuBack = document.getElementById("accountMenuBack");
            accountMenuBack.onclick = handleClickBackEvent;

            const handleClickClearEvent = (e) => {
              e.preventDefault();

              accountMenuRepInput.value = "";
              accountMenuToInput.value = "";
              accountMenuCcInput.value = "";
              accountMenuBccInput.value = "";
              accountMenuSubInput.value = "";
  
              accountMenuBodyTextArea.value = "";

              accountMenuPgpKeyCheckbox.checked = false;
              accountMenuVCardCheckbox.checked = false;
              accountMenuStatNotCheckbox.checked = false;
              accountMenuRetRecCheckbox.checked = false;
            }

            const accountMenuClear = document.getElementById("accountMenuClear");
            accountMenuClear.onclick = handleClickClearEvent;

            const handleClickAcceptEvent = (e) => {
              e.preventDefault();

              // Save data
              SysTrayX.Settings.newMessageDefaults[SysTrayX.Settings.activeFrom] = {
                replyTo: accountMenuRepInput.value,
                to: accountMenuToInput.value,
                cc: accountMenuCcInput.value,
                bcc: accountMenuBccInput.value,
                subject: accountMenuSubInput.value,
                
                body: accountMenuBodyTextArea.value,

                pgpKey: accountMenuPgpKeyCheckbox.checked,
                vCard: accountMenuVCardCheckbox.checked,
                statNot: accountMenuStatNotCheckbox.checked,
                retRec: accountMenuRetRecCheckbox.checked
              };

              // Return to main view
              divAccountsFolders.removeAttribute("style");
              divAccountMenuDialog.setAttribute("style", "display: none");

              SysTrayX.Settings.activeFrom = undefined;              
            }

            const accountMenuAccept = document.getElementById("accountMenuAccept");
            accountMenuAccept.onclick = handleClickAcceptEvent;
          }

          //  Create a usable folder tree
          let folders = [];

          folders = createFolderTree(
            accounts[prop][i].name,
            accounts[prop][i].folders
          );

          //  Recursive list creator
          function createListLevel(level, parent) {
            const typeLevelUl = document.createElement("ul");
            typeLevelUl.setAttribute("class", "nested");

            if (parent) {
              parent.subFolders = [];
              parent.originalName = parent.name;
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
                  mailFolderId: element.id,
                  type: element.type !== undefined ? element.type : "",
                  path: element.path,
                  name: element.originalName
                      ? element.originalName
                      : element.name,
                  version: SysTrayX.Info.version,
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
      let checkboxes = treeBase.querySelectorAll('input[type="checkbox"]:not([id="account"])');

      for (let x = 0; x < checkboxes.length; x++) {
        checkboxes[x].addEventListener("change", function (e) {
          let parentNode = this.parentNode;

          const cbDescendants = parentNode.querySelectorAll(
            'input[type="checkbox"]:not([id="account"])'
          );
          for (let y = 0; y < cbDescendants.length; y++) {
            cbDescendants[y].checked = this.checked;
            cbDescendants[y].indeterminate = false;
          }

          while (["ul", "li"].indexOf(parentNode.nodeName.toLowerCase()) >= 0) {
            const mainCb = parentNode.querySelector(
              ':scope > input[type="checkbox"]'
            );

            if (mainCb && mainCb !== this) {
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
