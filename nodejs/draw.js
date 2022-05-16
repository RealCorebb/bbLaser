import {Scene,Rect} from '@laser-dac/draw';
import { DAC } from '@laser-dac/core';
import { Simulator } from '@laser-dac/simulator';

const dac = new DAC();
dac.use(new Simulator());
const started = await dac.start();

const scene = new Scene();
const rect = new Rect({
  width: 0.2,
  height: 0.2,
  x: 0.4,
  y: 0.4,
  color: [127, 255, 0],
});
scene.add(rect);

if (started) {
  dac.stream(scene);
}