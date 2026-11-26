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
#include <shESPFSinit.h>
#include <shTaskManager.h>

// ==== кнопки, реле и выходы ========================
#if defined(ARDUINO_ARCH_ESP8266)
// пины кнопок на esp8266
#define BTN1_PIN D5
// выход на светодиодную ленту esp8266
#define LEDS_PWM_PIN D1
#elif defined(ARDUINO_ARCH_ESP32)
// пины кнопок на esp32
#define BTN1_PIN 16
// выход на светодиодную ленту  esp32
// ШИМ ленты на esp32 - канал 0 уже привязан к нему в leds_init
#define LEDS_PWM_PIN 18
#endif

// ==== плавное включение/выключение ленты (ШИМ 0..1023) ====
#define LEDS_PWM_MAX 1023   // максимум ШИМ (10 бит)
#define LEDS_FADE_MS 5      // период шага плавности, мс
#define LEDS_FADE_TIME 5000 // длительность плавного перехода, мс
#define LEDS_GAMMA 2.2      // показатель гамма-кривой (восприятие глаза)

// текущее и целевое значения «воспринимаемой» яркости 0..1023
int16_t leds_level = 0;
int16_t leds_level_target = 0;
int16_t leds_fade_step = 1; // шаг изменения за один тик

// ==== WiFiConfig ===================================
shWiFiConfig wifi_config;
String wifi_config_page = "/wifi_config";
String apSsid = "WIFI_LED_LIGHT_01";

// ==== SRControl ====================================
WiFiUDP udp;
// локальный порт для прослушивания udp-пакетов
const uint16_t local_port = 54321;

shRelayControl relay_control;
String tasks_config_page = "/tasks_config";

// ==== прочее =======================================

shTaskManager tasks(1);
shHandle led_guard;
shESPFSinit esp_fs;

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

// ===================================================

void leds_init();
void leds_fade_tick();
void leds_start_stop_01();
void leds_start_stop_02();
// ==== светодиод ====================================

#if defined(ARDUINO_ARCH_ESP8266)
const int8_t ledPin = D2;
#else
const int8_t ledPin = 2;
#endif

// ===================================================

// таблица гамма-коррекции (2.2) — перевод линейной яркости 0..1023 в ШИМ
  static const uint16_t gamma_lut[1024] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8,
  8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10,
  11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13,
  14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17,
  17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21,
  21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25,
  26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30,
  31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36,
  36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42,
  42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48,
  49, 49, 49, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 55, 55,
  55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62,
  63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70,
  71, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79,
  79, 80, 80, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88,
  88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97,
  98, 98, 99, 100, 100, 101, 102, 102, 103, 103, 104, 105, 105, 106, 107, 107,
  108, 109, 109, 110, 110, 111, 112, 112, 113, 114, 114, 115, 116, 116, 117, 118,
  118, 119, 120, 121, 121, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 129,
  130, 130, 131, 132, 132, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141,
  141, 142, 143, 144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153,
  154, 154, 155, 156, 157, 157, 158, 159, 160, 161, 161, 162, 163, 164, 165, 166,
  166, 167, 168, 169, 170, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179,
  180, 181, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190, 191, 192, 193,
  194, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215, 216, 216, 217, 218, 219, 220, 221, 222,
  223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238,
  239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
  255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 269, 270, 271,
  272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 283, 284, 285, 286, 287, 288,
  289, 290, 291, 292, 294, 295, 296, 297, 298, 299, 300, 301, 303, 304, 305, 306,
  307, 308, 309, 311, 312, 313, 314, 315, 316, 317, 319, 320, 321, 322, 323, 324,
  326, 327, 328, 329, 330, 332, 333, 334, 335, 336, 338, 339, 340, 341, 342, 344,
  345, 346, 347, 348, 350, 351, 352, 353, 355, 356, 357, 358, 360, 361, 362, 363,
  365, 366, 367, 368, 370, 371, 372, 373, 375, 376, 377, 378, 380, 381, 382, 384,
  385, 386, 387, 389, 390, 391, 393, 394, 395, 397, 398, 399, 401, 402, 403, 405,
  406, 407, 409, 410, 411, 413, 414, 415, 417, 418, 419, 421, 422, 423, 425, 426,
  427, 429, 430, 432, 433, 434, 436, 437, 438, 440, 441, 443, 444, 445, 447, 448,
  450, 451, 452, 454, 455, 457, 458, 459, 461, 462, 464, 465, 467, 468, 469, 471,
  472, 474, 475, 477, 478, 480, 481, 483, 484, 485, 487, 488, 490, 491, 493, 494,
  496, 497, 499, 500, 502, 503, 505, 506, 508, 509, 511, 512, 514, 515, 517, 518,
  520, 521, 523, 524, 526, 527, 529, 530, 532, 534, 535, 537, 538, 540, 541, 543,
  544, 546, 548, 549, 551, 552, 554, 555, 557, 559, 560, 562, 563, 565, 567, 568,
  570, 571, 573, 575, 576, 578, 579, 581, 583, 584, 586, 587, 589, 591, 592, 594,
  596, 597, 599, 601, 602, 604, 605, 607, 609, 610, 612, 614, 615, 617, 619, 620,
  622, 624, 625, 627, 629, 631, 632, 634, 636, 637, 639, 641, 642, 644, 646, 648,
  649, 651, 653, 654, 656, 658, 660, 661, 663, 665, 667, 668, 670, 672, 674, 675,
  677, 679, 681, 682, 684, 686, 688, 689, 691, 693, 695, 697, 698, 700, 702, 704,
  705, 707, 709, 711, 713, 714, 716, 718, 720, 722, 724, 725, 727, 729, 731, 733,
  735, 736, 738, 740, 742, 744, 746, 747, 749, 751, 753, 755, 757, 759, 760, 762,
  764, 766, 768, 770, 772, 774, 776, 777, 779, 781, 783, 785, 787, 789, 791, 793,
  795, 796, 798, 800, 802, 804, 806, 808, 810, 812, 814, 816, 818, 820, 822, 824,
  826, 828, 829, 831, 833, 835, 837, 839, 841, 843, 845, 847, 849, 851, 853, 855,
  857, 859, 861, 863, 865, 867, 869, 871, 873, 875, 877, 879, 881, 883, 885, 887,
  889, 892, 894, 896, 898, 900, 902, 904, 906, 908, 910, 912, 914, 916, 918, 920,
  922, 925, 927, 929, 931, 933, 935, 937, 939, 941, 943, 945, 948, 950, 952, 954,
  956, 958, 960, 962, 965, 967, 969, 971, 973, 975, 977, 980, 982, 984, 986, 988,
  990, 992, 995, 997, 999, 1001, 1003, 1005, 1008, 1010, 1012, 1014, 1016, 1019, 1021, 1023,
};

void leds_init()
{
#if defined(ARDUINO_ARCH_ESP32)
#if ESP_ARDUINO_VERSION_MAJOR < 3
  ledcSetup(0, 1000, 10);
  ledcAttachPin(LEDS_PWM_PIN, 0);
#else
  ledcAttach(LEDS_PWM_PIN, 1000, 10);
#endif
#endif

  leds_level = 0;
  leds_level_target = 0;
  led_guard = tasks.addTask(LEDS_FADE_MS, leds_fade_tick, false);
}

void leds_fade_tick()
{
  // линейно двигаем «воспринимаемую» яркость к цели
  if (leds_level < leds_level_target)
  {
    leds_level += leds_fade_step;
    if (leds_level > leds_level_target)
      leds_level = leds_level_target;
  }
  else if (leds_level > leds_level_target)
  {
    leds_level -= leds_fade_step;
    if (leds_level < leds_level_target)
      leds_level = leds_level_target;
  }

  // пишем в ШИМ через гамма-таблицу
  uint16_t pwm_val = gamma_lut[leds_level];
#if defined(ARDUINO_ARCH_ESP32) && ESP_ARDUINO_VERSION_MAJOR < 3
  ledcWrite(0, pwm_val);
#else
  analogWrite(LEDS_PWM_PIN, pwm_val);
#endif

  // если дошли до цели — останавливаем задачу
  if (leds_level >= LEDS_PWM_MAX || leds_level <= 0)
  {
    leds_level = (leds_level >= LEDS_PWM_MAX) ? LEDS_PWM_MAX : 0;
    pwm_val = gamma_lut[leds_level];
#if defined(ARDUINO_ARCH_ESP32) && ESP_ARDUINO_VERSION_MAJOR < 3
    ledcWrite(0, pwm_val);
#else
    analogWrite(LEDS_PWM_PIN, pwm_val);
#endif
    tasks.stopTask(led_guard);
  }
}

// переключение ленты (первое виртуальное реле): плавное вкл/выкл
void leds_start_stop_01()
{
  leds_level_target = (leds_level_target == 0) ? LEDS_PWM_MAX : 0;

  leds_fade_step = LEDS_PWM_MAX * LEDS_FADE_MS / LEDS_FADE_TIME;
  if (leds_fade_step < 1)
    leds_fade_step = 1;

  if (!tasks.getTaskState(led_guard))
  {
    tasks.startTask(led_guard);
  }
}

// переключение ленты (второе виртуальное реле): обычное вкл/выкл
void leds_start_stop_02()
{
  leds_level_target = (leds_level_target == 0) ? LEDS_PWM_MAX : 0;
  leds_level = leds_level_target;

#if defined(ARDUINO_ARCH_ESP32) && ESP_ARDUINO_VERSION_MAJOR < 3
  ledcWrite(0, leds_level);
#else
  analogWrite(LEDS_PWM_PIN, leds_level);
#endif
}
