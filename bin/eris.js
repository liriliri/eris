#!/usr/bin/env node

const yargs = require('yargs')
const noop = require('licia/noop')

yargs
  .usage('Usage: <command> <module>')
  .command('format', 'format code', noop, format)
