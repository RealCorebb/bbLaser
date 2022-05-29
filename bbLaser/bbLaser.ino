#include "ILDAFile.h"
#include "SPIRenderer.h"
#include <WiFi.h>

static const char *files[] = {
    "/bbLaser/hello.ild"
};

void setup() {
  Serial.begin(115200);
  setupSD();
  // read all the files in
  std::vector<ILDAFile *> ilda_files;
  ILDAFile *ilda = new ILDAFile();
  ilda->read(SD,files[0]);
  ilda_files.push_back(ilda);
  
  SPIRenderer *renderer = new SPIRenderer(ilda_files);
  renderer->start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
