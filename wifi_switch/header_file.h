#pragma once

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPUpdateServer.h>
#include <WebServer.h>
#endif
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include <shButton.h>

#define FILESYSTEM SPIFFS

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

shButton btn1(BTN1_PIN);
shButton btn2(BTN2_PIN);
shButton btn3(BTN3_PIN);

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
        "relay1",
        false,
        IPAddress(192, 168, 4, 1),
        &btn1,
        ""},
    (shSwitchData){
        "relay2",
        false,
        IPAddress(192, 168, 4, 1),
        &btn2,
        ""},
    (shSwitchData){
        "relay3",
        false,
        IPAddress(192, 168, 4, 1),
        &btn3,
        ""}};

shSwitchControl switch_control(switches, switch_count);

// ==== сервера ======================================
// Web интерфейс для устройства
#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer HTTP(80);
// сервер обновления по воздуху через web-интерфейс
ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ARDUINO_ARCH_ESP32)
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

#if defined(ARDUINO_ARCH_ESP8266)
    // ==== MDNS =======================================
    String host = wifi_config.getApSsid();
    host.toLowerCase();
    MDNS.begin(host.c_str());

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

// ==== свеодиод =====================================

const int8_t ledPin = 4;