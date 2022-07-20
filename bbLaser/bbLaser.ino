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

volatile unsigned long timeStart;
String frameData = "";
// ================= Streaming -_,- =========================//
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      for(size_t i=0; i < info->len; i++){
        //Serial.print(data[i]);
        //data[len] = 0;
       // frameData = (char *)data;
       Serial.println(len);
      }
    }
    else{
      if(info->index == 0){
        if(info->num == 0)
          //Serial.println("MSG Start");
        Serial.println("Frame Start");
        timeStart = millis();
        //frameData = "";
      }
      //data[len] = 0;
      Serial.println(len);
      //frameData += String((char*) data);
      if((info->index + len) == info->len){
        Serial.println("Frame End");
        if(info->final){
          Serial.println(millis()-timeStart);
          Serial.println("MSG End");
          //Serial.println(frameData.length());
        }
      }
    }

    /*
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      /*
          StaticJsonDocument<100> pdoc;
          DeserializationError err = deserializeJson(pdoc, data);
          if (err) {
              Serial.print(F("deserializeJson() failed with code "));
              Serial.println(err.c_str());
              return;
          }
          //Serial.println(count);
          //Serial.println(pdoc["x"].as<float>());
    }
    else{
       
    }*/
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    ESP_LOGI("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnecteds
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
  /*
  if(micros() - timeOld >= kppsTime){
      timeOld = micros();
      draw_task();
   }*/
}
