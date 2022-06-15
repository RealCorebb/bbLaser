#include <AsyncTCP.h> //https://github.com/me-no-dev/AsyncTCP
#include "AsyncUDP.h"
#include <ESPAsyncWebServer.h>  //https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>
#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <esp_attr.h>
#define MAXFPS 60
#define MAXRECORDS 3000

static const char *TAG = "ilda";

AsyncWebServer server(80);

//=================  Basic Utils -_,-  =========================

void setupWifi(){
    WiFi.begin("Hollyshit_A", "00197633");
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Hi! I am bbLaser 66.");
    });
    AsyncElegantOTA.begin(&server);    // Start ElegantOTA
    server.begin();
 
  }

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void setupSD(){
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    
  }

//========================================================//


//================== ILDA -_,- ==============================//
typedef struct
{
  char ilda[4];
  uint8_t reserved1[3];
  uint8_t format;
  char frame_name[8];
  char company_name[8];
  volatile uint16_t records;
  uint16_t frame_number;
  uint16_t total_frames;
  uint8_t projector_number;
  uint8_t reserved2;
} ILDA_Header_t;

typedef struct
{
  volatile int16_t x;
  volatile int16_t y;
  volatile int16_t z;
  volatile uint8_t status_code;
  volatile uint8_t color;
} ILDA_Record_t;

typedef struct
{
  ILDA_Record_t *records;
  uint16_t number_records;
  bool isBuffered = false;
} ILDA_Frame_t;

class ILDAFile
{
private:
  void dump_header(const ILDA_Header_t &header);

public:
  ILDAFile();
  ~ILDAFile();
  bool read(fs::FS &fs,const char *fname);
  bool tickNextFrame();
  ILDA_Frame_t *frames;
  int buffer_frames;
  volatile int file_frames;
  volatile int cur_frame;
  volatile int cur_buffer;
};

ILDAFile::ILDAFile()
{
  buffer_frames = 10;
  frames = NULL;
  file_frames = 0;
  cur_frame = 0;
  cur_buffer = 0;
}

ILDAFile::~ILDAFile()
{
  free(frames);
}

void ILDAFile::dump_header(const ILDA_Header_t &header)
{
  char tmp[100];
  strncpy(tmp, header.ilda, 4);
  tmp[5] = '\0';
  ESP_LOGI(TAG, "Header: %s", tmp);
  ESP_LOGI(TAG, "Format Code: %d", header.format);
  strncpy(tmp, header.frame_name, 8);
  tmp[8] = '\0';
  ESP_LOGI(TAG, "Frame Name: %s", tmp);
  strncpy(tmp, header.company_name, 8);
  tmp[8] = '\0';
  ESP_LOGI(TAG, "Company Name: %s", tmp);
  ESP_LOGI(TAG, "Number records: %d", header.records);
  ESP_LOGI(TAG, "Number frames: %d", header.total_frames);
}


ILDA_Header_t header;
File file;
unsigned long frameStart;
ILDAFile *ilda = new ILDAFile();

bool ILDAFile::read(fs::FS &fs, const char *fname)
{
  file = fs.open(fname);
  if (!file)
  {
    return false;  
  }
  file.read((uint8_t *)&header, sizeof(ILDA_Header_t));
  header.records = ntohs(header.records);
  header.total_frames = ntohs(header.total_frames);
  dump_header(header);
  file_frames = header.total_frames;
  frameStart = file.position();
  return true;
}


bool ILDAFile::tickNextFrame()
{
    if(frames[cur_buffer].isBuffered == false){
      frames[cur_buffer].number_records = header.records;
      //frames[cur_buffer].records = (ILDA_Record_t *)ps_malloc(sizeof(ILDA_Record_t) * header.records);
      ILDA_Record_t *records = frames[cur_buffer].records;
      for (int i = 0; i < header.records; i++)
      {
        file.read((uint8_t *)(records + i), sizeof(ILDA_Record_t));
        records[i].x = ntohs(records[i].x);
        records[i].y = ntohs(records[i].y);
        records[i].z = ntohs(records[i].z);
      }
      // read the next header
      file.read((uint8_t *)&header, sizeof(ILDA_Header_t));
      header.records = ntohs(header.records);
      header.total_frames = ntohs(header.total_frames);
      
      cur_buffer++;
      if(cur_buffer > buffer_frames - 1) cur_buffer = 0;

      cur_frame++;
      //Serial.println(cur_frame);
      if(cur_frame > file_frames - 1){
          cur_frame = 0;
          Serial.println("Restart");
          ilda->read(SD,files[0]);
        }
      return true;
    }
    else return false;  //This frame has been buffered and not display yet.. 该帧已缓存且未Render，可能是读文件、串流太快了？忽视掉就好 0w0
}

//==============   Renderer -_,- ========================//

TaskHandle_t fileBufferHandle; 

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus
class SPIRenderer
{
private:
  TaskHandle_t spi_task_handle;
  spi_device_handle_t spi;
  volatile int draw_position;
  volatile int frame_position;

public:
  SPIRenderer();
  void IRAM_ATTR draw();
  void start();
  friend void spi_draw_timer(void *para);
};

#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <Ticker.h>

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 25
#define PIN_NUM_CLK 26
#define PIN_NUM_CS 27
#define PIN_NUM_LDAC 33
#define PIN_NUM_LASER_R 12
#define PIN_NUM_LASER_G 13
#define PIN_NUM_LASER_B 14

Ticker drawer;

SPIRenderer *renderer;


void draw_task(){
  renderer->draw();
}


void IRAM_ATTR SPIRenderer::draw()
{
  // Clear the interrupt
  // do we still have things to draw?
  if (draw_position < ilda->frames[frame_position].number_records)
  {
    const ILDA_Record_t &instruction = ilda->frames[frame_position].records[draw_position];
    int y = 2048 + (instruction.x * 1024) / 32768;
    int x = 2048 + (instruction.y * 1024) / 32768;
    //Serial.print(instruction.x);
    //Serial.print(" ");
    //Serial.println(instruction.y);
    // channel A
    spi_transaction_t t1 = {};
    t1.length = 16;
    t1.flags = SPI_TRANS_USE_TXDATA;
    t1.tx_data[0] = 0b11010000 | ((x >> 8) & 0xF);
    t1.tx_data[1] = x & 255;
    spi_device_polling_transmit(spi, &t1);
    // channel B
    spi_transaction_t t2 = {};
    t2.length = 16;
    t2.flags = SPI_TRANS_USE_TXDATA;
    t2.tx_data[0] = 0b01010000 | ((y >> 8) & 0xF);
    t2.tx_data[1] = y & 255;
    spi_device_polling_transmit(spi, &t2);
    // set the laser state
    digitalWrite(PIN_NUM_LASER_R, LOW);
    digitalWrite(PIN_NUM_LASER_G, LOW);
    digitalWrite(PIN_NUM_LASER_B, LOW);
    
    // DAC Load       
    digitalWrite(PIN_NUM_LDAC, LOW);
    digitalWrite(PIN_NUM_LDAC, HIGH);
    
    //让我们先把DAC ↑移动了，再把激光开启↓
    if ((instruction.status_code & 0b01000000) == 0)
    {
      if(instruction.color <=9){  //RED
        digitalWrite(PIN_NUM_LASER_R, HIGH);
      }
      else if (instruction.color <= 18){  //YELLOW
        digitalWrite(PIN_NUM_LASER_R, HIGH);
        digitalWrite(PIN_NUM_LASER_G, HIGH);
      }
      else if (instruction.color <= 27){ //GREEN
        digitalWrite(PIN_NUM_LASER_G, HIGH);
      }
      else if (instruction.color <= 36){  //CYAN
        digitalWrite(PIN_NUM_LASER_G, HIGH);
        digitalWrite(PIN_NUM_LASER_B, HIGH);
      }
      else if (instruction.color <= 45){ //BLUE
        digitalWrite(PIN_NUM_LASER_B, HIGH);
      }
      else if (instruction.color <= 54){ //Magenta
        digitalWrite(PIN_NUM_LASER_B, HIGH);
        digitalWrite(PIN_NUM_LASER_R, HIGH);
      }
      else if (instruction.color <= 63){ //WHITE
        digitalWrite(PIN_NUM_LASER_B, HIGH);
        digitalWrite(PIN_NUM_LASER_R, HIGH);
        digitalWrite(PIN_NUM_LASER_G, HIGH);
      }
    }
    
    draw_position++;
  }
  else
  {
    ilda->frames[frame_position].isBuffered = false;
    draw_position = 0;
    frame_position++;
    if (frame_position >= ilda->buffer_frames)
    {
      frame_position = 0;
    }
    xTaskNotifyGive( fileBufferHandle );
  }
}

SPIRenderer::SPIRenderer()
{
  frame_position = 0;
  draw_position = 0;
}

void SPIRenderer::start()
{

  pinMode(PIN_NUM_LASER_R,OUTPUT);
  pinMode(PIN_NUM_LASER_G,OUTPUT);
  pinMode(PIN_NUM_LASER_B,OUTPUT);
  pinMode(PIN_NUM_LDAC,OUTPUT);

  // setup SPI output
  esp_err_t ret;
  spi_bus_config_t buscfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = PIN_NUM_MISO,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0};
  spi_device_interface_config_t devcfg = {
      .command_bits = 0,
      .address_bits = 0,
      .dummy_bits = 0,
      .mode = 0,
      .clock_speed_hz = 40000000,
      .spics_io_num = PIN_NUM_CS, //CS pin
      .flags = SPI_DEVICE_NO_DUMMY,
      .queue_size = 2,
  };
  //Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  printf("Ret code is %d\n", ret);
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  printf("Ret code is %d\n", ret);

  xTaskCreatePinnedToCore(
    fileBufferLoop
    ,  "fileBufferHandle"
    ,  5000  // Stack size
    ,  NULL
    ,  4  // Priority
    ,  &fileBufferHandle
    ,  0); 
}


//Current ILDA Buffer  当前的ILDA内存，采用Buffer的形式，为了能更快的加载大型ILDA文件。动态读取文件，申请内存，避免一下子把整个ILDA文件的所有帧的内存都申请了（没有那么多PSRAM）

void setupRenderer(){
    Serial.print("RAM Before:");
    Serial.println(ESP.getFreePsram());
    ilda->frames = (ILDA_Frame_t *)ps_malloc(sizeof(ILDA_Frame_t) * ilda->buffer_frames);
    for(int i =0;i<ilda -> buffer_frames;i++){
        ilda->frames[i].records = (ILDA_Record_t *)ps_malloc(sizeof(ILDA_Record_t) * MAXRECORDS);
      }
    Serial.print("RAM After:");
    Serial.println(ESP.getFreePsram());
    ilda->read(SD,files[0]);
    renderer = new SPIRenderer();
    renderer->start();
  }

//===================================//

//  Core 1 //

void fileBufferLoop(void *pvParameters){
  for (;;)
  {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    if(!ilda->tickNextFrame()){
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
     }
  }
}


// ================= Streaming -_,- =========================//
AsyncUDP udp;

void setupUdpStream(){
  if(udp.listen(6969)) {
    udp.onPacket([](AsyncUDPPacket packet) {
        Serial.print("UDP Packet Type: ");
        Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
        Serial.print(", From: ");
        Serial.print(packet.remoteIP());
        Serial.print(":");
        Serial.print(packet.remotePort());
        Serial.print(", To: ");
        Serial.print(packet.localIP());
        Serial.print(":");
        Serial.print(packet.localPort());
        Serial.print(", Length: ");
        Serial.print(packet.length());
        Serial.print(", Data: ");
        Serial.write(packet.data(), packet.length());
        Serial.println();
        //reply to the client
        packet.printf("Got %u bytes of data", packet.length());
  
         /*
         // read the next header
          file.read((uint8_t *)&header, sizeof(ILDA_Header_t));
          header.records = ntohs(header.records);
       
         for (int i = 0; i < header.records; i++)
          {
            file.read((uint8_t *)(records + i), sizeof(ILDA_Record_t));
            records[i].x = ntohs(records[i].x);
            records[i].y = ntohs(records[i].y);
            records[i].z = ntohs(records[i].z);
          }
          
        */
    });
  }
}
