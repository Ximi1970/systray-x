function saveOptions(e) {
  e.preventDefault();

  console.debug("Save preferences");

  browser.storage.sync.set({
    optionsRadioTest: document.querySelector(
      'input[name="options_test"]:checked'
    ).value,
    optionsCheck1: document.querySelector('input[name="check1"]').checked,
    optionsCheck2: document.querySelector('input[name="check2"]').checked,
    optionsCheck3: document.querySelector('input[name="check3"]').checked
  });

  /*
   * Save accounts and filters
   */

  console.debug("Store accounts and filters");

  let treeBase = document.getElementById("accountsTree");
  let inputs = treeBase.querySelectorAll("input");
  let accounts = [];
  let filters = [];
  for (let i = 0; i < inputs.length; ++i) {
    let account = JSON.parse(inputs[i].value);
    let checked = inputs[i].checked;
    accounts.push({ ...account, checked: checked });

    if (checked) {
      let inboxMailFolder = account.folders.find(obj => obj.type === "inbox");

      if (inboxMailFolder) {
        console.debug("Filter Id: " + inboxMailFolder.accountId);
        console.debug("Filter Path: " + inboxMailFolder.path);

        filters.push({
          unread: true,
          folder: inboxMailFolder
        });
      }
    }
  }

  //  Store accounts
  browser.storage.sync.set({
    accounts: accounts
  });

  //  Store query filters
  browser.storage.sync.set({
    filters: filters
  });

  console.debug("Store accounts and filters done");
}

function restoreOptions() {
  console.debug("Restore preferences");

  function setCurrentRadioChoice(result) {
    let selector = result.optionsRadioTest || "Option1";
    let radioButton = document.querySelector(`[value=${selector}]`);
    radioButton.checked = true;
  }

  function setCurrentCheckChoice(result) {
    let checkbox1 = document.querySelector('[name="check1"]');
    checkbox1.checked = result.optionsCheck1 || false;
    let checkbox2 = document.querySelector('[name="check2"]');
    checkbox2.checked = result.optionsCheck2 || false;
    let checkbox3 = document.querySelector('[name="check3"]');
    checkbox3.checked = result.optionsCheck3 || false;
  }

  function onError(error) {
    console.log(`Error: ${error}`);
  }

  var getting = browser.storage.sync.get("optionsRadioTest");
  getting.then(setCurrentRadioChoice, onError);

  var getting = browser.storage.sync.get([
    "optionsCheck1",
    "optionsCheck2",
    "optionsCheck3"
  ]);
  getting.then(setCurrentCheckChoice, onError);
}

document.addEventListener("DOMContentLoaded", restoreOptions);
document
  .querySelector('[name="saveform"]')
  .addEventListener("submit", saveOptions);