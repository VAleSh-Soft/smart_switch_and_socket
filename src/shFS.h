#pragma once

/*
 * Модуль для работы с файловой системой esp8266 и esp32; 
 *
 * Если вы хотите использовать файловую систему SPIFFS, то раскомментируйте
 * строку '#define USE_SPIFFS'; при этом для ESP32 опциях  платы нужно выбрать 
 * в "Partition cheme" схему "Default 4MB with spiffs";
 * В случае, если строка '#define USE_SPIFFS' останется закомментированной,
 * тип файловой системы будет выбран в зависимости от вашего МК.
 *
 * Для ESP32 используется файловая система FFat; для работы с ней в опциях 
 * платы нужно выбрать в "Partition cheme" схему "Default 4MB with ffat", если 
 * у вас есть модуль ESP32 c 4 МБ флеш-памяти, или "16MB FATFS", если у вас
 * модуль ESP32 c 16 МБ флеш-памяти;
 * 
 * Для ESP8266 используется файловая система LittleFS, в опциях платы ничего 
 * дополнительно выбирать не нужно
 *
 * Для ESP32 форматирование файловой системы выполняется автоматически при 
 * самом первом запуске или при первом запуске после изменения типа файловой
 * системы в коде; после форматирования модуль автоматически перезагружается;
 */

#include <FS.h>
#include <Arduino.h>

// #define USE_SPIFFS

// ===================================================

#if !defined(USE_SPIFFS)
#if defined(ARDUINO_ARCH_ESP32)

#define USE_FFAT // доступна только для esp32

#else

#define USE_LITTLEFS

#endif
#endif

// ===================================================

#if defined(USE_SPIFFS)
#if defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
#endif
#define FILESYSTEM SPIFFS
#elif defined(USE_LITTLEFS)
#include <LittleFS.h>
#define FILESYSTEM LittleFS
#elif defined(USE_FFAT) && defined(ARDUINO_ARCH_ESP32)
#include <FFat.h>
#define FILESYSTEM FFat
#else
#error First, specify the file system in the line '#define USE_xxxxx' from among the available
#endif

bool fs_init();

static const char FORMATTING_FS[] PROGMEM = "Formatting the file system...";
static const char WRITE_LABEL[] PROGMEM = "Write label file...";
static const char CHECKING_FS_LAB[] PROGMEM = "Checking file system label...";
static const char FS_INIT[] PROGMEM = "File system initialization...";
static const char FS_NAME[] PROGMEM = "File system - ";
static const char UNKN_FS[] PROGMEM = "Unsupported file system specified";
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
#else
  Serial.println(FPRST(UNKN_FS));
  return (false);
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