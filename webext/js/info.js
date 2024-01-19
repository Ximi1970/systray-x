SysTrayX.Info = {
  version: "0",
  platformInfo: {},
  browserInfo: {},

  displayInfo: function () {
    console.debug("Info Addon version: " + SysTrayX.Info.version);
    console.debug(
      "Info Platform: " + JSON.stringify(SysTrayX.Info.platformInfo)
    );
    console.debug("Info Browser: " + JSON.stringify(SysTrayX.Info.browserInfo));
  },
};
