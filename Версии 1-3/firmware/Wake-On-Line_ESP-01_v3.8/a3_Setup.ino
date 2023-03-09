//********** ФУНКЦИЯ SETUP **********//


void setup() {
  //Объявление и настройка пинов
  pinMode(pwr_sw, OUTPUT);           //Назначаем pin управления Транзистором Вкл./Выкл. как ВЫХОД
  pinMode(pwr_led, INPUT);           //Назначаем pin отслеживания состояния LED как ВХОД
  digitalWrite(pwr_sw, LOW);         //Устанавливаем низкое значение на выходе Транзистора

  //Serial.begin(115200);
  //Режим Station
  WiFi.mode(WIFI_STA);
  wm.setDebugOutput(false); //Отключаем отладку для Serial

  //Если нужен Статический IP адрес, тогда раскомментировать строку ниже (статика задаётся на странице WiFiManager в режиме AP)
  //wm.setSTAStaticIPConfig(IPAddress(192, 168, 50, 20), IPAddress(192, 168, 50, 1), IPAddress(255, 255, 255, 0)); // 4-й аргумент DNS - не обязательный

  //Установка личных параметров WiFiManager
  //В скобках - ID параметра, метка над полем ввода, значение по умолчанию в поле ввода, MAX длина символов для ввода
  WiFiManagerParameter custom_html("<p>Настройка WEB Авторизации</p>");       // Выводим только кастомный HTML над формами ввода
  WiFiManagerParameter custom_webLogin("login", "Введите Логин:", "", 15);    // Пустое поле ввода Логина с ID "login" и MAX 15 символов
  WiFiManagerParameter custom_webPasswd("pswd", "Введите пароль:", "", 15);   // Пустое поле ввода Пароля с ID "pswd" и MAX 15 символов

  //Добавляем параметры на WEB морду WiFiManager
  wm.addParameter(&custom_html);
  wm.addParameter(&custom_webLogin);
  wm.addParameter(&custom_webPasswd);

  wm.setDarkMode(DarkMode);    // Установка тёмной темы
  wm.setHostname(host_Name);   // Установка HOSTNAME
  wm.setConnectRetries(10);    // Кол-во повторных попыток подключения к WiFI

  //При запуске ESP вызываем функцию для смены флага на сохранение Логина/Пароля для WEB морды из WiFiManager,
  wm.setSaveConfigCallback(saveConfigCallback);

  //Пользовательское меню WiFiManager. Меняем пункты на какие нужно в первой строке
  std::vector<const char *> menu = {"wifi", "info", "sep", "erase", "update", "restart"};
  wm.setMenu(menu);   //  Устанавливаем параметры для отображения меню в WiFiManager

  //Для корректной работы "wm.autoConnect" должен быть включён после всех настроек WiFiManager
  wm.autoConnect(ssid_AP, pswd_AP);  // SSID и пароль для точки доступа AP


  //Блок записи Логина/Пароля из WiFiManager
  if (shouldSaveConfig) {   // Если флаг "shouldSaveConfig" стоит на ИСТИНА, тогда выполняется сохранение Логина/Пароля

    String login_apWM = custom_webLogin.getValue();   // Получаем сохранённый Логин из WiFiManager
    String pass_apWM = custom_webPasswd.getValue();   // Получаем сохранённый Пароль из WiFiManager

    unsigned char* buf = new unsigned char[64];

    EEPROM.begin(EEPROM_begin);   // Выделяем объём памяти в байтах под EEPROM

    if (login_apWM != "") {
      EEPROM.write(loginNew, login_apWM.length());    //Функция "EEPROM.write" записывает данные ЛОГИНА не на саму энергонезависимую память, а в ОЗУ
      EEPROM.write(passNew, pass_apWM.length());      //Функция "EEPROM.write" записывает данные ПАРОЛЯ не на саму энергонезависимую память, а в ОЗУ
      login_apWM.getBytes(buf, login_apWM.length() + 1);
      for (byte i = 0; i < login_apWM.length(); i++)
        EEPROM.write(i, buf[i]);
      pass_apWM.getBytes(buf, pass_apWM.length() + 1);
      for (byte i = 0; i < pass_apWM.length(); i++)
        EEPROM.write(i + 32, buf[i]);
      EEPROM.commit();  //Функция "EEPROM.commit" записывает готовые данные из ОЗУ в во FLASH
      EEPROM.end();     //И очищаем данные из ОЗУ

      // Выводим информацию о успешном сохранении данных в EEPROM
      //   Serial.print F(("Login: "));
      //   Serial.println(login_apWM);
      //   Serial.print F(("Passwd: "));
      //   Serial.println(pass_apWM);
    }
    else {
      // Serial.println F(("Error save to EEPROM"));   // Выдаём ошибку, что данные не сохранены
    }
  }


  //Блок для чтения Логина/Пароля из EEPROM для WEB морды в обычном рабочем режиме
  byte len_login, len_pass;

  EEPROM.begin(EEPROM_begin);   // Выделяем и резервируем объём памяти в байтах под EEPROM

  len_login = EEPROM.read(loginNew);    //Метод "EEPROM.read" возвращает байт из памяти по адресу loginNew
  len_pass = EEPROM.read(passNew);      //Метод "EEPROM.read" возвращает байт из памяти по адресу passNew
  if (len_pass > 64) len_pass = 0;

  unsigned char* buf_login = new unsigned char[32];
  unsigned char* buf_pass = new unsigned char[64];

  for (byte i = 0; i < len_login; i++) buf_login[i] = char(EEPROM.read(i));
  buf_login[len_login] = '\x0';

  const char *login  = (const char*)buf_login;
  for (byte i = 0; i < len_pass; i++) buf_pass[i] = char(EEPROM.read(i + 32));

  const char *pswd  = (const char*)buf_pass;
  buf_pass[len_pass] = '\x0';

  //Преобразуем локальные переменные"(const char*)login/pswd" в глобальные переменные "(const char*)www_login/pswd".
  //Глобальные переменные: "www_login" и "www_pswd" при авторизации на WEB морде.
  www_login = (const char*)login;
  www_pswd = (const char*)pswd;


  //При соответствии ссылки в браузере Server вызывает соответствующую функцию
  server.on("/authorization", PageAuthorization);   //Страница с авторизацией
  server.on("/", PageRoot);      //Стартовая страница управления питанием PC
  server.on("/on", PageOn);      //Вкл. PC
  server.on("/off", PageOff);    //Выкл. PC
  server.on("/hard", PageHard);  //Принудительное выключения PC в случае его зависания
  server.on("/settings", PageSettings);           // Общая страница с настройками
  server.on("/ipInform", PageInfo);               // Страница с информацией об устройстве
  server.on("/webPasswd", PageLogPaswd);          // Страница с настройками Логина/Пароля на WEB интерфейсе
  server.on("/rebootESP-01", PageReboot);         // Страница для перезагрузки ESP-01
  server.on("/resetEraseESP", PageEraseESP);      // Страница для сброса Wi-Fi сети
  server.on("/saveDataEEPROM", PageSaveEEPROM);   // Вызываем функцию для сохранения Логина/Пароля WEB морды в EEPROM

  //Страница для обновления OTA прошивки (по Воздуху)
  httpUpdater.setup(&server, OTAPATH, www_login, www_pswd);

  //Инициализация сервера
  server.onNotFound(handleNotFound);              // Вызывается, когда обработчик не назначен
  server.collectHeaders("User-Agent", "Cookie");  // Отслеживание сокетов
  server.begin();   // Запускаем HTTP сервер
}
