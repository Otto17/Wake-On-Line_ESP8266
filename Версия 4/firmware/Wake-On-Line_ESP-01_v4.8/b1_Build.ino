//********** КОНСТРУКТОР САЙТА **********//


//Функция с конструктором (страница - Управление)
void build() {
  //Базовые настройки
  GP.BUILD_BEGIN(400);            // Начать построение, ширина колонки 400px
  GP.THEME(GP_DARK);              // Тема из памяти прошивки [доступные темы: GP_LIGHT, GP_DARK]
  GP.PAGE_TITLE("Wake-On-Line");  // Переименовать окно в браузере [строка]

  // Блок навигации (Динамические вкладки)
  GP.NAV_TABS("Управление, Настройки, Информация");  // Названия вкладок


  //Главная страница - "Управление"
  GP.NAV_BLOCK_BEGIN();       // Начало вкладки навигации
  GP.TITLE("Управление ПК");  // Заголовок на странице [строка]
  GP.HR(GP_ORANGE);           // Горизонтальная линия-разделитель + цвет из списка (см. GP. Цвета) или цвет в формате PSTR("#rrggbb")
  // GP.JQ_SUPPORT();            // Поддержка jQuery. Файл скачается с https://code.jquery.com/
  GP.JQ_SUPPORT_FILE();      // поддержка jquery, файл скачается из памяти (/gp_data/jquery.js)
  GP.JQ_UPDATE_BEGIN(2000);  // Начать обновляемый блок с периодом 1 секунда. Один на страницу!

  M_BLOCK(                                 // Блок для LED (чтобы выделялся от фона, для красоты)
    GP.LABEL("Состояние ПК:");             // Подпись (для кнопок, полей, чекбоксов и тд.)
    GP.LED("", led_State);                 // Индикатор состояния ПК
    GP.LABEL(PowerState, "", ColorTitle);  // Подпись ON (красного цвета) или OFF (зелёного цвета), в зависимости от состояния Power LED
    GP.BREAK();                            // Перенести строку

    GP.LABEL(GP.ICON_FILE("/icons/temp.svg", 15, GP_YELLOW_B) + "В корпусе ПК:"); GP.LABEL(str_tempC, "", GP_WHITE);  // Белая иконка с надпись и выводом температуры с DS18B20
  );


  GP.BUTTON("pwr_sw", ButState);  // Кнопка (Вкл./Выкл.) ПК, название меняется от состояния LED через переменную "ButState"
  GP.JQ_UPDATE_END();             // Закончить обновляемый блок jQuery


  GP.BREAK();                                                        // Перенести строку
  GP.CONFIRM("inf_pwr_res", "Точно желаешь ЖЁСТКО выключить ПК ?");  // Всплывающее информационное окно с подтверждением для кнопки "Reset (Off)". Нужно ответить на update цифрой 1, тогда будет отображён указанный текст, выполняется в action()
  GP.BUTTON_MINI("pwr_res", "Reset (Off)", "", GP_RED_B);            // Кнопка "Reset", при нажатии на которую ПК ЖЁСТКО выключится. Ярко красная кнопка "GP_RED_B"
  GP.UPDATE_CLICK("inf_pwr_res", "pwr_res");                         // Механизм обновления. Вызвать update у компонентов в списке "inf_pwr_res" [строка] при клике по компонентам из списка "pwr_res" [строка]

  GP.NAV_BLOCK_END();  // Конец вкладки навигации



  //Cтраница - "Настройки"
  GP.NAV_BLOCK_BEGIN();
  GP.TITLE("Настройки");  // Заголовок на странице [строка]
  GP.HR(GP_ORANGE);       // Горизонтальная линия-разделитель + цвет из списка (см. GP. Цвета) или цвет в формате PSTR("#rrggbb")

  //Блок настроек - Файловый менеджер
  GP_MAKE_BLOCK_TAB(                                        // Блок нужен что бы чтобы содержимое выделялось от фона, для красоты
    "Файловый менеджер",                                    // Название блока
    GP.FILE_MANAGER(&LittleFS, "/");                        // Файловый менеджер. Передай &ссылку на свою файловую систему [например &LittleFS, &SPIFFS, &SD] + указать путь, по которому выводить файлы [пример "/myData/foler/"]
    GP.FILE_UPLOAD("uploadFile", "Загрузить файл");         // Кнопка для загрузки файла (файлов) на сервер + текст на кнопке
    GP.FOLDER_UPLOAD("uploadFolder", "Загрузить каталог");  // Кнопка для загрузки каталога с файлами на сервер. Название файла будет содержать полный путь в указанной папке + текст на кнопке
  );



  //Блок настроек - WEB авторизация
  GP_MAKE_BLOCK_TAB(      // Блок нужен что бы чтобы содержимое выделялось от фона, для красоты
    "Обновление данных",  // Название блока

    GP.FORM_BEGIN("/ssid_pass");                    // Начать форму с именем (/ssid_pass)
    GP.LABEL("WiFi подключение", "", GP_ORANGE_B);  // Надпись
    //Надписи и поля для ввода данных WiFi
    GP_MAKE_BOX(GP.LABEL("SSID:   "); GP.TEXT("ssid_WiFi_UI", "1-32 символов", "", "", 0, "^(?=.{1,32}$).*"););      // Паттерн для Логина WiFi разрешает ввод любых от 1 до 32 символов
    GP_MAKE_BOX(GP.LABEL("Пароль:"); GP.PASS_EYE("pass_WiFi_UI", "8-32 символов", "*", "", 0, "^(?=.{8,32}$).*"););  // Паттерн для Пароля WiFi разрешает ввод любых от 8 до 32 символов

    GP.SUBMIT_MINI("Сохранить ✓", GP_ORANGE_B);  // Кнопка отправки данных с формы (текст + цвет из списка цветов)
    GP.FORM_END();                               // Завершить форму для WiFi


    GP.BREAK();     // Перенести строку
    GP.HR(GP_RED);  // Горизонтальная линия-разделитель красного цвета
    GP.BREAK();     // Перенести строку


    GP.FORM_BEGIN("/log_pass");                  // Начать форму с именем (/log_pass)
    GP.LABEL("WEB авторизация", "", GP_VIOL_B);  // Надпись
    //Надписи и поля для ввода данных на WEB интерфейсе
    GP_MAKE_BOX(GP.LABEL("Логин:  "); GP.TEXT("lg_UI", "2-20 символов", "", "", 0, "^(?=.{2,20}$).*"););      // Паттерн для Логина UI разрешает ввод любых от 2 до 20 символов
    GP_MAKE_BOX(GP.LABEL("Пароль:"); GP.PASS_EYE("ps_UI", "4-20 символов", "*", "", 0, "^(?=.{4,20}$).*"););  // Паттерн для Пароля UI разрешает ввод любых от 4 до 20 символов

    GP.SUBMIT_MINI("Сохранить ✓", GP_VIOL_B);  // Кнопка отправки данных с формы (текст + цвет из списка цветов)
    GP.FORM_END();                             // Завершить форму для авторизации на WEB интерфейсе
  );



  //Блок настроек - Обслуживание ESP
  GP_MAKE_BLOCK_TAB(     // Блок нужен что бы чтобы содержимое выделялось от фона, для красоты
    "Обслуживание ESP",  // Название блока
    //Обновление прошивки ESP
    GP.OTA_FIRMWARE("Обновить прошивку", GP_ORANGE_B);  // Загрузка файла прошивки для обновления
    GP.HR();                                            // Горизонтальная линия-разделитель

    //Перезагрузка ESP
    GP.CONFIRM("inf_rebootESP", "Желаешь перезагрузить ESP ?");     // Всплывающее информационное окно с подтверждением для кнопки "Reset (Off)". Нужно ответить на update цифрой 1, тогда будет отображён указанный текст, выполняется в action()
    GP.BUTTON_MINI("rebootESP", "Перезагрузить ESP", "", GP_BLUE);  // Кнопка "Reset", при нажатии на которую ПК ЖЁСТКО выключится. Голубая кнопка
    GP.UPDATE_CLICK("inf_rebootESP", "rebootESP");                  // Механизм обновления. Вызвать update у компонентов в списке "inf_pwr_res" [строка] при клике по компонентам из списка "pwr_res" [строка]

    GP.HR();  // Горизонтальная линия-разделитель

    // Сброс настроек
    GP.CONFIRM("inf_resetESP", "Точно хочешь сбрасить настройки ESP и перейти в режим AP ?");  // Всплывающее информационное окно с подтверждением для кнопки "Reset (Off)". Нужно ответить на update цифрой 1, тогда будет отображён указанный текст, выполняется в action()
    GP.BUTTON_MINI("resetESP", "Сброс настроек ESP", "", GP_RED);                              // Кнопка для Сброса настроек ESP
    GP.UPDATE_CLICK("inf_resetESP", "resetESP");                                               // Механизм обновления. Вызвать update у компонентов в списке "inf_pwr_res" [строка] при клике по компонентам из списка "pwr_res" [строка]
  );
  GP.NAV_BLOCK_END();



  //Cтраница - "Информация"
  GP.NAV_BLOCK_BEGIN();
  GP.TITLE("Информация");   // Заголовок на странице [строка]
  GP.HR(GP_ORANGE);         // Горизонтальная линия-разделитель + цвет из списка (см. GP. Цвета) или цвет в формате PSTR("#rrggbb")
  GP.SYSTEM_INFO(VERSION);  // Таблица с системной информацией + версия вашей программы (в таблице появится строка Version с указанным текстом), [строка]

  GP.NAV_BLOCK_END();

  GP.BUILD_END();  // Завершить отрисовку всего блока
}
