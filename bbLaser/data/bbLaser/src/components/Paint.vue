<template>
      <div id="wrapper">
        <div id="buildZone">
          <canvas id="canvas"></canvas>
          <div id="controls">
            <i id="square" class="feather icon-square"></i>
            <i id="triangle" class="feather icon-triangle"></i>
			<i id="pen" class="feather icon-edit"></i>

            <i id="clear" class="feather icon-trash"></i>
          </div>
        </div>
        <div id="styleZone"></div>
		<el-button @click="toDraw">SVG</el-button>
      </div>
</template>

<script>
import { DAC } from '@laser-dac/core';
import { Scene } from '@laser-dac/draw';
import { fabric } from "fabric";
export default {
  data: () => ({
    paintVisible: true,
	canvas:'',
	svgData:''
  }),
  created() {
      console.log('paint created');
      const scene = new Scene();
    },
   methods:{
		toDraw(){
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
					ele = ele.map(x => typeof(x) == 'number' ? (x * 0.0015625).toFixed(3):x)
					
					finalPath.push(ele.join(" "))
				}
				console.log(item.stroke,finalPath.join(" "))
				//item.path array of array to string

			}
		}
	},
  mounted(){
      console.log('paint mounted')
	  const canvas = new fabric.Canvas('canvas', { width: 640, height: 640 });
	  this.canvas = canvas

	  	canvas.isDrawingMode = true;
		canvas.freeDrawingBrush.width = 5;
		canvas.freeDrawingBrush.color = '#FF0000';
		// Resize canvas

		const buildZone = document.getElementById('buildZone');
		const wrapper = document.getElementById('wrapper');
		const paddingShift = 60;

		function resizeCanvas() {
			// Width
			const newWidth = canvas.getWidth() + (window.innerWidth - (buildZone.offsetWidth + paddingShift));
			if(newWidth < 640 && newWidth > 200) canvas.setWidth(newWidth);
			
			// Height
			const newHeight = canvas.getHeight() + (window.innerHeight - (wrapper.offsetHeight + paddingShift));
			if(newHeight < 360 && newHeight > 250) canvas.setHeight(newHeight);
		}

		window.addEventListener('resize', resizeCanvas);
		resizeCanvas();


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
		const colors = ['#FF0000', '#00FF00', '#0000FF', '#FFFF00', '#00FFFF', '#FF00FF','#FFFFFF'];
		let defaultColor = colors[0];
		let activeElement = null;
		const isSelectedClass = 'isSelected';

		colors.forEach((color, i) => {
			const span = document.createElement('span');
			span.style.background = color;
			
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