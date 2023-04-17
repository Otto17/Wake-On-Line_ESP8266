//********** НАСТРОЙКИ **********//


//Пины
#define POWER1_3V3 14  // GPIO14 - Постоянный выход 3.3V с ножки Witty Cloud (так как на модуле нет выхода 3.3V, а только 5V)
#define POWER2_3V3 13  // GPIO13 - Постоянный выход 3.3V с ножки Witty Cloud (так как на модуле нет выхода 3.3V, а только 5V)


#define PWR_LED 15   // GPIO15 - Вход с оптопары от светодиода "Power" на ПК
#define PIN_TEMP 12  // GPIO12 - Датчик температуры DS18B20
#define PWR_SW 5     // GPIO5  - Выход на оптопару для управления компьютером (Вкл./Выкл./жёсткое выкл. PC)
#define PIN_BUT 4    // GPIO4  - Кнопки Вкл./Выкл ПК, сброса настроек ESP и Перехода в режим AP
#define INDICATOR 2  // GPIO2  - Распаяный LED на модуле ESP8266 (просто как визуальный индикатор)


//Время удержания оптопары в открытом состоянии
#define OPTO_POWER 500   // Время (в мс) короткого нажатия кнопки Power Вкл./Выкл.
#define OPTO_RESET 5010  // Время (в мс) долгого удержания кнопки Power Reset(Off)


//Настройка таймаута удержания кнопки на ESP
#define SET_HOLD_TIMEOUT 5000  // Установить время удержания кнопки, мс (до 8 000). Для сброса настроек (форматирования FS, EEPROM и перезагрузка ESP в режим AP)


//Коррекция температуры датчика DS18B20
#define ENABLE_CORRECT 1  //  1 - включить, 0 - отключить коррекцию температуры
#define CORRECTION -1.0   // Коэффициент коррекции в °C (может быть как со знаком минус, так и со знаком плюс)


//Таймаут работы портала (в мс), после истичения которого портал настроек закроется и вкл. режим "WIFI_STA"
#define PORTAL_TIMEOUT 300000  // Портал закроется через 5 минут


//RemoteXY (через облако)
#define ENABLE_REMOTEXY 1                                        //  1 - включить, 0 - отключить управление через RemoteXY
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"               // Адрес облака
#define REMOTEXY_CLOUD_PORT 6376                                 // Порт облака
#define REMOTEXY_CLOUD_TOKEN "d12470b612e4***************d714a"  // Токен


//EEPROM
#define EEPROM_BEGIN 120  // Выделяем объём памяти в байтах под EEPROM. (Размер может быть от 4 до 4096 байт)


//FS
#define ENABLE_FORMAT_FS 0  //  1 - включить, 0 - отключить форматирование файловой системы при сбросе настроек


//Указываем вручную версию скетча, для вывода её на WEB морде
#define VERSION "4.8"
