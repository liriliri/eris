# Eris windrive

Enumerate Windows logical drives (盘符) and basic disk space info.

## Install

```bash
npm install eris-windrive --save
```

```javascript
const { getLogicalDrives, getDriveLetters } = require('eris-windrive')

console.log(getDriveLetters())
// e.g. [ 'C:\\', 'D:\\' ]

console.log(getLogicalDrives())
// [{ name: 'C:\\', type: 'FIXED', bytesPerSect, freeClusters, ... }, ...]
```

Windows only.
