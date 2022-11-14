<template>
  <div>
    <img alt="bbLaser" src="./assets/logo.png" />
    <div style="display:flex;justify-content:space-evenly;width:100%">
      <el-button @click="paintVisible = true" type="primary">
          <div class="moduleButton">
            <el-icon :size="64" :color="color">
              <Aim />
            </el-icon>
            <span>实时绘制</span>
          </div>
      </el-button>

<!--
      <el-button @click="musicVisible = true" type="primary">
          <div class="moduleButton">
            <el-icon :size="64" :color="color">
              <Aim />
            </el-icon>
            音乐
          </div>
      </el-button>
      -->
      
      <el-button @click="camVisible = true" type="primary">
         <div class="moduleButton">
            <el-icon :size="64" :color="color">
              <VideoCamera />
            </el-icon>
              <span>相机串流</span>
          </div>
      </el-button>

        <el-dialog
          v-model="paintVisible"
          title="实时绘制"
          width="60%"
        >
          <Paint></Paint>
        </el-dialog>
        <!--
        <el-dialog
          v-model="musicVisible"
          title="音乐"
          width="50%"
        >
          <Music></Music>
        </el-dialog>
        -->
        <el-dialog
          @opened="startCam"
          @closed="stopCam"
          v-model="camVisible"
          title="相机串流"
          width="50%"
        >
          <CameraPage ref="camera"></CameraPage>
        </el-dialog>
    </div>
        

  </div>
  
</template>


<script>
  import Paint from './components/Paint.vue';
  //import Music from './components/Music.vue';
  import CameraPage from './components/Camera.vue';
  export default {
    data: () => ({
      paintVisible: false,
      musicVisible: false,
      camVisible: false
    }),
    components:{
      Paint,
      //Music,
      CameraPage
    },
    methods:{
      startCam(){
        //call Camera start
        this.$refs.camera.start();
      },
      stopCam(){
        this.$refs.camera.stop();
      }
    }
  }
</script>


<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}
body{
  height:100vh;
  width: 100vw;
  background-color: #232c35;
}

.moduleButton{
  display: inline-grid;
}

.el-button{
  height: unset !important;
}
.el-dialog__header  {
    color:white;
    margin-right: 0px;
    background-color: #393f44;
  }
  /*body背景色*/
  .el-dialog__body  {
    background-color: #393f44;
  }
  .el-dialog__title{
    color:white;
  }
</style>

