# Eris profiler

Record V8 CPU profile.

## Install

```bash
npm install eris-profiler --save
```

```javascript
const fs = require('fs')
const profiler = require('eris-profiler')

profiler.start('test')

// Do something

fs.writeFile('test.cpuprofile', profiler.stop('test'), function (err) {
  if (err) {
    console.log(err)
  }
})
```