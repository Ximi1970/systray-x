var SysTrayX = {};

SysTrayX.SaveOptions = {
  start: function(e) {
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

    //
    // Save accounts and filters
    //

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

    //
    //  Save debug state
    //
    let debug = document.querySelector('input[name="debug"]').checked;
    browser.storage.sync.set({
      debug: `${debug}`
    });
    console.debug("Store debug state: " + `${debug}`);

    //
    // Save icon preferences
    //

    console.debug("Store icon preferences");

    let iconType = document.querySelector('input[name="iconType"]:checked')
      .value;

    //  Store icon type
    browser.storage.sync.set({
      iconType: iconType
    });

    let iconDiv = document.getElementById("icon");
    let iconBase64 = iconDiv.getAttribute("data-icon");
    let iconMime = iconDiv.getAttribute("data-icon-mime");

    //  Store icon (base64)
    browser.storage.sync.set({
      iconMime: iconMime,
      icon: iconBase64
    });

    console.debug("Store icon preferences done");

    //  Mark add-on preferences changed
    browser.storage.sync.set({
      addonprefchanged: true
    });
  }
};

SysTrayX.RestoreOptions = {
  start: function() {
    console.debug("Restore preferences");

    //
    //  Test 1
    //
    let getting = browser.storage.sync.get("optionsRadioTest");
    getting.then(
      SysTrayX.RestoreOptions.setCurrentRadioChoice,
      SysTrayX.RestoreOptions.onError
    );

    //
    //  Test 2
    //
    getting = browser.storage.sync.get([
      "optionsCheck1",
      "optionsCheck2",
      "optionsCheck3"
    ]);
    getting.then(
      SysTrayX.RestoreOptions.setCurrentCheckChoice,
      SysTrayX.RestoreOptions.onError
    );

    console.debug("Restore icon preferences");

    //
    //  Restore debug state
    //
    let getDebug = browser.storage.sync.get("debug");
    getDebug.then(
      SysTrayX.RestoreOptions.setDebug,
      SysTrayX.RestoreOptions.onDebugError
    );

    //
    //  Restore icon type
    //
    let getIconType = browser.storage.sync.get("iconType");
    getIconType.then(
      SysTrayX.RestoreOptions.setIconType,
      SysTrayX.RestoreOptions.onIconTypeError
    );

    //
    //  Restore icon
    //
    let getIcon = browser.storage.sync.get(["iconMime", "icon"]);
    getIcon.then(
      SysTrayX.RestoreOptions.setIcon,
      SysTrayX.RestoreOptions.onIconError
    );

    console.debug("Restore icon preferences done");
  },

  //
  //  Test 1 Callback
  //
  setCurrentRadioChoice: function(result) {
    let selector = result.optionsRadioTest || "Option1";
    let radioButton = document.querySelector(`[value=${selector}]`);
    radioButton.checked = true;
  },

  //
  //  Test 2 Callback
  //
  setCurrentCheckChoice: function(result) {
    let checkbox1 = document.querySelector('[name="check1"]');
    checkbox1.checked = result.optionsCheck1 || false;
    let checkbox2 = document.querySelector('[name="check2"]');
    checkbox2.checked = result.optionsCheck2 || false;
    let checkbox3 = document.querySelector('[name="check3"]');
    checkbox3.checked = result.optionsCheck3 || false;
  },

  //
  //  Test 1+2 error callback
  //
  onError: function(error) {
    console.log(`Error: ${error}`);
  },

  //
  //  Restore debug state callbacks
  //
  setDebug: function(result) {
    let debug = result.debug || "false";

    console.debug("Debug: " + debug);

    let checkbox = document.querySelector(`input[name="debug"]`);
    checkbox.checked = (debug === "true");
  },

  onDebugError: function(error) {
    console.log(`Debug Error: ${error}`);
  },

  //
  //  Restore icon type callbacks
  //
  setIconType: function(result) {
    let iconType = result.iconType || "0";
    let radioButton = document.querySelector(`[value="${iconType}"]`);
    radioButton.checked = true;
  },

  onIconTypeError: function(error) {
    console.log(`Icon type Error: ${error}`);
  },

  //
  //  Restore icon
  //
  setIconMime: function(result) {
    let iconMime = result.iconMime || "image/png";

    let iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon-mime", iconMime);
  },

  setIconData: function(result) {
    let iconBase64 = result.icon || "";

    let iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon", iconBase64);
  },

  updateIconImage: function() {
    let iconDiv = document.getElementById("icon");
    icon_mime = iconDiv.getAttribute("data-icon-mime");
    icon_data = iconDiv.getAttribute("data-icon");

    let image = document.getElementById("customIconImage");
    image.setAttribute("src", `data:${icon_mime};base64,${icon_data}`);
  },

  setIcon: function(result) {
    SysTrayX.RestoreOptions.setIconMime(result);
    SysTrayX.RestoreOptions.setIconData(result);

    SysTrayX.RestoreOptions.updateIconImage();
  },

  onIconError: function(error) {
    console.log(`Icon Error: ${error}`);
  }
};

SysTrayX.StorageChanged = {
  changed: function(changes, area) {
    //  Try to keep the preferences of the add-on and the app in sync
    let changedItems = Object.keys(changes);

    let changed_icon = false;
    let changed_icon_mime = false;
    for (let item of changedItems) {
      if (item === "iconMime") {
        SysTrayX.RestoreOptions.setIconMime({
          iconMime: changes[item].newValue
        });
      }
      if (item === "icon") {
        SysTrayX.RestoreOptions.setIcon({ icon: changes[item].newValue });
        changed_icon = true;
      }
      if (item === "iconType") {
        SysTrayX.RestoreOptions.setIconType({
          iconType: changes[item].newValue
        });
        changed_icon_mime = true;
      }
      if (item === "debug") {
        SysTrayX.RestoreOptions.setDebug({
          debug: changes[item].newValue
        });
      }
    }

    if (changed_icon_mime && changed_icon) {
      SysTrayX.RestoreOptions.updateIconImage();
    }

    //
    //  Update element
    //
    document.getElementById("debugselect").className = "active";
    document.getElementById("iconselect").className = "active";
  }
};

document.addEventListener("DOMContentLoaded", SysTrayX.RestoreOptions.start);
document
  .querySelector('[name="saveform"]')
  .addEventListener("submit", SysTrayX.SaveOptions.start);

browser.storage.onChanged.addListener(SysTrayX.StorageChanged.changed);
