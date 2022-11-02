<template>
      <div id="wrapper">
        <div id="buildZone">
          <canvas id="canvas"></canvas>
          <div id="controls">
            <i id="square" class="feather icon-square"></i>
            <i id="triangle" class="feather icon-triangle"></i>
			<i id="pen" class="feather icon-edit"></i>
			<i id="target" class="feather icon-crosshair"></i>
			<i id="text" @click="appendText" class="feather icon-type"></i>
			<i id="navi" @click="navigation" class="feather icon-map-pin"></i>
            <i id="clear" class="feather icon-trash"></i>
          </div>
        </div>
		<el-input style="width:30%;margin:20px 5px;float: left;" v-model="textLabel" placeholder="文本"></el-input>
		
        <div id="styleZone"></div>
		 <el-slider :min="10" :max="500" v-model="res"></el-slider>
		 <el-button @click="toDraw">手动更新</el-button>
      </div>
</template>

<script>
import { Scene,Rect,Path,Circle,HersheyFont} from '@laser-dac/draw';
import { fabric } from "fabric";
import {makeStreamBuffer,hexToILDAColor} from '../utils/util'
import fontText from '../assets/bbLaserFont.txt?raw'

function loadHersheyFont() {
    const fontFile = fontText;
    const characters = [];
    let readPos = 0;
	function hersheyCharToNumber(char){
		return char.charCodeAt(0) - 'R'.charCodeAt(0);
	}
    function readNextCharacter() {
        // 0-4 number (not used)
        // 5-7 number of vertices
        // 8   left pos
        // 9   right pos
        // 10- vertices
        // newline
		//console.log('readnext')
        const vertexCount = Number.parseInt(fontFile.substr(readPos + 5, 3), 10);
        const leftPos = hersheyCharToNumber(fontFile.charAt(readPos + 8));
        const rightPos = hersheyCharToNumber(fontFile.charAt(readPos + 9));
        const vertices = [];
        readPos += 10;
		//console.log(readPos,fontFile.charAt(readPos) !== '\n')
        while (fontFile.charAt(readPos) !== '\n') {
            let vertex = '';
            for (let i = 0; i < 2; i++) {
				//console.log(readPos,fontFile.charAt(readPos))
				if(fontFile.charAt(readPos) == '\n') break;
                vertex += fontFile.charAt(readPos);
                readPos++;
            }
			
            vertices.push(vertex);
        }
        readPos++;
        return {
            leftPos,
            rightPos,
            vertexCount,
            vertices,
        };
    }
    while (readPos < fontFile.length) {
        characters.push(readNextCharacter());
    }
	//console.log('Loaded Font:',characters);
    return characters;
}

const font = loadHersheyFont();

export default {
  data: () => ({
	isDown:false,
	curPosition:[],
	textLabel:'',
	res:100,
    paintVisible: true,
	canvas:'',
	svgData:'',
	scene:'',
	socket:'',
	currentColor:'#FF0000',
	isTargeting: false
  }),
  created() {
      console.log('paint created');
      this.scene = new Scene({resolution:this.res});
	  //get params of ip
	  let urlParams = new URLSearchParams(window.location.search);
	  this.socket = new WebSocket('ws://'+urlParams.get('ip')+'/ws');

    },
   methods:{
		appendText(){
			this.canvas.add(new fabric.Text(this.textLabel, { 
				fontFamily: 'Delicious_500', 
				left: 100, 
				top: 100,
				fill: this.currentColor
			}));
		},
		navigation(){
			const watchID = navigator.geolocation.watchPosition((position) => {
				console.log(position.coords.latitude, position.coords.longitude);
			});
		},
		toDraw(){
			let start = new Date().getTime();
			this.scene = new Scene({resolution:this.res});
			var self = this
			let data = this.canvas.toJSON();
			this.svgData = data
        	console.log(data.objects);

			//data.objects is an array of objects
			//loop through

		    for (let item of data.objects){
				//item.path is an array of path,if its item is typeof number,multiply it by 0.1
				let finalPath = []
				
				if (item.type == 'path'){
					for(let ele of item.path){
						//loop through ele if its typeof number multiply by 0.1
						ele = ele.map(x => typeof(x) == 'number' ? (x /320).toFixed(3):x)
						
						finalPath.push(ele.join(" "))
					}
					//console.log(item.stroke,finalPath.join(" "))
					//item.path array of array to string
					const cross = new Path({
						path: finalPath.join(" "),
						color: hexToILDAColor(item.stroke),
						x: 0,
						y: 0,
					});
					this.scene.add(cross);
				}
				else if (item.type == 'text'){
					console.log('text object')
					console.log(item.left /320,item.top /320,hexToILDAColor(item.fill))
					const text = new HersheyFont({
						font,
						text:self.textLabel,
						x: (item.left /320),
						y: (item.top /320),
						color: hexToILDAColor(item.fill),
						spacingFactor: 1.0,
						charWidth: 0.2,
					});
					//console.log(text)
					this.scene.add(text);
					console.log("SC:",this.scene)
				}
			}
			
			let pointData = JSON.parse(JSON.stringify(this.scene))
			

			var frameData = new Uint8Array()
			frameData = makeStreamBuffer(pointData)
			console.log('Scene:',pointData.points)
			//console.log(frameData)
			this.socket.send(frameData)
			//console.log(pointData.points)
			let end = new Date().getTime();
			//console.log(end-start)
		}
	},
	watch:{
		'currentColor': function(){
			if(this.currentColor == '#f5dfed'){
				console.log('Rainbow time !!!')
				var rainbowInterval  = setInterval(() => {
					//current color is loop of ['#FF0000', '#00FF00', '#0000FF', '#FFFF00', '#00FFFF', '#FF00FF','#FFFFFF']
					this.currentColor = this.currentColor == '#FF0000' ? '#00FF00' : this.currentColor == '#00FF00' ? '#0000FF' : this.currentColor == '#0000FF' ? '#FFFF00' : this.currentColor == '#FFFF00' ? '#00FFFF' : this.currentColor == '#00FFFF' ? '#FF00FF' : this.currentColor == '#FF00FF' ? '#FFFFFF' : '#FF0000'
					this.canvas.freeDrawingBrush.color = this.currentColor
				}, 1000);
			}
		},
	},
  mounted(){
	  var self = this
      console.log('paint mounted')
	  const canvas = new fabric.Canvas('canvas', { width: 320, height: 320 });
	  this.canvas = canvas

	  	canvas.isDrawingMode = true;
		canvas.freeDrawingBrush.width = 5;
		canvas.freeDrawingBrush.color = '#FF0000';

		canvas.on('mouse:up', function({e}) {
			self.isDown = false
			if (self.isTargeting) {
				canvas.clear()
			}
			else{
				self.toDraw()
			}
		}).on('mouse:down', function({e}) {
			self.isDown = true
		}).on('mouse:move', function({e}) {
			if (self.isTargeting) {
				if(self.isDown){
					console.log('move:',e.offsetX,e.offsetY)
					var frameData = new Uint8Array()
					let pointData = {
						points: [
							{
								x: e.offsetX /320,
								y: e.offsetY /320,
								r: hexToILDAColor(self.currentColor)[0],
								g: hexToILDAColor(self.currentColor)[1],
								b: hexToILDAColor(self.currentColor)[2]
							}
						]
					}
					frameData = makeStreamBuffer(pointData)
					console.log('Scene:',pointData.points)
					//console.log(frameData)
					self.socket.send(frameData)
				}
			}
		});

		// Resize canvas

		const buildZone = document.getElementById('buildZone');
		const wrapper = document.getElementById('wrapper');
		const paddingShift = 60;

		/*
		function resizeCanvas() {
			console.log('resize Canvas')
			// Width
			const newWidth = canvas.getWidth() + (window.innerWidth - (buildZone.offsetWidth + paddingShift));
			if(newWidth < 320 && newWidth > 200) canvas.setWidth(newWidth);
			
			// Height
			const newHeight = canvas.getHeight() + (window.innerHeight - (wrapper.offsetHeight + paddingShift));
			if(newHeight < 360 && newHeight > 250) canvas.setHeight(newHeight);
		}

		window.addEventListener('resize', resizeCanvas);
		resizeCanvas();*/


		// Clear canvas - Delete shapes

		document.getElementById('clear').addEventListener('click', () => {
			!deleteActiveObjects() && canvas.clear();
		});

		document.addEventListener('keydown', (event) => {
			event.keyCode === 46 && deleteActiveObjects();
		})

		function deleteActiveObjects() {
			const activeObjects = canvas.getActiveObjects();
			if(!activeObjects.length) return false;
			
			if(activeObjects.length) {
				activeObjects.forEach(function(object) {
					canvas.remove(object);
				});
			} else {
				canvas.remove(activeObjects);
			}
			
			return true;
		}


		// SHAPES STYLES  ―――――――――――――――――――――――――

		const styleZone = document.getElementById('styleZone');
		const colors = ['#FF0000', '#00FF00', '#0000FF', '#FFFF00', '#00FFFF', '#FF00FF','#FFFFFF','#000000','#f5dfed'];
		let defaultColor = colors[0];
		let activeElement = null;
		const isSelectedClass = 'isSelected';

		colors.forEach((color, i) => {
			const span = document.createElement('span');
			span.style.background = color;
			if(color == '#f5dfed'){
				span.innerHTML = '🌈'	
			}

			if(i === 0) {
				span.className = isSelectedClass;
				activeElement = span;
			}
			
			let icon = document.createElement('i');
			icon.className = 'feather icon-check';

			span.appendChild(icon);
			
			
			
			styleZone.appendChild(span);
			
			span.addEventListener('click', (event) => {

				if(span.className !== isSelectedClass) {
					span.classList.toggle(isSelectedClass);
					activeElement.classList.remove(isSelectedClass);
					activeElement = span;
					strokeColor = color;
					self.currentColor = color
					canvas.freeDrawingBrush.color = color;
				}
				
				if(canvas.getActiveObject()) {
					const activeObjects = canvas.getActiveObjects();
					if (!activeObjects.length) return;

					activeObjects.forEach(function (object) {
						object.set('stroke', strokeColor);
					});
					
					canvas.renderAll();
				}
			})
		});


		// SHAPES CREATION  ―――――――――――――――――――――――――

		let strokeWidth = 2;
		let strokeColor = defaultColor;

		// Square

		document.getElementById('square').addEventListener('click', () => {
			canvas.isDrawingMode = false
			canvas.add(new fabric.Rect({
				strokeWidth: strokeWidth,
				stroke: strokeColor,
				fill: 'transparent',
				width: 50,
				height: 50,
				left: 100,
				top: 100
			}));
		});
		// Triangle

		document.getElementById('triangle').addEventListener('click', () => {
			canvas.isDrawingMode = false
			canvas.add(new fabric.Triangle({
				strokeWidth: strokeWidth,
				stroke: strokeColor,
				fill: 'transparent',
				width: 50,
				height: 50,
				left: 100,
				top: 100
			}));
		});

		//Pen

		document.getElementById('pen').addEventListener('click', () => {
			canvas.isDrawingMode = !canvas.isDrawingMode
		});
		document.getElementById('target').addEventListener('click', () => {
			canvas.isDrawingMode = true
			self.isTargeting = !self.isTargeting
		});

		/*
		setInterval(() => {
			this.toDraw()
		}, 1000/30)*/
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
#buildZone { display: flex; }

.canvas-container, #canvas { transition: all 0.2 ease-in-out; }

#canvas {
	background: darken(#2c2f36, 5%);
	border-radius: 5px;
}


// CONTROLS  ―――――――――――――――――――――――――

#controls {
	margin-left: 30px;
	display: flex;
	flex-direction: column;
	
	:deep(i) {
		background: #4881b8;
		padding: 15px;
		display: flex;
		align-items: center;
		justify-content: center;
		font-size: 1.3em;
		border-radius: 100%;
		transition: all 0.2s ease-in-out;
		margin: 5px 0;
		color:white;
		&:hover {
			background: lighten(#4881b8, 8%);
			cursor: pointer;
		}
	}
	
	:deep(#clear) {
		background: #e54f6b;
		margin-top: auto;
		&:hover { background: lighten(#e54f6b, 8%); }
	}
}

// STYLE-ZONE  ―――――――――――――――――――――――――

#styleZone {
	display: flex;
	margin-top: 20px;
	
	:deep(span) {
		width: 60px;
		height: 30px;
		border-radius: 3px;
		margin-right: 15px;
		display: flex;
		align-items: center;
		justify-content: center;
		transition: all 0.2s ease-in-out;
		border: 2px solid transparent;
		
		&.isSelected i {
			opacity: 1;
			transform: scale(1);
		}
		
		&:hover, &.isSelected {
			cursor: pointer;
			border-color: white;
		}
	}
	
	:deep(i) {
		color:#000;
		opacity: 0;
		transition: all 0.3s cubic-bezier(.56, .35, 0, 1.7);
		transform: scale(0.6);
	}
}
</style>