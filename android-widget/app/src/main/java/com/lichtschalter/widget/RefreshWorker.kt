package com.lichtschalter.widget

import android.appwidget.AppWidgetManager
import android.content.ComponentName
import android.content.Context
import androidx.work.Worker
import androidx.work.WorkerParameters

/**
 * Wird alle 15 Minuten von WorkManager angestossen (die kuerzeste garantierte periodische
 * Ausfuehrung unter Android - kuerzere Intervalle werden vom System ohnehin ignoriert) und
 * stoesst ein Neuladen der Widget-Liste an. Der eigentliche HTTP-Request nach /state passiert
 * dann in LichtRemoteViewsFactory.onDataSetChanged().
 */
class PeriodicRefreshWorker(context: Context, params: WorkerParameters) : Worker(context, params) {
    override fun doWork(): Result {
        val manager = AppWidgetManager.getInstance(applicationContext)
        val ids = manager.getAppWidgetIds(ComponentName(applicationContext, LichtWidgetProvider::class.java))
        if (ids.isNotEmpty()) {
            manager.notifyAppWidgetViewDataChanged(ids, R.id.widget_list)
        }
        return Result.success()
    }
}

/** Fuehrt einen einzelnen Schaltbefehl aus (z.B. "/?button1on") und laedt danach die Liste neu. */
class ToggleWorker(context: Context, params: WorkerParameters) : Worker(context, params) {
    override fun doWork(): Result {
        val ip = inputData.getString(KEY_IP) ?: return Result.failure()
        val path = inputData.getString(KEY_PATH) ?: return Result.failure()
        val widgetId = inputData.getInt(KEY_WIDGET_ID, AppWidgetManager.INVALID_APPWIDGET_ID)

        ControllinoApi.sendCommand(ip, path)

        if (widgetId != AppWidgetManager.INVALID_APPWIDGET_ID) {
            AppWidgetManager.getInstance(applicationContext)
                .notifyAppWidgetViewDataChanged(widgetId, R.id.widget_list)
        }
        return Result.success()
    }

    companion object {
        const val KEY_IP = "ip"
        const val KEY_PATH = "path"
        const val KEY_WIDGET_ID = "widget_id"
    }
}
