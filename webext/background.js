console.log("Starting background.js");

var SysTrayX = {};

SysTrayX.Messaging = {

  initialized: false,
  
  unreadFilter: { unread: true },
  
  init: function() {
    if (this.initialized) {
      console.log("Messaging already initialized");
      return;
    }
    console.log("Enabling Messaging");

    this.unReadMessages(this.unreadFilter).then(this.unreadCb);

    this.initialized = true;
  },
  
  /*
   * Use the messages API to get the unread messages (Promise)
   * Be aware that the data is only avaiable inside the callback
   */
  unReadMessages: async function(filter) {
    let page = await browser.messages.query(filter);
    
    let unread = page.messages.length;
    
    while (page.id) {
      page = await browser.messages.continueList(page.id);

      unread = unread + page.messages.length
    }

    return unread;
  },
  
  /*
   * Callback for unReadMessages
   */
  unreadCb: function(count) {
    console.log("SysTrayX unread "+count);
  }
  
};

console.log("Starting SysTray-X");

SysTrayX.Messaging.init();

console.log("Done");
