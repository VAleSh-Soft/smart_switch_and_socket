Простые модули WiFi-реле и WiFi-выключателя, построенные на библиотках [hSRControl](https://github.com/VAleSh-Soft/shSRControl) и [shWiFiConfig](https://github.com/VAleSh-Soft/shWiFiConfig). 

Имеются варианты: 
- [двухрелейный модуль WiFi-реле](wifi_socket/readme.md);
- [трехкнопочный модуль WiFi-выключателя](wifi_switch/readme.md);

Количество реле/кнопок в модулях может быть изменено, см. описание к библиотеке  [shSRControl](https://github.com/VAleSh-Soft/shSRControl).

Модули могут работать как на **esp8266**, так и на **esp32**.

Отдельно представлен [модуль](esp-01_socket/readme.md) WiFi-реле на основе **esp-01** с [однорелейным модулем](https://aliexpress.ru/item/32845077134.html).

Пины для подключения реле и кнопок см. в файле **header_file.h** соответствующего модуля

<hr>

Если возникнут вопросы, пишите на valesh-soft@yandex.ru 