#!/usr/bin/env node

const program = require('commander')
const noop = require('licia/noop')
const clone = require('licia/clone')
const promisify = require('licia/promisify')
const mkdir = promisify(require('licia/mkdir'))
const extend = require('licia/extend')
const fs = require('licia/fs')
const shell = require('shelljs')
const path = require('path')
const execa = require('execa')

program
  .command('dev <module>')
  .description('clean, configure and build')
  .action(dev)

program.command('build <module>').description('build packages').action(build)

program.command('test <module>').description('run test').action(test)

const args = process.argv
if (args[2] === '--help' || args[2] === '-h') args[2] = 'help'

program.parse(args)

async function test(module) {
  await runScript('mocha', [resolve(`../src/${module}/test.js`)])
}

async function build(module) {
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

async function dev(module) {
  await runScript(
    'node-gyp',
    ['rebuild', '--debug'],
    resolve(`../src/${module}`)
  )
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
