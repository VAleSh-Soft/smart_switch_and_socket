#pragma once

/*
 * Модуль для работы с файловой системой esp8266 и esp32; для выбора желаемой ФС нужно раскомментировать соответствующий дефайн;

 * Файловая система FFat доступна только для esp32; для ее использования в опциях платы нужно выбрать в "Partition cheme" схему "Default 4MB with ffat", если у вас есть модуль ESP32 c 4 МБ флеш-памяти, или "16MB FAT", если у вас модуль ESP32 c 16 МБ флеш-памяти.

 * Для esp32 форматирование файловой системы выполняется автоматически при первом запуске; после форматирования модуль автоматически перезагружается;
 */

#include <FS.h>

// #define USE_SPIFFS
#define USE_LITTLEFS
// #define USE_FFAT // доступна только для esp32

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
