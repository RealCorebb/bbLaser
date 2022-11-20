ref-array-di
============
### Create C typed "array" instances on top of Buffers

[![Greenkeeper badge](https://badges.greenkeeper.io/node-ffi-napi/ref-array-di.svg)](https://greenkeeper.io/)

[![NPM Version](https://img.shields.io/npm/v/ref-array-di.svg?style=flat)](https://npmjs.org/package/ref-array-di)
[![NPM Downloads](https://img.shields.io/npm/dm/ref-array-di.svg?style=flat)](https://npmjs.org/package/ref-array-di)
[![Build Status](https://travis-ci.org/node-ffi-napi/ref-array-di.svg?style=flat&branch=master)](https://travis-ci.org/node-ffi-napi/ref-array-di?branch=master)
[![Coverage Status](https://coveralls.io/repos/node-ffi-napi/ref-array-di/badge.svg?branch=master)](https://coveralls.io/r/node-ffi-napi/ref-array-di?branch=master)
[![Dependency Status](https://david-dm.org/node-ffi-napi/ref-array-di.svg?style=flat)](https://david-dm.org/node-ffi-napi/ref-array-di)

This module offers an "array" implementation on top of Node.js Buffers using
the ref "type" interface.

**Note**: The only difference to `ref-array` is that this module takes its
dependency on `ref` via dependency injection, so that it is easier to use
e.g. `ref-napi` instead.

Installation
------------

Install with `npm`:

``` bash
$ npm install ref-array-di
```


Examples
--------

#### Basic usage

``` js
var ref = require('ref')
var ArrayType = require('ref-array-di')(ref)

// typedef
var int = ref.types.int

// define the "int[]" type
var IntArray = ArrayType(int)

// now we can create array instances; the constructor takes the same arguments
// the native JS Array class

var a = new IntArray(5) // by length
a.length // 5
a[0] = 0
a[1] = 1
a[2] = -1
a[3] = 2
a[4] = -2

var b = new IntArray([1, 2, 3, 4, 5]) // with an existing Array
b.length // 5
b[0] // 1
b[1] // 2
b[2] // 3
b[3] // 4
b[4] // 5
```

#### Reading a `NULL`-terminated Array

``` js
// sometimes you get a variable length array that is terminated by a NULL byte.
var buf = Buffer.alloc(int.size * 3)
int.set(buf, int.size * 0, 5)
int.set(buf, int.size * 1, 8)
int.set(buf, int.size * 2, 0) // <- terminate with 0s

// you can create an array instance with the length automatically determined
var array = IntArray.untilZeros(buf)
console.log(array.length)
// 2
console.log(array)
// [ 5, 8 ]
```

#### With `node-ffi`

``` js
var ffi = require('ffi')

// the "int[]" type may be used as a "type" in FFI'd functions or callbacks
var func = ffi.ForeignFunction(funcPointer, int, [ IntArray, int ])

var arg = new IntArray(3)
arg[0] = 1234
arg[1] = -9999
arg[2] = 1

var rtn = func(arg, arg.length)
```


License
-------

(The MIT License)

Copyright (c) 2012 Nathan Rajlich &lt;nathan@tootallnate.net&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
