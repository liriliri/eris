function requireAddon(name) {
  try {
    return require('./build/Release/' + name + '.node')
  } catch (e) {
    if (e.code !== 'MODULE_NOT_FOUND') throw e
    return require('./build/Debug/' + name + '.node')
  }
}

const addon = requireAddon('profiler')

module.exports = {
  start: addon.start,
  stop(name) {
    return JSON.stringify(addon.stop(name))
  },
}
