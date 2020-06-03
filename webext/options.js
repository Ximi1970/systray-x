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
    const accounts = treeBase.querySelectorAll(
      'input[type="checkbox"][name*="account"]'
    );

    let checkedFolders = [];
    accounts.forEach((account) => {
      if (account.checked || account.indeterminate) {
        //  Find all selected folders
        const folders = Array.from(
          account.parentNode.querySelectorAll(
            'input[type="checkbox"]:not([name^="account"]):not([name^="parent-"])'
          )
        ).filter((folder) => folder.checked);

        checkedFolders = checkedFolders.concat(folders);
      }
    });

    let filters = [];
    checkedFolders.forEach((folder) => {
      const mailFolderExt = JSON.parse(folder.value);

      filters.push({
        unread: true,
        folder: mailFolderExt,
      });
    });

    //  Store extended query filters
    browser.storage.sync.set({
      filters: filters,
    });

    //
    //  Save debug state
    //
    const debug = document.querySelector('input[name="debug"]').checked;
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
    const startMinimized = document.querySelector(
      'input[name="startMinimized"]'
    ).checked;
    browser.storage.sync.set({
      startMinimized: `${startMinimized}`,
    });

    //
    //  Save minimize on close state
    //
    let minimizeOnClose = document.querySelector(
      'input[name="minimizeOnClose"]'
    ).checked;
    browser.storage.sync.set({
      minimizeOnClose: `${minimizeOnClose}`,
    });

    //
    // Save default icon preferences
    //
    const defaultIconType = document.querySelector(
      'input[name="defaultIconType"]:checked'
    ).value;

    //  Store default icon type
    browser.storage.sync.set({
      defaultIconType: defaultIconType,
    });

    const defaultIconDiv = document.getElementById("defaultIcon");
    const defaultIconBase64 = defaultIconDiv.getAttribute("data-default-icon");
    const defaultIconMime = defaultIconDiv.getAttribute(
      "data-default-icon-mime"
    );

    //  Store default icon (base64)
    browser.storage.sync.set({
      defaultIconMime: defaultIconMime,
      defaultIcon: defaultIconBase64,
    });

    //
    //  Save hide default icon state
    //
    let hideDefaultIcon = document.querySelector(
      'input[name="hideDefaultIcon"]'
    ).checked;
    browser.storage.sync.set({
      hideDefaultIcon: `${hideDefaultIcon}`,
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

    const iconDiv = document.getElementById("icon");
    const iconBase64 = iconDiv.getAttribute("data-icon");
    const iconMime = iconDiv.getAttribute("data-icon-mime");

    //  Store icon (base64)
    browser.storage.sync.set({
      iconMime: iconMime,
      icon: iconBase64,
    });

    //
    //  Save enable number state
    //
    const showNumber = document.querySelector('input[name="showNumber"]')
      .checked;
    browser.storage.sync.set({
      showNumber: `${showNumber}`,
    });

    //
    //  Save number color
    //
    const numberColor = document.querySelector('input[name="numberColor"]')
      .value;
    browser.storage.sync.set({
      numberColor: `${numberColor}`,
    });

    //
    //  Save number size
    //
    const numberSize = document.querySelector('input[name="numberSize"]').value;
    browser.storage.sync.set({
      numberSize: numberSize,
    });

    //
    // Save count type preferences
    //
    const countType = document.querySelector('input[name="countType"]:checked')
      .value;
    browser.storage.sync.set({
      countType: countType,
    });

    //  Mark add-on preferences changed
    browser.storage.sync.set({
      addonprefchanged: true,
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
    //  Restore minimize on close
    //
    const getMinimizeOnClose = browser.storage.sync.get("minimizeOnClose");
    getMinimizeOnClose.then(
      SysTrayX.RestoreOptions.setMinimizeOnClose,
      SysTrayX.RestoreOptions.onMinimizeOnCloseError
    );

    //
    //  Restore default icon type
    //
    const getDefaultIconType = browser.storage.sync.get("defaultIconType");
    getDefaultIconType.then(
      SysTrayX.RestoreOptions.setDefaultIconType,
      SysTrayX.RestoreOptions.onDefaultIconTypeError
    );

    //
    //  Restore default icon
    //
    const getDefaultIcon = browser.storage.sync.get([
      "defaultIconMime",
      "defaultIcon",
    ]);
    getDefaultIcon.then(
      SysTrayX.RestoreOptions.setDefaultIcon,
      SysTrayX.RestoreOptions.onDefaultIconError
    );

    //
    //  Restore hide default icon
    //
    const getHideDefaultIcon = browser.storage.sync.get("hideDefaultIcon");
    getHideDefaultIcon.then(
      SysTrayX.RestoreOptions.setHideDefaultIcon,
      SysTrayX.RestoreOptions.onHideDefaultIconError
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
    //  Restore filters
    //
    const getFilters = browser.storage.sync.get("filters");
    getFilters.then(
      SysTrayX.RestoreOptions.setFilters,
      SysTrayX.RestoreOptions.onFiltersError
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

    //
    //  Restore number size
    //
    const getNumberSize = browser.storage.sync.get("numberSize");
    getNumberSize.then(
      SysTrayX.RestoreOptions.setNumberSize,
      SysTrayX.RestoreOptions.onNumberSizeError
    );

    //
    //  Restore count type
    //
    const getCountType = browser.storage.sync.get("countType");
    getCountType.then(
      SysTrayX.RestoreOptions.setCountType,
      SysTrayX.RestoreOptions.onCountTypeError
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
  //  Restore minimize on close callbacks
  //
  setMinimizeOnClose: function (result) {
    const minimizeOnClose = result.minimizeOnClose || "true";

    const checkbox = document.querySelector(`input[name="minimizeOnClose"]`);
    checkbox.checked = minimizeOnClose === "true";
  },

  onMinimizeOnCloseError: function (error) {
    console.log(`minimizeOnClose Error: ${error}`);
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
  //  Restore default icon type callbacks
  //
  setDefaultIconType: function (result) {
    const defaultIconType = result.defaultIconType || "0";
    const radioButton = document.querySelector(
      `input[name="defaultIconType"][value="${defaultIconType}"]`
    );
    radioButton.checked = true;
  },

  onDefaultIconTypeError: function (error) {
    console.log(`Default icon type Error: ${error}`);
  },

  //
  //  Restore default icon
  //
  setDefaultIconMime: function (result) {
    const defaultIconMime = result.defaultIconMime || "";

    const valid = defaultIconMime !== "";
    if (valid) {
      const defaultIconDiv = document.getElementById("defaultIcon");
      defaultIconDiv.setAttribute("data-default-icon-mime", defaultIconMime);
    }

    return valid;
  },

  setDefaultIconData: function (result) {
    const defaultIconBase64 = result.defaultIcon || "";

    const valid = defaultIconBase64 !== "";
    if (valid) {
      const defaultIconDiv = document.getElementById("defaultIcon");
      defaultIconDiv.setAttribute("data-default-icon", defaultIconBase64);
    }

    return valid;
  },

  updateDefaultIconImage: function () {
    const defaultIconDiv = document.getElementById("defaultIcon");
    default_icon_mime = defaultIconDiv.getAttribute("data-default-icon-mime");
    default_icon_data = defaultIconDiv.getAttribute("data-default-icon");

    const image = document.getElementById("defaultCustomIconImage");
    image.setAttribute(
      "src",
      `data:${default_icon_mime};base64,${default_icon_data}`
    );
  },

  setDefaultIcon: function (result) {
    const validMime = SysTrayX.RestoreOptions.setDefaultIconMime(result);
    const validData = SysTrayX.RestoreOptions.setDefaultIconData(result);

    if (validMime && validData) {
      SysTrayX.RestoreOptions.updateDefaultIconImage();
    }
  },

  onDefaultIconError: function (error) {
    console.log(`Default icon Error: ${error}`);
  },

  //
  //  Restore hide default icon callbacks
  //
  setHideDefaultIcon: function (result) {
    const hideDefaultIcon = result.hideDefaultIcon || "true";

    const checkbox = document.querySelector(`input[name="hideDefaultIcon"]`);
    checkbox.checked = hideDefaultIcon === "true";
  },

  onHideDefaultIconError: function (error) {
    console.log(`hideDefaultIcon Error: ${error}`);
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
  //  Restore number size
  //
  setNumberSize: function (result) {
    const numberSize = result.numberSize || 10;

    const input = document.querySelector(`input[name="numberSize"]`);
    input.value = numberSize;
  },

  onNumberSizeError: function (error) {
    console.log(`numberSize Error: ${error}`);
  },

  //
  //  Restore count type
  //
  setCountType: function (result) {
    const countType = result.countType || "0";

    const radioButton = document.querySelector(
      `input[name="countType"][value="${countType}"]`
    );
    radioButton.checked = true;
  },

  onCountTypeError: function (error) {
    console.log(`countType Error: ${error}`);
  },

  //
  //  Restore filters callbacks
  //
  setFilters: function (result) {
    let filters = result.filters || undefined;

    //  No filters stored?
    if (filters == undefined) {
      //  Create default filters

      const treeBase = document.getElementById("accountsTree");
      const accountsBoxes = treeBase.querySelectorAll(
        'input[type="checkbox"][name*="account"]'
      );

      let accounts = [];
      accountsBoxes.forEach((acoountbox) => {
        const value = JSON.parse(acoountbox.value);
        accounts.push({ folders: value.folders });
      });

      filters = [];
      accounts.forEach((account) => {
        const inbox = account.folders.filter(
          (folder) => folder.type == "inbox"
        );

        if (inbox.length > 0) {
          let folder = {
            ...inbox[0],
            accountName: account.name,
            path: "/" + inbox[0].name,
          };
          delete folder.type;
          delete folder.subFolders;

          filters.push({
            unread: true,
            folder: folder,
          });
        }
      });
    }

    if (filters) {
      const treeBase = document.getElementById("accountsTree");

      filters.forEach((filter) => {
        const folder = filter.folder;

        const account = treeBase.querySelector(
          `input[name=${folder.accountId}]`
        );
        const checkboxes = Array.from(
          account.parentNode.querySelectorAll(
            'input[type="checkbox"]:not([name^="account"]):not([name^="parent-"])'
          )
        );

        checkboxes.forEach((checkbox) => {
          const value = JSON.parse(checkbox.value);
          if (value.path === folder.path) {
            checkbox.checked = true;

            const event = document.createEvent("HTMLEvents");
            event.initEvent("change", false, true);
            checkbox.dispatchEvent(event);
          }
        });
      });
    }
  },

  onFiltersError: function (error) {
    console.log(`Filters Error: ${error}`);
  },
};

SysTrayX.StorageChanged = {
  changed: function (changes, area) {
    //  Try to keep the preferences of the add-on and the app in sync
    const changedItems = Object.keys(changes);

    let changed_icon = false;
    let changed_default_icon = false;
    for (let item of changedItems) {
      if (item === "iconMime") {
        SysTrayX.RestoreOptions.setIconMime({
          iconMime: changes[item].newValue,
        });
        changed_icon = true;
      }
      if (item === "icon") {
        SysTrayX.RestoreOptions.setIcon({ icon: changes[item].newValue });
        changed_icon = true;
      }
      if (item === "iconType") {
        SysTrayX.RestoreOptions.setIconType({
          iconType: changes[item].newValue,
        });
      }
      if (item === "defaultIconMime") {
        SysTrayX.RestoreOptions.setDefaultIconMime({
          defaultIconMime: changes[item].newValue,
        });
        changed_default_icon = true;
      }
      if (item === "defaultIcon") {
        SysTrayX.RestoreOptions.setDefaultIcon({
          defaultIcon: changes[item].newValue,
        });
        changed_default_icon = true;
      }
      if (item === "defaultIconType") {
        SysTrayX.RestoreOptions.setDefaultIconType({
          defaultIconType: changes[item].newValue,
        });
      }
      if (item === "hideDefaultIcon") {
        SysTrayX.RestoreOptions.setHideDefaultIcon({
          hideDefaultIcon: changes[item].newValue,
        });
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
      if (item === "numberSize") {
        SysTrayX.RestoreOptions.setNumberSize({
          numberSize: changes[item].newValue,
        });
      }
      if (item === "countType") {
        SysTrayX.RestoreOptions.setCountType({
          countType: changes[item].newValue,
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
      if (item === "minimizeOnClose") {
        SysTrayX.RestoreOptions.setMinimizeOnClose({
          minimizeOnClose: changes[item].newValue,
        });
      }
      if (item === "debug") {
        SysTrayX.RestoreOptions.setDebug({
          debug: changes[item].newValue,
        });
      }
    }

    if (changed_icon) {
      SysTrayX.RestoreOptions.updateIconImage();
    }

    if (changed_default_icon) {
      SysTrayX.RestoreOptions.updateDefaultIconImage();
    }

    //
    //  Update element
    //
    document.getElementById("debugselect").className = "active";
    document.getElementById("defaulticonselect").className = "active";
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
