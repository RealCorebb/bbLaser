# @laser-dac/helios

This package makes Laser DAC compatible with the [Helios](http://pages.bitlasers.com/helios/). Currently macOS + Windows only.

```
yarn add @laser-dac/helios
npm i @laser-dac/helios
```

## Usage

```js
import { DAC } from '@laser-dac/core';
import { Helios } from '@laser-dac/helios';

const dac = new DAC();
dac.use(new Helios());
const started = await dac.start();
if (started) {
  const pps = 30000; // points per second
  const fps = 120; // frames per second
  // draw a horizontal red line from left to right in the center
  // @laser-dac/draw can help you with drawing points!
  const scene = {
    points: [
      { x: 0.1, y: 0.5, r: 1, g: 0, b: 0 },
      { x: 0.9, y: 0.5, r: 1, g: 0, b: 0 },
    ],
  };
  dac.stream(scene, pps, fps);
}
```

See for more usage info and examples the [Laser DAC project on GitHub](https://github.com/Volst/laser-dac).
