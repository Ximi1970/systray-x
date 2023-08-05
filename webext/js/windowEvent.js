// This is the important part. It implements the functions and events defined in schema.json.
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
    this.MESSAGE_CLOSE_TYPE_MIN_MAIN_TRAY_CLOSE_CHILDREN = 3;
    this.MESSAGE_CLOSE_TYPE_MIN_ALL_TRAY = 4;

    this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_MAIN_CLOSE_CHILDREN;
  }

  setCloseType(closeType) {
    if (closeType === 0) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_DEFAULT;
    } else if (closeType === 1) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_MAIN_TRAY_CLOSE_CHILDREN;
    } else if (closeType === 2) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_ALL_TRAY;
    } else if (closeType === 3) {
      this.closeType = this.MESSAGE_CLOSE_TYPE_MIN_MAIN_CLOSE_CHILDREN;
    } else if (closeType === 4) {
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
              windowListener.MESSAGE_CLOSE_TYPE_MIN_ALL ||
            windowListener.closeType ===
              windowListener.MESSAGE_CLOSE_TYPE_MIN_ALL_TRAY
          ) {
            window.addEventListener(
              "close",
              windowListener.onCloseButton,
              true
            );
            windowListener.hijackTitlebarCloseButton(window);

            windowListener.oldClose = window.close;
            window.close = () => windowListener.onCloseButton(null);

            console.log("Close listener added");
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
          window.close = windowListener.oldClose;

          console.log("Close listener removed");
        }
      }
      ExtensionSupport.unregisterWindowListener("closeButtonListener");
    }
  }

  onCloseButton(event) {
    if (event) event.preventDefault();
    windowListener.emit("close-clicked");
    return true;
  }

  hijackTitlebarCloseButton(window) {
    if (
      windowListener.replaceCommand(window, "titlebar-close", function () {
        return windowListener.onCloseButton(null);
      })
    ) {
      console.log("replaced command= " + "titlebar-close");
    }
  }

  replaceCommand(window, eltId, gotHidden) {
    let elt = window.document.getElementById(eltId);
    if (!elt) {
      console.log("Element '" + eltId + "' not found. Command not replaced.");
      return false;
    }

    let prevent = null;
    if (elt.command) {
      prevent = {
        event: "click",
        func: function (e) {
          e.preventDefault();
        },
      };
    } else if (elt.getAttribute("oncommand")) {
      prevent = {
        event: "command",
        func: function (e) {
          e.stopPropagation();
        },
      };
    } else {
      console.warn("Could not replace oncommand on " + eltId);
      return false;
    }

    let callback = function (event) {
      if (event.target.id === eltId) {
        console.debug(prevent.event + " on " + eltId);
        if (gotHidden()) prevent.func(event);
      }
    };

    /* We put listeners on the "titlebar" parent node, because:
     - we can hardly short-circuit command/oncommand (probably because they are
       registered first)
     - we'd have otherwise to alter "oncommand"/"command" attribute and use
       Function(), which do not pass review nowadays. */
    elt.parentNode.addEventListener(prevent.event, callback, true);

    return true;
  }
})();
