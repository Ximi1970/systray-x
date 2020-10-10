//
//  Create folder tree from accounts
//
function getFolderTree(mailAccounts, browserInfo) {
  function traverse(accountName, path, folders) {
    if (!folders) {
      return;
    }
    for (let f of folders) {
      f.accountName = accountName;
      f.path = path + "/" + f.name;
      traverse(accountName, path + "/" + f.name, f.subFolders);
    }
  }

  function createFolderTreePre74(accountName, folders) {
    let result = [];
    let level = { result };

    folders.forEach((folder) => {
      folder.path
        .slice(1)
        .split("/")
        .reduce((r, name, i, a) => {
          if (!r[name]) {
            r[name] = { result: [] };
            r.result.push({
              accountId: folder.accountId,
              name: folder.name,
              subFolders: r[name].result,
            });
          }

          return r[name];
        }, level);
    });

    traverse(accountName, "", result);
    return result;
  }

  function createFolderTree(accountName, folders) {
    traverse(accountName, "", folders);
    return folders;
  }

  let accounts = new Object();

  for (let i = 0; i < mailAccounts.length; i++) {
    if (false) {
      console.debug("Accounts id: " + mailAccounts[i].id);
      console.debug("Accounts name: " + mailAccounts[i].name);
      console.debug("Accounts type: " + mailAccounts[i].type);
      for (let j = 0; j < mailAccounts[i].folders.length; j++) {
        console.debug(
          "Accounts folder path: " + mailAccounts[i].folders[j].path
        );
        console.debug(
          "Accounts folder name: " + mailAccounts[i].folders[j].name
        );
        console.debug(
          "Accounts folder type: " + mailAccounts[i].folders[j].type
        );
      }
    }

    if (!accounts[mailAccounts[i].type]) {
      accounts[mailAccounts[i].type] = [];
    }
    accounts[mailAccounts[i].type].push({
      id: mailAccounts[i].id,
      name: mailAccounts[i].name,
      folders: mailAccounts[i].folders,
    });
  }

  /*
   *  Build tree
   */
  let folderTree = {};
  for (let prop in accounts) {
    if (accounts[prop]) {
      for (let i = 0; i < accounts[prop].length; ++i) {
        //  Create a usable folder tree
        let folders = [];
        if (browserInfo.version.split(".")[0] < 74) {
          //  Pre TB74 accounts API
          folders = createFolderTreePre74(
            accounts[prop][i].name,
            accounts[prop][i].folders
          );
        } else {
          //  TB74+ accounts API, (this shit never ends...)
          folders = createFolderTree(
            accounts[prop][i].name,
            accounts[prop][i].folders
          );
        }

        //  Store the tree
        folderTree[accounts[prop][i].name] = folders;
      }
    }
  }

  return folderTree;
}

//
//  Create a paths array from the folder tree
//
function getFolderPaths(folders) {
  function traverse(folders) {
    if (!folders) {
      return;
    }
    for (let f of folders) {
      paths.push(f.path);
      traverse(f.subFolders);
    }
  }

  let paths = [];
  traverse(folders);

  return paths;
}

//
//  Find the difference between two path arrays
//
function findFolderPathsDiff(paths1, paths2) {
  let diff = [];

  paths1.forEach((path) => {
    if (paths2.includes(path) === false) {
      diff.push(path);
    }
  });

  paths2.forEach((path) => {
    if (paths1.includes(path) === false) {
      diff.push(path);
    }
  });

  return diff;
}

//
//  Is the path in the filter folder list?
//
function findFolderPath(filters, path) {
  for (const filter of filters) {
    if (filter.folder.path === path) {
      return filter;
    }
  }
  return undefined;
}
