#include <WiFi.h>
#include <vector>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <AsyncTCP.h> //https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  //https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int kppsTime = 1000000/(20*1000);
volatile unsigned long timeOld;


// ================= Streaming -_,- =========================//

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if((char *)data[0] == "S"){
            Serial.println("START");
          }
        else if ((char *)data[0] =="E"){
           Serial.println("END");
          }
        else{
          //StaticJsonDocument<200> pdoc;
          DynamicJsonDocument pdoc(200);
          DeserializationError err = deserializeJson(pdoc, data);
          if (err) {
              Serial.print(F("deserializeJson() failed with code "));
              Serial.println(err.c_str());
              return;
          }
          //Serial.println(pdoc["x"].as<float>());
        }
    }
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    ESP_LOGI("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    ESP_LOGI("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_DATA){
    handleWebSocketMessage(arg, data, len);
  }
}



void setup() {
  Serial.begin(115200);
  setupSD();
  

  WiFi.begin("Hollyshit_A", "00197633");
    
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am bbLaser 66.");
  });
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  // attach AsyncWebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();

    
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
