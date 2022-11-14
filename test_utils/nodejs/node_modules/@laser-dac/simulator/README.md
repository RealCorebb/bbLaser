# @laser-dac/simulator

This package can simulate a physical laser DAC so you can develop without having a laser or DAC at all. It has a web-based simulator for the laser, that tries its best to mimick the limitations of the laser.

```
yarn add @laser-dac/simulator
npm i @laser-dac/simulator
```

## Usage

```js
import { DAC } from '@laser-dac/core';
import { Simulator } from '@laser-dac/simulator';

const dac = new DAC();
dac.use(new Simulator());
const started = await dac.start();
if (started) {
  // draw a horizontal red line from left to right in the center
  // @laser-dac/draw can help you with drawing points!
  const scene = {
    points: [
      { x: 0.1, y: 0.5, r: 1, g: 0, b: 0 },
      { x: 0.9, y: 0.5, r: 1, g: 0, b: 0 },
    ],
  };
  dac.stream(scene);
}
```

See for more usage info and examples the [Laser DAC project on GitHub](https://github.com/Volst/laser-dac).
