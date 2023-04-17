//********** ФУНКЦИЯ SETUP **********//


void setup() {
  //Объявление и настройка пинов
  pinMode(POWER1_3V3, OUTPUT);  // Выход постоянных 3.3V с ножки Witty Cloud
  pinMode(POWER2_3V3, OUTPUT);  // Выход постоянных 3.3V с ножки Witty Cloud
  pinMode(INDICATOR, OUTPUT);   // LED Индикатор на модуле ESP8266
  pinMode(PWR_SW, OUTPUT);      // Управление оптопары (Вкл./Выкл./жёсткое выкл. PC) как ВЫХОД
  pinMode(PWR_LED, INPUT);      // Отслеживания состояния LED как ВХОД
  pinMode(PIN_BUT, INPUT);      // Кнопка (Вкл./Выкл ПК, сброса настроек ESP и Перехода в режим AP) как ВХОД

  digitalWrite(POWER1_3V3, HIGH);  // Устанавливаем постоянное и неизменное высокое значение на ножке
  digitalWrite(POWER2_3V3, HIGH);  // Устанавливаем постоянное и неизменное высокое значение на ножке
  digitalWrite(INDICATOR, LOW);    // По умолчанию индикатор выключен
  digitalWrite(PWR_SW, LOW);       // Устанавливаем низкое значение на оптопару


  // Serial.begin(115200);
  // delay(2000);

  //Считываем структуру с данными WiFi и Логина/Пароля от UI из EEPROM
  EEPROM.begin(EEPROM_BEGIN);  // Инициализируем EEPROM
  EEPROM.get(0, portalCfg);    // Считываем структуру начиная с нулевой ячейки


  //Проверка EEPROM при полном её стирании с завода или самостоятельно через Arduino IDE (в этих случаях ячейки заполнены значениями 255)
  byte Check_EEPROM;            // Сюда будем считывать данные первой ячейки из EEPROM
  EEPROM.get(0, Check_EEPROM);  // Считываем нулевой адрес из EEPROM


  //Если нажата кнопка при старте ESP ИЛИ
  // переменная из структуры пустая (при сбросе настроек ESP мы заполнением EEPROM нулями) ИЛИ
  // EEPROM находится в заводском состоянии (при стирании всего Flash из Arduino IDE или просто новый чип), то память заполнена значениями 255, то...
  if (!digitalRead(PIN_BUT) || String(portalCfg.ssid) == "" || Check_EEPROM == 255) portalRun(PORTAL_TIMEOUT);  // Запускаем портал настроек WiFi и авторизации на WEB с таймаутом "PORTAL_TIMEOUT"

  //Получение данных WiFi и Логина/Пароля от UI с портала и сохранение их в EEPROM как структуру
  if (portalStatus() == SP_SUBMIT) {  // Если кнопка "Сохранить" с портала нажата, то получаем данные с формы через структуру
    EEPROM.begin(EEPROM_BEGIN);       // Инициализируем EEPROM
    EEPROM.put(0, portalCfg);         // Подготавливаем структуру к записи с нулевой ячейки (сохраняем её в ОЗУ)
    EEPROM.commit();                  // Записываем из ОЗУ в EEPROM
    EEPROM.end();                     // И очищаем данные из ОЗУ
    WiFi.softAPdisconnect();          // Отключаем режим AP
  }



  //ОТЛАДКА
  // Serial.print("SSID: ");
  // Serial.println(portalCfg.ssid);
  // Serial.print("Pass WiFi: ");
  // Serial.println(portalCfg.pass);
  // Serial.print("Login UI: ");
  // Serial.println(portalCfg.loginUI);
  // Serial.print("Pass UI: ");
  // Serial.println(portalCfg.passUI);


#if (ENABLE_REMOTEXY == 1)  // Выполняется, если включено управление через RemoteXY

  //Подключаемся к WiFi через библиотеку RemoteXY
#define REMOTEXY_WIFI_SSID portalCfg.ssid      // Подставляем в директиву SSID данные из структуры, считанные из EEPROM
#define REMOTEXY_WIFI_PASSWORD portalCfg.pass  // Подставляем в директиву PASSWORD данные из структуры, считанные из EEPROM

#else

  // Подключаемся к WiFi (оставил закомментированным на всякий случий)
  Serial.print("Connect to: ");
  Serial.println(portalCfg.ssid);
  WiFi.mode(WIFI_STA);                         // Запускаем WiFi в режиме "Станция"
  WiFi.hostname("WAKE-ON-LINE");               // Задаём имя хоста
  WiFi.begin(portalCfg.ssid, portalCfg.pass);  // Берём данные из структуры
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! Local IP: ");
  Serial.println(WiFi.localIP());

#endif


  // Уставливаем в режим таймера (остановится после срабатывания)
  tmr_OnOff.setTimerMode();  // Таймер для Вкл./Выкл. ПК
  tmr_Reset.setTimerMode();  // Таймер для Reset(Off) ПК


  //Кнопка на ESP
  ButtSwRes.setHoldTimeout(SET_HOLD_TIMEOUT);  // Устанавливаем время удержания кнопки


  LittleFS.begin();                                    // Инициализируем FS
  ui.attachBuild(build);                               // Подключаем конструктор UI и запускаем его
  ui.attach(action);                                   // Подключаем функцию с действиями от WEB интерфейса
  ui.start();                                          // Запустить портал
  ui.enableOTA();                                      // Подключить OTA обновление (без пароля)
  ui.enableAuth(portalCfg.loginUI, portalCfg.passUI);  // Включить авторизацию на WEB интерфейсе

#if (ENABLE_REMOTEXY == 1)  // Выполняется, если включено управление через RemoteXY

  RemoteXY_Init();  // Инициализация RemoteXY

#endif
}
