# Eris snapshot

Take V8 snapshot for later inspection.

## Install

```bash
npm install eris-snapshot --save
```

```javascript
const fs = require('fs')
const snapshot = require('eris-snapshot')

fs.writeFile('test.heapsnapshot', snapshot, function (err) {
  if (err) {
    console.log(err)
  }
})
```