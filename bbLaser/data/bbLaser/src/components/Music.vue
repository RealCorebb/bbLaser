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
      this.scene = new Scene({resolution:500});
	  this.socket = new WebSocket('ws://192.168.2.130:80/ws');
    },
   methods:{
		toDraw(){
			this.scene = new Scene({resolution:100});
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