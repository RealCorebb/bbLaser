# ILDA Reader

This package can read an [ILDA](http://ilda.com/) file (`.ild`) and convert it to a JSON array of points. All ILDA formats are supported (0 to 5).

This is heavily inspired by [ilda.js](https://github.com/possan/ilda.js), but this tool only works in the browser and we wanted to be able to use it in both Nodejs and the browser.

To write a JSON array of points to an ILDA file, see its counterpart [ilda-writer](https://github.com/Volst/laser-dac/tree/master/packages/ilda-writer).

## Install

```
npm install @laser-dac/ilda-reader
```

## Usage

For a full example see `./test.ild` and `./test.ts` in the repository.

```js
const buffer = fs.readFileSync('./test.ild');
const output = fromByteArray(new Uint8Array(buffer));

console.log(output);
```
