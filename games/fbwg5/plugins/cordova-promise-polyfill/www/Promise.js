cordova.define("cordova-promise-polyfill.Promise", function(require, exports, module) {
if (typeof window !== 'undefined' && !window.Promise) {
  window.Promise = require('./promise.min')
}

});
