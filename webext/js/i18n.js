/*
  license: The MIT License, Copyright (c) 2016-2018 YUKI "Piro" Hiroshi
  original:
    http://github.com/piroor/webextensions-lib-l10n
*/
var i18n = {
  updateString(aString) {
    return aString.replace(/__MSG_(.+?)__/g, (aMatched) => {
      const key = aMatched.slice(6, -2);
      return browser.i18n.getMessage(key) || aMatched;
    });
  },

  updateDocument() {
    const texts = document.evaluate(
      'descendant::text()[contains(self::text(), "__MSG_")]',
      document,
      null,
      XPathResult.ORDERED_NODE_SNAPSHOT_TYPE,
      null
    );
    for (let i = 0, maxi = texts.snapshotLength; i < maxi; i++) {
      const text = texts.snapshotItem(i);
      text.nodeValue = this.updateString(text.nodeValue);
    }

    const attributes = document.evaluate(
      'descendant::*/attribute::*[contains(., "__MSG_")]',
      document,
      null,
      XPathResult.ORDERED_NODE_SNAPSHOT_TYPE,
      null
    );
    for (let i = 0, maxi = attributes.snapshotLength; i < maxi; i++) {
      const attribute = attributes.snapshotItem(i);
      attribute.value = this.updateString(attribute.value);
    }
  }
};

document.addEventListener('DOMContentLoaded', () => {
  i18n.updateDocument();
}, { once: true });
