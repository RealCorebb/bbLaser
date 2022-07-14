


<template>
  <div>
    <img alt="bbLaser" src="./assets/logo.png" />
    <div style="display:flex;justify-content:space-evenly;width:100%">
      <el-button @click="paintVisible = true" type="primary">
          <div class="moduleButton">
            <el-icon :size="64" :color="color">
              <Aim />
            </el-icon>
            实时绘制
          </div>
      </el-button>
      
      <el-card class="moduleButton" shadow="always">
         <div class="moduleButton">
            <el-icon :size="64" :color="color">
              <Camera />
            </el-icon>
              相机串流 
          </div>
      </el-card>
        <el-dialog
          v-model="paintVisible"
          title="实时绘制"
          width="50%"
        >
          <Paint></Paint>
        </el-dialog>
    </div>
        

  </div>
  
</template>


<script>
  import Paint from './components/Paint.vue';
  export default {
    data: () => ({
      paintVisible: false
    }),
    components:{
      Paint
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

