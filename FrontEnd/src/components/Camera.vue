<template>
    <div id="wrapper">
      <div>
          <video style="display:none" id="video">Video stream not available.</video>
          <canvas style="display:none" id="canvasCam">
          </canvas>
          <div style="display:none" class="output">
              <img id="photo">
          </div>
          <div id="svgcontainer">
          </div>
          <el-slider :min="10" :max="500" v-model="res"></el-slider>
          <el-switch
              v-model="forceColor"
              active-text="替换颜色"
              inactive-text="原本颜色">
          </el-switch>
          <el-color-picker v-model="currentColor"></el-color-picker>
      </div>
      
    </div>
</template>

<script>
import ImageTracer from 'imagetracerjs'
import { parse} from 'svg-parser';
import { Scene,Svg} from '@laser-dac/draw';
import {makeStreamBuffer,hexToILDAColor} from '../utils/util'

export default {
  data: () => ({
          forceColor: false,
          currentColor: '#00FFF00',
          takepictureint:'',
          captureIntervalMS: 30,
          width:320,
          height:0,
          res:50,
          scene:'',
      }),
  created() {
    console.log('paint created');
    this.scene = new Scene({resolution:this.res});
    //get params of ip
    let urlParams = new URLSearchParams(window.location.search);
    this.socket = new WebSocket('ws://'+urlParams.get('ip')+'/ws');

  },
  methods:{
      startup() {
          var self = this
          console.log('start UP')
          var streaming = false;

          navigator.getUserMedia = ( navigator.getUserMedia ||
                     navigator.webkitGetUserMedia ||
                     navigator.mozGetUserMedia ||
                     navigator.msGetUserMedia);

          navigator.getUserMedia(
          {
              video: true,
              audio: false
          },
          function(stream) {
              if (navigator.mozGetUserMedia) {
              document.getElementById('video').mozSrcObject = stream;
              } else {
              var vendorURL = window.URL || window.webkitURL;
              document.getElementById('video').srcObject=stream;
              }
              document.getElementById('video').play();
          },
          function(err) {
              console.log("An error occured! " + err);
          }
          );

          document.getElementById('video').addEventListener('canplay', function(ev){
          if (!streaming) {
              console.log('video Width and Height',document.getElementById('video').videoHeight,document.getElementById('video').videoWidth)
              self.height = document.getElementById('video').videoHeight / (document.getElementById('video').videoWidth/self.width);
              console.log('width: ' + self.width + ' height: ' + self.height,document.getElementById('video').videoHeight,document.getElementById('video').videoWidth);
              // Firefox currently has a bug where the height can't be read from
              // the video, so we will make assumptions if this happens.
          
              if (isNaN(self.height)) {
              self.height = self.width / (4/3);
              }
          
              document.getElementById('video').setAttribute('width', self.width);
              document.getElementById('video').setAttribute('height', self.height);
              document.getElementById('canvasCam').setAttribute('width', self.width);
              document.getElementById('canvasCam').setAttribute('height', self.height);
              streaming = true;
          }
          }, false);
          
          self.clearphoto();
      },
      clearphoto() {
          var context = document.getElementById('canvasCam').getContext('2d');
          context.fillStyle = "#AAA";
          context.fillRect(0, 0, document.getElementById('canvasCam').width, document.getElementById('canvasCam').height);

          var data = document.getElementById('canvasCam').toDataURL('image/png');
          document.getElementById('photo').setAttribute('src', data);
      },
              
      takepicture() {
          var start = Date.now()
          var self = this
          var context = document.getElementById('canvasCam').getContext('2d');
          if (this.width && this.height) {
          document.getElementById('canvasCam').width = this.width;
          document.getElementById('canvasCam').height = this.height;
          context.drawImage(document.getElementById('video'), 0, 0, this.width, this.height);
          var data = document.getElementById('canvasCam').toDataURL('image/png');
          //console.log('get data')
          ImageTracer.imageToSVG(
              data,
              function(svgstr){
                  //console.log('converted')
              document.getElementById('svgcontainer').innerHTML = '';
              

              //append viewBox="0 0 320 180" at svgstr index of 4
              svgstr = svgstr.slice(0,4) + ' viewBox="0 0 320 180" ' + svgstr.slice(4)
              //remove border  M 0 0 L 320 0 L 320 180 L 0 180 L 0 0 Z 
              svgstr = svgstr.replace('M 0 0 L 320 0 L 320 180 L 0 180 L 0 0 Z ','')

              //replace all rgb values with hex values in svgstr
              
              
              if(self.forceColor == false){
                  
                  svgstr = svgstr.replace(/rgb\((\d+),\s*(\d+),\s*(\d+)\)/g, function(match, r, g, b) {
                    //console.log('origin Color:',r,g,b,match,'#' + ('0' + parseInt(r,10).toString(16)).slice(-2) + ('0' + parseInt(g,10).toString(16)).slice(-2) + ('0' + parseInt(b,10).toString(16)).slice(-2))
                      return '#' + ('0' + parseInt(r,10).toString(16)).slice(-2) + ('0' + parseInt(g,10).toString(16)).slice(-2) + ('0' + parseInt(b,10).toString(16)).slice(-2);
                  });
              }
              //console.log(svgstr)
              let svgFile = parse(svgstr);
              //console.log(svgFile)

              
              const svgBuf = new Svg({
                  file: svgFile,
                  x: 0,
                  y: 0,
                  color: hexToILDAColor('#FF0000')
                });
              
                  

              self.scene = new Scene({resolution:self.res});    
              self.scene.add(svgBuf);

              ImageTracer.appendSVGString( svgstr, 'svgcontainer' ); 
              
              
              //WS ----------
              let pointData = JSON.parse(JSON.stringify(self.scene))
              var frameData = new Uint8Array()
              frameData = makeStreamBuffer(pointData)
              console.log('Scene:',pointData.points)
              //console.log(frameData)
              self.socket.send(frameData)
              
              //-----------

              var end = Date.now()
              console.log(end-start)
              },
              //{ltres:1,pathomit:1,blurradius:5, qtres:1, colorsampling:0, colorquantcycles:1, numberofcolors:4, strokewidth:0 }
              //{qtres:0.1, colorsampling:0, linefilter:false ,pathomit:8,numberofcolors:7,colorquantcycles:10}
              //{qtres:0.1,colorsampling:0, numberofcolors:2}
              {colorsampling:0, numberofcolors:2}
          );
          } else {
          this.clearphoto();
          }
          
      },
      start(){
          console.log('start Cam')
          this.startup()
          this.takepictureint = setInterval(this.takepicture, this.captureIntervalMS);
      },
      stop(){
          console.log('stop Cam')
          clearInterval(this.takepictureint);
      }
  }
}
</script>

<style>
#video {
border: 1px solid black;
box-shadow: 2px 2px 3px black;
width: 640px;
height: 640px;
}
</style>