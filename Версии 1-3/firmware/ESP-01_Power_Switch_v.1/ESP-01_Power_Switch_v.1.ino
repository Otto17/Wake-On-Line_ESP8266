/*
	Дата создания: 10.04.2020г.
    Версия: 1.0
    Автор: Otto.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Параметры подключения к точке доступа Wi-Fi
const char* ssid = "SSID";       //Вводим свой SSID (Wi-Fi)
const char* password = "Пароль";  //Вводим пароль от Wi-Fi

//Если нужен Статический IP адрес, тогда расскоментировать эти строки, так же ещё в двух местах раскоментировать ниже в коде
//IPAddress ip(192, 168, 1, 12);     //Статический IP-адрес, меняем на свой
//IPAddress gateway(192, 168, 1, 1); //IP-адрес роутера, меняем на свой
//IPAddress subnet(255, 255, 255, 0);


ESP8266WebServer server(80);  //Стандартный порт 80 (Web морда)

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

  //Стартовая страница с информацией о состоянии
  server.on("/", []() {         //Если введён ip адрес, тогда отображаем только состояние PC
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
    if (digitalRead(pwr_led) == LOW) {
      pc();
    }
    server.send(200, "text/html", webPage);
  });

  //Отдельная команда к ip адресу для Выкл. PC
  server.on("/off", []() {
    if (digitalRead(pwr_led) == HIGH) {
      pc();
    }
    server.send(200, "text/html", webPage);
  });


  //Отдельная команда к ip адресу для Принудительного выключения PC в случае его зависания (исключена из WEB Интерфейса, что бы нечаянно не нажать)
  server.on("/hard", []() {
    if (digitalRead(pwr_led) == HIGH) {
      pc_hard();
    }
    server.send(200, "text/html", webPage);
  });

  //Режим Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Если нужен Статический IP адрес, тогда расскоментировать строку ниже
  //  WiFi.config(ip, gateway, subnet);

  // Ожидание соединения
  if (WiFi.status() != WL_CONNECTED) {    //Ожидаем соединения
    for (int i = 0; i <= 16; i++) {
      delay(200);
    }
  }
}
void loop() {
  currentMillis = millis(); //Запускаем таймер
  server.handleClient();

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
  //Если нужен Статический IP адрес, тогда расскоментировать строку ниже
  //  WiFi.config(ip, gateway, subnet);

  // Ожидание соединения...
  if (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i <= 12; i++) {
      delay(100);
    }
  }
}

void pc () {      //Функция Вкл. транзистора на пол секунды (эмитация нажатия кнопки Power на PC)
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
  String message = "File Not Found!\n\n";
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
