# svg-parser

Take a string representing an SVG document or fragment, turn it into a plain old JavaScript object.


## Installation

`npm install svg-parser`, or grab it from [npmcdn.com/svg-parser](https://npmcdn.com/svg-parser).


## Usage

```js
import { parse } from 'svg-parser';

const parsed = parse( `
	<svg viewBox='0 0 100 100'>
		<!-- stuff goes here... -->
	</svg>
` );
// { name: 'svg', attributes: { viewBox: '0 0 100 100' }, children: [...] }
```


## License

MIT
