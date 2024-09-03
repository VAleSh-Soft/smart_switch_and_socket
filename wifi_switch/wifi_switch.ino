#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"
#include "shFS.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  switch_control.init(switches, switch_count);

  wifi_config.begin(&HTTP, &FILESYSTEM);
  // ==== инициализируем файловую систему ============
  if (fs_init())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    switch_control.attachWebInterface(&HTTP, &FILESYSTEM);
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
    switch_control.setCheckTimer(300000);
    switch_control.startDevice(&udp, local_port);
  }
  else
  {
    Serial.println(F("failed, restart"));
    ESP.restart();
  }
  // ==== запускаем HTTP-сервер ======================
  server_init();
  // ==== настраиваем первую кнопку ==================
  btn1.setLongClickMode(LCM_ONLYONCE);
  btn1.setTimeoutOfLongClick(2000);
}

void loop()
{
  wifi_config.tick();
  switch_control.tick();

  // при удержании первой кнопки более двух секунд выключаем все реле сразу
  if (btn1.getLastState() == BTN_LONGCLICK)
  {
    switch_control.setStateForAll(false);
  }

  HTTP.handleClient();

  delay(1);
}
