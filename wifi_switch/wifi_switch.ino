#include <shSRControl.h>
#include <shWiFiConfig.h>
#include "header_file.h"
#include "shFS.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // работаем с четырехканальным модулем
  switch_control.init(4);
  switch_control.addRelay("lamp01", &btn1);
  switch_control.addRelay("socket1", &btn2);
  switch_control.addRelay("socket2", &btn3);
  switch_control.addRelay("socket_tv", &btn4);
  // включаем озвучку нажатия кнопок
  switch_control.setErrorBuzzerState(true, BUZZER_PIN);

  wifi_config.begin(&HTTP, &FILESYSTEM, wifi_config_page);
  // ==== инициализируем файловую систему ============
  if (fs_init())
  {
    // ==== восстанавливаем настройки ================
    wifi_config.loadConfig();
    switch_control.attachWebInterface(&HTTP,
                                      &FILESYSTEM,
                                      relay_config_page,
                                      wifi_config_page);
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
}
