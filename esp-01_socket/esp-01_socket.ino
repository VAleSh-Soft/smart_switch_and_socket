#include <ESP8266WiFi.h>
#include <FS.h>
#define SH_LOG_OFF
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"

void setup()
{
  // работаем с однорелейным модулем
  relay_control.init(1);
  relay_control.addRelay("lamp01", relay_pin, LOW, btn_pin);

  relay_control.setUdpPort(local_port);

  // восстанавливаем режим пина кнопки (на всякий случай, на esp8266 после старта режим этого пина может измениться)
  pinMode(btn_pin, INPUT_PULLUP);

  wifi_config.setUseLed(true, LED_BUILTIN);
  wifi_config.setApSsid(apSsid); // пароль оставляем по умолчанию - 12345678
  // ==== инициализируем файловую систему ============
  if (FILESYSTEM.begin())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.begin(&HTTP, &FILESYSTEM, wifi_config_page);
    relay_control.attachWebInterface(&HTTP,
                                     &FILESYSTEM,
                                     tasks_config_page,
                                     wifi_config_page);
  }
  // ==== запускаем UDP ==============================
  if (!relay_control.startUdpInteraction(&udp))
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
}
