const sleep = require('./')
const now = require('licia/now')
const expect = require('chai').expect

describe('sleep', () => {
  it('basic', () => {
    const start = now()
    sleep(1000)
    expect(now() - start).to.be.above(999)
  })
})
