#include "ILDAFile.h"
#include <WiFi.h>
#include <vector>

static const char *files[] = {
    "/bbLaser/hello.ild"
};

void setup() {
  Serial.begin(115200);
  setupSD();
  setupRenderer();
}

void loop() {
  // put your main code here, to run repeatedly:

}
