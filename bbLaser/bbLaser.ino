#include <WiFi.h>
#include <vector>
#include "FS.h"
#include "SD.h"
#include "SPI.h"


int kppsTime = 1000000/(20*1000);
volatile unsigned long timeOld;

void setup() {
  Serial.begin(115200);
  setupSD();
  setupWifi();
  setupRenderer();

Serial.println(kppsTime);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(micros() - timeOld >= kppsTime){
      timeOld = micros();
      draw_task();
   }
}
