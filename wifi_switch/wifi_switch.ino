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
  switch_control.addRelay("lamp01", BTN1_PIN);
  switch_control.addRelay("socket1", BTN2_PIN);
  switch_control.addRelay("socket2", BTN3_PIN);
  switch_control.addRelay("socket_tv", BTN4_PIN);
  // включаем озвучку нажатия кнопок
  switch_control.setErrorBuzzerState(true, BUZZER_PIN);

  switch_control.setCheckTimer(300000);
  switch_control.setUdpPort(local_port);

  wifi_config.setUseLed(true, ledPin);
  wifi_config.setApSsid(apSsid); // пароль оставляем стандартный - 12345678
  // ==== инициализируем файловую систему ============
  if (fs_init())
  {
    wifi_config.begin(&HTTP, &FILESYSTEM, wifi_config_page);
    switch_control.attachWebInterface(&HTTP,
                                      &FILESYSTEM,
                                      tasks_config_page,
                                      wifi_config_page);
  }
  // ==== подключаем WiFi ============================
  if (!wifi_config.startWiFi())
  {
    ESP.restart();
  }
  // ==== запускаем UDP ==============================
  if (!switch_control.startUdpInteraction(&udp))
  {
    Serial.println(F("failed, restart"));
    ESP.restart();
  }
  // ==== запускаем HTTP-сервер ======================
  server_init();
  // ==== настраиваем первую кнопку ==================
  switch_control.setButtonTimeoutOfLongClick(0, 2000);
}

void loop()
{
  wifi_config.tick();
  switch_control.tick();

  // при удержании первой кнопки более двух секунд выключаем все реле сразу
  if (switch_control.getButtonState(0) == BTN_LONGCLICK)
  {
    switch_control.setStateForAll(false);
  }
}
