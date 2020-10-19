const addon = require('./util').requireAddon('profiler')

module.exports = {
  start: addon.start,
  stop(name) {
    return JSON.stringify(addon.stop(name))
  },
}
