#include <ESP8266WiFi.h>
#include <FS.h>
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // работаем с однорелейным модулем
  relay_control.init(1);
  relay_control.addRelay("lamp01", relay_pin, LOW, &btn);

  // восстанавливаем режим пина кнопки (на всякий случай)
  pinMode(btn_pin, INPUT_PULLUP);

  wifi_config.begin(&HTTP, &FILESYSTEM, wifi_config_page);
  // ==== инициализируем файловую систему ============
  if (FILESYSTEM.begin())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    wifi_config.setCheckTimer(120000);
    relay_control.attachWebInterface(&HTTP,
                                     &FILESYSTEM,
                                     relay_config_page,
                                     wifi_config_page);
  }
  // ==== запускаем UDP ==============================
  Serial.println(F("Starting UDP"));
  if (udp.begin(local_port))
  {
    relay_control.startDevice(&udp, local_port);
  }
  else
  {
    Serial.println(F("failed, restart"));
    ESP.restart();
  }
  // ==== подключаем WiFi ============================
  wifi_config.setUseLed(true, LED_BUILTIN);
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
}
