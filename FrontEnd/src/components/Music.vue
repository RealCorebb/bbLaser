<template>
      <div id="wrapper">
        <div id="buildZone">
          <canvas id="canvas"></canvas>
        </div>
        <div id="styleZone"></div>
		<el-button @click="toDraw">SVG</el-button>
      </div>
</template>

<script>
import { Scene,Rect,Path,Circle } from '@laser-dac/draw';
import { fabric } from "fabric";
import {Buffer} from 'buffer';
import {makeStreamBuffer,hexToILDAColor} from '../utils/util'

export default {
  data: () => ({
  }),
  created() {
      console.log('paint created');
      this.scene = new Scene({resolution:50});
	  let urlParams = new URLSearchParams(window.location.search);
	  this.socket = new WebSocket('ws://'+urlParams.get('ip')+'/ws');
    },
   methods:{
		toDraw(){
			this.scene = new Scene({resolution:50});
			var self = this
			let data = this.canvas.toJSON();
			this.svgData = data
        	//console.log(data.objects);

			//data.objects is an array of objects
			//loop through

		    for (let item of data.objects){
				//item.path is an array of path,if its item is typeof number,multiply it by 0.1
				let finalPath = []

				for(let ele of item.path){
					//loop through ele if its typeof number multiply by 0.1
					ele = ele.map(x => typeof(x) == 'number' ? (x).toFixed(3):x)
					
					finalPath.push(ele.join(" "))
				}
				console.log(item.stroke,finalPath.join(" "))
				//item.path array of array to string
				const cross = new Path({
					path: finalPath.join(" "),
					color: hexToILDAColor(item.stroke),
					x: 0,
					y: 0,
				});
				//this.scene.add(cross);
			}
			
			const rect = new Rect({
				width: 0.2,
				height: 0.2,
				x: 0.4,
				y: 0.4,
				color: [0, 1, 0],
				});

			this.scene.add(rect);
			let pointData = JSON.parse(JSON.stringify(this.scene))
			console.log('Scene:',pointData.points)
			frameData = makeStreamBuffer(pointData)
			console.log(frameData)
			this.socket.send(frameData)
			console.log(pointData.points)
		}
	},
  mounted(){
	var Analyser = {
    analyser: {},
    canvas: {},
    rectangles: [],
    init: function (src) {
        this.initCanvas();
        this.initPlayer(src);
        this.initAnalyser();
    },
    initCanvas: function () {
        var self = this;
        this.canvas = new fabric.Canvas('canvas');
        this.canvas.selection = false;
        this.canvas.setHeight(window.innerHeight);
        this.canvas.setWidth(window.innerWidth);

        var lineTop = window.innerHeight * 0.9;
        var lineLeft = 0 + (window.innerWidth * 0.1);
        var lineRight = window.innerWidth - (window.innerWidth * 0.1);
        var width = window.innerWidth - window.innerWidth * 0.2;

        var rect = function (top, left) {
            var rect = new fabric.Rect({
                width: 20,
                height: 2,
                fill: '#00FF00',
                opacity: 0.9,
                top: top,
                left: left,
                'selectable': false
            });
            return rect;
        }

        var rectNums = Math.round(width / 20);
        for (var i = 0; i < rectNums; i++) {
            var R = rect(lineTop, lineLeft + i * 20);
            this.rectangles.push(R);
            this.canvas.add(R);
        }

        window.addEventListener('resize', function () {
            self.canvas.setHeight(window.innerHeight);
            self.canvas.setWidth(window.innerWidth);
        }, false);
    },
    initPlayer: function (src) {
        this.audio = new Audio();
        this.audio.src = src;
        this.audio.controls = true;
        this.audio.autoplay = true;
        this.audio.crossOrigin = "anonymous";

        self = this;
        this.audio.addEventListener('canplay', function () {
            if (!self.source) {
                self.source = self.context.createMediaElementSource(self.audio);
                self.source.connect(self.analyser);
                self.analyser.connect(self.node);
                self.node.connect(self.context.destination);
                self.source.connect(self.context.destination);

                setInterval(function () {
                    self.analyser.getByteFrequencyData(self.bands);
                    if (!self.audio.paused) {
                        self.updateCanvas();
                    }
                }, 100);
            }
        });
    },
    initAnalyser: function () {
        AudioContext = window.AudioContext || window.webkitAudioContext;
        this.context = new AudioContext();
        this.node = this.context.createScriptProcessor(2048, 1, 1);
        this.analyser = this.context.createAnalyser();
        this.analyser.smoothingTimeConstant = 0.3;
        this.analyser.fftSize = 512;
        this.bands = new Uint8Array(this.analyser.frequencyBinCount);
    },
    updateCanvas: function () {
        var self = this;
        this.rectangles.forEach(function (e, i) {
            var height = e.height;
            e.animate('height', -self.bands[i], {
                onChange: self.canvas.renderAll.bind(self.canvas),
                duration: (50),
                easing: fabric.util.ease.easeOutBounce
            });
        });
    }
	}
	Analyser.init('http://m10.music.126.net/20220729154521/10ade4bed524047a88fc6c6c24c1f3d3/ymusic/1a26/b8ab/4f06/e5f7a3659a92d2fc96438cb9eeeb7147.mp3');
  }
  
}
</script>

<style>
@import "http://at.alicdn.com/t/font_o5hd5vvqpoqiwwmi.css";

* {
  box-sizing: border-box;
}

body{
	background: black;
}

</style>



<style lang="scss" scoped>

// GLOBAL ―――――――――――――――――――――――――

* {
	box-sizing: border-box;
	padding: 0;
	margin: 0;
}

body {
	display: flex;
	height: 100vh;
	font-size: 14px;
	background: #2c2f36;
	color: white;
	padding: 30px;
}

#wrapper { width:80%;margin:0 auto }
</style>