#include <Arduino.h>
#include "shFS.h"

static const char FORMATTING_FS[] PROGMEM = "Formatting the file system...";
static const char WRITE_LABEL[] PROGMEM = "Write label file...";
static const char CHECKING_FS_LAB[] PROGMEM = "Checking file system label...";
static const char FS_INIT[] PROGMEM = "File system initialization...";
static const char FS_NAME[] PROGMEM = "File system - ";
static const char FAILED_STR[] PROGMEM = "failed";
static const char OK_STR[] PROGMEM = "OK";

static String fsLabelFile = "fs";
static String fsName;

static void getLabel(FS *file_system, String &res)
{
  File file = file_system->open(fsLabelFile, "r");
  if (file)
  {
    size_t _size = file.size();
    uint8_t _buf[_size + 1] = {0};
    file.read(_buf, _size);
    char *_res = (char *)_buf;
    res = (String)_res;
  }
  file.close();
}

static bool fs_format(FS *file_system)
{
  Serial.print(FPSTR(FORMATTING_FS));
  bool result = file_system->format();

  if (result)
  {
    Serial.println(FPSTR(OK_STR));
    Serial.print(FPSTR(WRITE_LABEL));
    File file = file_system->open(fsLabelFile, "w");
    result = file;
    if (result)
    {
      uint8_t *_buf = new uint8_t[fsName.length()];
      memcpy(_buf, fsName.c_str(), fsName.length());
      file.write(_buf, fsName.length());
      delete[] _buf;
    }
    file.close();
    String lab;
    getLabel(file_system, lab);
    result = (lab == fsName);
    (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
  }
  else
  {
    Serial.println(FPSTR(FAILED_STR));
  }

  return (result);
}

static bool fs_check(FS *file_system)
{
  Serial.print(FPSTR(CHECKING_FS_LAB));
  bool result = file_system->exists(fsLabelFile);

  if (result)
  {
    String x;
    getLabel(file_system, x);
    result = (x == fsName);
  }
  (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));

  return (result);
}

bool fs_init(FS *file_system, const char *fs_name)
{
  fsName = (String)fs_name;

  Serial.print(FPSTR(FS_INIT));

  bool result = file_system->begin();
  (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));

  if (result)
  {
    Serial.print(FPSTR(FS_NAME));
    Serial.println(fsName);
    result = fs_check(file_system);
  }

  if (!result)
  {
    fs_format(file_system);
  }

  return (result);
}