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
#define PIN_NUM_LDAC GPIO_NUM_33
#define PIN_NUM_LASER GPIO_NUM_12

Ticker drawer;
int kpps = 15;
SPIRenderer *renderer;


void draw_task(){
  renderer->draw();
}


void IRAM_ATTR SPIRenderer::draw()
{
  // Clear the interrupt
  Serial.println("Draw");
  // do we still have things to draw?
  if (draw_position < ilda_files[file_position]->frames[frame_position].number_records)
  {
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
    if ((instruction.status_code & 0b01000000) == 0)
    {
      gpio_set_level(PIN_NUM_LASER, 1);
    }
    else
    {
      gpio_set_level(PIN_NUM_LASER, 0);
    }
    // load the DAC
    gpio_set_level(PIN_NUM_LDAC, 0);
    gpio_set_level(PIN_NUM_LDAC, 1);

    draw_position++;
  }
  else
  {
    draw_position = 0;
    frame_position++;
    if (frame_position == ilda_files[file_position]->num_frames)
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

void spi_timer_setup(void *param)
{
  printf("Setup Timer");
  drawer.attach(1/(15 * 1000), draw_task);
  printf("Setup Timer Done");
}

void SPIRenderer::start()
{
  // setup the laser
  gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT);

  // setup the LDAC output
  gpio_set_direction(PIN_NUM_LDAC, GPIO_MODE_OUTPUT);

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
  //assert(ret == ESP_OK);
  //Attach the SPI device
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  printf("Error message %s\n", esp_err_to_name(ret));
  printf("Ret code is %d\n", ret);
  //assert(ret == ESP_OK);

  // this will oin the timer task to core 1 - probably not needed if you aren't using WiFi etc..
  printf("Xtask set");
  TaskHandle_t timer_setup_handle;
  
  xTaskCreatePinnedToCore(spi_timer_setup, "Draw Task", 4096, this, 0, &timer_setup_handle, 1);
  printf("Xtask set done");
}

void setupRenderer(){
    std::vector<ILDAFile *> ilda_files;
    ILDAFile *ilda = new ILDAFile();
    ilda->read(SD,files[0]);
    ilda_files.push_back(ilda);
    renderer = new SPIRenderer(ilda_files);
    renderer->start();
  }
