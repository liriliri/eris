// Built by eustia.
/* eslint-disable */

var _ = {};

/* ------------------------------ requireAddon ------------------------------ */
_.requireAddon = (function (exports) {
    exports = function (name) {
      try {
        return require('./build/Release/' + name + '.node')
      } catch (e) {
        if (e.code !== 'MODULE_NOT_FOUND') throw e
        return require('./build/Debug/' + name + '.node')
      }
    }

    return exports;
})({});

module.exports = _;