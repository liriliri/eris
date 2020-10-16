const profiler = require('./')
const expect = require('chai').expect

describe('profiler', () => {
  it('basic', () => {
    profiler.start('Profiler')
    const profile = profiler.stop('Profiler')
    console.log(profile)
  })
})
