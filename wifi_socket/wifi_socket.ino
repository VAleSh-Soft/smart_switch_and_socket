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
  relay_control.addRelay("socket1", RELAY1_PIN, HIGH, &btn1);
  relay_control.addRelay("socket2", RELAY2_PIN, HIGH, &btn2);

  wifi_config.begin(&HTTP, &FILESYSTEM);
  // ==== инициализируем файловую систему ============
  if (fs_init())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    relay_control.attachWebInterface(&HTTP, &FILESYSTEM);
  }
  // ==== подключаем WiFi ============================
  wifi_config.setUseLed(true, ledPin);
  if (!wifi_config.startWiFi())
  {
    ESP.restart();
  }
  // ==== запускаем UDP ==============================
  Serial.print(F("Starting UDP..."));
  if (udp.begin(local_port))
  {
    Serial.println(F("OK"));
    relay_control.startDevice(&udp, local_port);
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
  relay_control.tick();
  HTTP.handleClient();

  delay(1);
}
