var EXPORTED_SYMBOLS = ["windowEvent"];

var windowEvent = {
  value : 0,
  incValue: function() {
    this.value++;
  },
  getValue: function() {
    return this.value;
  },
};

console.log("Loading windowEvent.jsm");
