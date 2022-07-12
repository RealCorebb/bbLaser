<template>
    <span>This is a message</span>
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
    }
}
</script>

<style>

</style>