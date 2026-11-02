const { getLogicalDrives, getDriveLetters } = require('./')
const expect = require('chai').expect
const isWindows = require('licia/isWindows')

describe('windrive', () => {
  it('getLogicalDrives', () => {
    if (!isWindows) {
      expect(() => getLogicalDrives()).to.throw(/Windows/)
      return
    }

    const drives = getLogicalDrives()
    expect(drives).to.be.an('array').that.is.not.empty

    for (const drive of drives) {
      expect(drive)
        .to.have.property('name')
        .that.matches(/^[A-Z]:\\$/)
      expect(drive).to.have.property('type').that.is.a('string')
    }
  })

  it('getDriveLetters', () => {
    if (!isWindows) {
      expect(() => getDriveLetters()).to.throw(/Windows/)
      return
    }

    const letters = getDriveLetters()
    expect(letters).to.be.an('array').that.is.not.empty
    expect(letters[0]).to.match(/^[A-Z]:\\$/)
  })
})
