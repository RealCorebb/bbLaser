#include <WiFi.h>
#include <vector>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <AsyncTCP.h> //https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  //https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>
#include "Button2.h"
#include <Adafruit_NeoPixel.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int kppsTime = 1000000 / (20 * 1000);
volatile unsigned long timeOld;
int isAutoNext = 1;

volatile unsigned long timeStart;
// ================= Streaming -_,- =========================//
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  //单帧
  if (info->final && info->index == 0 && info->len == len) {
    handleStream(data, len, 0, info->len);
  }
  //多帧
  else {
    if (info->index == 0) {
      //if (info->num == 0)
        //Serial.println("MSG Start");
        //Serial.println("Frame Start");
        //handleStream(data, len, 0, info->len);
    }
    //Serial.print(info->index);
    //Serial.print(" ");
    //Serial.println(len);
    if ((info->index + len) == info->len) {
      //Serial.println("Frame End");
      if (info->final) {
        //Serial.println("MSG End");
        //Serial.println(frameLen);
        handleStream(data, len, info->index, info->len);
      }
    }
    else{
      handleStream(data, len, info->index, info->len);
    }
  }
}

bool isStreaming = false;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    //client connected
    ESP_LOGI("ws[%s][%u] connect\n", server->url(), client->id());
    //client->printf("I am bbLaser :)", client->id());
    //client->ping();
    isStreaming = true;
  } else if (type == WS_EVT_DISCONNECT) {
    //client disconnecteds
    ESP_LOGI("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
    isStreaming = false;
  } else if (type == WS_EVT_DATA) {
    handleWebSocketMessage(arg, data, len);

  }
}

// ================ LEDS  -_,- ======================/

  #define LED_COUNT 9
  Adafruit_NeoPixel strip(LED_COUNT, 2, NEO_GRB + NEO_KHZ800);     // 10 WS2812 @ PIN2
  unsigned long pixelPrevious = 0;        // Previous Pixel Millis
  int           pixelInterval = 50;       // Pixel Interval (ms)
  int           pixelQueue = 0;           // Pattern Pixel Queue
  int           pixelCycle = 0;           // Pattern Pixel Cycle
  uint16_t      pixelCurrent = 0;         // Pattern Current Pixel Number
  uint16_t      pixelNumber = LED_COUNT;  // Total Number of Pixels
  uint16_t      progressNum = 0;
  
// ==================================

Button2 buttonL, buttonR , buttonHappy;

void setup() {

  
  
  Serial.begin(115200);
  setupSD();

  //---------------- Core Featuers -_,-  --------------------//
  WiFi.begin("Hollyshit_A", "00197633");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->redirect("http://bblaser.bbrealm.com/?ip=" + WiFi.localIP().toString());
  });
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  // attach AsyncWebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
  setupRenderer();
  
   //---------------- Buttons -_,-  --------------------//
  buttonL.begin(21 ,INPUT_PULLUP ,false ,false);
  buttonL.setTapHandler(click);

  buttonR.begin(22 ,INPUT_PULLUP ,false ,false);
  buttonR.setTapHandler(click);

  buttonHappy.begin(15 ,INPUT_PULLUP ,false ,false);
  buttonHappy.setPressedHandler(pressed);
  buttonHappy.setReleasedHandler(released);

  //----------------  LEDS -_,- ------------------------//
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(100);

  xTaskCreatePinnedToCore(
    ledLoop
    ,  "ledLoop"
    ,  2048  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL
    ,  0); 
    
}

void loop() {
  // put your main code here, to run repeatedly:

  if (micros() - timeOld >= kppsTime) {
    timeOld = micros();
    draw_task();
  }
  buttonL.loop();
  buttonR.loop();
  buttonHappy.loop();
}

void click(Button2& btn) {
    if (btn == buttonL) {
      goPrev();
    } else if (btn == buttonR) {
      goNext();
    }
}

void pressed(Button2& btn) {
    if (btn == buttonHappy) {
      isAutoNext = 1;
      //Serial.println("T");
    }
}

void released(Button2& btn) {
    if (btn == buttonHappy) {
      isAutoNext = 0;
      //Serial.println("F");
    }
}
