module.exports = {
  sleep: genConf('sleep'),
  snapshot: genConf('snapshot'),
  profiler: genConf('profiler'),
}

function genConf(name) {
  return {
    files: 'src/' + name + '/*.js',
    output: 'src/' + name + '/util.js',
    format: 'commonjs',
  }
}
