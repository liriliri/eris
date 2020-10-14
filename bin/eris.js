#!/usr/bin/env node

const yargs = require('yargs')
const noop = require('licia/noop')
const clone = require('licia/clone')
const promisify = require('licia/promisify')
const mkdir = promisify(require('licia/mkdir'))
const extend = require('licia/extend')
const fs = require('licia/fs')
const shell = require('shelljs')
const path = require('path')
const execa = require('execa')

yargs
  .usage('Usage: <command> <module>')
  .command('dev', 'clean, configure and build', noop, dev)
  .command('build', 'build package', noop, build)
  .command('test', 'run test', noop, test)
  .help('h').argv

async function test(argv) {
  const module = getModule(argv)
  if (!module) return

  await runScript('mocha', [resolve(`../src/${module}/test.js`)])
}

async function build(argv) {
  const module = getModule(argv)
  if (!module) return

  await mkdir(resolve(`../dist/${module}`))

  const pkg = require('../package.json')
  delete pkg.scripts
  delete pkg.devDependencies
  delete pkg.bin
  pkg.gypfile = true
  const modulePkg = require(`../src/${module}/package.json`)
  extend(pkg, modulePkg)

  await fs.writeFile(
    resolve(`../dist/${module}/package.json`),
    JSON.stringify(pkg, null, 2),
    'utf8'
  )

  const files = ['README.md', '{index,util}.js', '*.cc', 'binding.gyp']

  files.forEach((file) => {
    shell.cp(resolve(`../src/${module}/${file}`), resolve(`../dist/${module}`))
  })
}

async function dev(argv) {
  const module = getModule(argv)
  if (!module) return

  await runScript(
    'node-gyp',
    ['rebuild', '--debug'],
    resolve(`../src/${module}`)
  )
}

function getModule(argv) {
  const _ = clone(argv._)
  _.shift()
  const module = _.shift()

  if (!module) {
    console.log('A module name must be given.')
  }

  return module
}

function resolve(p) {
  return path.resolve(__dirname, p)
}

function runScript(name, args, cwd) {
  return execa(name, args, {
    preferLocal: true,
    cwd: cwd || resolve('../'),
    stdio: 'inherit',
  })
}
