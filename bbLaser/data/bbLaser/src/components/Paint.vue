<template>
      <div id="wrapper">
        <div id="buildZone">
          <canvas id="canvas"></canvas>
          <div id="controls">
            <i id="square" class="feather icon-square"></i>
            <i id="circle" class="feather icon-circle"></i>
            <i id="triangle" class="feather icon-triangle"></i>
            <i id="clear" class="feather icon-trash"></i>
          </div>
        </div>
        <div id="styleZone"></div>
      </div>
</template>

<script>
import { DAC } from '@laser-dac/core';
import { Scene } from '@laser-dac/draw';
export default {
  data: () => ({
    paintVisible: true
  }),
  created() {
      console.log('paint created');
      const scene = new Scene();
      const dac = new DAC();
    },
  mounted(){
      console.log('paint mounted')
  }
}
</script>

<style>

* {
  box-sizing: border-box;
}

main {
  width: 800px;
  border: 1px solid #e0e0e0;
  margin: 0 auto;
  display: flex;
  flex-grow: 1;
}

</style>



<style lang="scss" scoped>
  // VARIABLES  ―――――――――――――――――――――――――

$mainTint: #2c2f36;
$defaultTextColor: white;
$primaryColor: #4881b8;
$dangerColor: #e54f6b;
$controlsRatio: 1.3;


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
	background: $mainTint;
	color: $defaultTextColor;
	padding: 30px;
}

#wrapper { margin: auto; }
#buildZone { display: flex; }

.canvas-container, #canvas { transition: all 0.2 ease-in-out; }

#canvas {
	background: darken($mainTint, 5%);
	border-radius: 5px;
}


// CONTROLS  ―――――――――――――――――――――――――

#controls {
	margin-left: 20px;
	display: flex;
	flex-direction: column;
	
	i {
		background: $primaryColor;
		padding: 15px;
		display: flex;
		align-items: center;
		justify-content: center;
		font-size: #{$controlsRatio}em;
		border-radius: 100%;
		transition: all 0.2s ease-in-out;
		margin: 5px 0;
		
		&:hover {
			background: lighten($primaryColor, 8%);
			cursor: pointer;
		}
	}
	
	#clear {
		background: $dangerColor;
		margin-top: auto;
		&:hover { background: lighten($dangerColor, 8%); }
	}
}

// STYLE-ZONE  ―――――――――――――――――――――――――

#styleZone {
	display: flex;
	margin-top: 20px;
	
	span {
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
			border-color: $defaultTextColor;
		}
	}
	
	i {
		opacity: 0;
		transition: all 0.3s cubic-bezier(.56, .35, 0, 1.7);
		transform: scale(0.6);
	}
}
</style>