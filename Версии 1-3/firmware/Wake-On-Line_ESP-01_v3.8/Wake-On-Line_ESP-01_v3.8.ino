/*

  Обновление прошивки Wake-On-Line на ESP-01 ver.3.x
  P.S. Изменения только в прошивке, железо не тронуто. Скетч максимально подробно прокоментирован!

  Возможности:
  - Переделана аутентификация по Логину/Паролю на WEB морде;
  - Добавлена поддержка "Cookie", что бы ненужно было постоянно авторизовываться;
  - Теперь браузер будет запрашивать сохранить Логин/Пароль от WEB морды или нет?;
  - Легко можно разлогиниться, а так же удобно обновлять страницу с управлением PC;
  - Информирование о состоянии PC в реальном времени(Включён или выключен);
  - Управление PC (Вкл./Выкл./Reset с подтвержением от случаного нажатия);
  - Добавлена страница "Настройки" с пунктами:
        - Информация (мониторинг ESP8266, Uptime, счётчик неудачных авторизаций на WEB морде);
        - Настройки WEB авторизации (смена Логина/Пароля через WEB морду);
        - Обновление прошивки ESP (по Воздуху), тут отдельно запрос Логина/Пароля без сохранения в браузере;
        - Перезагрузить ESP-01 (перезагружает ESP8266, так же обнуляет Uptime и счётчик неудачных авторизаций);
        - Сбросить пароль WiFi (Стирает сохранённе данные маршрутизатора и переходит в режим AP).


  Параметры платы в Arduino IDE:
  1) Выбрать плату "Generic ESP8266 Module"
  2) Builtin Led: 1
  3) Upload Speed: 115200
  4) CPU Frequency: 80 MHz
  5) Flash Size: 1MB (FS:64KB OTA:~470KB), то есть ставим по минимуму, так как Файловую систему не использовал
  6) Flash Frequency: 40 MHz
  Остальное не трогал, стоит по умолчанию.


  Краткое описание действий:
  После первой загрузки скетча подключаемся со смартфона или компьютера по WiFi к ESP-01 -
  SSID: Wake-On-Line
  Пароль: 123456789
  Тыркаем по кнопке "Configure WiFi", выбираем из списка или пишем вручную SSID и password роутера.
  Ниже пишем логин и пароль для доступа к WEB морде и нажимаем "Save".
  Готово, осталось посмотреть полученный по DHCP ip адрес на роутере или просканировать сеть любым сканером портов.
  Подключаемся по стандартному 80 порту и авторизуемся.

  P.S. Если забыли пароль от WEB морды, то просто отключаем роутер и перезагружаем по питанию ESP8266,
  ждём около 2-х минут, пока не появится на Смартфоне в списке WiFi сетей "Wake-On-Line", подключаемся к ней,
  далее включаем роутер и переподключаемся заново с указанием нового Логина/Пароля от WEB морды.

Изменения:
       - Первая версия скетча версии 3.x, описаная чуть выше (Версия: 3.7, Дата создания: 08.01.2022г.)
       - Исправил баг, при котором частенько ESP8266 не перезагружался фактически и не сбрасывал настройки, при нажатии кнопок "Перезагрузить ESP-01" или "Сбросить пароль WiFi",
         добавил на стараницу с Информацией возможность указать свою версию прошивки, так же немного береработал вкладки (Версия: 3.8, Дата создания: 13.09.2022г.)


  Дата создания: 13.09.2022г.
  Версия: 3.8
  Автор: Otto.

*/


