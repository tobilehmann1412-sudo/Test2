package com.lichtschalter.widget

import android.app.Activity
import android.appwidget.AppWidgetManager
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText

/** Wird von Android automatisch geoeffnet, sobald das Widget auf den Homescreen gezogen wird. */
class WidgetConfigureActivity : Activity() {

    private var appWidgetId = AppWidgetManager.INVALID_APPWIDGET_ID

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setResult(Activity.RESULT_CANCELED)
        setContentView(R.layout.widget_configure)

        appWidgetId = intent?.extras?.getInt(
            AppWidgetManager.EXTRA_APPWIDGET_ID,
            AppWidgetManager.INVALID_APPWIDGET_ID
        ) ?: AppWidgetManager.INVALID_APPWIDGET_ID

        if (appWidgetId == AppWidgetManager.INVALID_APPWIDGET_ID) {
            finish()
            return
        }

        val ipInput = findViewById<EditText>(R.id.ip_input)
        ipInput.setText(Prefs.loadIp(this, appWidgetId))

        findViewById<Button>(R.id.save_button).setOnClickListener {
            val ip = ipInput.text.toString().trim()
            if (ip.isNotEmpty()) {
                Prefs.saveIp(this, appWidgetId, ip)

                val appWidgetManager = AppWidgetManager.getInstance(this)
                LichtWidgetProvider.updateWidget(this, appWidgetManager, appWidgetId)

                val resultValue = Intent().putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId)
                setResult(Activity.RESULT_OK, resultValue)
                finish()
            }
        }
    }
}
