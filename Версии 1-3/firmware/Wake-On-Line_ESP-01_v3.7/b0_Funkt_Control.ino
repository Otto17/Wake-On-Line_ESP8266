//********** ФУНКЦИИ УПРАВЛЕНИЯ И ФОРМИРОВАНИЯ/ОТОБРАЖЕНИЯ СТРАНИЦ **********//


//Функция смены флага для сохранения Логина/Пароля через WiFiManager
void saveConfigCallback () {
  shouldSaveConfig = true;    // Меняем флаг на ИСТИНУ
}

//Функция Вкл./Выкл. PC (имитация короткого нажатия кнопки Power на PC)
void pc () {
  digitalWrite(pwr_sw, HIGH);   // Подаём питание на транзистор
  delay(Delay_OnOff);           // Ждём указанное кол-во миллисекунд
  digitalWrite(pwr_sw, LOW);    // убераем питание с транзистора
}

//Функция принудительного Выкл. PC. Необходима для принудительной перезагрузки PC в случае его зависания (имитация долгого нажатия кнопки Power на PC)
void pc_hard () {
  digitalWrite(pwr_sw, HIGH);  // Подаём питание на транзистор
  delay(Delay_Hard);           // Ждём указанное кол-во миллисекунд
  digitalWrite(pwr_sw, LOW);   // убераем питание с транзистора
}

//Функция главной страницы для управления PC
void PageRoot() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  if (digitalRead(pwr_led) == LOW) {   // проверяем условие, если оно выполняется, тогда...
    webPage = webHeader + webControl_off;     // формируем страницу
  }
  else if (digitalRead(pwr_led) == HIGH) {    // Иначе выполняется это условие, и если оно выполняется, тогда...
    webPage = webHeader + webControl_on;      // формируем страницу
  }
  server.send(200, "text/html", webPage);     // Отображаем ту страницу, которая подошла под условие
}

//Функция Вкл. PC
void PageOn() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  if (digitalRead(pwr_led) == LOW) {   // проверяем условие, если оно выполняется, тогда...
    webPage = webHeader + webBackPage + webLoading;   // формируем страницу
    pc();   // Вкл. PC
  }
  server.send(200, "text/html", webPage);             // Отображаем страницу
}

//Функция Выкл. PC
void PageOff() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  if (digitalRead(pwr_led) == HIGH) {   // проверяем условие, если оно выполняется, тогда...
    webPage = webHeader + webBackPage + webLoading;   // формируем страницу
    pc();   // Выкл. PC
  }
  server.send(200, "text/html", webPage);             // Отображаем страницу
}

//Функция для жёсткого выключения PC через WEB морду
void PageHard() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  webPage = webHeader + webBackPage + webLoading;   // формируем страницу
  pc_hard();    // Жёстко выключаем PC
  server.send(200, "text/html", webPage);           // Отображаем страницу
}

//Функция отображения страницы с общими настройками
void PageSettings() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  webPage = webHeader + webSettings;        // формируем страницу
  server.send(200, "text/html", webPage);   // Отображаем страницу
}

//Функция отображения информации об устройстве на WEB морде
void PageInfo() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...

  //Получаем разные характеристики для страницы "Информация"
  String FlashChipSpeed = (String)(ESP.getFlashChipSpeed() / 1000000);    // Получаем частоту FLASH памяти, в Гц делим на 1000000, конвертируем в строку и записываем "FlashChipSpeed"
  String CpuFreqMHz = (String)ESP.getCpuFreqMHz();                        // Получаем частоту процессора в МГц как 8-битовое целое число без знака и конвертируем в строку

  String FreeHeap = (String)ESP.getFreeHeap();                     // Получаем размер свободной ОЗУ памяти в байтах и конвертируем в строку

  String FlashChipSize = (String)ESP.getFlashChipSize();           // Получаем размер флеш памяти в байтах, так, как его определяет SDK (может быть меньше реального размера)
  String FlashChipRealSize = (String)ESP.getFlashChipRealSize();   // Получаем реальный размер чипа в байтах на основе идентификатора флеш-чипа
  String FreeSketchSpace = (String)ESP.getFreeSketchSpace();       // Получаем свободное пространство эскиза (в виде 32-битного целого числа)

  String HeapFragmentation = (String)ESP.getHeapFragmentation();   // Получаем показатель фрагментации (0% - чистый, более ~ 50% - небезопасный)
  String ResetReason = ESP.getResetReason();                       // Получаем последную причину сброса питания как строку

  String myMAC = WiFi.macAddress();   // Получаем MAC адрес ESP8266 как строку
  String myBSSID = WiFi.BSSIDstr();   // Получаем MAC-адрес маршрутизатора, к которому подключены (как строка)

  String ChipId = (String)ESP.getChipId();                // Получаем от ESP8266 chip IDE (в виде 32-битного целого числа)
  String FlashChipId = (String)ESP.getFlashChipId();      // Получаем FLASH chip ID (в виде 32-битного целого числа)
  String CoreVersion = ESP.getCoreVersion();              // Получаем версию (ядра) платы "ESP8266 Boards" в Arduino IDE, как строку


  //Данная строка с HTML должна быть в этом месте, а не в глобальных переменных, иначе все динамические данные не будут отображаться/обновляться на странице с Информацией
  String webInfo2 = "<strong style=\"color:#0000FF\">Uptime:&nbsp;</strong>" + TimeUptime + "<br><strong style=\"color:#0000FF\">Частоту FLASH:&nbsp;</strong>" + FlashChipSpeed + "&nbsp;МГц<br><strong style=\"color:#0000FF\">Частоту CPU:&nbsp;</strong>" + CpuFreqMHz + "&nbsp;МГц<br><br><strong style=\"color:#0000FF\">Свободно ОЗУ:&nbsp;</strong>" + FreeHeap + "&nbsp;байт<br><br><strong style=\"color:#0000FF\">Размер FLASH:&nbsp;</strong>" + FlashChipSize + "&nbsp;байт<br><strong style=\"color:#0000FF\">Реальный размер FLASH:&nbsp;</strong>" + FlashChipRealSize + "&nbsp;байт<br><strong style=\"color:#0000FF\">Свободно FLASH:&nbsp;</strong>" + FreeSketchSpace + "&nbsp;байт<br><br><strong style=\"color:#0000FF\">Фрагментация:&nbsp;</strong>" + HeapFragmentation + "&nbsp;% (более 50% ПЛОХО)<br><strong style=\"color:#0000FF\">Неудачных авторизаций:&nbsp;</strong>" + eroorAuthoriz + "&nbsp;раз<br><strong style=\"color:#0000FF\">Причина сброса питания:&nbsp;</strong>" + ResetReason + "<br><br><strong style=\"color:#0000FF\">MAC адрес:&nbsp;</strong>" + myMAC + "<br><strong style=\"color:#0000FF\">BSSID:&nbsp;</strong>" + myBSSID + "<br><strong style=\"color:#0000FF\">HOSTNAME:&nbsp;</strong>" + host_Name + "<br><br><strong style=\"color:#0000FF\">Chip ID ESP8266:&nbsp;</strong>" + ChipId + "<br><strong style=\"color:#0000FF\">Chip ID FLASH:&nbsp;</strong>" + FlashChipId + "<br><strong style=\"color:#0000FF\">Версия платы ESP:&nbsp;</strong>" + CoreVersion + "</body></html>";
  webPage = webHeader + webInfo1 + webInfo2;    // формируем страницу
  server.send(200, "text/html", webPage);       // Отображаем страницу
}

//Функция отображения страницы со сменой Логина/Пароля для WEB морды
void PageLogPaswd() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  webPage = webHeader + webLoginPasswd;     // формируем страницу
  server.send(200, "text/html", webPage);   // Отображаем страницу
}

//Функция перезагрузки ESP-01
void PageReboot() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  webPage = webHeader + webBackPage + webLoading;   // формируем страницу
  server.send(200, "text/html", webPage);   // Отображаем страницу

  server.stop();  // Останавливаем web сервер
  delay(150);     // Ждём догрузки страницы "Загрузка..."
  ESP.restart();  // Перезагружаем ESP-01
}

//Функция сброса настроек (стирает сохранённые учётные данные WiFi). Они хранятся в библиотеке "WiFiManager.h" и перезагружает ESP-01
void PageEraseESP() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  webPage = webHeader + webResetSettings;   // формируем страницу
  server.send(200, "text/html", webPage);   // Отображаем страницу

  server.stop();      // Останавливаем web сервер
  delay(150);         // Ждём догрузки страницы "Загрузка..."
  wm.resetSettings(); // Стираем настройки Wi-Fi
  delay(150);         // Ещё ждём и...
  ESP.restart();      // перезагружаем ESP-01
}

//Функция записи в EEPROM, при успешной авторизации на WEB морде
void PageSaveEEPROM() {
  String header;
  if (!is_authenticated()) {    // Если авторизация по Логину/Паролю не прошла...
    server.sendHeader("Location", "/authorization");    // тогда переходим на страницу авторизации и...
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }// Если авторизация прошла успешно, тогда...
  collecting();   // вызываем функцию записи в EEPROM.
}
