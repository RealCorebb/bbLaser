# @laser-dac/ether-dream

This package makes Laser DAC compatible with the [Ether Dream](https://ether-dream.com/). Should work on all operating systems.

This package is originally based on [node-etherdream](https://github.com/possan/node-etherdream) but with many changes.

```
yarn add @laser-dac/ether-dream
npm i @laser-dac/ether-dream
```

## Usage

```js
import { DAC } from '@laser-dac/core';
import { EtherDream } from '@laser-dac/ether-dream';

const dac = new DAC();
dac.use(new EtherDream());
const started = await dac.start();
if (started) {
  const pps = 30000; // points per second
  // draw a horizontal red line from left to right in the center
  // @laser-dac/draw can help you with drawing points!
  const scene = {
    points: [
      { x: 0.1, y: 0.5, r: 1, g: 0, b: 0 },
      { x: 0.9, y: 0.5, r: 1, g: 0, b: 0 },
    ],
  };
  dac.stream(scene, pps);
}
```

See for more usage info and examples the [Laser DAC project on GitHub](https://github.com/Volst/laser-dac).
