/// This is the important part. It implements the functions and events defined in schema.json.
// The variable must have the same name you've been using so far, "myapi" in this case.
var windowEvent = class extends ExtensionCommon.ExtensionAPI {
  getAPI(context) {
    console.log("windowEvent API started");

    // To be notified of the extension going away, call callOnClose with any object that has a
    // close function, such as this one.
    context.callOnClose(this);

    return {
      // Again, this key must have the same name.
      windowEvent: {
        setCloseType: async function (type) {
          windowListener.setCloseType(type);
        },

        // An event. Most of this is boilerplate you don't need to worry about, just copy it.
        onCloseButtonClick: new ExtensionCommon.EventManager({
          context,
          name: "windowEvent.onCloseButtonClick",
          // In this function we add listeners for any events we want to listen to, and return a
          // function that removes those listeners. To have the event fire in your extension,
          // call fire.async.
          register(fire) {
            function callback(event) {
              return fire.async();
            }

            windowListener.addOnCloseButton(callback);
            return function () {
              windowListener.removeOnCloseButton(callback);
            };
          },
        }).api(),
      },
    };
  }

  close() {
    // This function is called if the extension is disabled or removed, or Thunderbird closes.
    // We registered it with callOnClose, above.
    console.log("windowEvent API closed");

    // Unload the JSM we imported above. This will cause Thunderbird to forget about the JSM, and
    // load it afresh next time `import` is called. (If you don't call `unload`, Thunderbird will
    // remember this version of the module and continue to use it, even if your extension receives
    // an update.) You should *always* unload JSMs provided by your extension.
    Cu.unload(extension.getURL("modules/windowEvent.jsm"));
  }
};

// A helpful class for listening to windows opening and closing.
// (This file had a lowercase E in Thunderbird 65 and earlier.)
var { ExtensionSupport } = ChromeUtils.import(
  "resource:///modules/ExtensionSupport.jsm"
);

// This object is just what we're using to listen for toolbar clicks. The implementation isn't
// what this example is about, but you might be interested as it's a common pattern. We count the
// number of callbacks waiting for events so that we're only listening if we need to be.
var windowListener = new (class extends ExtensionCommon.EventEmitter {
  constructor() {
    super();
    this.callbackOnCloseButtonCount = 0;
    this.callbackOnLoadWindowCount = 0;

    this.MESSAGE_CLOSE_TYPE_DEFAULT = 0;
    this.MESSAGE_CLOSE_TYPE_MIN_MAIN_CLOSE_CHILDREN = 1;
    this.MESSAGE_CLOSE_TYPE_MIN_ALL = 2;

    this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_MAIN_CLOSE_CHILDREN;
  }

  setCloseType(closeType) {
    if (closeType === 0) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_DEFAULT;
    } else if (closeType === 1) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_MAIN_CLOSE_CHILDREN;
    } else if (closeType === 2) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_ALL;
    } else console.log("Unknown close type: " + closeType);
  }

  addOnCloseButton(callback) {
    if (this.callbackOnCloseButtonCount == 0) {
      this.on("close-clicked", callback);
      this.callbackOnCloseButtonCount++;

      ExtensionSupport.registerWindowListener("closeButtonListener", {
        chromeURLs: [
          "chrome://messenger/content/messenger.xhtml",
          "chrome://messenger/content/messenger.xul",
        ],
        onLoadWindow: function (window) {
          windowListener.callbackOnLoadWindowCount++;
          if (
            windowListener.callbackOnLoadWindowCount === 1 ||
            windowListener.closeType ===
              windowListener.MESSAGE_CLOSE_TYPE_MIN_ALL
          ) {
            window.addEventListener(
              "close",
              windowListener.onCloseButton,
              true
            );
          }
        },
      });
    }
  }

  removeOnCloseButton(callback) {
    if (this.callbackOnCloseButtonCount == 1) {
      this.off("close-clicked", callback);
      this.callbackOnCloseButtonCount--;

      for (let window of ExtensionSupport.openWindows) {
        if (
          [
            "chrome://messenger/content/messenger.xhtml",
            "chrome://messenger/content/messenger.xul",
          ].includes(window.location.href)
        ) {
          window.removeEventListener(
            "close",
            windowListener.onCloseButton,
            true
          );
        }
      }
      ExtensionSupport.unregisterWindowListener("closeButtonListener");
    }
  }

  onCloseButton(event) {
    windowListener.emit("close-clicked");
    if (event) event.preventDefault();
    return true;
  }
})();
