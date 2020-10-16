const snapshot = require('./')
const expect = require('chai').expect

describe('snapshot', () => {
  it('basic', () => {
    const data = JSON.parse(snapshot())
    expect(data).to.have.property('snapshot')
  })
})
