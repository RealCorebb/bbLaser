#include <esp_attr.h>

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus
class ILDAFile;
class SPIRenderer
{
private:
  TaskHandle_t spi_task_handle;
  spi_device_handle_t spi;
  
  const std::vector<ILDAFile *> &ilda_files;
  volatile int draw_position;
  volatile int frame_position;
  volatile int file_position;

public:
  SPIRenderer(const std::vector<ILDAFile *> &ilda_files);
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
#include "ILDAFile.h"

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
std::vector<ILDAFile *> ilda_files;


void draw_task(){
  renderer->draw();
}


void IRAM_ATTR SPIRenderer::draw()
{
  // Clear the interrupt
  // do we still have things to draw?
  if (draw_position < ilda_files[file_position]->frames[frame_position].number_records)
  {
    //ESP_LOGI(TAG, "instruction: %d ,%d , %d", file_position,frame_position,draw_position);
    const ILDA_Record_t &instruction = ilda_files[file_position]->frames[frame_position].records[draw_position];
    int y = 2048 + (instruction.x * 1024) / 32768;
    int x = 2048 + (instruction.y * 1024) / 32768;
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
    draw_position = 0;
    frame_position++;
    if (frame_position >= ilda_files[file_position]->buffer_frames)
    {
      frame_position = 0;
      file_position++;
      if (file_position == ilda_files.size())
      {
        file_position = 0;
      }
    }
  }
}

SPIRenderer::SPIRenderer(const std::vector<ILDAFile *> &ilda_files) : ilda_files(ilda_files)
{
  file_position = 0;
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
}


//Current ILDA Buffer  当前的ILDA内存，采用Buffer的形式，为了能更快的加载大型ILDA文件。动态读取文件，申请内存，避免一下子把整个ILDA文件的所有帧的内存都申请了（没有那么多PSRAM）
ILDAFile *ilda = new ILDAFile();

void setupRenderer(){
    
    
    ilda->read(SD,files[0]);
    ilda_files.push_back(ilda);
    renderer = new SPIRenderer(ilda_files);
    renderer->start();
  }
