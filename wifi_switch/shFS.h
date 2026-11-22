#pragma once

/*
 * Модуль для работы с файловой системой esp8266 и esp32; 
 *
 * Если вы хотите использовать файловую систему SPIFFS, то раскомментируйте
 * строку '#define SH_USE_SPIFFS'; при этом для ESP32 опциях  платы нужно выбрать 
 * в "Partition cheme" схему "Default 4MB with spiffs";
 * В случае, если строка '#define SH_USE_SPIFFS' останется закомментированной,
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

// #define SH_USE_SPIFFS

// ===================================================

#if !defined(SH_USE_SPIFFS)
#if defined(ARDUINO_ARCH_ESP32)

#define SH_USE_FFAT // доступна только для esp32

#else

#define SH_USE_LITTLEFS

#endif
#endif

// ===================================================

#if defined(SH_USE_SPIFFS)
#if defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
#endif
#define FILESYSTEM SPIFFS
#elif defined(SH_USE_LITTLEFS)
#include <LittleFS.h>
#define FILESYSTEM LittleFS
#elif defined(SH_USE_FFAT) && defined(ARDUINO_ARCH_ESP32)
#include <FFat.h>
#define FILESYSTEM FFat
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
static const char TOO_LARGE[] PROGMEM = " - too large (max 4096 bytes)";
static const char RESTART[] PROGMEM = "The module will restart now";
static const char FAILED_STR[] PROGMEM = "failed";
static const char OK_STR[] PROGMEM = "OK";

static const char *shFsLabelFile = "/fs";
static String shFsName;

String shReadFile(const char *path)
{
#if defined(ARDUINO_ARCH_ESP32)
  File file = FILESYSTEM.open(path, "r", false);
#else
  File file = FILESYSTEM.open(path, "r");
#endif
  if (!file || file.isDirectory())
  {
    Serial.print(path);
    Serial.println(FPSTR(NO_READ));
    return (emptyString);
  }

  size_t _size = file.size();
  if (_size > 4096)
  {
    Serial.print(path);
    Serial.println(FPSTR(TOO_LARGE));
    file.close();
    return (emptyString);
  }

  String content = file.readString();
  file.close();

  return (content);
}

static bool shWriteFile(const char *_path, const String &_text)
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

  result = (file.write((const uint8_t *)_text.c_str(),
                       _text.length()) == _text.length());

  file.close();

  return (result);
}

static bool fs_format()
{
  Serial.print(FPSTR(FORMATTING_FS));
  FILESYSTEM.end();
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
  bool result = FILESYSTEM.exists(shFsLabelFile);
  if (!result)
  {
    Serial.print(FPSTR(WRITE_LABEL));
    result = shWriteFile(shFsLabelFile, shFsName);
    (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
  }
  else
  {
    Serial.print(FPSTR(CHECKING_FS_LAB));
    String x = shReadFile(shFsLabelFile);
    result = (x == shFsName);
    (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));
  }

  return (result);
}

bool fs_init()
{
#if defined(SH_USE_SPIFFS)
  shFsName = "SPIFFS";
#elif defined(SH_USE_LITTLEFS)
  shFsName = "LittleFS";
#elif defined(SH_USE_FFAT) && defined(ARDUINO_ARCH_ESP32)
  shFsName = "FFat";
#else
  Serial.println(FPSTR(UNKN_FS));
  return (false);
#endif

  Serial.print(FPSTR(FS_INIT));
  bool result = FILESYSTEM.begin();
  (result) ? Serial.println(FPSTR(OK_STR)) : Serial.println(FPSTR(FAILED_STR));

  if (result)
  {
    Serial.print(FPSTR(FS_NAME));
    Serial.println(shFsName);

    result = fs_check();
  }

  if (!result)
  {
    result = fs_format();
#if !defined(ARDUINO_ARCH_ESP32)
    if (result)
    {
      // примонтировать и записать метку
      result = FILESYSTEM.begin() && fs_check();
    }
#endif
  }

  return (result);
}