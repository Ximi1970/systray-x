//
//  Get the prefered storage
//
function storage(store) {
  if (SysTrayX.Info.browserInfo.majorVersion < 91 || store === "sync") {
    console.log("Using sync storage");
    return browser.storage.sync;
  } else {
    console.log("Using local storage");
    return browser.storage.local;
  }
}

SysTrayX.SaveOptions = {
  start: async function (e) {
    e.preventDefault();

    //
    // Save accounts and filters
    //
    const treeBase = document.getElementById("accountsTree");
    const accounts = treeBase.querySelectorAll(
      'input[type="checkbox"][name*="account"]'
    );

    let filters = [];
    if (SysTrayX.Info.browserInfo.majorVersion < 91) {
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

      checkedFolders.forEach((folder) => {
        const mailFolderExt = JSON.parse(folder.value);

        filters.push({
          unread: true,
          folder: mailFolderExt,
        });
      });
    } else {
      accounts.forEach((account) => {
        if (account.checked || account.indeterminate) {
          //  Find all selected folders
          const folders = Array.from(
            account.parentNode.querySelectorAll(
              'input[type="checkbox"]:not([name^="account"]):not([name^="parent-"])'
            )
          ).filter((folder) => folder.checked);

          if (folders.length > 0) {
            const mailFolderExt = JSON.parse(folders[0].value);
            filters.push({
              accountId: mailFolderExt.accountId,
              version: mailFolderExt.version,
              folders: [],
            });

            folders.forEach((folder) => {
              const mailFolderExt = JSON.parse(folder.value);
              filters[filters.length - 1].folders.push(mailFolderExt.path);
            });
          }
        }
      });
    }

    //  Store extended query filters

    console.debug("Saving filters: " + JSON.stringify(filters));

    await storage().set({
      filters: filters,
    });

    //
    //  Save debug state
    //
    const debug = document.querySelector('input[name="debug"]').checked;
    await storage().set({
      debug: `${debug}`,
    });

    //
    // Save minimize preferences
    //
    const minimizeType = document.querySelector(
      'input[name="minimizeType"]:checked'
    ).value;

    //  Store minimize preferences
    await storage().set({
      minimizeType: minimizeType,
    });

    //
    // Save minimize icon preferences
    //
    const minimizeIconType = document.querySelector(
      'input[name="minimizeIconType"]:checked'
    ).value;

    //  Store minimize icon preferences
    await storage().set({
      minimizeIconType: minimizeIconType,
    });

    //
    // Save close preferences
    //
    const closeType = document.querySelector(
      'input[name="closeType"]:checked'
    ).value;

    //  Store close preferences
    await storage().set({
      closeType: closeType,
    });

    //
    //  Save start minimized state
    //
    const startMinimized = document.querySelector(
      'input[name="startMinimized"]'
    ).checked;
    await storage().set({
      startMinimized: `${startMinimized}`,
    });

    //
    //  Save restore window positions state
    //
    const restorePositions = document.querySelector(
      'input[name="restorePositions"]'
    ).checked;
    await storage().set({
      restorePositions: `${restorePositions}`,
    });

    //
    // Save default icon preferences
    //
    const defaultIconType = document.querySelector(
      'input[name="defaultIconType"]:checked'
    ).value;

    //  Store default icon type
    await storage().set({
      defaultIconType: defaultIconType,
    });

    const defaultIconDiv = document.getElementById("defaultIcon");
    const defaultIconBase64 = defaultIconDiv.getAttribute("data-default-icon");
    const defaultIconMime = defaultIconDiv.getAttribute(
      "data-default-icon-mime"
    );

    //  Store default icon (base64)
    await storage().set({
      defaultIconMime: defaultIconMime,
      defaultIcon: defaultIconBase64,
    });

    //
    //  Save hide default icon state
    //
    let hideDefaultIcon = document.querySelector(
      'input[name="hideDefaultIcon"]'
    ).checked;
    await storage().set({
      hideDefaultIcon: `${hideDefaultIcon}`,
    });

    //
    // Save icon preferences
    //
    const iconType = document.querySelector(
      'input[name="iconType"]:checked'
    ).value;

    //  Store icon type
    await storage().set({
      iconType: iconType,
    });

    const iconDiv = document.getElementById("icon");
    const iconBase64 = iconDiv.getAttribute("data-icon");
    const iconMime = iconDiv.getAttribute("data-icon-mime");

    //  Store icon (base64)
    await storage().set({
      iconMime: iconMime,
      icon: iconBase64,
    });

    //
    // Save theme preferences
    //
    const theme = document.querySelector('input[name="theme"]:checked').value;

    //  Store theme preferences
    await storage().set({
      theme: theme,
    });

    //
    //  Save show number state
    //
    const showNumber = document.querySelector(
      'input[name="showNumber"]'
    ).checked;
    await storage().set({
      showNumber: `${showNumber}`,
    });

    //
    //  Save show new indicator state
    //
    const showNewIndicator = document.querySelector(
      'input[name="showNewIndicator"]'
    ).checked;
    await storage().set({
      showNewIndicator: `${showNewIndicator}`,
    });

    //
    // Save count type preferences
    //
    const countType = document.querySelector(
      'input[name="countType"]:checked'
    ).value;
    await storage().set({
      countType: countType,
    });

    //
    //  Save startup delay
    //
    const startupDelay = document.querySelector(
      'input[name="startupDelay"]'
    ).value;
    await storage().set({
      startupDelay: startupDelay,
    });

    //
    //  Save number color
    //
    let numberColor = document.querySelector('input[name="numberColor"]').value;

    //  Force different color?
    if (theme == "0" && numberColor == "#ffffff") {
      numberColor = "#000000";
    } else if (theme == "1" && numberColor == "#000000") {
      numberColor = "#ffffff";
    }

    await storage().set({
      numberColor: `${numberColor}`,
    });

    //
    //  Save number size
    //
    const numberSize = document.querySelector('input[name="numberSize"]').value;
    await storage().set({
      numberSize: numberSize,
    });

    //
    //  Save number alignment
    //
    const numberAlignment = document.querySelector(
      'select[name="numberAlignment"]'
    ).value;
    await storage().set({
      numberAlignment: numberAlignment,
    });

    //
    //  Save number margins
    //
    const numberMarginLeft = document.querySelector(
      'input[name="numberMarginLeft"]'
    ).value;
    const numberMarginTop = document.querySelector(
      'input[name="numberMarginTop"]'
    ).value;
    const numberMarginRight = document.querySelector(
      'input[name="numberMarginRight"]'
    ).value;
    const numberMarginBottom = document.querySelector(
      'input[name="numberMarginBottom"]'
    ).value;
    await storage().set({
      numberMargins: {
        left: numberMarginLeft,
        top: numberMarginTop,
        right: numberMarginRight,
        bottom: numberMarginBottom,
      },
    });

    //
    // Save new indicator type
    //
    const newIndicatorType = document.querySelector(
      'input[name="newIndicatorType"]:checked'
    ).value;

    //  Store neew indicator type
    await storage().set({
      newIndicatorType: newIndicatorType,
    });

    //
    //  Save new shade color
    //
    let newShadeColor = document.querySelector('input[name="newShadeColor"]').value;

    await storage().set({
      newShadeColor: `${newShadeColor}`,
    });

    //
    //  Save launch parameters
    //
    const startAppInput = document.getElementById("startAppInput");
    const startApp = startAppInput.value;
    const startAppArgsInput = document.getElementById("startAppArgsInput");
    const startAppArgs = startAppArgsInput.value;

    const closeAppInput = document.getElementById("closeAppInput");
    const closeApp = closeAppInput.value;
    const closeAppArgsInput = document.getElementById("closeAppArgsInput");
    const closeAppArgs = closeAppArgsInput.value;

    //  Store launch parameters
    await storage().set({
      startApp: startApp,
      startAppArgs: startAppArgs,
      closeApp: closeApp,
      closeAppArgs: closeAppArgs,
    });

    //  Mark add-on preferences changed
    await storage().set({
      addonprefchanged: true,
    });

    //    const inUse = await browser.storage.sync.getBytesInUse();
    //    console.log("Storage in use: " + inUse);
  },
};

SysTrayX.RestoreOptions = {
  start: async function () {
    //
    //  Restore debug state
    //
    await storage()
      .get("debug")
      .then(
        SysTrayX.RestoreOptions.setDebug,
        SysTrayX.RestoreOptions.onDebugError
      );

    //
    //  Restore filters
    //
    await storage()
      .get("filters")
      .then(
        SysTrayX.RestoreOptions.setFilters,
        SysTrayX.RestoreOptions.onFiltersError
      );

    //
    //  Restore minimize type
    //
    await storage()
      .get("minimizeType")
      .then(
        SysTrayX.RestoreOptions.setMinimizeType,
        SysTrayX.RestoreOptions.onMinimizeTypeError
      );

    //
    //  Restore minimize icon type
    //
    await storage()
      .get("minimizeIconType")
      .then(
        SysTrayX.RestoreOptions.setMinimizeIconType,
        SysTrayX.RestoreOptions.onMinimizeIconTypeError
      );

    //
    //  Restore close type
    //
    await storage()
      .get("closeType")
      .then(
        SysTrayX.RestoreOptions.setCloseType,
        SysTrayX.RestoreOptions.onCloseTypeError
      );

    //
    //  Restore start minimized
    //
    await storage()
      .get("startMinimized")
      .then(
        SysTrayX.RestoreOptions.setStartMinimized,
        SysTrayX.RestoreOptions.onStartMinimizedError
      );

    //
    //  Restore restore position state
    //
    await storage()
      .get("restorePositions")
      .then(
        SysTrayX.RestoreOptions.setRestorePositions,
        SysTrayX.RestoreOptions.onRestorePositionsError
      );

    //
    //  Restore default icon type
    //
    await storage()
      .get("defaultIconType")
      .then(
        SysTrayX.RestoreOptions.setDefaultIconType,
        SysTrayX.RestoreOptions.onDefaultIconTypeError
      );

    //
    //  Restore default icon
    //
    await storage()
      .get(["defaultIconMime", "defaultIcon"])
      .then(
        SysTrayX.RestoreOptions.setDefaultIcon,
        SysTrayX.RestoreOptions.onDefaultIconError
      );

    //
    //  Restore hide default icon
    //
    await storage()
      .get(["kdeIntegration", "hideDefaultIcon"])
      .then(
        SysTrayX.RestoreOptions.setHideDefaultIcon,
        SysTrayX.RestoreOptions.onHideDefaultIconError
      );

    //
    //  Restore icon type
    //
    await storage()
      .get("iconType")
      .then(
        SysTrayX.RestoreOptions.setIconType,
        SysTrayX.RestoreOptions.onIconTypeError
      );

    //
    //  Restore icon
    //
    await storage()
      .get(["iconMime", "icon"])
      .then(
        SysTrayX.RestoreOptions.setIcon,
        SysTrayX.RestoreOptions.onIconError
      );

    //
    //  Restore theme
    //
    await storage()
      .get("theme")
      .then(
        SysTrayX.RestoreOptions.setTheme,
        SysTrayX.RestoreOptions.onThemeError
      );

    //
    //  Restore show number state
    //
    await storage()
      .get("showNumber")
      .then(
        SysTrayX.RestoreOptions.setShowNumber,
        SysTrayX.RestoreOptions.onShowNumberError
      );

    //
    //  Restore show new indicator state
    //
    await storage()
      .get("showNewIndicator")
      .then(
        SysTrayX.RestoreOptions.setShowNewIndicator,
        SysTrayX.RestoreOptions.onShowNewIndicatorError
      );

    //
    //  Restore count type
    //
    await storage()
      .get("countType")
      .then(
        SysTrayX.RestoreOptions.setCountType,
        SysTrayX.RestoreOptions.onCountTypeError
      );

    //
    //  Restore startup delay
    //
    await storage()
      .get("startupDelay")
      .then(
        SysTrayX.RestoreOptions.setStartupDelay,
        SysTrayX.RestoreOptions.ontartupDelayError
      );

    //
    //  Restore number color
    //
    await storage()
      .get("numberColor")
      .then(
        SysTrayX.RestoreOptions.setNumberColor,
        SysTrayX.RestoreOptions.onNumberColorError
      );

    //
    //  Restore number size
    //
    await storage()
      .get("numberSize")
      .then(
        SysTrayX.RestoreOptions.setNumberSize,
        SysTrayX.RestoreOptions.onNumberSizeError
      );

    //
    //  Restore number alignment
    //
    await storage()
      .get("numberAlignment")
      .then(
        SysTrayX.RestoreOptions.setNumberAlignment,
        SysTrayX.RestoreOptions.onNumberAlignmentError
      );

    //
    //  Restore number margins
    //
    await storage()
      .get("numberMargins")
      .then(
        SysTrayX.RestoreOptions.setNumberMargins,
        SysTrayX.RestoreOptions.onNumberMarginsError
      );

    //
    //  Restore new indicator type
    //
    await storage()
      .get("newIndicatorType")
      .then(
        SysTrayX.RestoreOptions.setNewIndicatorType,
        SysTrayX.RestoreOptions.onNewIndicatorTypeError
      );

    //
    //  Restore new shade color
    //
    await storage()
      .get("newShadeColor")
      .then(
        SysTrayX.RestoreOptions.setNewShadeColor,
        SysTrayX.RestoreOptions.onNewShadeColorError
      );

    //
    //  Restore launch parameters
    //
    await storage()
      .get("startApp")
      .then(
        SysTrayX.RestoreOptions.setStartApp,
        SysTrayX.RestoreOptions.onStartAppError
      );

      await storage()
      .get("startAppArgs")
      .then(
        SysTrayX.RestoreOptions.setStartAppArgs,
        SysTrayX.RestoreOptions.onStartAppArgsError
      );

      await storage()
      .get("closeApp")
      .then(
        SysTrayX.RestoreOptions.setCloseApp,
        SysTrayX.RestoreOptions.onCloseAppError
      );

      await storage()
      .get("closeAppArgs")
      .then(
        SysTrayX.RestoreOptions.setCloseAppArgs,
        SysTrayX.RestoreOptions.onCloseAppArgsError
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
    const minimizeType = result.minimizeType || "1";

    // Tweak option for platform
    //    if (SysTrayX.Info.platformInfo.os === "win") {
    //    document.getElementById("minimizemethod1label").innerHTML =
    //      "Minimize to tray";
    //    document
    //      .getElementById("minimizemethod2")
    //      .setAttribute("style", "display:none;");

    if (minimizeType === "2") {
      minimizeType = "1";
    }
    //    }

    const radioButton = document.querySelector(
      `input[name="minimizeType"][value="${minimizeType}"]`
    );
    radioButton.checked = true;
  },

  onMinimizeTypeError: function (error) {
    console.log(`Minimize type Error: ${error}`);
  },

  //
  //  Restore minimize type callbacks
  //
  setMinimizeIconType: function (result) {
    const minimizeIconType = result.minimizeIconType || "1";

    const radioButton = document.querySelector(
      `input[name="minimizeIconType"][value="${minimizeIconType}"]`
    );
    radioButton.checked = true;
  },

  onMinimizeIconTypeError: function (error) {
    console.log(`Minimize icon type Error: ${error}`);
  },

  //
  //  Restore close type callbacks
  //
  setCloseType: function (result) {
    const closeType = result.closeType || "1";

    const radioButton = document.querySelector(
      `input[name="closeType"][value="${closeType}"]`
    );
    radioButton.checked = true;
  },

  onCloseTypeError: function (error) {
    console.log(`Close type Error: ${error}`);
  },

  //
  //  Restore start minimized callbacks
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
  //  Restore restore position state callbacks
  //
  setRestorePositions: function (result) {
    const restorePositions = result.restorePositions || "false";

    // Tweak option for platform
    if (SysTrayX.Info.platformInfo.os === "win") {
      document
        .getElementById("restorePos")
        .setAttribute("style", "display:none;");
      document
        .getElementById("restorePositionsLabel")
        .setAttribute("style", "display:none;");
    }

    const checkbox = document.querySelector(`input[name="restorePositions"]`);
    checkbox.checked = restorePositions === "true";
  },

  onRestorePositionsError: function (error) {
    console.log(`RestorePositions Error: ${error}`);
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
    const kdeIntegration = result.kdeIntegration || "true";
    const hideDefaultIcon = result.hideDefaultIcon || "false";

    const checkbox = document.querySelector(`input[name="hideDefaultIcon"]`);

    if (kdeIntegration === "false") {
      checkbox.parentNode.setAttribute("style", "display: none;");
    }

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
  //  Restore theme callbacks
  //
  setTheme: function (result) {
    const theme = result.theme || "0";

    const radioButton = document.querySelector(
      `input[name="theme"][value="${theme}"]`
    );
    radioButton.checked = true;
  },

  onThemeError: function (error) {
    console.log(`Theme Error: ${error}`);
  },

  //
  //  Restore show number state
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
  //  Restore show new indicator state
  //
  setShowNewIndicator: function (result) {
    const showNewIndicator = result.showNewIndicator || "true";

    const checkbox = document.querySelector(`input[name="showNewIndicator"]`);
    checkbox.checked = showNewIndicator === "true";
  },

  onShowNewIndicatorError: function (error) {
    console.log(`showNewIndicator Error: ${error}`);
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
  //  Restore startup delay
  //
  setStartupDelay: function (result) {
    const startupDelay = result.startupDelay || "5";

    const input = document.querySelector(`input[name="startupDelay"]`);
    input.value = startupDelay;
  },

  onStartupDelayError: function (error) {
    console.log(`StartupDelay Error: ${error}`);
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
    const numberSize = result.numberSize || "10";

    const input = document.querySelector(`input[name="numberSize"]`);
    input.value = numberSize;
  },

  onNumberSizeError: function (error) {
    console.log(`numberSize Error: ${error}`);
  },

  //
  //  Restore number alignment
  //
  setNumberAlignment: function (result) {
    const numberAlignment = result.numberAlignment || "4";

    const input = document.querySelector(`select[name="numberAlignment"]`);
    input.value = numberAlignment;
  },

  onNumberAlignmentError: function (error) {
    console.log(`numberAlignment Error: ${error}`);
  },

  //
  //  Restore number margins
  //
  setNumberMargins: function (result) {
    const numberMargins = result.numberMargins || {
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
    };

    const inputLeft = document.querySelector(`input[name="numberMarginLeft"]`);
    inputLeft.value = numberMargins.left;

    const inputTop = document.querySelector(`input[name="numberMarginTop"]`);
    inputTop.value = numberMargins.top;

    const inputRight = document.querySelector(
      `input[name="numberMarginRight"]`
    );
    inputRight.value = numberMargins.right;

    const inputBottom = document.querySelector(
      `input[name="numberMarginBottom"]`
    );
    inputBottom.value = numberMargins.bottom;
  },

  onNumberMarginsError: function (error) {
    console.log(`numberMargins Error: ${error}`);
  },

  //
  //  Restore new indicator type
  //
  setNewIndicatorType: function (result) {
    const newIndicatorType = result.newIndicatorType || "2";

    const radioButton = document.querySelector(
      `input[name="newIndicatorType"][value="${newIndicatorType}"]`
    );
    radioButton.checked = true;
  },

  onNewIndicatorTypeError: function (error) {
    console.log(`newIndicatorType Error: ${error}`);
  },

  //
  //  Restore number color
  //
  setNewShadeColor: function (result) {
    const newShadeColor = result.newShadeColor || "#ff8000";

    const input = document.querySelector(`input[name="newShadeColor"]`);
    input.value = newShadeColor;
  },

  onNewShadeColorError: function (error) {
    console.log(`newShadeColor Error: ${error}`);
  },

  //
  //  Restore start app callbacks
  //
  setStartApp: function (result) {
    const startApp = result.startApp || "";

    const startAppInput = document.getElementById("startAppInput");
    startAppInput.value = startApp;
  },

  onStartAppError: function (error) {
    console.log(`StartApp Error: ${error}`);
  },

  //
  //  Restore start app args callbacks
  //
  setStartAppArgs: function (result) {
    const startAppArgs = result.startAppArgs || "";

    const startAppArgsInput = document.getElementById("startAppArgsInput");
    startAppArgsInput.value = startAppArgs;
  },

  onStartAppArgsError: function (error) {
    console.log(`StartAppArgs Error: ${error}`);
  },

  //
  //  Restore close app callbacks
  //
  setCloseApp: function (result) {
    const closeApp = result.closeApp || "";

    const closeAppInput = document.getElementById("closeAppInput");
    closeAppInput.value = closeApp;
  },

  onCloseAppError: function (error) {
    console.log(`CloseApp Error: ${error}`);
  },

  //
  //  Restore close app args callbacks
  //
  setCloseAppArgs: function (result) {
    const closeAppArgs = result.closeAppArgs || "";

    const closeAppArgsInput = document.getElementById("closeAppArgsInput");
    closeAppArgsInput.value = closeAppArgs;
  },

  onCloseAppArgsError: function (error) {
    console.log(`CloseAppArgs Error: ${error}`);
  },

  //
  //  Restore filters callbacks
  //
  setFilters: function (result) {
    let filters = result.filters || undefined;

    const accounts = document
      .getElementById("accountsTree")
      .querySelectorAll('input[type="checkbox"][name*="account"]');

    //  No filters stored?
    if (filters === undefined || filters.length === 0) {
      //  Create default filters

      const treeBase = document.getElementById("accountsTree");
      const accountsBoxes = treeBase.querySelectorAll(
        'input[type="checkbox"][name*="account"]'
      );

      let accounts = [];
      accountsBoxes.forEach((accountbox) => {
        const value = JSON.parse(accountbox.value);
        accounts.push({ folders: value.folders });
      });

      filters = [];
      accounts.forEach((account) => {
        const inbox = account.folders.filter(
          (folder) => folder.type === "inbox"
        );

        if (inbox.length > 0) {
          let folder = {};
          if (SysTrayX.Info.browserInfo.majorVersion < 91) {
            folder = {
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
          } else {
            filters.push({
              accountId: inbox[0].accountId,
              folders: [inbox[0].path],
            });
          }
        }
      });
    }

    if (filters) {
      const treeBase = document.getElementById("accountsTree");

      if (SysTrayX.Info.browserInfo.majorVersion < 91) {
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
      } else {
        filters.forEach((filter) => {
          const account = treeBase.querySelector(
            `input[name=${filter.accountId}]`
          );
          const checkboxes = Array.from(
            account.parentNode.querySelectorAll(
              'input[type="checkbox"]:not([name^="account"]):not([name^="parent-"])'
            )
          );

          filter.folders.forEach((path) => {
            checkboxes.forEach((checkbox) => {
              const value = JSON.parse(checkbox.value);
              if (value.path === path) {
                checkbox.checked = true;

                const event = document.createEvent("HTMLEvents");
                event.initEvent("change", false, true);
                checkbox.dispatchEvent(event);
              }
            });
          });
        });
      }
    }
  },

  onFiltersError: function (error) {
    console.log(`Filters Error: ${error}`);
  },
};

SysTrayX.StorageReset = {
  reset: async function () {
    // Clear checkboxes
    const checkboxes = document
      .getElementById("accountsTree")
      .querySelectorAll('input[type="checkbox"]');
    checkboxes.forEach((checkbox) => {
      checkbox.checked = false;

      const event = document.createEvent("HTMLEvents");
      event.initEvent("change", false, false);
      checkbox.dispatchEvent(event);
    });

    await browser.storage.sync.clear();
    await browser.storage.local.clear();

    //  Set new inbox checkboxes
    SysTrayX.RestoreOptions.setFilters({
      filters: undefined,
    });
  },
};

SysTrayX.StorageChanged = {
  changed: function (changes, area) {
    //  Try to keep the preferences of the add-on and the app in sync
    const changedItems = Object.keys(changes);

    let changed_icon = false;
    let changed_default_icon = false;
    for (let item of changedItems) {
      if (item === "minimizeType") {
        SysTrayX.RestoreOptions.setMinimizeType({
          minimizeType: changes[item].newValue,
        });
      }
      if (item === "minimizeIconType") {
        SysTrayX.RestoreOptions.setMinimizeIconType({
          minimizeIconType: changes[item].newValue,
        });
      }
      if (item === "closeType") {
        SysTrayX.RestoreOptions.setCloseType({
          closeType: changes[item].newValue,
        });
      }
      if (item === "startMinimized") {
        SysTrayX.RestoreOptions.setStartMinimized({
          startMinimized: changes[item].newValue,
        });
      }
      if (item === "restorePositions") {
        SysTrayX.RestoreOptions.setRestorePositions({
          restorePositions: changes[item].newValue,
        });
      }
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
      if (item === "theme") {
        SysTrayX.RestoreOptions.setTheme({
          theme: changes[item].newValue,
        });
      }
      if (item === "showNumber") {
        SysTrayX.RestoreOptions.setShowNumber({
          showNumber: changes[item].newValue,
        });
      }
      if (item === "showNewIndicator") {
        SysTrayX.RestoreOptions.setShowNewIndicator({
          showNewIndicator: changes[item].newValue,
        });
      }
      if (item === "countType") {
        SysTrayX.RestoreOptions.setCountType({
          countType: changes[item].newValue,
        });
      }
      if (item === "startupDelay") {
        SysTrayX.RestoreOptions.setStartupDelay({
          startupDelay: changes[item].newValue,
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
      if (item === "numberAlignment") {
        SysTrayX.RestoreOptions.setNumberAlignment({
          numberAlignment: changes[item].newValue,
        });
      }
      if (item === "numberMargins") {
        SysTrayX.RestoreOptions.setNumberMargins({
          numberMargins: changes[item].newValue,
        });
      }
      if (item === "newIndicatorType") {
        SysTrayX.RestoreOptions.setIndicatorType({
          newIndicatorType: changes[item].newValue,
        });
      }
      if (item === "newShadeColor") {
        SysTrayX.RestoreOptions.setNewShadeColor({
          newShadeColor: changes[item].newValue,
        });
      }
      if (item === "startApp") {
        SysTrayX.RestoreOptions.setStartApp({
          startApp: changes[item].newValue,
        });
      }
      if (item === "startAppArgs") {
        SysTrayX.RestoreOptions.setStartAppArgs({
          startAppArgs: changes[item].newValue,
        });
      }
      if (item === "closeApp") {
        SysTrayX.RestoreOptions.setCloseApp({
          closeApp: changes[item].newValue,
        });
      }
      if (item === "closeAppArgs") {
        SysTrayX.RestoreOptions.setCloseAppArgs({
          closeAppArgs: changes[item].newValue,
        });
      }

      if (item === "filters") {
        SysTrayX.RestoreOptions.setFilters({
          filters: changes[item].newValue,
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
    document.getElementById("minimizeiconselect").className = "active";
    document.getElementById("closeselect").className = "active";
    document.getElementById("themeselect").className = "active";
    document.getElementById("newindicatorselect").className = "active";
    document.getElementById("startappselect").className = "active";
    document.getElementById("closeappselect").className = "active";
  },
};

//
//  Main
//

async function start() {
  //  Set platform
  SysTrayX.Info.platformInfo = await browser.runtime.getPlatformInfo();

  //  Set browser
  SysTrayX.Info.browserInfo = await browser.runtime.getBrowserInfo();

  const version = SysTrayX.Info.browserInfo.version.split(".");
  SysTrayX.Info.browserInfo.majorVersion = version[0];
  SysTrayX.Info.browserInfo.minorVersion = version[1];

  //  Get addon version
  SysTrayX.Info.version = browser.runtime.getManifest().version;

  SysTrayX.Info.displayInfo();

  // Set link in options pageF
  document.getElementById("VersioHomeLink").href =
    "https://github.com/Ximi1970/systray-x/releases/tag/" +
    SysTrayX.Info.version;

  // Disable incompatible items
  if (SysTrayX.Info.browserInfo.majorVersion > 89 && SysTrayX.Info.browserInfo.majorVersion < 91) {
    document.getElementById("counttype").style.display = "none";
  }

  // Set the right default icon
  if (SysTrayX.Info.browserInfo.majorVersion < 115) {
    document.getElementById("defaultCustomIconImage").setAttribute("src","icons/Thunderbird.png");
  } else {
    document.getElementById("defaultCustomIconImage").setAttribute("src","icons/Thunderbird115.png");
  }

  if (SysTrayX.Info.platformInfo.os !== "linux") {
    document.getElementById("kdeintegration").style.display = "none";
  }

  // Setup account tree
  const accountsInitPromise = () =>
    new Promise((res) => res(SysTrayX.Accounts.init()));
  await accountsInitPromise();

  // Set the options
  //document.addEventListener("DOMContentLoaded", SysTrayX.RestoreOptions.start);
  SysTrayX.RestoreOptions.start();

  // Enable save button
  document
    .querySelector('[name="saveform"]')
    .addEventListener("submit", SysTrayX.SaveOptions.start);

  // Enable reset button
  document
    .querySelector('[name="resetbutton"]')
    .addEventListener("click", SysTrayX.StorageReset.reset);

  browser.storage.onChanged.addListener(SysTrayX.StorageChanged.changed);

  document.addEventListener("visibilitychange", function () {
    if (document.visibilityState === "hidden") {
      browser.storage.onChanged.removeListener(SysTrayX.StorageChanged.changed);
    }
  });
}

start();
