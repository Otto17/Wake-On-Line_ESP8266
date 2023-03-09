//********** ФУНКЦИИ ПРОВЕРКИ АВТОРИЗАЦИИ И ВЫВОДА СТРАНИЦЫ С ОШИБКОЙ 404**********//


//Функция проверки, присутствует ли заголовок "Cookie"
bool is_authenticated() {
  if (server.hasHeader("Cookie")) {                 // Если Cookie найдены в браузере, тогда...
    String cookie = server.header("Cookie");        // присваиваем переменной "cookie" эти данные.
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {   // Если нашли совпадения в "ESPSESSIONID", то условие истино
      return true;
    }
  }// Иначе лож
  return false;
}

//Функция авторизации и выхода на WEB морде
void PageAuthorization() {
  String msgError;    // Переменная для вывода ошибки, в случае неудачной авторизации
  if (server.hasHeader("Cookie")) {             // Если Cookie найдены в браузере, тогда...
    String cookie = server.header("Cookie");    // присваиваем переменной "cookie" эти данные.
  }
  if (server.hasArg("DISCONNECT")) {    // Если пришла команда DISCONNECT с WEB морды, тогда...
    server.sendHeader("Location", "/authorization");    // Переходим на страницу авторизации
    server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");  // Меняем флаг "ESPSESSIONID" на 0
    server.send(301);   // Получаем ответ от HTTP сервера
    return;
  }
  if (server.hasArg("login") && server.hasArg("pswd")) {    // Если в запросе к серверу есть аргументы с Логином и Паролем, тогда...
    if (server.arg("login") == www_login &&  server.arg("pswd") == www_pswd) {    // проверяем второе условие на валидацию этого же Логина м Пароля, если успешно, тогда...
      server.sendHeader("Location", "/");                 // Переходим на главную страницу Управления PC
      server.sendHeader("Cache-Control", "no-cache");     // отправляем запрос на сервер для валидации ресурса
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");  // Меняем флаг "ESPSESSIONID" на 1
      server.send(301);   // Получаем ответ от HTTP сервера
      return;
    }
    // Прибавляем счётчик на +1 и выводим надпись с ошибкой авторизации
    eroorAuthoriz += 1;   // Счётчик неудачных попыток авторизации после включения ESP8266
    msgError = "<font style=\"color:#ff0000\"><strong>Неверный Логин или Пароль!</strong></font>";
  }
  String content = "</head><body><form action=\"/authorization\" method=\"POST\"><h3>Страница авторизации</h3><label style=\"color:#0000FF\">Логин:&nbsp;&nbsp;&nbsp;</label><input type=\"text\" name=\"login\" placeholder=\"Введите Логин\" required><br><br><label style=\"color:#0000FF\">Пароль:&nbsp;</label><input type=\"password\" name=\"pswd\" placeholder=\"Введите Пароль\" required><br><br><input type=\"submit\" name=\"SUBMIT\" value=\"Войти в систему\"></form>" + msgError + "</body></html>";
  webPage = webHeader + content;            // Формируем страницу
  server.send(200, "text/html", webPage);   // Отображаем страницу
}

//Функция обрабатки страницы с ошибкой 404
void handleNotFound() {
  webPage = webHeader + webPageNotFound;    // Формируем страницу
  server.send(404, "text/html", webPage);   // Отображаем сформированную страницу с ошибкой
}
