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

        </div>
        
      </div>
</template>

<script>
import ImageTracer from 'imagetracerjs'

export default {
    data: () => ({
            takepictureint:'',
            captureIntervalMS: 30,
            width:320,
            height:0,
        }),
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
                console.log(svgstr)


                //remove border  M 0 0 L 320 0 L 320 180 L 0 180 L 0 0 Z 

                ImageTracer.appendSVGString( svgstr, 'svgcontainer' ); 
                var end = Date.now()
                console.log(end-start)
                },
                {ltres:1,pathomit:1, qtres:1, colorsampling:0, colorquantcycles:1, numberofcolors:4, strokewidth:0 }
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