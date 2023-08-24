#if defined(ARDUINO_ARCH_ESP8266)
#include <FS.h>
#endif
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"

#if defined(ARDUINO_ARCH_ESP8266)
#if FILESYSTEM == LittleFS
#include <LittleFS.h>
#endif
#elif defined(ARDUINO_ARCH_ESP32)
#if FILESYSTEM == FFat
#include <FFat.h>
#endif
#if FILESYSTEM == SPIFFS
#include <SPIFFS.h>
#endif
#endif

void setup()
{
  Serial.begin(115200);
  Serial.println();

  wifi_config.begin(&HTTP, &FILESYSTEM);
  // ==== инициализируем файловую систему ============
  if (FILESYSTEM.begin())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    switch_control.attachWebInterface(&HTTP, &FILESYSTEM);
  }
  // ==== подключаем WiFi ============================
  if (!wifi_config.startWiFi())
  {
    ESP.restart();
  }
  // ==== запускаем UDP ==============================
  Serial.println(F("Starting UDP"));
  if (udp.begin(local_port))
  {
    switch_control.begin(&udp, local_port, switch_count, relays);
    wifi_config.setUseLed(true, LED_BUILTIN);
  }
  else
  {
    Serial.println(F("failed, restart"));
    ESP.restart();
  }
  // ==== запускаем HTTP-сервер ======================
  server_init();
}

void loop()
{
  wifi_config.tick();
  switch_control.tick();
  HTTP.handleClient();

  delay(1);
}
