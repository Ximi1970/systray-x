//
//  Get the prefered storage
//
function storage() {
    console.log("Using local storage");
    return browser.storage.local;
}

//
//  Get API count method preference
//
async function getApiCountMethod() {
  function resolve(result) {
    const apiCountMethod = result.apiCountMethod || "false";
    return apiCountMethod;
  }

  function reject() {
    return "false";
  }

  return await storage().get("apiCountMethod").then(resolve, reject);
}

//
//  Get window startup state
//
async function getStartupType() {
  function resolve(result) {
    const startupType = result.startupType || "0";
    return startupType;
  }

  function reject() {
    return "2";
  }

  return await storage().get("startupType").then(resolve, reject);
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
//  Find a path in the account folder tree
//
function findFolder(rootFolder, path) {
  function traverse(folder) {
    if (folder === undefined) {
      return undefined;
    }

    if (folder.path === path) {
      return folder;
    }

    if (folder.subFolders === undefined) {
      return undefined;
    }

    for (let sub of folder.subFolders) {
      const found = traverse(sub);
      if( found !== undefined ) {
        return found;
      }
    }

    return undefined;
  }

  const found = traverse(rootFolder);

  return found;
}

//
//  Get filters
//
async function getFilters() {
  async function resolve(result) {
    let filters = result.filters || undefined;

    if (filters === undefined || filters.length === 0) {
      //
      //  No filters defined, create base filters
      //
      console.debug("Stored filters: undefined or empty");

      filters = [];
      for (const account of SysTrayX.Messaging.accounts) {
        let inbox;
        if (account.rootFolder !== undefined) {
          inbox = account.rootFolder.subFolders.filter(
            (folder) => folder.type === "inbox"
          );
        } else {
          inbox = account.folders.filter(
            (folder) => folder.type === "inbox"
          );
        }

        if (inbox.length > 0) {
          filters.push({
            accountId: inbox[0].accountId,
            version: SysTrayX.Info.version,
            folders: [{mailFolderId: inbox[0].id, path: inbox[0].path}],
          });
        }
      }

      console.debug("Force new filters: " + JSON.stringify(filters));
    } else {
      console.debug("Stored filters: " + JSON.stringify(filters));

      if (filters[0].folders.length > 0) {
        if (typeof(filters[0].folders[0]) === "string") {

          console.debug("Conversion needed");

          //  Convert the filter folder path list to an object
          //  containing at least the path and if available the mail folder id (TB 121 and newer)

          const newFilters = [];
          for (const account of SysTrayX.Messaging.accounts) {
            const filter = filters.filter(
              (filter) => filter.accountId === account.id
            );

            console.debug("Found filters: " + filter.length);
            console.debug("Found filter: " + JSON.stringify(filter));
    
            if (filter.length === 1) {
              let newFilter = {
                accountId: filter[0].accountId,
                version: SysTrayX.Info.version,
                folders: []
              };

              for (const path of filter[0].folders) {
                const accountFolder = findFolder(account.rootFolder, path)

                if (accountFolder !== undefined)
                {
                  newFilter.folders.push({mailFolderId: accountFolder.id, path: accountFolder.path});
                }
              }

              newFilters.push(newFilter);
            }
          }

          filters = newFilters;

          console.debug("Force new filters: " + JSON.stringify(filters));

          //  Save the new filters
          await storage().set({
            filters: filters,
          });
        } else {
          if (filters[0].folders[0].mailFolderId === undefined &&
                SysTrayX.Info.browserInfo.majorVersion >= 121) {

            console.debug("Update needed");

            //  Update filter folder object
            //  with the the mail folder id

            const newFilters = [];
            for (const account of SysTrayX.Messaging.accounts) {
              const filter = filters.filter(
                (filter) => filter.accountId === account.id
              );
    
              console.debug("Found filters: " + filter.length);
              console.debug("Found filter: " + JSON.stringify(filter));
      
              if (filter.length === 1) {
                let newFilter = {
                  accountId: filter[0].accountId,
                  version: SysTrayX.Info.version,
                  folders: []
                };
      
                for (const folder of filter[0].folders) {
                  const accountFolder = findFolder(account.rootFolder, folder.path)
    
                  if (accountFolder !== undefined)
                  {
                    newFilter.folders.push({mailFolderId: accountFolder.id, path: accountFolder.path});
                  }
                }
    
                newFilters.push(newFilter);
              }
            }
    
            filters = newFilters;

            console.debug("Force new filters: " + JSON.stringify(filters));
    
            //  Save the new filters
            await storage().set({
              filters: filters,
            });
          }
        }
      }
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
    return "0";
  }

  return await storage().get("countType").then(resolve, reject);
}

//
//  Get show new indicator
//
async function getShowNewIndicator() {
  function resolve(result) {
    return result.showNewIndicator || "false";
  }

  function reject() {
    return "false";
  }

  return await storage().get("showNewIndicator").then(resolve, reject);
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
    const folders = SysTrayX.Messaging.filters[accountIndex].folders;

    if (folders[0] === "string") {
      //  Filters pre TB 121
      return (
        folders.filter(
          (path) => path === folder.path
        ).length > 0
      );
    } else {
      //  Filters TB 121
      return (
        folders.map(f => f.path).filter(
          (path) => path === folder.path
        ).length > 0
      );
    }
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
    const folders = SysTrayX.Messaging.filters[accountIndex].folders;

    if (folders[0] === "string") {
      //  Filters pre TB 121
      return (
        folders.filter(
          (path) => path === parentPath
        ).length > 0
      );
    } else {
      //  Filters TB 121
      return (
        folders.map(f => f.path).filter(
          (path) => path === parentPath
        ).length > 0
      );
    }
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

    if (account.folders[0] === "string" ) {
      //  Filters pre TB 121
      account.folders = account.folders.filter((path) => path !== folder.path);
    } else {
      //  Filters TB 121
      account.folders = account.folders.filter((f) => f.path !== folder.path);
    }

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

    if (account.folders[0] === "string" ) {
      account.folders.push(newFolder.path);
    } else {
      account.folders.push({mailFolderId: newFolder.id, path: newFolder.path});
    }

    //  Store the new filter
    await storage().set({
      filters: SysTrayX.Messaging.filters,
    });
  }
}

// Collect unread mail
const collectUnreadMail = async () => {
  for (const filter of SysTrayX.Messaging.filters) {
    
    const accountId = filter.accountId;
    for (const storedFolder of filter.folders) {
      let mailFolderInfo = {};

      let path;
      if (typeof(storedFolder) === "string") {
        //  Filters pre TB 121
        path = storedFolder;

        try {
          mailFolderInfo = await browser.folders.getFolderInfo({
              accountId: accountId,
              path: storedFolder,
            });
        } catch (err) {
          //console.debug("Filter error: " + err);
          //console.debug("Filter error: " + JSON.stringify(folder));

          //  Get all accounts
          SysTrayX.Messaging.accounts = await browser.accounts.list();

          // Check the filters for the accounts
          SysTrayX.Messaging.accountFilterCheck();
        }
      } else {
        //  Filters TB 121
        path = storedFolder.path;
        
        try {
          mailFolderInfo = await browser.folders.getFolderInfo(storedFolder.mailFolderId);
        } catch (err) {
          //console.debug("Filter error: " + err);
          //console.debug("Filter error: " + JSON.stringify(folder));

          //  Get all accounts
          SysTrayX.Messaging.accounts = await browser.accounts.list();

          // Check the filters for the accounts
          SysTrayX.Messaging.accountFilterCheck();
        }
      }

      if (mailFolderInfo.unreadMessageCount !== undefined) {
        if (SysTrayX.Messaging.unread[accountId] === undefined) {
          SysTrayX.Messaging.unread[accountId] = {};
        }

        if (SysTrayX.Messaging.new[accountId] === undefined) {
          SysTrayX.Messaging.new[accountId] = {};
        }

        if (
          SysTrayX.Messaging.new[accountId][path] === undefined
        ) {
          SysTrayX.Messaging.new[accountId][path] = [];
        }

        SysTrayX.Messaging.unread[accountId][path] =
          mailFolderInfo.unreadMessageCount;
      }
    }
  }
};

//  Count and send the unread and new mails
const sendMailCountPre115 = () => {
  if (SysTrayX.Info.browserInfo.majorVersion < 115 || SysTrayX.Messaging.apiCountMethod === "false") {

    // Collect the unread mail
    collectUnreadMail();

    // Count the collected mail
    let unreadCount = 0;
    let newCount = 0;
    SysTrayX.Messaging.filters.forEach((filter) => {
      const accountId = filter.accountId;
      filter.folders.forEach((storedFolder) => {
        let path;
        if (typeof(storedFolder) === "string") {
          //  Filters pre TB 121
          path = storedFolder;
        } else {
          //  Filters TB 121
          path = storedFolder.path;
        }

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
  if (SysTrayX.Info.browserInfo.majorVersion >= 115 && SysTrayX.Messaging.apiCountMethod === "true") {

    // New only works for >=TB106 

    let unreadCount = 0;
    let newCount = 0;

    if( SysTrayX.Messaging.countType === "1" || SysTrayX.Messaging.showNewIndicator === "true" ) {

      //  Get both unread and new message count

      if (SysTrayX.Info.browserInfo.majorVersion < 121) {
        for (const filter of SysTrayX.Messaging.filters) {
          for (const storedFolder of filter.folders) {
            let listParam;
            if (typeof(storedFolder) === "string") {
              //  Filters pre TB 121
              listParam = {
                accountId: filter.accountId,
                path: storedFolder,
              };  
            } else {
              //  Filters TB 121
              listParam = storedFolder.mailFolderId;
            }

            async function* listMessages(listParam) {
              let page = await messenger.messages.list(listParam);
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
            
            let messages = listMessages(listParam);
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
      } else {

        //  Unread and new count TB 121 and newer

        for (const filter of SysTrayX.Messaging.filters) {
          for (const folder of filter.folders) {
            let mailFolderInfo = {};
            try {
              mailFolderInfo = await browser.folders.getFolderInfo(folder.mailFolderId);
            } catch (err) {
              //console.debug("Filter error: " + err);
              //console.debug("Filter error: " + JSON.stringify(folder));
      
              //  Get all accounts
              SysTrayX.Messaging.accounts = await browser.accounts.list();
      
              // Check the filters for the accounts
              SysTrayX.Messaging.accountFilterCheck();
            }

            if (mailFolderInfo.unreadMessageCount !== undefined) {
              unreadCount = unreadCount + mailFolderInfo.unreadMessageCount;
            }

            if (mailFolderInfo.newMessageCount !== undefined) {
              newCount = newCount + mailFolderInfo.newMessageCount;
            }
          }
        }
      }
    } else {

      //  Only unread count

      for (const filter of SysTrayX.Messaging.filters) {
        for (const storedFolder of filter.folders) {
          let folderParam;
          if (typeof(storedFolder) === "string") {
            //  Filters pre TB 121
            folderParam = {
              accountId: filter.accountId,
              path: storedFolder,
            };
          } else {
            //  Filters TB 121
            folderParam = storedFolder.mailFolderId;
          }
    
          let mailFolderInfo = {};
          try {
            mailFolderInfo = await browser.folders.getFolderInfo(folder);
          } catch (err) {
            //console.debug("Filter error: " + err);
            //console.debug("Filter error: " + JSON.stringify(folder));
    
            //  Get all accounts
            SysTrayX.Messaging.accounts = await browser.accounts.list();
    
            // Check the filters for the accounts
            SysTrayX.Messaging.accountFilterCheck();
          }

          if (mailFolderInfo.unreadMessageCount !== undefined) {
            unreadCount = unreadCount + mailFolderInfo.unreadMessageCount;
          }
        }
      }
    }

    //console.debug("sendMailCount Unread: " + unreadCount);
    //console.debug("sendMailCount New: " + newCount);
    
    SysTrayX.Link.postSysTrayXMessage( { mailCount: { unread: unreadCount, new: newCount } } );
  }
};
