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
      this.onNewWindowCallbackCount = 0;
      this.onCloseButtonClickCallbackCount = 0;

      this.closeType = this.MESSAGE_CLOSE_TYPE_DEFAULT;
      this.oldClose = undefined;
    }

    get listenerIdNewWindow() {
      return `window_event_listener_new_window_${this.extension.uuid}_${this.extension.instanceId}`;
    }

    get listenerIdCloseButton() {
      return `window_event_listener_close_button_${this.extension.uuid}_${this.extension.instanceId}`;
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
 
    addOnNewWindow( callback ) {
      // Registering the callback for "new-window".
      this.on("new-window", callback);
      this.onNewWindowCallbackCount++;

      if (this.onNewWindowCallbackCount == 1) {
        ExtensionSupport.registerWindowListener( this.listenerIdNewWindow, {
          chromeURLs: [
            "chrome://messenger/content/messenger.xhtml",
            "chrome://messenger/content/messenger.xul",
          ],
          onLoadWindow: function ( window ) {

            windowListener.emit( "new-window" );

            console.log("New window added");
          },
        });
      }
    }

    removeOnNewWindow( callback ) {
      // Un-Registering the callback for "new-window".
      this.off("new-window", callback);
      this.onNewWindowCallbackCount--;

      if (this.onNewWindowCallbackCount == 0) {
        for (let window of ExtensionSupport.openWindows) {
          if ( [
            "chrome://messenger/content/messenger.xhtml",
            "chrome://messenger/content/messenger.xul",
          ].includes( window.location.href ) ) {
            console.log( "New window listener removed" );
          }
        }
        ExtensionSupport.unregisterWindowListener( this.listenerIdNewWindow );
      }
    }


    onCloseButton( event ) {
      if ( event ) event.preventDefault();
      windowListener.emit( "close-clicked" );
      return true;
    }
  
    addOnCloseButtonClick( callback, context, window ) {
      // Registering the callback for "close-clicked".
      this.on( "close-clicked", callback );
      this.onCloseButtonClickCallbackCount++;

      if (this.onCloseButtonClickCallbackCount == 1) {
        ExtensionSupport.registerWindowListener( this.listenerIdCloseButton, {
          chromeURLs: [
            "chrome://messenger/content/messenger.xhtml",
            "chrome://messenger/content/messenger.xul",
          ],
          onLoadWindow: function ( window ) {
            window.addEventListener(
              "close",
              windowListener.onCloseButton,
              true
            );

            windowListener.oldClose = window.close;
            window.close = () => windowListener.onCloseButton( null );

//            console.debug("Window: " + JSON.stringify(window));

/*
console.debug( "Window id1: " + window.id );
console.debug( "Window id1: " + window.windowId );

            let windowId2 = getInnerWindowID(window);
            console.debug( "Window id2: " + windowId2 );

            // Get a real window from a window ID:
            let windowObject = context.extension.windowManager.get(windowId2);
//            let windowObject = context.extension.windowManager.get(windowId);
            let realWindow = windowObject.window;

            console.debug( "WinObj: " + JSON.stringify( windowObject ) );

            // Get a window ID from a real window:
            const id = context.extension.windowManager.getWrapper(realWindow).id;

            console.debug( "WinObj Real: " + id );

            // Get all windows: (note this returns a Generator, not an array like the API)
//            context.extension.windowManager.getAll();




*/
            console.log( "Close listener added" );
          },
        });
      }
    }

    removeOnCloseButtonClick( callback ) {
      // Un-Registering the callback for "close-clicked".
      this.off( "close-clicked", callback );
      this.onCloseButtonClickCallbackCount--;

      if ( this.onCloseButtonClickCallbackCount == 0 ) {
        for ( let window of ExtensionSupport.openWindows ) {
          if ( [
            "chrome://messenger/content/messenger.xhtml",
            "chrome://messenger/content/messenger.xul",
          ].includes( window.location.href ) ) {
            window.removeEventListener(
              "close",
              windowListener.onCloseButton,
              true
            );

            window.close = windowListener.oldClose;

            console.log( "Close listener removed" );
          }
        }
        ExtensionSupport.unregisterWindowListener( this.listenerIdCloseButton );
      }
    }
  };

  // This is the important part. It implements the functions and events defined
  // in the schema.json. The name must match what you've been using so far,
  // "windowEvent2" in this case.
  class windowEvent2 extends ExtensionCommon.ExtensionAPI {
    // An alternative to defining a constructor here, is to use the onStartup
    // event. However, this causes the API to be instantiated directly after the
    // add-on has been loaded, not when the API is first used. Depends on what is
    // desired.
    constructor(extension) {
      super(extension);
      windowListener = new WindowListener(extension);
    }

    getAPI(context) {
      console.log("windowEvent API started");

      return {
        // This key must match the class name.
        windowEvent2: {
          setCloseType: async function (type) {
            windowListener.setCloseType(type);
          },

          // An event. Most of this is boilerplate you don't need to worry about, just copy it.
          onNewWindow: new ExtensionCommon.EventManager({
            context,
            name: "windowEvent2.onNewWindow",
            // In this function we add listeners for any events we want to listen to, and return a
            // function that removes those listeners. To have the event fire in your extension,
            // call fire.async.
            register(fire) {
              function callback(event) {
                return fire.async();
              }

              windowListener.addOnNewWindow(callback);
              return function () {
                windowListener.removeOnNewWindow(callback);
              };
            },
          }).api(),

          // An event. Most of this is boilerplate you don't need to worry about, just copy it.
          onCloseButtonClick: new ExtensionCommon.EventManager({
            context,
            name: "windowEvent2.onCloseButtonClick",
            // In this function we add listeners for any events we want to listen to, and return a
            // function that removes those listeners. To have the event fire in your extension,
            // call fire.async.
            register(fire) {
              function callback(event) {
                return fire.async();
              }

              windowListener.addOnCloseButtonClick(callback,context);
              return function () {
                windowListener.removeOnCloseButtonClick(callback);
              };
            },
          }).api(),

        },
      };
    }

    onShutdown(isAppShutdown) {
      // This function is called if the extension is disabled or removed, or Thunderbird closes.
      // We usually do not have to do any cleanup, if Thunderbird is shutting down entirely
      if (isAppShutdown) {
        return;
      }

      console.log("windowEvent API closed");
    }
  };

  // Export the api by assigning in to the exports parameter of the anonymous closure
  // function, which is the global this.
  exports.windowEvent2 = windowEvent2;

})(this)
