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
    return result.startMinimized === "true" ? "minimized" : "normal";
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
    return result.minimizeOnClose === "true";
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
  let newFilters = filters;

  if (filters === undefined) {
    //  Create base filters
  } else if (filters.length > 0) {
    //  Check the format
    if (filters[0].folder.accountName === undefined) {
      //  Old format, convert
      console.debug("Converting filters....");
      console.debug("Old filters: " + JSON.stringify(filters));
      console.debug("Accounts: " + JSON.stringify(SysTrayX.Messaging.accounts));

      accountNames = {};
      for (const account of SysTrayX.Messaging.accounts) {
        accountNames[account.id] = account.name;
      }
      console.debug("AccountNames: " + JSON.stringify(accountNames));

      function findFolder(accountId, path) {
        const account = SysTrayX.Messaging.accounts.filter(
          (account) => account.id === accountId
        );

        console.debug(
          "Account scanning: " + accountId + ": " + JSON.stringify(account)
        );

        if (SysTrayX.browserInfo.version.split(".")[0] < 74) {
          const folder = account[0].folders.filter(
            (folder) => folder.path === path
          );

          const folders = account[0].folders.filter((folder) =>
            folder.pathstartsWith(path)
          );

          if (folders.length > 0) {
            return "^ Add base folder";
          } else {
            return folder[0].name;
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

          if (arrayOfFolders[0].subFolders.length > 0) {
            return "^ Add base folder";
          } else {
            return arrayOfFolders[0].name;
          }
        }
      }

      for (let i = 0; i < newFilters.length; ++i) {
        newFilters[i].folder.accountName =
          accountNames[newFilters[i].folder.accountId];
        newFilters[i].folder.name = findFolder(
          newFilters[i].folder.accountId,
          newFilters[i].folder.path
        );
      }

      /*
      newFilters.foreach((filter) => {
        filter.folder.accountName = accountNames[filter.accountId];
      });
      */

      console.debug("New filters: " + JSON.stringify(newFilters));

      /*
      let folderArrays = {};
      SysTrayX.Messaging.accounts.foreach((account) => {
        if (SysTrayX.browserInfo.version.split(".")[0] < 74) {
          //  Pre TB74 accounts API
          folderArrays[account.id] = account.folders;
        } else {
          //  TB74+ accounts API, (this shit never ends...)
          let folderArray = [];
  
          function traverse(folders) {
            if (!folders) {
              return;
            }
            for (let f of folders) {
              arrayOfFolders.push(f);
              traverse(f.subFolders);
            }
          }
  
          traverse(account.folders);
  
          folderArrays[account.id] = folderArray;
        }
      });
  
      console.debug("FolderArrays: " + JSON.stringify(folderArrays));
*/

      //    accountName: accountName,
      //    name: folder.name,
      //    Special:  "name":"^ Add base folder"
      //    TB74+ check for subfolders var,
      //    before TB75 scan all folders for substring in path?

      //  Store extended query filters
      /*
      browser.storage.sync.set({
        filters: newFilters,
      });
      */
    }
  }

  return newFilters;
}

//
//  Get filters
//
async function getFilters() {
  function getFiltersCb(result) {
    let filters = result.filters || undefined;

    filters = checkFilters(filters);

    return filters;
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
