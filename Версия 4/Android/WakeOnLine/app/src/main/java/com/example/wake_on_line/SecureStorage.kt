package com.example.wake_on_line

import android.content.Context
import androidx.core.content.edit
import android.content.SharedPreferences
import androidx.security.crypto.MasterKey
import androidx.security.crypto.EncryptedSharedPreferences

class SecureStorage(context: Context) {

    companion object {
        private const val FILE_NAME  = "wake_secure_prefs"
        private const val KEY_URL      = "url"
        private const val KEY_LOGIN    = "login"
        private const val KEY_PASSWORD = "password"
        private const val KEY_SAVED    = "is_saved"
    }

    // Создаёт мастер-ключ шифрования (хранится в Android Keystore)
    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .build()

    // Создаёт зашифрованное хранилище
    private val prefs: SharedPreferences = EncryptedSharedPreferences.create(
        context,
        FILE_NAME,
        masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
    )

    // Проверяет — были ли данные уже сохранены ранее
    fun isConfigured(): Boolean = prefs.getBoolean(KEY_SAVED, false)

    // Сохраняет все данные разом
    fun save(url: String, login: String, password: String) {
        prefs.edit {
            putString(KEY_URL, url)
            putString(KEY_LOGIN, login)
            putString(KEY_PASSWORD, password)
            putBoolean(KEY_SAVED, true)
        }
    }

    // Читает данные (возвращает пустую строку если нет)
    fun getUrl():      String = prefs.getString(KEY_URL,      "") ?: ""
    fun getLogin():    String = prefs.getString(KEY_LOGIN,    "") ?: ""
    fun getPassword(): String = prefs.getString(KEY_PASSWORD, "") ?: ""
}