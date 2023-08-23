#pragma once

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include <shButton.h>

#define FILESYSTEM SPIFFS

// ==== кнопка =======================================
// т.к. в esp-01 мало пинов, для кнопки используется пин Rx, поэтому в setup() нужно не забыть заново установить ему режим INPUT_PULLUP
shButton btn(3);

// ==== WiFiConfig ===================================
shWiFiConfig wifi_config;

// ==== SRControl ====================================
shRelayControl relay_control;
WiFiUDP udp;
// локальный порт для прослушивания udp-пакетов
const uint16_t local_port = 54321;
// Пин подключения сигнального контакта реле - GPIO0
const byte relay_pin = 0; 

// ==== работаем с однорелейным модулем ==============
const uint8_t relays_count = 1;
shRelayData relays[relays_count] = {
    (shRelayData){
        "relay1",
        relay_pin,
        LOW,
        false,
        &btn,
        ""}};

// ==== сервера ======================================
ESP8266WebServer HTTP(80);
// сервер обновления по воздуху через web-интерфейс
ESP8266HTTPUpdateServer httpUpdater;

void server_init()
{
  // ==== MDNS =======================================
  String host = wifi_config.getApSsid();
  host.toLowerCase();
  MDNS.begin(host.c_str());

  // ==== HTTP =======================================
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []()
          { SSDP.schema(HTTP.client()); });
  HTTP.onNotFound([]()
                  { HTTP.send(404, "text/plan", F("404. File not found.")); });
  // настройка сервера обновлений
  httpUpdater.setup(&HTTP, "/firmware");
  HTTP.begin();

  // ==== SSDP =======================================
  // Если версия  2.0.0 закомментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(host);
  SSDP.setSerialNumber("000000001240");
  SSDP.setURL("/");
  SSDP.setModelName("WiFi_Relay");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("https://github.com/VAleSh-Soft");
  SSDP.setManufacturer("VAleSh-Soft");
  SSDP.setManufacturerURL("https://github.com/VAleSh-Soft");
  SSDP.begin();
}
