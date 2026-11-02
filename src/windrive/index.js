function requireAddon(name) {
  try {
    return require('./build/Release/' + name + '.node')
  } catch (e) {
    if (e.code !== 'MODULE_NOT_FOUND') throw e
    return require('./build/Debug/' + name + '.node')
  }
}

const addon = requireAddon('windrive')

function getLogicalDrives() {
  return addon.getLogicalDrives()
}

function getDriveLetters() {
  return getLogicalDrives().map((drive) => drive.name)
}

module.exports = {
  getLogicalDrives,
  getDriveLetters,
}
