#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <esp_attr.h>
#define MAXRECORDS 3000
#include <string>

File root;
static const char *TAG = "ilda";
const int bufferFrames = 3;


DynamicJsonDocument doc(4096);
JsonArray avaliableMedia = doc.to<JsonArray>();
int curMedia = -1;
bool isAutoNext = true;




//=================  Basic Utils -_,-  =========================


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

    root=SD.open("/bbLaser"); 
    while(true){
      File entry = root.openNextFile();
      if(!entry) break;
      if(!entry.isDirectory() && String(entry.name()).indexOf(".ild") != -1){
          Serial.println(entry.name());
          avaliableMedia.add(String(entry.name()));
        }
      }
    
  }

  void goNext(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 30) 
    {
      nextMedia(0);
    }
    last_interrupt_time = interrupt_time;
  }

  void goPrev(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 30) 
    {
      nextMedia(-1);
    }
    last_interrupt_time = interrupt_time;
  }

  void changeAutoNext(){
    if(digitalRead(15) == LOW) isAutoNext = true;
    else isAutoNext = false;
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
  bool parseStream(uint8_t *data, size_t len, int index, int totalLen);
  ILDA_Frame_t *frames;
  volatile int file_frames;
  volatile int cur_frame;
  volatile int cur_buffer;
};

ILDAFile::ILDAFile()
{
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
  //Serial.println(file_frames);
  return true;
}


bool ILDAFile::tickNextFrame()
{
    if(frames[cur_buffer].isBuffered == false){
      //frames[cur_buffer].isBuffered = true;
      frames[cur_buffer].number_records = header.records;
      //frames[cur_buffer].records = (ILDA_Record_t *)malloc(sizeof(ILDA_Record_t) * header.records);
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
      if(cur_buffer > bufferFrames - 1) cur_buffer = 0;

      cur_frame++;
      //Serial.println(cur_frame);
      if(cur_frame > file_frames - 1){
          cur_frame = 0;
          if(isAutoNext){
            nextMedia(0);
          }
        }
      
      return true;
    }
    else return false;  //This frame has been buffered and not display yet.. 该帧已缓存且未Render，可能是读文件、串流太快了？忽视掉就好 0w0
}

#define bufferLen 6
int loadedLen = 0;
bool ILDAFile::parseStream(uint8_t *data, size_t len, int frameIndex, int totalLen)
{
    if(frames[cur_buffer].isBuffered == false){
      //frames[cur_buffer].isBuffered = true;
      frames[cur_buffer].number_records = totalLen/bufferLen;
      ILDA_Record_t *records = frames[cur_buffer].records;
     
      Serial.print("Len: ");
      Serial.println(len);
      Serial.print("Get Frame: ");
      
      Serial.print(frameIndex);
      Serial.print(" / ");
      Serial.print(totalLen);
      Serial.print("  ");
      Serial.println(cur_buffer);

      for(size_t i=0; i < len/bufferLen;i++){
        int16_t x = (data[i*bufferLen] << 8) | data[i*bufferLen + 1];
        int16_t y = (data[i*bufferLen + 2] << 8) | data[i*bufferLen + 3];
        
        /*
        Serial.print(frameIndex/bufferLen + i);
        Serial.print(",");
        Serial.print(x);
        Serial.print(",");
        Serial.println(y);
        */
        /*
        Serial.print(",");
        Serial.print(data[i*bufferLen+4]);
        Serial.print(",");
        Serial.println(data[i*bufferLen+5]);
        Serial.println((data[i*bufferLen+5] & 0b01000000) == 0);*/
        
        
        records[frameIndex/bufferLen + i].x = x;
        records[frameIndex/bufferLen + i].y = y;
        records[frameIndex/bufferLen + i].z = 0;
        records[frameIndex/bufferLen + i].color = data[i*bufferLen+4];
        records[frameIndex/bufferLen + i].status_code = data[i*bufferLen+5];
      }
      loadedLen += len;
      
      if(loadedLen >= totalLen){
        Serial.println("Frame End");
        loadedLen = 0;
        cur_buffer++;
        if(cur_buffer > bufferFrames - 1) cur_buffer = 0;

        cur_frame++;
        //Serial.println(cur_frame);
        if(cur_frame > file_frames - 1){
            cur_frame = 0;
          }
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
#define PIN_NUM_LASER_R 13
#define PIN_NUM_LASER_G 16
#define PIN_NUM_LASER_B 17

Ticker drawer;

SPIRenderer *renderer;


void draw_task(){
  renderer->draw();
}


void IRAM_ATTR SPIRenderer::draw()
{
  // Clear the interrupt
  // do we still have things to draw?
  //Serial.println(ilda->frames[frame_position].number_records);
  if (draw_position < ilda->frames[frame_position].number_records)
  {
    const ILDA_Record_t &instruction = ilda->frames[frame_position].records[draw_position];
    int y = 2048 + (instruction.x * 1024) / 32768;
    int x = 2048 + (instruction.y * 1024) / 32768;
    //Serial.print(instruction.x);
   //Serial.print(" ");
    //Serial.println(instruction.y);
    // set the laser state
    
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
    
    
    
    //把激光开启↓
    if ((instruction.status_code & 0b01000000) == 0)
    {
      if(instruction.color <=9){  //RED
        digitalWrite(PIN_NUM_LASER_R, LOW);
      }
      else if (instruction.color <= 18){  //YELLOW
        digitalWrite(PIN_NUM_LASER_R, LOW);
        digitalWrite(PIN_NUM_LASER_G, LOW);
      }
      else if (instruction.color <= 27){ //GREEN
        digitalWrite(PIN_NUM_LASER_G, LOW);
      }
      else if (instruction.color <= 36){  //CYAN
        digitalWrite(PIN_NUM_LASER_G, LOW);
        digitalWrite(PIN_NUM_LASER_B, LOW);
      }
      else if (instruction.color <= 45){ //BLUE
        digitalWrite(PIN_NUM_LASER_B, LOW);
      }
      else if (instruction.color <= 54){ //Magenta
        digitalWrite(PIN_NUM_LASER_B, LOW);
        digitalWrite(PIN_NUM_LASER_R, LOW);
      }
      else if (instruction.color <= 63){ //WHITE
        digitalWrite(PIN_NUM_LASER_B, LOW);
        digitalWrite(PIN_NUM_LASER_R, LOW);
        digitalWrite(PIN_NUM_LASER_G, LOW);
      }
    }
    else{   //不亮的Point
      digitalWrite(PIN_NUM_LASER_R, HIGH);
      digitalWrite(PIN_NUM_LASER_G, HIGH);
      digitalWrite(PIN_NUM_LASER_B, HIGH);
    }

    // DAC Load   
    digitalWrite(PIN_NUM_LDAC, LOW);
    digitalWrite(PIN_NUM_LDAC, HIGH);
    
    draw_position++;
  }
  else
  {
    ilda->frames[frame_position].isBuffered = false;
    draw_position = 0;
    frame_position++;
    if (frame_position >= bufferFrames)
    {
      frame_position = 0;
    }
    if(!isStreaming){
      xTaskNotifyGive( fileBufferHandle );
    }
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
    ,  3  // Priority
    ,  &fileBufferHandle
    ,  0); 
}


//Current ILDA Buffer  当前的ILDA内存，采用Buffer的形式，为了能更快的加载大型ILDA文件。动态读取文件，申请内存，避免一下子把整个ILDA文件的所有帧的内存都申请了（没有那么多PSRAM）
uint8_t chunkTemp[64];
int tempLen = 0;

void setupRenderer(){
    Serial.print("RAM Before:");
    Serial.println(ESP.getFreeHeap());
    ilda->frames = (ILDA_Frame_t *)malloc(sizeof(ILDA_Frame_t) * bufferFrames);
    for(int i =0;i<bufferFrames;i++){
        ilda->frames[i].records = (ILDA_Record_t *)malloc(sizeof(ILDA_Record_t) * MAXRECORDS);
      }
    Serial.print("RAM After:");
    Serial.println(ESP.getFreeHeap());
    nextMedia(1);
    renderer = new SPIRenderer();
    renderer->start();
  }


void handleStream(uint8_t *data, size_t len,int index, int totalLen){
    //Serial.println("Stream");

    int newtempLen = (tempLen+len) % 6;
    //Serial.print("newTemp:");
    //Serial.println(newtempLen);
    if(tempLen > 0){      
      //memcpy(chunkTemp+tempLen, data, len - newtempLen);
      uint8_t concatData[len-newtempLen+tempLen];
      memcpy(concatData, chunkTemp, tempLen);
      memcpy(concatData + tempLen, data, len-newtempLen);  // copy the address
      //Serial.print("Temp Concat Len: ");
      //Serial.println(len-newtempLen+tempLen);
      ilda->parseStream(concatData,len-newtempLen+tempLen,index-tempLen,totalLen);
    }
    else{
      //Serial.print("No Concat Len: ");
      //Serial.println(len-newtempLen+tempLen);
      ilda->parseStream(data,len-newtempLen,index,totalLen);
    }
    for(size_t i=0; i < newtempLen;i++){
      chunkTemp[i] = data[len - newtempLen + i];
    }
    tempLen = newtempLen;
  }

void nextMedia(int position){
  if(position == 0){
    curMedia++; 
  }
  else if(position == -1){
    curMedia--;
  }
  else{
    curMedia = curMedia + position;
  }
  if(curMedia >= avaliableMedia.size()) curMedia=0;
  if(curMedia < 0) curMedia = avaliableMedia.size() - 1;
  String filePath = String("/bbLaser/") += avaliableMedia[curMedia].as<String>();
  ilda->read(SD,filePath.c_str());
}

//===================================//

//  Core 2 //
unsigned long timeDog = 0;
void fileBufferLoop(void *pvParameters){
  for (;;)
  {
    if(millis() - timeDog > 1000){
      timeDog = millis();
      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed=1;
      TIMERG0.wdt_wprotect=0;
    }
    if(!isStreaming){    
      if(!ilda->tickNextFrame()){
          ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
      }
    }
  }
}
