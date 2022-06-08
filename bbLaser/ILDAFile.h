#pragma once

#include <stdint.h>
#include "FS.h"
#include "SD.h"

#pragma pack(1)
typedef struct
{
  char ilda[4];
  uint8_t reserved1[3];
  uint8_t format;
  char frame_name[8];
  char company_name[8];
  uint16_t records;
  uint16_t frame_number;
  uint16_t total_frames;
  uint8_t projector_number;
  uint8_t reserved2;
} ILDA_Header_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
  uint8_t status_code;
  uint8_t color;
} ILDA_Record_t;
#pragma pack()

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
  int file_frames;
  volatile int cur_frame;
  volatile int cur_buffer;
};
