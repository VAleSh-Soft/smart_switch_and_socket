#pragma once

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <shSRControl.h>
#include <shWiFiConfig.h>

// ==== файловая система =============================

// для esp-01 использовать что-то кроме SPIFFS накладно - у нее слишком мало памяти
#define FILESYSTEM SPIFFS

// ==== кнопка =======================================
// т.к. в esp-01 мало пинов, для кнопки используется пин Rx, поэтому в setup() нужно не забыть заново установить ему режим INPUT_PULLUP
const uint8_t btn_pin = 3;
srButton btn(btn_pin);

// ==== WiFiConfig ===================================
shWiFiConfig wifi_config;
String wifi_config_page = "/wifi_config";

// ==== SRControl ====================================
WiFiUDP udp;
// локальный порт для прослушивания udp-пакетов
const uint16_t local_port = 54321;
// Пин подключения сигнального контакта реле - GPIO0
const byte relay_pin = 0;

shRelayControl relay_control;
String relay_config_page = "/relay_config";

// ==== сервера ======================================
ESP8266WebServer HTTP(80);
// сервер обновления по воздуху через web-интерфейс
ESP8266HTTPUpdateServer httpUpdater;

void server_init()
{
  // ==== HTTP =======================================
    HTTP.onNotFound([]()
                  { HTTP.send(404, "text/plan", F("404. File not found.")); });
  // настройка сервера обновлений
  httpUpdater.setup(&HTTP, "/firmware");
  HTTP.begin();
}
