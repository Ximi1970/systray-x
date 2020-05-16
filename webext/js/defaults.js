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
    const startMinimized = result.startMinimized || "false";
    return startMinimized === "true" ? "minimized" : "normal";
  }

  function onStartupStateError() {
    return "normal";
  }

  const getState = browser.storage.sync.get("startMinimized");
  return await getState.then(getStartupState, onStartupStateError);
}

//
//  Get minimize on close state
//
async function getMinimizeOnClose() {
  function getMinimizeOnClosePref(result) {
    const minimizeOnClose = result.minimizeOnClose || "true";
    return minimizeOnClose === "true";
  }

  function onMinimizeOnClosePrefError() {
    return true;
  }

  const getState = browser.storage.sync.get("minimizeOnClose");
  return await getState.then(
    getMinimizeOnClosePref,
    onMinimizeOnClosePrefError
  );
}

//
//  Check filters
//
function checkFilters(filters) {
  let newFilters = [];

  if (filters === undefined) {
    //  Create base filters
    for (const account of SysTrayX.Messaging.accounts) {
      const inbox = account.folders.filter((folder) => folder.type == "inbox");

      if (inbox.length > 0) {
        let folder = { ...inbox[0], accountName: account.name };
        delete folder.type;
        delete folder.subFolders;

        newFilters.push({
          unread: true,
          folder: folder,
        });
      }
    }
  } else if (filters.length > 0) {
    //  Check the format

    //  Find a folder in a account structure
    function findFolder(accountId, path) {
      const account = SysTrayX.Messaging.accounts.filter(
        (account) => account.id === accountId
      );

      if (SysTrayX.browserInfo.version.split(".")[0] < 74) {
        //
        //  Search the pre TB74 account structure
        //
        const folder = account[0].folders.filter(
          (folder) => folder.path === path
        );

        const folders = account[0].folders.filter((folder) =>
          folder.path.startsWith(path)
        );

        if (folders.length === 0) {
          return undefined;
        } else if (folders.length === 1) {
          return folder[0].name;
        } else {
          return "^ Add base folder";
        }
      } else {
        //
        //  Search the TB74+ account structure
        //
        let arrayOfFolders = [];

        function traverse(folders, path) {
          if (!folders) {
            return;
          }
          for (let f of folders) {
            if (f.path === path) arrayOfFolders.push(f);
            traverse(f.subFolders, path);
          }
        }

        traverse(account[0].folders, path);

        if (arrayOfFolders.length === 0) {
          return undefined;
        } else if (arrayOfFolders[0].subFolders.length > 0) {
          return "^ Add base folder";
        } else {
          return arrayOfFolders[0].name;
        }
      }
    }

    if (filters[0].folder.accountName === undefined) {
      //  Old format, convert

      console.log("Converting old filters");

      //  Create an id -> name map
      accountNames = {};
      for (const account of SysTrayX.Messaging.accounts) {
        accountNames[account.id] = account.name;
      }

      for (let i = 0; i < filters.length; ++i) {
        let filter = filters[i];

        const accountName = accountNames[filter.folder.accountId];
        if (accountName === undefined) {
          continue;
        }

        //  Account still active
        filter.folder.accountName = accountName;

        const name = findFolder(filter.folder.accountId, filter.folder.path);
        if (name === undefined) {
          continue;
        }

        //  Folder still active
        filter.folder.name = name;

        //  Store the new filter
        newFilters.push(filter);
      }

      //
      //  Store the converted filters
      //
      browser.storage.sync.set({
        filters: newFilters,
      });
    } else {
      // Need to check if the accounts still exist

      //  Create an id -> name map
      accountNames = {};
      for (const account of SysTrayX.Messaging.accounts) {
        accountNames[account.id] = account.name;
      }

      let filtersChanged = false;
      for (let i = 0; i < filters.length; ++i) {
        let filter = filters[i];

        const accountName = accountNames[filter.folder.accountId];
        if (accountName === undefined) {
          let filtersChanged = true;
          continue;
        }

        const name = findFolder(filter.folder.accountId, filter.folder.path);
        if (name === undefined) {
          let filtersChanged = true;
          continue;
        }

        //  Store the new filter
        newFilters.push(filter);
      }

      if (filtersChanged) {
        //
        //  Store the converted filters
        //
        browser.storage.sync.set({
          filters: newFilters,
        });
      }
    }
  }

  return newFilters;
}

//
//  Get filters
//
async function getFilters() {
  function getFiltersCb(result) {
    const filters = result.filters || undefined;

    const newFilters = checkFilters(filters);

    return newFilters;
  }

  function onFiltersError() {
    return undefined;
  }

  const getFiltersStorage = browser.storage.sync.get("filters");
  return await getFiltersStorage.then(getFiltersCb, onFiltersError);
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
