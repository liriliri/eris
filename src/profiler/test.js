const profiler = require('./')
const expect = require('chai').expect

describe('profiler', () => {
  it('basic', () => {
    profiler.start('Profiler')
    loop()
    let profile = profiler.stop('Profiler')
    profile = JSON.parse(profile)
    ;['startTime', 'endTime', 'samples', 'timeDeltas', 'nodes'].forEach(
      (key) => {
        expect(profile).to.have.property(key)
      }
    )
  })
})

function loop() {
  let sum = 0
  for (let i = 0; i < 100000; i++) {
    sum += i
  }
}
