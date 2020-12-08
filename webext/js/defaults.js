//
//  Set default default icon
//  Use <div> as storage
//
async function getDefaultIcon() {
  function getStoredDefaultIcon(result) {
    return result.defaultIconMime && result.defaultIcon;
  }

  function onStoredDefaultIconError() {
    return false;
  }

  const getDefaultIcon = browser.storage.sync.get([
    "defaultIconMime",
    "defaultIcon",
  ]);
  const defaultIconStored = await getDefaultIcon.then(
    getStoredDefaultIcon,
    onStoredDefaultIconError
  );

  if (!defaultIconStored) {
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
    let { defaultIconMime, defaultIconBase64 } = await toDataURL(
      "icons/Thunderbird.png"
    ).then((dataUrl) => {
      const data = dataUrl.split(":").pop().split(",");
      return {
        defaultIconMime: data[0].split(";")[0],
        defaultIconBase64: data[1],
      };
    });

    //  Store default icon (base64)
    browser.storage.sync.set({
      defaultIconMime: defaultIconMime,
      defaultIcon: defaultIconBase64,
    });

    //  Store in HTML
    const defaultIconDiv = document.getElementById("defaultIcon");
    defaultIconDiv.setAttribute("data-default-icon-mime", defaultIconMime);
    defaultIconDiv.setAttribute("data-default-icon", defaultIconBase64);
  }
}

//
//  Set default unread icon
//  Use <div> as storage
//
async function getIcon() {
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
  function getStartupStateCb(result) {
    const startMinimized = result.startMinimized || "false";
    return startMinimized === "true" ? "minimized" : "normal";
  }

  function onStartupStateError() {
    return "normal";
  }

  const getState = browser.storage.sync.get("startMinimized");
  return await getState.then(getStartupStateCb, onStartupStateError);
}

//
//  Get window restore position state
//
async function getRestorePositionsState() {
  function getRestorePositionsStateCb(result) {
    const restorePositions = result.restorePositions || "false";
    return restorePositions;
  }

  function onRestorePositionsStateError() {
    return "false";
  }

  const getState = browser.storage.sync.get("restorePositions");
  return await getState.then(
    getRestorePositionsStateCb,
    onRestorePositionsStateError
  );
}

//
//  Get window startup window positions
//
async function getStartupWindowPositions() {
  function getStartupWindowPositionsCb(result) {
    const windowPositions = result.windowPositions || [];
    return windowPositions;
  }

  function onStartupWindowPositionsError() {
    return [];
  }

  const getWindowPositions = browser.storage.sync.get("windowPositions");
  return await getWindowPositions.then(
    getStartupWindowPositionsCb,
    onStartupWindowPositionsError
  );
}

//
//  Get KDE integration, default icon hide
//
async function getHideDefaultIcon() {
  function getHideDefaultIconPref(result) {
    const hideDefaultIcon = result.hideDefaultIcon || "false";
    return hideDefaultIcon === "true";
  }

  function onHideDefaultIconPrefError() {
    return false;
  }

  const getState = browser.storage.sync.get("hideDefaultIcon");
  return await getState.then(
    getHideDefaultIconPref,
    onHideDefaultIconPrefError
  );
}

//  Check if the filters are for existing accounts
function checkAccountFilters(filters) {
  let filtersChanged = false;
  let newFilters = [];

  //  Create an id -> name map
  accountNames = {};
  for (const account of SysTrayX.Messaging.accounts) {
    accountNames[account.id] = account.name;
  }

  for (let i = 0; i < filters.length; ++i) {
    let filter = filters[i];

    const accountName = accountNames[filter.folder.accountId];
    if (accountName === undefined) {
      filtersChanged = true;
      continue;
    }

    //  Store the passed filter
    newFilters.push(filter);
  }

  return filtersChanged ? newFilters : undefined;
}

//  Create a folders array with paths for TB74+
function createFoldersArrayPre74(baseFolders) {
  let result = [];
  let level = { result };

  baseFolders.forEach((folder) => {
    folder.path
      .slice(1)
      .split("/")
      .reduce((r, name, i, a) => {
        if (!r[name]) {
          r[name] = { result: [] };
          r.result.push({
            accountId: folder.accountId,
            name: folder.name,
            type: folder.type,
            path: folder.path,
            subFolders: r[name].result,
          });
        }

        return r[name];
      }, level);
  });

  return createFoldersArray(result);
}

//  Create a folders array with paths for TB74+
function createFoldersArray(baseFolders) {
  let newFolders = [];

  function traverse(path, folders) {
    if (!folders) {
      return;
    }
    for (let i = 0; i < folders.length; ++i) {
      newFolders.push({
        ...folders[i],
        pathOrig: folders[i].path,
        path: path + "/" + folders[i].name,
      });
      traverse(folders[i].path, folders[i].subFolders);
    }
  }

  traverse("", baseFolders);

  return newFolders;
}

//  Check if the filter folders still exists
function checkFolderFilters(filters) {
  let filtersChanged = false;
  let newFilters = [];

  //  Create an id -> name map
  accountNames = {};
  accountFolders = {};
  for (const account of SysTrayX.Messaging.accounts) {
    accountNames[account.id] = account.name;

    if (SysTrayX.browserInfo.version.split(".")[0] < 74) {
      //  Pre TB74 accounts API
      accountFolders[account.id] = createFoldersArrayPre74(account.folders);
    } else {
      //  TB74+ accounts API
      accountFolders[account.id] = createFoldersArray(account.folders);
    }
  }

  for (let i = 0; i < filters.length; ++i) {
    let filter = filters[i];

    const found = accountFolders[filter.folder.accountId].filter((folder) => {
      if (filter.folder.version) {
        const folderPaths = folder.path.split("/");
        const folderPathLast = folderPaths.pop();
        const folderPathFirst = folderPaths.join("/");

        const filterFolderPaths = filter.folder.path.split("/");
        const filterFolderPathLast = filterFolderPaths.pop();
        const filterFolderPathFirst = filterFolderPaths.join("/");

        if (
          folderPathFirst === filterFolderPathFirst &&
          folderPathLast !== filterFolderPathLast &&
          ((folder.type === "inbox" && filter.folder.type === "inbox") ||
            (folder.type === "drafts" && filter.folder.type === "drafts") ||
            (folder.type === "sent" && filter.folder.type === "sent") ||
            (folder.type === "trash" && filter.folder.type === "trash") ||
            (folder.type === "templates" &&
              filter.folder.type === "templates") ||
            (folder.type === "archives" && filter.folder.type === "archives") ||
            (folder.type === "junk" && filter.folder.type === "junk") ||
            (folder.type === "outbox" && filter.folder.type === "outbox"))
        ) {
          filter.folder.path = folder.path;
          filter.folder.name = folder.name;
          filtersChanged = true;
          return true;
        }

        if (folder.path === filter.folder.path) {
          if (
            folder.type != filter.folder.type &&
            filter.folder.type == undefined
          ) {
            filter.folder.type = folder.type !== undefined ? folder.type : "";
            filtersChanged = true;
          }
          return true;
        }

        return false;
      } else {
        return folder.pathOrig === filter.folder.path;
      }
    });

    if (found.length === 0) {
      console.debug("Removed filter: " + JSON.stringify(filter));

      filtersChanged = true;
      continue;
    }

    //  Upgrade filter
    let folder = filter.folder;
    if (filter.folder.accountName === undefined) {
      folder.accountName = accountNames[found[0].accountId];
      filtersChanged = true;
    }
    if (filter.folder.name === undefined) {
      folder.name = found[0].name;
      filtersChanged = true;
    }
    if (filter.folder.version === undefined) {
      folder.version = SysTrayX.version;
      folder.path = found[0].path;
      folder.name = found[0].path.split("/").pop();
      filtersChanged = true;
    }

    //  Store the passed filter
    newFilters.push(filter);
  }

  return filtersChanged ? newFilters : undefined;
}

//
//  Check filters
//
function checkFilters(filters) {
  let newFilters = [];

  console.debug(
    "Current accounts: " + JSON.stringify(SysTrayX.Messaging.accounts)
  );

  if (filters === undefined) {
    //  Create base filters
    for (const account of SysTrayX.Messaging.accounts) {
      /*
      //
      //  Display specials
      //
      accountFolders = [];
      if (SysTrayX.browserInfo.version.split(".")[0] < 74) {
        //  Pre TB74 accounts API
        accountFolders = createFoldersArrayPre74(account.folders);
      } else {
        //  TB74+ accounts API
        accountFolders = createFoldersArray(account.folders);
      }

      const specials = accountFolders.filter(
        (folder) => folder.type != undefined
      );
      console.debug("Special folders: " + JSON.stringify(specials));
*/

      const inbox = account.folders.filter((folder) => folder.type == "inbox");

      if (inbox.length > 0) {
        let folder = {
          ...inbox[0],
          accountName: account.name,
          path: "/" + inbox[0].name,
          version: SysTrayX.version,
        };
        delete folder.type;
        delete folder.subFolders;

        newFilters.push({
          unread: true,
          folder: folder,
        });
      }
    }
  } else if (filters.length > 0) {
    //  Check the filters

    let filtersChanged = false;
    let tmpFilters = undefined;
    newFilters = filters;

    // Check if the filters are for the current accounts
    tmpFilters = checkAccountFilters(newFilters);
    if (tmpFilters) {
      newFilters = tmpFilters;
      filtersChanged = true;
    }

    // Check if the filters are for the current folders
    tmpFilters = checkFolderFilters(newFilters);
    if (tmpFilters) {
      newFilters = tmpFilters;
      filtersChanged = true;
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

  return newFilters;
}

//
//  Get filters
//
async function getFilters() {
  function getFiltersCb(result) {
    const filters = result.filters || undefined;

    console.debug("Stored filters: " + JSON.stringify(filters));

    const newFilters = checkFilters(filters);

    console.debug("Checked filters: " + JSON.stringify(newFilters));

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

//
//  Get close type
//
async function getCloseType() {
  function getCloseTypeCb(result) {
    return result.closeType || "1";
  }

  function onCloseTypeError() {
    return undefined;
  }

  const getCloseType = browser.storage.sync.get("closeType");
  return await getCloseType.then(getCloseTypeCb, onCloseTypeError);
}
