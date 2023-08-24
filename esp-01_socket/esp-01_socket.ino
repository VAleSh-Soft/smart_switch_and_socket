#include <ESP8266WiFi.h>
#include <FS.h>
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // восстанавливаем режим пина кнопки (на всякий случай)
  pinMode(btn_pin, INPUT_PULLUP);

  wifi_config.begin(&HTTP, &FILESYSTEM);
  // ==== инициализируем файловую систему ============
  if (FILESYSTEM.begin())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    relay_control.attachWebInterface(&HTTP, &FILESYSTEM);
  }
  // ==== запускаем UDP ==============================
  Serial.println(F("Starting UDP"));
  if (udp.begin(local_port))
  {
    wifi_config.setUseLed(true, LED_BUILTIN);
    relay_control.begin(&udp, local_port, relays_count, relays);
  }
  else
  {
    Serial.println(F("failed, restart"));
    ESP.restart();
  }
  // ==== подключаем WiFi ============================
  if (!wifi_config.startWiFi())
  {
    ESP.restart();
  }
  // ==== запускаем HTTP-сервер ======================
  server_init();
}

void loop()
{
  wifi_config.tick();
  relay_control.tick();
  HTTP.handleClient();

  delay(1);
}
