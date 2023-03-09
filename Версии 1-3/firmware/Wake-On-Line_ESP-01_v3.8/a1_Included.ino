//********** ПОДКЛЮЧЕНИЕ БИБЛИОТЕК, ОБЪЕКТОВ И КОНСТАНТ **********//

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>         // Локальный веб-сервер, используемый для обслуживания портала конфигурации
#include <ESP8266HTTPUpdateServer.h>  // Библиотека для Обновления прошивки по воздуху
#include <WiFiManager.h>              // Библиотека WiFiManager для сохранения и установки ISSD и пароля на WiFi
#include <DNSServer.h>                // Локальный DNS-сервер используется для перенаправления всех запросов на портал конфигурации
#include <EEPROM.h>                   // Библиотека для работы функций Записи/Чтения на ESP8266 (эмуляция EEPROM во Flash-памяти)
#include <SmartDelay.h>               // Библиотека для удобной неблокирующей задержки
#include <uptime.h>                   // Библиотека для вычисления Uptime без переполнения 49 дней для функции "millis()"



//Макрос для помещения данных во Flash (только для чтения), вместо ОЗУ (сильно экономит ОЗУ)
//В данном случае используется для хранения HTML страниц во Flash, вместо ОЗУ, что позволяет делать более объёмные страницы.
#define PROGMEM   ICACHE_RODATA_ATTR


//Глобальная инициализация объектов
ESP8266WebServer server(80);          // Стандартный порт 80 (WEB морда)
ESP8266HTTPUpdateServer httpUpdater;  // Создаём объект "httpUpdater" класса "ESP8266HTTPUpdateServer"
WiFiManager wm;   // Глобальная инициализация объекта "wm" класса "WiFiManager"
SmartDelay delayUptime(1000000UL);    // выполнения кода раз в 1 сек.

const char* www_login;   // Отсюда берётся логин  для доступа к WEB морде, тут менять ничего НЕ нужно.
const char* www_pswd;    // Отсюда берётся пароль для доступа к WEB морде, тут менять ничего НЕ нужно.

bool shouldSaveConfig = false;    // Флаг для сохранения данных (Логин/Пароль для WEB морды) из WiFiManager

unsigned int eroorAuthoriz = 0;   // Счётчик неудачных попыток авторизации после включения ESP8266
char ver[] = VERSION;     // Символьная переменная для подставления версии прошивки в HTML код на старнице с Информацией
String TimeUptime = "";   // Получаем сюда данные "Uptime" из цикла Loop и выводим в HTML
