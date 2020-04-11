var SysTrayX = {
  platformInfo: undefined,

  version: 0,
};

SysTrayX.SaveOptions = {
  start: function (e) {
    e.preventDefault();

    //
    // Save accounts and filters
    //
    const treeBase = document.getElementById("accountsTree");
    const inputs = treeBase.querySelectorAll("input");
    let accounts = [];
    let filters = [];
    for (let i = 0; i < inputs.length; ++i) {
      const account = JSON.parse(inputs[i].value);
      const checked = inputs[i].checked;
      accounts.push({ ...account, checked: checked });

      if (checked) {
        let inboxMailFolder = account.folders.find(
          (obj) => obj.type === "inbox"
        );

        if (inboxMailFolder) {
          filters.push({
            unread: true,
            folder: inboxMailFolder,
          });
        }
      }
    }

    //  Store accounts
    browser.storage.sync.set({
      accounts: accounts,
    });

    //  Store query filters
    browser.storage.sync.set({
      filters: filters,
    });

    //
    //  Save poll startup delay state
    //
    const pollStartupDelay = document.querySelector(
      'input[name="pollStartupDelay"]'
    ).value;
    browser.storage.sync.set({
      pollStartupDelay: pollStartupDelay,
    });

    //
    //  Save poll interval state
    //
    const pollInterval = document.querySelector('input[name="pollInterval"]')
      .value;
    browser.storage.sync.set({
      pollInterval: pollInterval,
    });

    //
    //  Save debug state
    //
    let debug = document.querySelector('input[name="debug"]').checked;
    browser.storage.sync.set({
      debug: `${debug}`,
    });

    //
    // Save minimize preferences
    //
    const minimizeType = document.querySelector(
      'input[name="minimizeType"]:checked'
    ).value;

    //  Store minimize preferences
    browser.storage.sync.set({
      minimizeType: minimizeType,
    });

    //
    //  Save start minimized state
    //
    let startMinimized = document.querySelector('input[name="startMinimized"]')
      .checked;
    browser.storage.sync.set({
      startMinimized: `${startMinimized}`,
    });

    //
    // Save icon preferences
    //
    const iconType = document.querySelector('input[name="iconType"]:checked')
      .value;

    //  Store icon type
    browser.storage.sync.set({
      iconType: iconType,
    });

    let iconDiv = document.getElementById("icon");
    let iconBase64 = iconDiv.getAttribute("data-icon");
    let iconMime = iconDiv.getAttribute("data-icon-mime");

    //  Store icon (base64)
    browser.storage.sync.set({
      iconMime: iconMime,
      icon: iconBase64,
    });

    //  Mark add-on preferences changed
    browser.storage.sync.set({
      addonprefchanged: true,
    });

    //
    //  Save enable number state
    //
    let showNumber = document.querySelector('input[name="showNumber"]').checked;
    browser.storage.sync.set({
      showNumber: `${showNumber}`,
    });

    //
    //  Save number color
    //
    let numberColor = document.querySelector('input[name="numberColor"]').value;
    browser.storage.sync.set({
      numberColor: `${numberColor}`,
    });
  },
};

SysTrayX.RestoreOptions = {
  start: function () {
    //
    //  Restore debug state
    //
    const getDebug = browser.storage.sync.get("debug");
    getDebug.then(
      SysTrayX.RestoreOptions.setDebug,
      SysTrayX.RestoreOptions.onDebugError
    );

    //
    //  Restore minimize type
    //
    const getMinimizeType = browser.storage.sync.get([
      "platformInfo",
      "minimizeType",
    ]);
    getMinimizeType.then(
      SysTrayX.RestoreOptions.setMinimizeType,
      SysTrayX.RestoreOptions.onMinimizeTypeError
    );

    //
    //  Restore start minimized
    //
    const getStartMinimized = browser.storage.sync.get("startMinimized");
    getStartMinimized.then(
      SysTrayX.RestoreOptions.setStartMinimized,
      SysTrayX.RestoreOptions.onStartMinimizedError
    );

    //
    //  Restore icon type
    //
    const getIconType = browser.storage.sync.get("iconType");
    getIconType.then(
      SysTrayX.RestoreOptions.setIconType,
      SysTrayX.RestoreOptions.onIconTypeError
    );

    //
    //  Restore icon
    //
    const getIcon = browser.storage.sync.get(["iconMime", "icon"]);
    getIcon.then(
      SysTrayX.RestoreOptions.setIcon,
      SysTrayX.RestoreOptions.onIconError
    );

    //
    //  Restore poll startup delay state
    //
    const getPollStartupDelay = browser.storage.sync.get("pollStartupDelay");
    getPollStartupDelay.then(
      SysTrayX.RestoreOptions.setPollStartupDelay,
      SysTrayX.RestoreOptions.onPollStartupDelayError
    );

    //
    //  Restore poll interval state
    //
    const getPollInterval = browser.storage.sync.get("pollInterval");
    getPollInterval.then(
      SysTrayX.RestoreOptions.setPollInterval,
      SysTrayX.RestoreOptions.onPollIntervalError
    );

    //
    //  Restore enable number state
    //
    const getShowNumber = browser.storage.sync.get("showNumber");
    getShowNumber.then(
      SysTrayX.RestoreOptions.setShowNumber,
      SysTrayX.RestoreOptions.onShowNumberError
    );

    //
    //  Restore number color
    //
    const getNumberColor = browser.storage.sync.get("numberColor");
    getNumberColor.then(
      SysTrayX.RestoreOptions.setNumberColor,
      SysTrayX.RestoreOptions.onNumberColorError
    );
  },

  //
  //  Restore debug state callbacks
  //
  setDebug: function (result) {
    const debug = result.debug || "false";

    const checkbox = document.querySelector(`input[name="debug"]`);
    checkbox.checked = debug === "true";
  },

  onDebugError: function (error) {
    console.log(`Debug Error: ${error}`);
  },

  //
  //  Restore minimize type callbacks
  //
  setMinimizeType: function (result) {
    const platformInfo = result.platformInfo || { os: "linux" };
    const minimizeType = result.minimizeType || "1";

    // Tweak option for platform
    if (platformInfo.os === "win") {
      document.getElementById("minimizemethod1label").innerHTML =
        "Minimize to tray";
      document
        .getElementById("minimizemethod2")
        .setAttribute("style", "display:none;");

      if (minimizeType === "2") {
        minimizeType = "1";
      }
    }

    const radioButton = document.querySelector(
      `input[name="minimizeType"][value="${minimizeType}"]`
    );
    radioButton.checked = true;
  },

  onMinimizeTypeError: function (error) {
    console.log(`Minimize type Error: ${error}`);
  },

  //
  //  Restore hide on minimize callbacks
  //
  setStartMinimized: function (result) {
    const startMinimized = result.startMinimized || "false";

    const checkbox = document.querySelector(`input[name="startMinimized"]`);
    checkbox.checked = startMinimized === "true";
  },

  onStartMinimizedError: function (error) {
    console.log(`startMinimized Error: ${error}`);
  },

  //
  //  Restore icon type callbacks
  //
  setIconType: function (result) {
    const iconType = result.iconType || "0";
    const radioButton = document.querySelector(
      `input[name="iconType"][value="${iconType}"]`
    );
    radioButton.checked = true;
  },

  onIconTypeError: function (error) {
    console.log(`Icon type Error: ${error}`);
  },

  //
  //  Restore icon
  //
  setIconMime: function (result) {
    const iconMime = result.iconMime || "";

    const valid = iconMime !== "";
    if (valid) {
      const iconDiv = document.getElementById("icon");
      iconDiv.setAttribute("data-icon-mime", iconMime);
    }

    return valid;
  },

  setIconData: function (result) {
    const iconBase64 = result.icon || "";

    const valid = iconBase64 !== "";
    if (valid) {
      const iconDiv = document.getElementById("icon");
      iconDiv.setAttribute("data-icon", iconBase64);
    }

    return valid;
  },

  updateIconImage: function () {
    const iconDiv = document.getElementById("icon");
    icon_mime = iconDiv.getAttribute("data-icon-mime");
    icon_data = iconDiv.getAttribute("data-icon");

    const image = document.getElementById("customIconImage");
    image.setAttribute("src", `data:${icon_mime};base64,${icon_data}`);
  },

  setIcon: function (result) {
    const validMime = SysTrayX.RestoreOptions.setIconMime(result);
    const validData = SysTrayX.RestoreOptions.setIconData(result);

    if (validMime && validData) {
      SysTrayX.RestoreOptions.updateIconImage();
    }
  },

  onIconError: function (error) {
    console.log(`Icon Error: ${error}`);
  },

  //
  //  Restore enable number state
  //
  setShowNumber: function (result) {
    const showNumber = result.showNumber || "true";

    const checkbox = document.querySelector(`input[name="showNumber"]`);
    checkbox.checked = showNumber === "true";
  },

  onShowNumberError: function (error) {
    console.log(`showNumber Error: ${error}`);
  },

  //
  //  Restore number color
  //
  setNumberColor: function (result) {
    const numberColor = result.numberColor || "#000000";

    const input = document.querySelector(`input[name="numberColor"]`);
    input.value = numberColor;
  },

  onNumberColorError: function (error) {
    console.log(`numberColor Error: ${error}`);
  },

  //
  //  Restore poll startup delay state callbacks
  //
  setPollStartupDelay: function (result) {
    const pollStartupDelay = result.pollStartupDelay || 5;

    const input = document.querySelector(`input[name="pollStartupDelay"]`);
    input.value = pollStartupDelay;
  },

  onPollStartupDelayError: function (error) {
    console.log(`Poll startup delay Error: ${error}`);
  },

  //
  //  Restore poll interval state callbacks
  //
  setPollInterval: function (result) {
    const pollInterval = result.pollInterval || 5;

    const input = document.querySelector(`input[name="pollInterval"]`);
    input.value = pollInterval;
  },

  onPollPollInterval: function (error) {
    console.log(`Poll interval Error: ${error}`);
  },
};

SysTrayX.StorageChanged = {
  changed: function (changes, area) {
    //  Try to keep the preferences of the add-on and the app in sync
    const changedItems = Object.keys(changes);

    let changed_icon = false;
    let changed_icon_mime = false;
    for (let item of changedItems) {
      if (item === "iconMime") {
        SysTrayX.RestoreOptions.setIconMime({
          iconMime: changes[item].newValue,
        });
      }
      if (item === "icon") {
        SysTrayX.RestoreOptions.setIcon({ icon: changes[item].newValue });
        changed_icon = true;
      }
      if (item === "iconType") {
        SysTrayX.RestoreOptions.setIconType({
          iconType: changes[item].newValue,
        });
        changed_icon_mime = true;
      }
      if (item === "showNumber") {
        SysTrayX.RestoreOptions.setShowNumber({
          showNumber: changes[item].newValue,
        });
      }
      if (item === "numberColor") {
        SysTrayX.RestoreOptions.setNumberColor({
          numberColor: changes[item].newValue,
        });
      }
      if (item === "minimizeType") {
        SysTrayX.RestoreOptions.setMinimizeType({
          minimizeType: changes[item].newValue,
        });
      }
      if (item === "startMinimized") {
        SysTrayX.RestoreOptions.setStartMinimized({
          startMinimized: changes[item].newValue,
        });
      }
      if (item === "pollStartupDelay") {
        SysTrayX.RestoreOptions.setPollStartupDelay({
          pollStartupDelay: changes[item].newValue,
        });
      }
      if (item === "pollInterval") {
        SysTrayX.RestoreOptions.setPollInterval({
          pollInterval: changes[item].newValue,
        });
      }
      if (item === "debug") {
        SysTrayX.RestoreOptions.setDebug({
          debug: changes[item].newValue,
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
    document.getElementById("minimizeselect").className = "active";
  },
};

//
//  Main
//

//  Get addon version
SysTrayX.version = browser.runtime.getManifest().version;
document.getElementById("VersioHomeLink").href =
  "https://github.com/Ximi1970/systray-x/releases/tag/" + SysTrayX.version;

document.addEventListener("DOMContentLoaded", SysTrayX.RestoreOptions.start);
document
  .querySelector('[name="saveform"]')
  .addEventListener("submit", SysTrayX.SaveOptions.start);

browser.storage.onChanged.addListener(SysTrayX.StorageChanged.changed);
