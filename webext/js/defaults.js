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
    const toDataURL = url =>
      fetch(url)
        .then(response => response.blob())
        .then(
          blob =>
            new Promise((resolve, reject) => {
              const reader = new FileReader();
              reader.onloadend = () => resolve(reader.result);
              reader.onerror = reject;
              reader.readAsDataURL(blob);
            })
        );

    //  Convert image to storage param
    let { iconMime, iconBase64 } = await toDataURL("icons/blank-icon.png").then(
      dataUrl => {
        const data = dataUrl
          .split(":")
          .pop()
          .split(",");
        return { iconMime: data[0].split(";")[0], iconBase64: data[1] };
      }
    );

    //  Store default icon (base64)
    browser.storage.sync.set({
      iconMime: iconMime,
      icon: iconBase64
    });

    //  Store in HTML
    const iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon-mime", iconMime);
    iconDiv.setAttribute("data-icon", iconBase64);

    console.debug("Default: " + iconMime);
    console.debug("Default: " + iconBase64);
  }
}
