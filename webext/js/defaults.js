//
//  Get the prefered storage
//
function storage() {
  if (SysTrayX.Info.storageType === "sync") {
    console.log("Using sync storage");
    return browser.storage.sync;
  } else {
    console.log("Using local storage");
    return browser.storage.local;
  }
}

//
//  Get window startup state
//
async function getStartupState() {
  function resolve(result) {
    const startMinimized = result.startMinimized || "false";
    return startMinimized === "true" ? "minimized" : "normal";
  }

  function reject() {
    return "normal";
  }

  return await storage().get("startMinimized").then(resolve, reject);
}

//
//  Get window restore position state
//
async function getRestorePositionsState() {
  function resolve(result) {
    const restorePositions = result.restorePositions || "false";
    return restorePositions;
  }

  function reject() {
    return "false";
  }

  return await storage().get("restorePositions").then(resolve, reject);
}

//
//  Get window startup window positions
//
async function getStartupWindowPositions() {
  function resolve(result) {
    const windowPositions = result.windowPositions || [];
    return windowPositions;
  }

  function reject() {
    return [];
  }

  return await storage().get("windowPositions").then(resolve, reject);
}

//
//  Get close type
//
async function getCloseType() {
  function resolve(result) {
    return result.closeType || "1";
  }

  function reject() {
    return undefined;
  }

  return await storage().get("closeType").then(resolve, reject);
}

//
//  Get KDE integration, default icon hide
//
async function getHideDefaultIcon() {
  function resolve(result) {
    const hideDefaultIcon = result.hideDefaultIcon || "false";
    return hideDefaultIcon === "true";
  }

  function reject() {
    return false;
  }

  return await storage().get("hideDefaultIcon").then(resolve, reject);
}

//
//  Set default default icon
//  Use <div> as storage
//
async function getDefaultIcon() {
  function resolve(result) {
    return result.defaultIconMime && result.defaultIcon;
  }

  function reject() {
    return false;
  }

  const defaultIconStored = await storage()
    .get(["defaultIconMime", "defaultIcon"])
    .then(resolve, reject);

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
    await storage().set({
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
  function resolve(result) {
    return result.iconMime && result.icon;
  }

  function reject() {
    return false;
  }

  const iconStored = await storage()
    .get(["iconMime", "icon"])
    .then(resolve, reject);

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
    await storage().set({
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
//  Check if the filters are for existing accounts
//
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

    if (SysTrayX.Info.browserInfo.majorVersion < 74) {
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
      folder.version = SysTrayX.Info.version;
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

  if (filters === undefined || filters.length === 0) {
    //
    //  No filters defined, create base filters
    //
    for (const account of SysTrayX.Messaging.accounts) {
      const inbox = account.folders.filter((folder) => folder.type == "inbox");

      if (inbox.length > 0) {
        let folder = {};
        if (SysTrayX.Info.browserInfo.majorVersion < 91) {
          //          console.debug("Folder pre 91: " + JSON.stringify(inbox[0]));

          folder = {
            ...inbox[0],
            accountName: account.name,
            path: "/" + inbox[0].name,
            version: SysTrayX.Info.version,
          };
          delete folder.subFolders;

          newFilters.push({
            unread: true,
            folder: folder,
          });
        } else {
          //          console.debug("Folder 91+: " + JSON.stringify(inbox[0]));

          newFilters.push({
            accountId: inbox[0].accountId,
            version: SysTrayX.Info.version,
            folders: [inbox[0].path],
          });
        }
      }
    }
  } else if (filters.length > 0) {
    //  Check the filters
    newFilters = filters;

    /*
    let filtersChanged = false;
    let tmpFilters = undefined;

    // Check if the filters are for the current accounts
    tmpFilters = checkAccountFilters(newFilters);
    if (tmpFilters) {
      newFilters = tmpFilters;

      console.debug("Filters Accounts updated")

      filtersChanged = true;
    }

    // Check if the filters are for the current folders
    tmpFilters = checkFolderFilters(newFilters);
    if (tmpFilters) {
      newFilters = tmpFilters;

      console.debug("Filters Folders updated")

      filtersChanged = true;
    }

    if (filtersChanged) {
      //
      //  Store the converted filters
      //
      await storage().set({
        filters: newFilters,
      });
    }
*/
  }

  return newFilters;
}

//
//  Get filters
//
async function getFilters() {
  function resolve(result) {
    const filters = result.filters || undefined;

    if (filters !== undefined) {
      console.debug("Stored filters length: " + JSON.stringify(filters).length);
      console.debug("Stored filters: " + JSON.stringify(filters));
    } else {
      console.debug("Stored filters: undefined");
    }

    const newFilters = checkFilters(filters);

    if (filters === undefined || filters.length === 0) {
      console.debug("Force new filters: " + JSON.stringify(newFilters));

      return newFilters;
    } else {
      return filters;
    }

    /*
    console.debug("Checked filters: " + JSON.stringify(newFilters));

    return newFilters;
    */
  }

  function reject() {
    return undefined;
  }

  return await storage().get("filters").then(resolve, reject);
}

//
//  Get count type
//
async function getCountType() {
  function resolve(result) {
    return result.countType || "0";
  }

  function reject() {
    return undefined;
  }

  return await storage().get("countType").then(resolve, reject);
}

//  Helper funcs for TB91 and later folder handling

async function getMailFolderInfo(folder) {
  return await browser.folders.getFolderInfo(folder);
}

// Check if a folder is in the filter list
function isFolderInFilters(folder) {
  return (
    SysTrayX.Messaging.filters.filter(
      (filter) =>
        filter.folder.accountId === folder.accountId &&
        (filter.folder.path === folder.path ||
          filter.folder.path.toUpperCase() === folder.path)
    ).length !== 0
  );
}

// Check if the parent folder of a folder is in the filter list
function isParentFolderInFilters(folder) {
  const parentPath = folder.path.substring(0, folder.path.lastIndexOf("/"));

  return (
    SysTrayX.Messaging.filters.filter(
      (filter) =>
        filter.folder.accountId === folder.accountId &&
        (filter.folder.path === parentPath ||
          filter.folder.path.toUpperCase() === parentPath)
    ).length !== 0
  );
}

// Delete a folder from the filter list
async function deleteFolderFromFilters(folder) {
  const newFilters = SysTrayX.Messaging.filters.filter(
    (filter) =>
      !(
        filter.folder.accountId === folder.accountId &&
        (filter.folder.path === folder.path ||
          filter.folder.path.toUpperCase() === folder.path)
      )
  );

  //  Store the new filters
  await storage().set({
    filters: newFilters,
  });
}

// Get the account name from an id
function getAccountName(id) {
  const account = SysTrayX.Messaging.accounts.filter(
    (account) => account.id === id
  )[0];

  return account.name;
}

// Add a folder to the filter list
async function addFolderToFilters(newFolder) {
  const folder = {
    ...newFolder,
    accountName: getAccountName(newFolder.accountId),
    version: SysTrayX.Info.version,
  };

  const filter = {
    unread: true,
    folder: folder,
  };

  const newFilters = SysTrayX.Messaging.filters;
  newFilters.push(filter);

  //  Store the new filters
  await storage().set({
    filters: newFilters,
  });
}
