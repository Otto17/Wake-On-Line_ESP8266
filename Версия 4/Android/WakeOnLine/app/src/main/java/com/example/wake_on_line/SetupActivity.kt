package com.example.wake_on_line

import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.util.Base64
import android.view.inputmethod.InputMethodManager
import android.webkit.HttpAuthHandler
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class SetupActivity : AppCompatActivity() {

    private lateinit var etUrl:      EditText
    private lateinit var etLogin:    EditText
    private lateinit var etPassword: EditText
    private lateinit var btnSave:    Button
    private lateinit var storage:    SecureStorage

    private var pendingUrl      = ""
    private var pendingLogin    = ""
    private var pendingPassword = ""

    private lateinit var checkWebView: WebView

    // Флаги для отслеживания результата загрузки
    private var hadError       = false  // Была ли ошибка сети
    private var hadHttpError   = false  // Был ли HTTP-ответ с ошибкой (401, 403 и т.д.)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_setup)

        storage    = SecureStorage(this)
        etUrl      = findViewById(R.id.etUrl)
        etLogin    = findViewById(R.id.etLogin)
        etPassword = findViewById(R.id.etPassword)
        btnSave    = findViewById(R.id.btnSave)

        checkWebView = WebView(this)
        setupCheckWebView()

        btnSave.setOnClickListener {
            val url      = etUrl.text.toString().trim()
            val login    = etLogin.text.toString().trim()
            val password = etPassword.text.toString().trim()

            if (url.isEmpty() || login.isEmpty() || password.isEmpty()) {
                Toast.makeText(this, "Заполните все поля", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            val fullUrl = if (url.startsWith("http")) url else "http://$url"

            pendingUrl      = fullUrl
            pendingLogin    = login
            pendingPassword = password

            // Сбрасывает флаги перед каждой новой попыткой
            hadError     = false
            hadHttpError = false

            btnSave.isEnabled = false
            btnSave.text      = "Проверка подключения..."

            hideKeyboard()
            checkConnection(fullUrl, login, password)
        }
    }

    private fun setupCheckWebView() {

        @Suppress("SetJavaScriptEnabled")
        checkWebView.settings.javaScriptEnabled = true

        checkWebView.webViewClient = object : WebViewClient() {

            // Отвечает на Basic Auth
            override fun onReceivedHttpAuthRequest(
                view:    WebView,
                handler: HttpAuthHandler,
                host:    String,
                realm:   String
            ) {
                handler.proceed(pendingLogin, pendingPassword)
            }

            // Перехватывает HTTP-ответы с кодами ошибок (401, 403, 404, 5xx...)
            override fun onReceivedHttpError(
                view:     WebView,
                request:  WebResourceRequest,
                response: WebResourceResponse
            ) {
                super.onReceivedHttpError(view, request, response)

                // Реагирует только на ошибки основной страницы
                if (request.isForMainFrame) {
                    val code = response.statusCode
                    // 401 = неверный логин/пароль
                    // 403 = доступ запрещён
                    // всё >= 400 считает ошибкой
                    if (code >= 400) {
                        hadHttpError = true
                    }
                }
            }

            // Страница начала загружаться - сбрасывает флаги
            override fun onPageStarted(view: WebView, url: String, favicon: Bitmap?) {
                super.onPageStarted(view, url, favicon)
                hadError     = false
                hadHttpError = false
            }

            // Страница "загружена" — но проверяет флаги ошибок
            override fun onPageFinished(view: WebView, url: String) {
                super.onPageFinished(view, url)

                when {
                    // Была сетевая ошибка (нет интернета, DNS не разрешился и т.д.)
                    hadError -> {
                        showError("Не удалось подключиться.\nПроверьте адрес и доступность сети.")
                    }

                    // Был HTTP-ответ с ошибкой (неверный логин/пароль)
                    hadHttpError -> {
                        showError("Неверный логин или пароль.\nПроверьте данные и попробуйте снова.")
                    }

                    // Всё хорошо — сохраняет и переходит
                    else -> {
                        storage.save(pendingUrl, pendingLogin, pendingPassword)
                        val intent = Intent(this@SetupActivity, MainActivity::class.java)
                        intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or
                                Intent.FLAG_ACTIVITY_CLEAR_TASK
                        startActivity(intent)
                        finish()
                    }
                }
            }

            // Сетевая ошибка (DNS, timeout, нет соединения)
            override fun onReceivedError(
                view:    WebView,
                request: WebResourceRequest,
                error:   WebResourceError
            ) {
                super.onReceivedError(view, request, error)

                if (request.isForMainFrame) {
                    hadError = true
                    // onPageFinished всё равно вызовется после этого, там и обработается через флаг hadError
                }
            }
        }
    }

    // Показывает ошибку и разблокирует форму
    private fun showError(message: String) {
        runOnUiThread {
            btnSave.isEnabled = true
            btnSave.text      = "Сохранить и подключиться"
            Toast.makeText(this@SetupActivity, message, Toast.LENGTH_LONG).show()
        }
    }

    private fun checkConnection(url: String, login: String, password: String) {
        val credentials = "$login:$password"
        val base64 = Base64.encodeToString(
            credentials.toByteArray(),
            Base64.NO_WRAP
        )
        checkWebView.loadUrl(url, mapOf("Authorization" to "Basic $base64"))
    }

    private fun hideKeyboard() {
        val imm = getSystemService(INPUT_METHOD_SERVICE) as InputMethodManager
        currentFocus?.let {
            imm.hideSoftInputFromWindow(it.windowToken, 0)
        }
    }

    override fun onDestroy() {
        checkWebView.destroy()
        super.onDestroy()
    }
}