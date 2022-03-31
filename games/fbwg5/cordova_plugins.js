cordova.define('cordova/plugin_list', function(require, exports, module) {
module.exports = [
  {
    "id": "cc.fovea.cordova.purchase.InAppPurchase",
    "file": "plugins/cc.fovea.cordova.purchase/www/store-windows.js",
    "pluginId": "cc.fovea.cordova.purchase",
    "clobbers": [
      "store"
    ]
  },
  {
    "id": "cc.fovea.cordova.purchase.InAppPurchaseProxy",
    "file": "plugins/cc.fovea.cordova.purchase/src/windows/InAppPurchaseProxy.js",
    "pluginId": "cc.fovea.cordova.purchase",
    "merges": [
      ""
    ]
  },
  {
    "id": "cordova-promise-polyfill.Promise",
    "file": "plugins/cordova-promise-polyfill/www/Promise.js",
    "pluginId": "cordova-promise-polyfill",
    "runs": true
  },
  {
    "id": "cordova-promise-polyfill.promise.min",
    "file": "plugins/cordova-promise-polyfill/www/promise.min.js",
    "pluginId": "cordova-promise-polyfill"
  },
  {
    "id": "cordova-plugin-admob-free.AdMob",
    "file": "plugins/cordova-plugin-admob-free/www/admob.js",
    "pluginId": "cordova-plugin-admob-free",
    "clobbers": [
      "admob",
      "AdMob",
      "plugins.AdMob"
    ]
  },
  {
    "id": "cordova-plugin-splashscreen.SplashScreen",
    "file": "plugins/cordova-plugin-splashscreen/www/splashscreen.js",
    "pluginId": "cordova-plugin-splashscreen",
    "clobbers": [
      "navigator.splashscreen"
    ]
  },
  {
    "id": "cordova-plugin-splashscreen.SplashScreenProxy",
    "file": "plugins/cordova-plugin-splashscreen/www/windows/SplashScreenProxy.js",
    "pluginId": "cordova-plugin-splashscreen",
    "runs": true
  },
  {
    "id": "cordova-plugin-vibration.VibrationProxy",
    "file": "plugins/cordova-plugin-vibration/src/windows/VibrationProxy.js",
    "pluginId": "cordova-plugin-vibration",
    "runs": true
  },
  {
    "id": "cordova-plugin-vibration.notification",
    "file": "plugins/cordova-plugin-vibration/www/vibration.js",
    "pluginId": "cordova-plugin-vibration",
    "merges": [
      "navigator"
    ]
  }
];
module.exports.metadata = 
// TOP OF METADATA
{
  "cc.fovea.cordova.purchase": "7.2.4",
  "cordova-admob-sdk": "0.21.0",
  "cordova-promise-polyfill": "0.0.2",
  "cordova-plugin-admob-free": "0.23.0",
  "cordova-plugin-splashscreen": "5.0.2",
  "cordova-plugin-vibration": "3.1.0",
  "cordova-plugin-whitelist": "1.3.3"
};
// BOTTOM OF METADATA
});