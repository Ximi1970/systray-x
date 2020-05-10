//
//  Set default icon
//  Use <div> as storage
//
async function getDefaultIcon() {
  function getStoredIcon(result) {
    return result.iconMime && result.icon;
  }

  function onStoredIconError() {
    return false;
  }

  const getIcon = browser.storage.sync.get(["iconMime", "icon"]);
  const iconStored = await getIcon.then(getStoredIcon, onStoredIconError);

  if (!iconStored) {
    const toDataURL = (url) =>
      fetch(url)
        .then((response) => response.blob())
        .then(
          (blob) =>
            new Promise((resolve, reject) => {
              const reader = new FileReader();
              reader.onloadend = () => resolve(reader.result);
              reader.onerror = reject;
              reader.readAsDataURL(blob);
            })
        );

    //  Convert image to storage param
    let { iconMime, iconBase64 } = await toDataURL("icons/blank-icon.png").then(
      (dataUrl) => {
        const data = dataUrl.split(":").pop().split(",");
        return { iconMime: data[0].split(";")[0], iconBase64: data[1] };
      }
    );

    //  Store default icon (base64)
    browser.storage.sync.set({
      iconMime: iconMime,
      icon: iconBase64,
    });

    //  Store in HTML
    const iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon-mime", iconMime);
    iconDiv.setAttribute("data-icon", iconBase64);
  }
}

//
//  Get window startup state
//
async function getStartupState() {
  function getStartupState(result) {
    return result.startMinimized == "true" ? "minimized" : "normal";
  }

  function onStartupStateError() {
    return "normal";
  }

  const getState = browser.storage.sync.get("startMinimized");
  return await getState.then(getStartupState, onStartupStateError);
}

//
//  Get filters
//
async function getFilters() {
  function getFiltersCb(result) {
    return result.filters || undefined;
  }

  function onFiltersError() {
    return undefined;
  }

  const getFilters = browser.storage.sync.get("filters");
  return await getFilters.then(getFiltersCb, onFiltersError);
}

//
//  Get extended filters
//
async function getFiltersExt() {
  function getFiltersExtCb(result) {
    return result.filtersExt || undefined;
  }

  function onFiltersExtError() {
    return undefined;
  }

  const getFiltersExt = browser.storage.sync.get("filtersExt");
  return await getFiltersExt.then(getFiltersExtCb, onFiltersExtError);
}

//
//  Get count type
//
async function getCountType() {
  function getCountTypeCb(result) {
    return result.countType || "0";
  }

  function onCountTypeError() {
    return undefined;
  }

  const getCountType = browser.storage.sync.get("countType");
  return await getCountType.then(getCountTypeCb, onCountTypeError);
}
