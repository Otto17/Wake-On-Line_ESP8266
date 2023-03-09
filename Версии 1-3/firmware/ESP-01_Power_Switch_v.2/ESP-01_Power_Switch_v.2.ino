/*
    Скетч для Управления PC через WEB интерфейс.
    Возможности:
      - Информирование состояния PC (ON/OFF);
      - Управление PC (Вкл./Выкл.)
      - Обновление прошивки OTA (по Воздуху). Вызывается отдельно добавлением к ip адресу "/firmware". Прошивки заливать с расширением *.bin
      - Аутентификация по Логину/Паролю;
      - Возможность задать разные Логины/Пароли для страницы Управления PC и страницы Обновления прошивки;
      - ЖЁСТКОЕ (принудительное) выключение PC в случае его зависания. Исключена из WEB интерфейса
      для исключения ошибочного нажатия. Вызывается отдельно добавлением к ip адресу "/hard".

    Дата создания: 10.04.2020г.
    Версия: 2.0
    Автор: Otto.

    Ссылки на ресурсы с информацией которая была взята  за основу к данному Скетчу:
    1) https://www.youtube.com/watch?v=5u0XjdqB4Bs&t=6s
    2) http://psenyukov.ru/прошивка-по-воздуху-контроллера-esp8266-v2-0
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>         //Библиотека для Web Сервера
#include <ESP8266HTTPUpdateServer.h>  //Библиотека для Обновления прошивки по воздуху


// Параметры подключения к точке доступа Wi-Fi
const char* ssid = "SSID";       //Вводим свой SSID (Wi-Fi)
const char* password = "Пароль";  //Вводим пароль от Wi-Fi

//Раскомментировать эти две строчки ниже, а так же на 174 строчке кода, если нужен отдельный Логин/Пароль для Страницы обновления OTA прошивки (по Воздуху)
//#define OTAUSER      "admin"    // Логин для входа в OTA  (Обновление прошивки по Воздуху)
//#define OTAPASSWORD  "admin"    // Пароль для входа в ОТА (Обновление прошивки по Воздуху)
#define OTAPATH      "/firmware"  // Путь, который будем дописывать после ip адреса в браузере для Обновления прошивки

//Переменные для защиты страницы Логином/Паролем
const char* www_username = "myplay";   //Логин  для доступа к содержимому странички
const char* www_password = "L11M4dr37";   //Пароль для доступа к содержимому странички

// Добавляет произвольную информацию на форму ввода Логина/Пароля
const char* www_realm = "Vvedi svoi dannye ili VALI otsyuda";
// HTML-ответ в случае неправильно введённого Логина/Пароля
String authFailResponse = "Kakoj-to levyj u tebya Login/Parol', valika ot syuda...";


//Если нужен Статический IP адрес, тогда раскомментировать эти строки, так же ещё в двух местах раскоментировать ниже в коде
//IPAddress ip(192, 168, 1, 12);     //Статический IP-адрес, меняем на свой
//IPAddress gateway(192, 168, 1, 1); //IP-адрес роутера, меняем на свой
//IPAddress subnet(255, 255, 255, 0);


ESP8266WebServer server(80);          //Стандартный порт 80 (Web морда)
ESP8266HTTPUpdateServer httpUpdater;  //Создаём Объект "httpUpdater" Класса "ESP8266HTTPUpdateServer"

//Таймер для переподключения к Wi-Fi
long previousMillis = 0;
unsigned long currentMillis;
const long interval_wifi_searsh = 120000;

int pwr_sw = 0;      // GPIO0 (Нога для управления компьютером, Вкл./Выкл. PC)
int pwr_led = 2;     // GPIO2 (LED индикатор на кнопке Power для определения состояния PC)

// Набор переменных для Web интерфейса
String webPage, pageTop, webPcState_on, webPcState_off, webControl_on, webControl_off, EndBlock;

void setup() {
  pinMode(pwr_sw, OUTPUT);           //Назначаем pin управления Транзистором Вкл./Выкл. как ВЫХОД
  pinMode(pwr_led, INPUT);           //Назначаем pin отслеживания состояния LED как ВХОД
  digitalWrite(pwr_sw, LOW);         //Устанавливаем низкое значение на выходе Транзистора

  //Инициализация
  server.onNotFound(handleNotFound);
  server.begin();

  //Присваиваем элементы HTML к Переменным для отображения Web Интерфейса
  pageTop = "<!DOCTYPE html><title>Панель Управления</title><head><meta charset='UTF-8'><meta name=\"viewport\" content=\"width=device-width\"></head><body><h1>Управление PC</h1><table style=\"text-align: left; height: 222px; width: 324px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\"><tbody><tr><td style=\"font-size: 17pt; text-align: center; width: 80px;\">Состояние PC</td>";
  webPcState_on = "<td style=\"font-size: 20pt; text-align: center; background-color: green; width: 135px;\">ON</td></tr>";
  webPcState_off = "<td style=\"font-size: 20pt; text-align: center; background-color: red; width: 135px;\">OFF</td></tr>";
  webControl_on = "<tr><td style=\"font-size: 20pt; text-align: center; width: 60px;\">Система</td><td style=\"text-align: center; width: 140px;\"><a href=\"on\"><button style=\"font-size: 20pt;\">Вкл.</button></a>&nbsp;<a href=\"off\"><button disabled style=\"font-size: 20pt;\">Выкл.</button></a></td></tr>";
  webControl_off = "<tr><td style=\"font-size: 20pt; text-align: center; width: 60px;\">Система</td><td style=\"text-align: center; width: 140px;\"><a href=\"on\"><button disabled style=\"font-size: 20pt;\">Вкл.</button></a>&nbsp;<a href=\"off\"><button style=\"font-size: 20pt;\">Выкл.</button></a></td></tr>";
  EndBlock = "</tbody></table></body></html>";

  //Режим Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Если нужен Статический IP адрес, тогда раскомментировать строку ниже
  //  WiFi.config(ip, gateway, subnet);

  // Ожидание соединения
  if (WiFi.status() != WL_CONNECTED) {    //Ожидаем соединения
    for (int i = 0; i <= 16; i++) {
      delay(200);
    }
  }

  /*
    В этом участке ниже происходит Авторизация. При попытке зайти на главную страницу или на отдельные ВЕРНЫЕ адреса (control; on; off; hard)
    будет проверка Авторизации, если данные уже корректно введены (на одной из любых ВЕРНЫХ адресов: control; on; off; hard),
    тогда доступ разрешён на любую из этих страничек, иначе будет запрос Логина/Пароля.
  */
  //Стартовая страница с информацией о состоянии
  server.on("/", []() {         //Если введён ip адрес и верные Логин/Пароль, тогда отображаем только состояние PC
    if (!server.authenticate(www_username, www_password)) {                           //Если НЕ верно введены или просто НЕ введены Логин/Пароль, тогда...
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);  //...вернуться и повторить попытку ввода.
    }
    //Если введены верно, тогда отобразить стартовую страницу о состоянии PC
    if (digitalRead(pwr_led) == LOW) {
      webPage = pageTop + webPcState_off + EndBlock;
    }
    else if (digitalRead(pwr_led) == HIGH) {
      webPage = pageTop + webPcState_on + EndBlock;
    }
    server.send(200, "text/html", webPage);
  });


  //Страница Управления питанием PC
  server.on("/control", []() {    //Если к ip адресу добавить "/control", тогда откроется панель управления + состояние PC
    if (!server.authenticate(www_username, www_password)) {                           //Если НЕ верно введены или просто НЕ введены Логин/Пароль, тогда...
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);  //...вернуться и повторить попытку ввода.
    }
    //Если введены верно, тогда отобразить страницу управления питанием PC
    if (digitalRead(pwr_led) == LOW) {
      webPage = pageTop + webPcState_off + webControl_on + EndBlock;
    }
    else if (digitalRead(pwr_led) == HIGH) {
      webPage = pageTop + webPcState_on + webControl_off + EndBlock;
    }
    server.send(200, "text/html", webPage);
  });


  //Отдельная команда к ip адресу для Вкл. PC
  server.on("/on", []() {
    if (!server.authenticate(www_username, www_password)) {                           //Если НЕ верно введены или просто НЕ введены Логин/Пароль, тогда...
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);  //...вернуться и повторить попытку ввода.
    }
    //Если мы уже залогинены, тогда команда "on" отдельно выполнится, иначе запрос ввода Логина/Пароля
    if (digitalRead(pwr_led) == LOW) {
      pc();
    }
    server.send(200, "text/html", webPage);
  });


  //Отдельная команда к ip адресу для Выкл. PC
  server.on("/off", []() {
    if (!server.authenticate(www_username, www_password)) {                           //Если НЕ верно введены или просто НЕ введены Логин/Пароль, тогда...
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);  //...вернуться и повторить попытку ввода.
    }
    //Если мы уже залогинены, тогда команда "off" отдельно выполнится, иначе запрос ввода Логина/Пароля
    if (digitalRead(pwr_led) == HIGH) {
      pc();
    }
    server.send(200, "text/html", webPage);
  });


  //Отдельная команда к ip адресу для принудительного выключения PC в случае его зависания (исключена из WEB интерфейса, что бы нечаянно не нажать)
  server.on("/hard", []() {
    if (!server.authenticate(www_username, www_password)) {                           //Если НЕ верно введены или просто НЕ введены Логин/Пароль, тогда...
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);  //...вернуться и повторить попытку ввода.
    }
    //Если мы уже залогинены, тогда команда "hard" выполнится, иначе запрос ввода Логина/Пароля
    if (digitalRead(pwr_led) == HIGH) {
      pc_hard();
    }
    server.send(200, "text/html", webPage);
  });

  server.begin();

  //Страница для Обновления OTA прошивки (по Воздуху)
  //httpUpdater.setup(&server, OTAPATH, OTAUSER, OTAPASSWORD); //Раскомментировать эту строку, а нижнюю Закомментировать, если хотите использовать отдельный Логин/Пароль для страницы Обновления OTA прошивки (по Воздуху)
  httpUpdater.setup(&server, OTAPATH, www_username, www_password);   //Использую общий Логин/Пароль для Обновления OTA прошивки и доступа к Управлению PC

}
void loop() {
  currentMillis = millis(); //Запускаем таймер
  server.handleClient();    //Прослушивание HTTP-запросов от клиентов


  if (WiFi.status() != WL_CONNECTED)     //Проверяем статус Wi-Fi, если не подключены, тогда делаем переподключение
  {
    if (currentMillis - previousMillis > interval_wifi_searsh) {
      WIFI_Connect();
      previousMillis = currentMillis;
    }
  }
}

void WIFI_Connect()   //Функция подключения к Wi-Fi
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Если нужен Статический IP адрес, тогда раскомментировать строку ниже
  //  WiFi.config(ip, gateway, subnet);

  // Ожидание соединения...
  if (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i <= 12; i++) {
      delay(100);
    }
  }
}

void pc () {      //Функция Вкл. транзистора на пол секунды (имитация нажатия кнопки Power на PC)
  digitalWrite(pwr_sw, HIGH);
  delay(500);
  digitalWrite(pwr_sw, LOW);
}

void pc_hard () {      //Функция принудительного Выкл. PC. Необходима для принудительной перезагрузки PC в случае его зависания!
  digitalWrite(pwr_sw, HIGH);
  delay(4010);
  digitalWrite(pwr_sw, LOW);
}

void handleNotFound() {     //Функция вывода Несуществующий страницы, если введён неверный адрес
  String message = "Page Not Found!\n";
  message += "Idi v OCHKO s moej stranicy!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
