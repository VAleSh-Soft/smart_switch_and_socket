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
 * системы; после форматирования модуль автоматически перезагружается;
 */

#include <FS.h>

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
