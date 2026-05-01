package com.example.wake_on_line

import android.content.Intent
import android.os.Bundle
import android.util.Base64
import android.webkit.HttpAuthHandler
import android.webkit.WebChromeClient
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AppCompatActivity
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout

class MainActivity : AppCompatActivity() {

    private lateinit var webView:      WebView
    private lateinit var swipeRefresh: SwipeRefreshLayout
    private lateinit var storage:      SecureStorage

    // Данные загружаются из зашифрованного хранилища
    private var siteUrl  = ""
    private var login    = ""
    private var password = ""

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        storage = SecureStorage(this)

        // Если данные ещё не настроены - отправляет на экран настройки
        if (!storage.isConfigured()) {
            startActivity(Intent(this, SetupActivity::class.java))
            finish()
            return
        }

        // Читает сохранённые данные
        siteUrl  = storage.getUrl()
        login    = storage.getLogin()
        password = storage.getPassword()

        setContentView(R.layout.activity_main)

        webView      = findViewById(R.id.webView)
        swipeRefresh = findViewById(R.id.swipeRefresh)

        setupWebView()
        setupBackButton()

        swipeRefresh.setOnRefreshListener {
            webView.reload()
        }

        loadPageWithAuth()
    }

    private fun setupWebView() {

        @Suppress("SetJavaScriptEnabled")
        webView.settings.javaScriptEnabled = true

        webView.settings.apply {
            domStorageEnabled    = true
            loadWithOverviewMode = true
            useWideViewPort      = true
            builtInZoomControls  = true
            displayZoomControls  = false
        }

        webView.webViewClient = object : WebViewClient() {

            override fun onReceivedHttpAuthRequest(
                view:    WebView,
                handler: HttpAuthHandler,
                host:    String,
                realm:   String
            ) {
                handler.proceed(login, password)
            }

            override fun onPageFinished(view: WebView, url: String) {
                super.onPageFinished(view, url)
                swipeRefresh.isRefreshing = false
            }

            override fun onReceivedError(
                view:    WebView,
                request: WebResourceRequest,
                error:   WebResourceError
            ) {
                super.onReceivedError(view, request, error)

                if (request.isForMainFrame) {
                    swipeRefresh.isRefreshing = false
                    val errorHtml = """
                        <html>
                        <body style='font-family:sans-serif;padding:20px;text-align:center;'>
                            <h2 style='color:red;'>Ошибка подключения</h2>
                            <p><b>Адрес:</b> $siteUrl</p>
                            <p><b>Описание:</b> ${error.description}</p>
                            <p style='color:gray;'>
                                Проверьте, что ESP8266 включён и доступен в сети
                            </p>
                        </body>
                        </html>
                    """.trimIndent()
                    view.loadDataWithBaseURL(null, errorHtml, "text/html", "UTF-8", null)
                }
            }
        }

        webView.webChromeClient = object : WebChromeClient() {
            override fun onProgressChanged(view: WebView, newProgress: Int) {
                swipeRefresh.isRefreshing = newProgress < 100
            }
        }
    }

    private fun loadPageWithAuth() {
        val credentials = "$login:$password"
        val base64 = Base64.encodeToString(
            credentials.toByteArray(),
            Base64.NO_WRAP
        )
        webView.loadUrl(siteUrl, mapOf("Authorization" to "Basic $base64"))
    }

    private fun setupBackButton() {
        onBackPressedDispatcher.addCallback(
            this,
            object : OnBackPressedCallback(true) {
                override fun handleOnBackPressed() {
                    if (webView.canGoBack()) {
                        webView.goBack()
                    } else {
                        finishAndRemoveTask()
                    }
                }
            }
        )
    }
}