#include "header_file.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // работаем с двухрелейным модулем
  relay_control.init(2);
  relay_control.addRelay("socket1", RELAY1_PIN, HIGH, BTN1_PIN);
  relay_control.addRelay("socket2", RELAY2_PIN, HIGH, BTN2_PIN);

  relay_control.setUdpPort(local_port);

  wifi_config.setUseLed(true, ledPin);
  wifi_config.setApSsid(apSsid); // пароль оставляем по умолчанию - 12345678
  // ==== инициализируем файловую систему ============
  if (esp_fs.init())
  {
    wifi_config.begin(&HTTP, &ESP_FILESYSTEM, wifi_config_page);
    relay_control.attachWebInterface(&HTTP,
                                     &ESP_FILESYSTEM,
                                     tasks_config_page,
                                     wifi_config_page);
  }
  // ==== подключаем WiFi ============================
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
