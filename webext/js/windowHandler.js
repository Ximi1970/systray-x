/* eslint-disable object-shorthand */

"use strict";

// Using a closure to not leak anything but the API to the outside world.
(function (exports) {

  // Get various parts of the WebExtension framework that we need.
  var { ExtensionCommon } = ChromeUtils.import("resource://gre/modules/ExtensionCommon.jsm");

  // You probably already know what this does.
  var { Services } = ChromeUtils.import("resource://gre/modules/Services.jsm");

  // A helpful class for listening to windows opening and closing.
  var { ExtensionSupport } = ChromeUtils.import("resource:///modules/ExtensionSupport.jsm");

  /**
   * This object is just what we're using to listen for toolbar clicks. The implementation
   * isn't what this example is about, but you might be interested as it's a common pattern.
   * We count the number of callbacks waiting for events so that we're only listening if we
   * need to be.
   * 
   * An EventEmitter has the following basic functions:
   * 
   * EventEmitter.on(emitterName, callback)
   *   Registers a callback for a custom emitter.
   * 
   * EventEmitter.off(emitterName, callback)
   *   Unregisters a callback for a custom emitter.
   * 
   * EventEmitter.emit(emitterName)
   *   Emit a custom emitter, all provided parameters will be forwarded to the registered callbacks.
   */

  let windowListener;

  class WindowListener extends ExtensionCommon.EventEmitter {
    constructor(extension) {
      super();
      this.extension = extension;
      this.callbackCount = 0;
    }
  }

  // This is the important part. It implements the functions and events defined
  // in the schema.json. The name must match what you've been using so far,
  // "windowEvent2" in this case.
  class windowHandler extends ExtensionCommon.ExtensionAPI {
    // An alternative to defining a constructor here, is to use the onStartup
    // event. However, this causes the API to be instantiated directly after the
    // add-on has been loaded, not when the API is first used. Depends on what is
    // desired.
    constructor(extension) {
      super(extension);
      windowListener = new WindowListener(extension);
    }

    getAPI(context) {
      console.log("windowHandler API started");

      return {
        // This key must match the class name.
        windowHandler: {
          getWindowId: async function (windowId) {

            // Get a real window from a window ID:
            let windowObject = context.extension.windowManager.get(windowId);
            let realWindow = windowObject.window;

            // Get all windows: (note this returns a Generator, not an array like the API)
            let allWindowObjects = await context.extension.windowManager.getAll();

            for( wo in allWindowObjects ) {
              console.debug("Window: " + JSON.stringify( context.extension.windowManager.getWrapper(wo.window).id) );
            }


            // Get a window ID from a real window:
            let id = context.extension.windowManager.getWrapper(realWindow).id;

            return id;
          }
        }
      };
    }

    close() {
      console.log("windowHandler API closed");
    }
  };

  // Export the api by assigning in to the exports parameter of the anonymous closure
  // function, which is the global this.
  exports.windowHandler = windowHandler;

})(this)
