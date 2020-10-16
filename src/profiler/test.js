const profiler = require('./')
const expect = require('chai').expect

describe('profiler', () => {
  it('basic', () => {
    profiler.start('Profiler')
    const profile = profiler.stop('Profiler')
    ;['startTime', 'endTime'].forEach((key) => {
      expect(profile).to.have.property(key)
    })
  })
})
