package com.lichtschalter.widget

import android.content.Context

// Speichert pro Widget-Instanz (appWidgetId) die IP-Adresse des Controllino,
// damit man theoretisch mehrere Widgets fuer unterschiedliche Geraete anlegen koennte.
object Prefs {
    private const val PREFS_NAME = "com.lichtschalter.widget.WIDGET_PREFS"
    private const val KEY_PREFIX = "ip_"
    private const val DEFAULT_IP = "192.168.2.200"

    fun saveIp(context: Context, appWidgetId: Int, ip: String) {
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE).edit()
            .putString(KEY_PREFIX + appWidgetId, ip)
            .apply()
    }

    fun loadIp(context: Context, appWidgetId: Int): String {
        return context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
            .getString(KEY_PREFIX + appWidgetId, DEFAULT_IP) ?: DEFAULT_IP
    }

    fun deleteIp(context: Context, appWidgetId: Int) {
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE).edit()
            .remove(KEY_PREFIX + appWidgetId)
            .apply()
    }
}
