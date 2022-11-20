import WebsocketClient from './websocket.js';
import dat from './dat.gui.js';

class SimulatorOptions {
  constructor() {
    this.positionDelay = 0;
    this.afterglowAmount = 30;
    this.xyResolution = 1;
    this.numberOfPoints = '';
    this.totalPoints = '';
    this.showBlanking = false;
    this.showDots = false;
    this.forceTotalRender = true;
  }
}

const options = new SimulatorOptions();
var gui = new dat.GUI();
gui.add(options, 'positionDelay', 0, 10).step(1);
gui.add(options, 'afterglowAmount', 0, 300);
gui.add(options, 'showBlanking');
gui.add(options, 'showDots');
gui.add(options, 'forceTotalRender');
gui.add(options, 'xyResolution', 0, 1);
gui.add(options, 'numberOfPoints').listen();
gui.add(options, 'totalPoints').listen();
gui.width = 300;

let points = [];
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
let lastRenderTime;

function handleResize() {
  const pixelRatio = window.devicePixelRatio;

  ctx.scale(pixelRatio, pixelRatio);
  canvas.width = Math.floor(canvas.clientWidth * pixelRatio);
  canvas.height = Math.floor(canvas.clientHeight * pixelRatio);
  ctx.lineWidth = pixelRatio;
}
handleResize();
window.onresize = handleResize;

// Listen to changes in device pixel ratio.
window
  .matchMedia('screen and (min-resolution: 2dppx)')
  .addListener(handleResize);

function calculateRelativePosition(position) {
  return position / options.xyResolution;
}

function calculateColor(raw) {
  return Math.round(raw * 255);
}

function render() {
  const currentTime = new Date();
  if (lastRenderTime) {
    const frameInterval = currentTime - lastRenderTime;
    // We add variable afterglow depending on the time until the last render.
    ctx.fillStyle = `rgba(0, 0, 0, ${
      options.afterglowAmount ? frameInterval / options.afterglowAmount : 1
    })`;
  }
  lastRenderTime = currentTime;

  // This rectangle will use the afterglow style from the code above.
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  points.forEach(function (point, i) {
    // To simulate the behaviour of an actual laser, controlling the color
    // of the lasers is faster than moving the scanners to a position.
    // "Accurate and Efficient Drawing Method for Laser Projection" describes this as:
    // "... the command ‘turn on beam’ takes less time to execute than the actual ‘jump’ command."
    const colorIndex = i + options.positionDelay;
    const color =
      points[colorIndex < points.length ? colorIndex : points.length - 1];

    // Prevent drawing unnecessary lines.
    const isBlanking = !color || !(color.r || color.g || color.b);
    if ((!options.showBlanking && isBlanking) || i === 0) return;
    const previousPoint = points[i - 1];
    if (previousPoint.x === point.x && previousPoint.y === point.y) return;

    ctx.beginPath();
    ctx.moveTo(
      calculateRelativePosition(previousPoint.x) * canvas.width,
      calculateRelativePosition(previousPoint.y) * canvas.height
    );
    const canvasPointX = calculateRelativePosition(point.x) * canvas.width;
    const canvasPointY = calculateRelativePosition(point.y) * canvas.height;
    const canvasColor = `rgb(${calculateColor(color.r)}, ${calculateColor(
      color.g
    )}, ${calculateColor(color.b)})`;
    ctx.lineTo(canvasPointX, canvasPointY);

    if (isBlanking) {
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.3)';
    } else {
      ctx.strokeStyle = canvasColor;
    }
    ctx.stroke();
    if (options.showDots && !isBlanking) {
      ctx.fillStyle = canvasColor;
      ctx.beginPath();
      ctx.arc(canvasPointX, canvasPointY, 4, 0, Math.PI * 2);
      ctx.fill();
    }
  });
  requestAnimationFrame(render);
}
requestAnimationFrame(render);

const host = window.document.location.host.replace(/:.*/, '');
const ws = new WebsocketClient();
ws.open('ws://' + host + ':8080');
ws.onmessage = function (event) {
  const payload = JSON.parse(event.data);
  if (payload.type === 'POINTS') {
    if (options.forceTotalRender) {
      points = points.concat(payload.data);
      points = points.slice(Math.max(points.length - options.totalPoints, 0));
    } else {
      points = payload.data;
    }
    return;
  }
  if (payload.type === 'POINTS_INFO') {
    options.numberOfPoints = String(payload.data.numpoints);
    options.totalPoints = String(payload.data.totalPoints);
  }
};
