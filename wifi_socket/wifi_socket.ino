#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"
#include "shFS.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // работаем с двухрелейным модулем
  relay_control.init(2);
  relay_control.addRelay("socket1", RELAY1_PIN, HIGH, BTN1_PIN);
  relay_control.addRelay("socket2", RELAY2_PIN, HIGH, BTN2_PIN);

  relay_control.setUdpPort(local_port);

  wifi_config.begin(&HTTP, &FILESYSTEM, wifi_config_page);
  // ==== инициализируем файловую систему ============
  if (fs_init())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    relay_control.attachWebInterface(&HTTP,
                                     &FILESYSTEM,
                                     tasks_config_page,
                                     wifi_config_page);
  }
  // ==== подключаем WiFi ============================
  wifi_config.setUseLed(true, ledPin);
  if (!wifi_config.startWiFi())
  {
    ESP.restart();
  }
  // ==== запускаем UDP ==============================
  if (!relay_control.startUdpInteraction(&udp))
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
  relay_control.tick();
}
