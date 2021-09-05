SysTrayX.Info = {
  version: "0",
  platformInfo: {},
  browserInfo: {},

  storageType: "",

  displayInfo: function () {
    console.debug("Info Addon version: " + SysTrayX.Info.version);
    console.debug(
      "Info Platform: " + JSON.stringify(SysTrayX.Info.platformInfo)
    );
    console.debug("Info Browser: " + JSON.stringify(SysTrayX.Info.browserInfo));
    //    console.debug("Info Storage: " + SysTrayX.Info.storageType);
  },
};
