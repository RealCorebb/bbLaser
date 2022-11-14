# @laser-dac/core

This is an extremely lightweight wrapper that allows you to control any laser DAC you want with a standardized interface. This package does nothing on its own.

```
yarn add @laser-dac/core
npm i @laser-dac/core
```

A usage example together with [`@laser-dac/helios`](https://github.com/Volst/laser-dac/tree/master/packages/helios):

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

See for more usage info the [Laser DAC project on GitHub](https://github.com/Volst/laser-dac).
