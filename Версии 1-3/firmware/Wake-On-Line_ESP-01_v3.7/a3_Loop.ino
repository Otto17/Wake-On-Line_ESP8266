//********** ФУНКЦИЯ LOOP **********//


void loop() {
  server.handleClient();        // Постоянное прослушивание HTTP-запросов от клиентов
  uptime::calculateUptime();    // Вызываем Функцию для вычисления Uptime

  if (delayUptime.Now()) {    //Формирование строки Uptime выполняется раз в 1 секу билиотекой "<SmartDelay.h>"(без остановки кода)
    TimeUptime = (String) uptime::getDays() + ":" + uptime::getHours() + ":" + uptime::getMinutes() + ":" + uptime::getSeconds();
  }

}
