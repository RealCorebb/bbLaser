# @laser-dac/lasercube-wifi

This package makes Laser DAC compatible with the LaserCube Wifi.

```
yarn add @laser-dac/lasercube-wifi
npm i @laser-dac/lasercube-wifi
```

Many thanks to @s4y for deciphering the LaserOS demo and [porting it to Python](https://gist.github.com/s4y/0675595c2ff5734e927d68caf652e3af).

## Usage

```js
import { DAC } from '@laser-dac/core';
import { LasercubeWifi } from '@laser-dac/lasercube-wifi';

const dac = new DAC();
dac.use(new LasercubeWifi());
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
