#pragma once

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPUpdateServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#endif
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include <FS.h>

// ==== файловая система =============================

/*
 * раскомментируйте строку '#define USE_SPIFFS'только если хотите использовать
 * файловую систему SPIFFS; в противном случае файловая система будет выбрана 
 * автоматически в зависимости от используемого МК: для esp8266 будет
 * использована LittleFS, для esp32 будет использована FFat
 */

// #define USE_SPIFFS

// ==== кнопки =======================================
#if defined(ARDUINO_ARCH_ESP8266)
// пины кнопок на esp8266
#define BTN1_PIN 4
#define BTN2_PIN 5
#define BTN3_PIN 13
#elif defined(ARDUINO_ARCH_ESP32)
// пины кнопок на esp32
#define BTN1_PIN 16
#define BTN2_PIN 17
#define BTN3_PIN 18
#endif

srButton btn1(BTN1_PIN);
srButton btn2(BTN2_PIN);
srButton btn3(BTN3_PIN);

// ==== WiFiConfig ===================================
shWiFiConfig wifi_config;

// ==== SRControl ====================================
WiFiUDP udp;
// локальный порт для прослушивания udp-пакетов
const uint16_t local_port = 54321;

// работаем с трехкнопочным модулем
const uint8_t switch_count = 3;
shSwitchData switches[switch_count] = {
    (shSwitchData){
        "lamp01",
        false,
        IPAddress(192, 168, 4, 1),
        &btn1,
        ""},
    (shSwitchData){
        "socket1",
        false,
        IPAddress(192, 168, 4, 1),
        &btn2,
        ""},
    (shSwitchData){
        "socket2",
        false,
        IPAddress(192, 168, 4, 1),
        &btn3,
        ""}};

shSwitchControl switch_control;

// ==== сервера ======================================
#if defined(ARDUINO_ARCH_ESP8266)
// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
// сервер обновления по воздуху через web-интерфейс
ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ARDUINO_ARCH_ESP32)
// Web интерфейс для устройства
WebServer HTTP(80);
// сервер обновления по воздуху через web-интерфейс
HTTPUpdateServer httpUpdater;
#endif

void server_init()
{
// ==== HTTP =======================================
#if defined(ARDUINO_ARCH_ESP8266)
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []()
          { SSDP.schema(HTTP.client()); });
#endif
  HTTP.onNotFound([]()
                  { HTTP.send(404, "text/plan", F("404. File not found.")); });
  // настройка сервера обновлений
  httpUpdater.setup(&HTTP, "/firmware");
  HTTP.begin();

  // ==== MDNS =======================================
  String host = wifi_config.getApSsid();
  host.toLowerCase();
  MDNS.begin(host.c_str());

#if defined(ARDUINO_ARCH_ESP8266)
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
#endif
}

// ==== светодиод ====================================

#if defined(ARDUINO_ARCH_ESP8266)
const int8_t ledPin = 2; // LED_BUILTIN;
#else
const int8_t ledPin = 2;
#endif
