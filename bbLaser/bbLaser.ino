#include "ILDAFile.h"
#include <WiFi.h>
#include <vector>

static const char *files[] = {
    "/bbLaser/hello.ild"
};

int kppsTime = 1000000/(25*1000);
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
