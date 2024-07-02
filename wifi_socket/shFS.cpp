#include <Arduino.h>
#include "shFS.h"

static const char FORMATTING_FS[] PROGMEM = "Formatting the file system...";
static const char WRITE_LABEL[] PROGMEM = "Write label file...";
static const char CHECKING_FS_LAB[] PROGMEM = "Checking file system label...";
static const char FS_INIT[] PROGMEM = "File system initialization...";
static const char FS_NAME[] PROGMEM = "File system - ";
static const char NO_WRITE[] PROGMEM = " - failed to open file for writing";
static const char NO_READ[] PROGMEM = " - failed to open file for reading";
static const char RESTART[] PROGMEM = "The module will restart now";
static const char FAILED_STR[] PROGMEM = "failed";
static const char OK_STR[] PROGMEM = "OK";

static String fsLabelFile = "/fs";
String fsName;

String readFile(const char *path)
{
#if defined(ARDUINO_ARCH_ESP8266)
  File file = FILESYSTEM.open(path, "r");
#else
  File file = FILESYSTEM.open(path, "r", false);
#endif
  if (!file || file.isDirectory())
  {
    Serial.print(path);
    Serial.println(FPSTR(NO_READ));
    return (emptyString);
  }

  size_t _size = file.size();
  uint8_t _buf[_size + 1] = {0};
  file.read(_buf, _size);
  char *_res = (char *)_buf;

  file.close();

  return ((String)_res);
}

static bool writeFile(const char *_path, String _text)
{
  bool result = false;

#if defined(ARDUINO_ARCH_ESP8266)
  File file = FILESYSTEM.open(_path, "w");
#else
  File file = FILESYSTEM.open(_path, "w", true);
#endif
  if (!file)
  {
    Serial.print(_path);
    Serial.println(FPSTR(NO_WRITE));
    return (result);
  }

#if defined(ARDUINO_ARCH_ESP8266)
  uint8_t *_buf = new uint8_t[_text.length()];
  memcpy(_buf, _text.c_str(), _text.length());
  size_t n = file.write(_buf, _text.length());
  delete[] _buf;
  result = (n == _text.length());
#else
  result = file.print(_text.c_str());
#endif
  file.close();

  return (result);
}

static bool fs_format()
{
  Serial.print(FPSTR(FORMATTING_FS));
  bool result = FILESYSTEM.format();
  (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
#if defined(ARDUINO_ARCH_ESP32)
  if (result)
  {
    Serial.println(FPSTR(RESTART));
    Serial.println();
    ESP.restart();
  }
#endif

  return (result);
}

static bool fs_check()
{
  bool result = FILESYSTEM.exists(fsLabelFile.c_str());
  if (!result)
  {
    Serial.print(FPSTR(WRITE_LABEL));
    result = writeFile(fsLabelFile.c_str(), fsName);
    (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
  }
  else
  {
    Serial.print(FPSTR(CHECKING_FS_LAB));
    String x = readFile(fsLabelFile.c_str());
    result = (x == fsName);
    (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
  }

  return (result);
}

bool fs_init()
{
#if defined(USE_SPIFFS)
  fsName = "SPIFFS";
#elif defined(USE_LITTLEFS)
  fsName = "LittleFS";
#elif defined(USE_FFAT) && defined(ARDUINO_ARCH_ESP32)
  fsName = "FFat";
#endif

  Serial.print(FPSTR(FS_INIT));
  bool result = FILESYSTEM.begin();
  (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));

  if (result)
  {
    Serial.print(FPSTR(FS_NAME));
    Serial.println(fsName);

    result = fs_check();
  }

  if (!result)
  {
    result = fs_format();
  }

  return (result);
}
