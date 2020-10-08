const sleep = require('./')
const now = require('licia/now')

describe('sleep', () => {
  it('basic', () => {
    const start = now()
    sleep(5000)
    console.log(now() - start)
  })
})
