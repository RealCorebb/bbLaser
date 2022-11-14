# change-perspective

A very small JavaScript library (no dependencies!) for creating and applying perspective transforms. A perspective transform can easily be used to map one 2D quadrilateral to another, given the corner coordinates for the source and destination quadrilaterals. 

Based on [perspective-transform](https://github.com/jlouthan/perspective-transform) but refactored to use commonJS, provide TypeScript typings and remove a pesky `console.log` that bothered me in the code.

## Install

```
npm install change-perspective
```

## Usage

```js
const changePerspective = require('change-perspecitve');

const srcCorners = [158, 64, 494, 69, 495, 404, 158, 404];
const dstCorners = [100, 500, 152, 564, 148, 604, 100, 560];
const transform = changePerspective(srcCorners, dstCorners);
const srcPt = [250, 120];
const dstPt = transform(srcPt[0], srcPt[1]);
```