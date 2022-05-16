#pragma once

#include <esp_attr.h>

#include <vector>

typedef struct spi_device_t *spi_device_handle_t; ///< Handle for a device on a SPI bus

class ILDAFile;

class SPIRenderer
{
private:
  TaskHandle_t spi_task_handle;
  spi_device_handle_t spi;
  void IRAM_ATTR draw();
  const std::vector<ILDAFile *> &ilda_files;
  volatile int draw_position;
  volatile int frame_position;
  volatile int file_position;

public:
  SPIRenderer(const std::vector<ILDAFile *> &ilda_files);
  void start();
  friend void spi_draw_timer(void *para);
};
