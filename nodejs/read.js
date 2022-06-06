import { fromByteArray } from '@laser-dac/ilda-reader';
import * as fs from 'fs';

const buffer = fs.readFileSync('./test.ild');
const output = fromByteArray(new Uint8Array(buffer));

console.log(output);