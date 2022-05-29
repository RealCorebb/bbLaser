#include <string.h>
#include "esp_log.h"
#include "ILDAFile.h"
#include <WiFi.h>

static const char *TAG = "ilda";

ILDAFile::ILDAFile()
{
  frames = NULL;
  num_frames = 0;
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

bool ILDAFile::read(fs::FS &fs, const char *fname)
{
  File file = fs.open(fname);
  if (!file)
  {
    return false;  
  }
  ILDA_Header_t header;
  file.read((uint8_t *)&header, sizeof(ILDA_Header_t));
  header.records = ntohs(header.records);
  header.total_frames = ntohs(header.total_frames);
  dump_header(header);
  // allocate space for the frames
  frames = (ILDA_Frame_t *)ps_malloc(sizeof(ILDA_Frame_t) * header.total_frames);
  num_frames = header.total_frames;
  // read in each frame
  for (int frame_idx = 0; frame_idx < header.total_frames; frame_idx++)
  {
    frames[frame_idx].number_records = header.records;
    frames[frame_idx].records = (ILDA_Record_t *)ps_malloc(sizeof(ILDA_Record_t) * header.records);
    ILDA_Record_t *records = frames[frame_idx].records;
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
  }
  
  return true;
}
