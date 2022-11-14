# @laser-dac/draw

This package makes it easy for you to make laser drawings using programming. Of course there are many professional laser tools that you can use, but the advantage of this package is that since you can program it, it can handle interactive drawings.

There is also first-class support for using ILDA files (a standard used by the laser community for generating laser drawings with animations). You can include multiple ILDA files, control playback and position them anywhere you want on the scene.

There is support for converting SVG files as well, albeit it doesn't support all SVG syntax.

## Usage

This package has a couple of classes that let you draw something on a **scene**. A scene is the area the laser can draw things in.

```js
const scene = new Scene();
```

A scene has only one method; `add(shape)`.

Now we want to draw a rectangle on the scene. You can do it like this:

```js
const rect = new Rect({
  width: 0.2,
  height: 0.2,
  x: 0.4,
  y: 0.4,
  color: [0, 1, 0],
});

scene.add(rect);
```

Perhaps you'll have noticed that everything is based on a scale from **0 to 1**, even colors. Drawing starts from the top left. The rectangle is drawn at 40% from the top left, and takes in 20% of the scene.

`color` is an array of RGB values; so `[0, 1, 0]` means green.

`x` and `y` is the amount of offset starting from left or the top.

### `Path()`

With the `Path()` class you are able to draw a specific path, the exact same way as you can do with `<path/>` in a SVG.

```js
const cross = new Path({
  path: 'M0.2 0.2 h0.1 v0.1 h0.1 v0.1 h-0.1 v0.1 h-0.1 v-0.1 h-0.1 v-0.1 h0.1 z',
  color: [0, 1, 0],
  x: 0.1,
  y: 0.1,
});

scene.add(cross);
```

You can copy these values straight from a SVG; you'll

### `Rect()`

```js
const rect = new Rect({
  width: 0.2,
  height: 0.2,
  x: 0.4,
  y: 0.4,
  color: [0, 1, 0],
});

scene.add(rect);
```

### `Line()`

```js
const line = new Line({
  from: {
    x: 0.1,
    y: 0.1,
  },
  to: {
    x: 0.5,
    y: 0.5,
  }
  color: [0, 1, 0]
});

scene.add(line);
```

### `Circle()`

```js
const circle = new Circle({
  radius: 0.2,
  x: 0.5,
  y: 0.5,
  color: [0, 1, 0],
});

scene.add(circle);
```

### `Ilda()`

ILDA files (`.ild`) can be used directly by the drawing tool. This file format is widely supported by laser drawing tools. An example:

```js
const boeing = loadIldaFile(path.resolve(__dirname, './boeing.ild'));

const ilda = new Ilda({
  file: boeing,
  frame: 0,
  x: 0,
  y: 0,
});
scene.add(ilda);
```

The `frame` option controls what frame you want to display; it's easy to animate this and play all frames on whatever speed you like. See the [ilda-animation](https://github.com/Volst/laser-dac/tree/master/examples/ilda-animation) example.

### `HersheyFont()`

Hershey Font Files (`.jhf`) can be used to draw text. Hershey fonts can be found at https://emergent.unpythonic.net/software/hershey. The font `futural.jhf` is very usefull as it is a single stroke font.

```js
const font = loadHersheyFont(path.resolve(__dirname, './futural.jhf'));

const text = new HersheyFont({
  font,
  text,
  x: 0,
  y: 0,
  color: [1, 0, 0],
  spacingFactor: 1.0,
  charWidth: 0.1,
});
```

Note that the char width provided is the average char width, each individual character can have a different width.

### `Svg()`

SVG files can also be used directly by the drawing tool!

```js
const logoFile = loadIldaFile(path.resolve(__dirname, './logo.svg'));

const logo = new Svg({
  file: logoFile,
  x: 0,
  y: 0,
});
scene.add(logo);
```

A couple caveats:

- Only `<path/>` is supported; almost every vector tool has some way to convert a vector to only paths.
- CSS inside SVG is not supported
- `preserveAspectRatio` is not supported

The color for a path is decided by the `fill` property. It's possible to override the colors by adding a `color` property like `color: [0, 1, 0]`.

The size of the SVG is decided by the `viewBox` on the svg. At the moment the only possible way to reduce the size is to change the `viewBox` property in your SVG.
