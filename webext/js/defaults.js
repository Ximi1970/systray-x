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
    const defaultIconMime = result.defaultIconMime || "";
    const defaultIcon = result.defaultIcon || "";
    return { defaultIconMime, defaultIcon };
  }

  function reject() {
    return "";
  }

  const { defaultIconMime, defaultIcon } = await storage()
    .get(["defaultIconMime", "defaultIcon"])
    .then(resolve, reject);

  if (defaultIconMime === "" || defaultIcon === "") {
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
    var defaultIconPath;
    if (SysTrayX.Info.browserInfo.majorVersion < 115) {
      defaultIconPath = "icons/Thunderbird.png";
    } else {
      defaultIconPath = "icons/Thunderbird115.png";
    }

    const { defaultIconMimeUrl, defaultIconBase64Url } = await toDataURL(
      defaultIconPath
    ).then((dataUrl) => {
      const data = dataUrl.split(":").pop().split(",");
      return {
        defaultIconMimeUrl: data[0].split(";")[0],
        defaultIconBase64Url: data[1],
      };
    });

    //  Store default icon (base64)
    await storage().set({
      defaultIconMime: defaultIconMimeUrl,
      defaultIcon: defaultIconBase64Url,
    });

    //  Store in HTML
    const defaultIconDiv = document.getElementById("defaultIcon");
    defaultIconDiv.setAttribute("data-default-icon-mime", defaultIconMimeUrl);
    defaultIconDiv.setAttribute("data-default-icon", defaultIconBase64Url);
  }
}

//
//  Set default unread icon
//  Use <div> as storage
//
async function getIcon() {
  function resolve(result) {
    const iconMime = result.iconMime || "";
    const icon = result.icon || "";
    return { iconMime, icon };
  }

  function reject() {
    return "";
  }

  const { iconMime, icon } = await storage()
    .get(["iconMime", "icon"])
    .then(resolve, reject);

  if (iconMime === "" || icon === "") {
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
    const { iconMimeUrl, iconBase64Url } = await toDataURL(
      "icons/blank-icon.png"
    ).then((dataUrl) => {
      const data = dataUrl.split(":").pop().split(",");
      return { iconMimeUrl: data[0].split(";")[0], iconBase64Url: data[1] };
    });

    //  Store default icon (base64)
    await storage().set({
      iconMime: iconMimeUrl,
      icon: iconBase64Url,
    });

    //  Store in HTML
    const iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon-mime", iconMimeUrl);
    iconDiv.setAttribute("data-icon", iconBase64Url);
  }
}

//
//  Get filters
//
async function getFilters() {
  function resolve(result) {
    let filters = result.filters || undefined;

    if (filters === undefined || filters.length === 0) {
      //
      //  No filters defined, create base filters
      //
      console.debug("Stored filters: undefined or empty");

      filters = [];
      for (const account of SysTrayX.Messaging.accounts) {
        const inbox = account.folders.filter(
          (folder) => folder.type == "inbox"
        );

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

            filters.push({
              unread: true,
              folder: folder,
            });
          } else {
            //          console.debug("Folder 91+: " + JSON.stringify(inbox[0]));

            filters.push({
              accountId: inbox[0].accountId,
              version: SysTrayX.Info.version,
              folders: [inbox[0].path],
            });
          }
        }
      }

      console.debug("Force new filters: " + JSON.stringify(filters));
    } else {
      console.debug("Stored filters: " + JSON.stringify(filters));
    }

    return filters;
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

//
//  Get start app parameters
//
async function getStartAppParam() {
  function resolve(result) {
    const startApp = result.startApp || "";
    const startAppArgs = result.startAppArgs || "";
    return { startApp, startAppArgs };
  }

  function reject() {
    return { startApp: "", startAppArgs: "" };
  }

  return await storage()
    .get(["startApp", "startAppArgs"])
    .then(resolve, reject);
}

//  Helper funcs for TB91 and later folder handling

async function getMailFolderInfo(folder) {
  return await browser.folders.getFolderInfo(folder);
}

// Check if a folder is in the filter list
function isFolderInFilters(folder) {
  const accountIndex = SysTrayX.Messaging.filters.findIndex(
    (account) => account.accountId === folder.accountId
  );

  if (accountIndex !== -1) {
    return (
      SysTrayX.Messaging.filters[accountIndex].folders.filter(
        (path) => path === folder.path
      ).length > 0
    );
  } else {
    return false;
  }
}

// Check if the parent folder of a folder is in the filter list
function isParentFolderInFilters(folder) {
  const parentPath = folder.path.substring(0, folder.path.lastIndexOf("/"));

  const accountIndex = SysTrayX.Messaging.filters.findIndex(
    (account) => account.accountId === folder.accountId
  );

  if (accountIndex !== -1) {
    return (
      SysTrayX.Messaging.filters[accountIndex].folders.filter(
        (path) => path === parentPath
      ).length > 0
    );
  } else {
    return false;
  }
}

// Delete a folder from the filter list
async function deleteFolderFromFilters(folder) {
  const accountIndex = SysTrayX.Messaging.filters.findIndex(
    (account) => account.accountId === folder.accountId
  );

  if (accountIndex !== -1) {
    const account = SysTrayX.Messaging.filters[accountIndex];
    account.folders = account.folders.filter((path) => path !== folder.path);

    //  Store the new filters
    await storage().set({
      filters: SysTrayX.Messaging.filters,
    });
  }
}

// Get the account name from an id
function getAccountName(id) {
  const account = SysTrayX.Messaging.accounts.find(
    (account) => account.id === id
  );

  return account.name;
}

// Add a folder to the filter list
async function addFolderToFilters(newFolder) {
  const accountIndex = SysTrayX.Messaging.filters.findIndex(
    (account) => account.accountId === newFolder.accountId
  );

  if (accountIndex !== -1) {
    const account = SysTrayX.Messaging.filters[accountIndex];
    account.folders.push(newFolder.path);

    //  Store the new filter
    await storage().set({
      filters: SysTrayX.Messaging.filters,
    });
  }
}

//  Count the unread mail
const getUnreadMailCount = () => {
  let count = 0;
  SysTrayX.Messaging.filters.forEach((filter) => {
    const accountId = filter.accountId;
    filter.folders.forEach((path) => {
      if (SysTrayX.Messaging.unread[accountId] !== undefined) {
        if (SysTrayX.Messaging.unread[accountId][path] !== undefined) {
          count = count + SysTrayX.Messaging.unread[accountId][path];
        }
      }
    });
  });

  //console.debug("getUnreadMailCount: " + count);
  SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
};

//  Count the new mail
const getNewMailCount = () => {
  let count = 0;
  SysTrayX.Messaging.filters.forEach((filter) => {
    const accountId = filter.accountId;
    filter.folders.forEach((path) => {
      if (SysTrayX.Messaging.new[accountId] !== undefined) {
        if (SysTrayX.Messaging.new[accountId][path] !== undefined) {
          count = count + SysTrayX.Messaging.new[accountId][path].length;
        }
      }
    });
  });

  //console.debug("Filters: " + JSON.stringify(SysTrayX.Messaging.filters));
  //console.debug("New: " + JSON.stringify(SysTrayX.Messaging.new));

  //console.debug("getNewMailCount: " + count);
  SysTrayX.Link.postSysTrayXMessage({ unreadMail: count });
};
