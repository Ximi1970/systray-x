//
//  Get the prefered storage
//
function storage() {
    console.log("Using local storage");
    return browser.storage.local;
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

          //          console.debug("Folder 91+: " + JSON.stringify(inbox[0]));

          filters.push({
            accountId: inbox[0].accountId,
            version: SysTrayX.Info.version,
            folders: [inbox[0].path],
          });
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

// Collect unread mail
const collectUnreadMail = async () => {
  for (const filter of SysTrayX.Messaging.filters) {
    for (const path of filter.folders) {
      const folder = {
        accountId: filter.accountId,
        path: path,
      };

      let mailFolderInfo = {};
      try {
        mailFolderInfo = await browser.folders.getFolderInfo(folder);
      } catch (err) {
        console.debug("Filter error: " + err);
        console.debug("Filter error: " + JSON.stringify(folder));
      }

      if (mailFolderInfo.unreadMessageCount !== undefined) {
        if (SysTrayX.Messaging.unread[folder.accountId] === undefined) {
          SysTrayX.Messaging.unread[folder.accountId] = {};
        }

        if (SysTrayX.Messaging.new[folder.accountId] === undefined) {
          SysTrayX.Messaging.new[folder.accountId] = {};
        }

        if (
          SysTrayX.Messaging.new[folder.accountId][folder.path] === undefined
        ) {
          SysTrayX.Messaging.new[folder.accountId][folder.path] = [];
        }

        SysTrayX.Messaging.unread[folder.accountId][folder.path] =
          mailFolderInfo.unreadMessageCount;
      }
    }
  }
};

//  Count and send the unread and new mails
const sendMailCountPre115 = () => {
  if (SysTrayX.Info.browserInfo.majorVersion < 115) {

    // Collect the unread mail
    collectUnreadMail();

    // Count the collected mail
    let unreadCount = 0;
    let newCount = 0;
    SysTrayX.Messaging.filters.forEach((filter) => {
      const accountId = filter.accountId;
      filter.folders.forEach((path) => {
        if (SysTrayX.Messaging.unread[accountId] !== undefined) {
          if (SysTrayX.Messaging.unread[accountId][path] !== undefined) {
            unreadCount = unreadCount + SysTrayX.Messaging.unread[accountId][path];
          }
        }

        if (SysTrayX.Messaging.new[accountId] !== undefined) {
          if (SysTrayX.Messaging.new[accountId][path] !== undefined) {
            newCount = newCount + SysTrayX.Messaging.new[accountId][path].length;
          }
        }
      });
    });

    //console.debug("Filters: " + JSON.stringify(SysTrayX.Messaging.filters));
    //console.debug("New: " + JSON.stringify(SysTrayX.Messaging.new));

    //console.debug("sendMailCountPre115 Unread: " + unreadCount);
    //console.debug("sendMailCountPre115 New: " + newCount);

    SysTrayX.Link.postSysTrayXMessage( { mailCount: { unread: unreadCount, new: newCount } } );
  }
};

//  Count and send the unread and new mails (>TB115)
const sendMailCount = async () => {
  if (SysTrayX.Info.browserInfo.majorVersion >= 115) {

      // New only works for >=TB106 

    let unreadCount = 0;
    let newCount = 0;

    for (const filter of SysTrayX.Messaging.filters) {
      for (const path of filter.folders) {
        const folder = {
          accountId: filter.accountId,
          path: path,
        };

        async function* listMessages(folder) {
          let page = await messenger.messages.list(folder);
          for (let message of page.messages) {
            yield message;
          }
        
          while (page.id) {
            page = await messenger.messages.continueList(page.id);
            for (let message of page.messages) {
              yield message;
            }
          }
        }
        
        let messages = listMessages(folder);
        for await (let message of messages) {
          if( message.new )
          {
            newCount = newCount + 1;
          }

          if( !message.read )
          {
            unreadCount = unreadCount + 1;
          }
        }
      }
    }

    //console.debug("sendMailCount Unread: " + unreadCount);
    //console.debug("sendMailCount New: " + newCount);
    
    SysTrayX.Link.postSysTrayXMessage( { mailCount: { unread: unreadCount, new: newCount } } );
  }
};
