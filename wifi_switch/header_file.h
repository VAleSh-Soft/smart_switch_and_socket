#pragma once

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPUpdateServer.h>
#include <WebServer.h>
#include <SSDP.h>
#include <mDNS.h>
#endif
#include <shSRControl.h>
#include <shWiFiConfig.h>
#include <shButton.h>

#define FILESYSTEM SPIFFS

// ==== кнопка =======================================
#if defined(ARDUINO_ARCH_ESP8266)
shButton btn1(4);
shButton btn2(5);
shButton btn3(13);
#elif defined(ARDUINO_ARCH_ESP32)
shButton btn1(16);
shButton btn2(17);
shButton btn3(18);
#endif

// ==== WiFiConfig ===================================
shWiFiConfig wifi_config;

// ==== SRControl ====================================
shSwitchControl switch_control;
WiFiUDP udp;
// локальный порт для прослушивания udp-пакетов
const uint16_t local_port = 54321;

// работаем с трехкнопочным модулем
const uint8_t switch_count = 3;
shSwitchData relays[switch_count] = {
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
