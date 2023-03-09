//********** ФУНКЦИЯ ЗАПИСИ ЛОГИНА/ПАРОЛЯ WEB МОРДЫ В EEPROM **********//


//Запись через WEB морду в обычном рабочем режиме
void collecting() {
  String login_ap;
  String pass_ap;
  unsigned char* buf = new unsigned char[64];

  String webPage = "";
  webPage += webHeader;
  webPage += "</head><body>";

  EEPROM.begin(EEPROM_begin);   // Выделяем объём памяти в байтах под EEPROM

  login_ap = server.arg(0);
  pass_ap = server.arg(1);

  if (login_ap != "") {
    EEPROM.write(loginNew, login_ap.length());    //Функция "EEPROM.write" записывает данные ЛОГИНА не на саму энергонезависимую память, а в ОЗУ
    EEPROM.write(passNew, pass_ap.length());      //Функция "EEPROM.write" записывает данные ПАРОЛЯ не на саму энергонезависимую память, а в ОЗУ
    login_ap.getBytes(buf, login_ap.length() + 1);
    for (byte i = 0; i < login_ap.length(); i++)
      EEPROM.write(i, buf[i]);
    pass_ap.getBytes(buf, pass_ap.length() + 1);
    for (byte i = 0; i < pass_ap.length(); i++)
      EEPROM.write(i + 32, buf[i]);
    EEPROM.commit();  //Функция "EEPROM.commit" записывает готовые данные из ОЗУ в во FLASH
    EEPROM.end();     //И очищаем данные из ОЗУ

    // Выводим страничку о успешном сохранении данных
    webPage += "<p style=\"color:#ff0000\">Конфигурация сохранена во FLASH!</br>\
    Перезагрузите ESP-01</p></br></br>\
    <a href=\"/rebootESP-01\">Перезагрузить!</a></br>";
  }
  else {  // Страница с ошибкой вызывается, если попытаться вручную вбить ip/saveDataEEPROM или перейти на эту страницу по ссылке
    webPage += "<p>Ошибка, данные НЕ сохранены!</p></br>\
    <a href=\"/webPasswd\">Вернуться</a> на страницу ввода данных.</br>";
  }
  webPage += "</body></html>";
  server.send ( 200, "text/html", webPage );   // Отображаем сформированную страницу
}
