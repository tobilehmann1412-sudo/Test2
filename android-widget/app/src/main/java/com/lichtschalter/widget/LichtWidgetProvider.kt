package com.lichtschalter.widget

import android.app.PendingIntent
import android.appwidget.AppWidgetManager
import android.appwidget.AppWidgetProvider
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.widget.RemoteViews
import androidx.work.ExistingPeriodicWorkPolicy
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.PeriodicWorkRequestBuilder
import androidx.work.WorkManager
import androidx.work.workDataOf
import java.util.concurrent.TimeUnit

class LichtWidgetProvider : AppWidgetProvider() {

    override fun onUpdate(context: Context, appWidgetManager: AppWidgetManager, appWidgetIds: IntArray) {
        for (appWidgetId in appWidgetIds) {
            updateWidget(context, appWidgetManager, appWidgetId)
        }
        schedulePeriodicRefresh(context)
    }

    override fun onDeleted(context: Context, appWidgetIds: IntArray) {
        for (id in appWidgetIds) {
            Prefs.deleteIp(context, id)
        }
    }

    override fun onReceive(context: Context, intent: Intent) {
        super.onReceive(context, intent)
        when (intent.action) {
            ACTION_REFRESH -> {
                val appWidgetId = intent.getIntExtra(
                    AppWidgetManager.EXTRA_APPWIDGET_ID,
                    AppWidgetManager.INVALID_APPWIDGET_ID
                )
                refreshData(context, appWidgetId)
            }

            ACTION_TOGGLE -> {
                val appWidgetId = intent.getIntExtra(
                    AppWidgetManager.EXTRA_APPWIDGET_ID,
                    AppWidgetManager.INVALID_APPWIDGET_ID
                )
                val path = intent.getStringExtra(EXTRA_TOGGLE_PATH)
                if (path != null && appWidgetId != AppWidgetManager.INVALID_APPWIDGET_ID) {
                    val ip = Prefs.loadIp(context, appWidgetId)
                    val work = OneTimeWorkRequestBuilder<ToggleWorker>()
                        .setInputData(
                            workDataOf(
                                ToggleWorker.KEY_IP to ip,
                                ToggleWorker.KEY_PATH to path,
                                ToggleWorker.KEY_WIDGET_ID to appWidgetId
                            )
                        )
                        .build()
                    WorkManager.getInstance(context).enqueue(work)
                }
            }
        }
    }

    private fun refreshData(context: Context, appWidgetId: Int) {
        val manager = AppWidgetManager.getInstance(context)
        if (appWidgetId == AppWidgetManager.INVALID_APPWIDGET_ID) {
            val ids = manager.getAppWidgetIds(ComponentName(context, LichtWidgetProvider::class.java))
            if (ids.isNotEmpty()) {
                manager.notifyAppWidgetViewDataChanged(ids, R.id.widget_list)
            }
        } else {
            manager.notifyAppWidgetViewDataChanged(appWidgetId, R.id.widget_list)
        }
    }

    private fun schedulePeriodicRefresh(context: Context) {
        val request = PeriodicWorkRequestBuilder<PeriodicRefreshWorker>(15, TimeUnit.MINUTES).build()
        WorkManager.getInstance(context).enqueueUniquePeriodicWork(
            "lichtschalter_periodic_refresh",
            ExistingPeriodicWorkPolicy.KEEP,
            request
        )
    }

    companion object {
        fun updateWidget(context: Context, appWidgetManager: AppWidgetManager, appWidgetId: Int) {
            val views = RemoteViews(context.packageName, R.layout.widget_layout)

            // Jede Widget-Instanz braucht einen eindeutigen Intent, sonst verwechselt das
            // System die RemoteViewsService-Adapter mehrerer Widgets miteinander.
            val serviceIntent = Intent(context, LichtRemoteViewsService::class.java)
            serviceIntent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId)
            serviceIntent.data = Uri.parse(serviceIntent.toUri(Intent.URI_INTENT_SCHEME))
            views.setRemoteAdapter(R.id.widget_list, serviceIntent)

            val toggleIntent = Intent(context, LichtWidgetProvider::class.java).apply {
                action = ACTION_TOGGLE
                putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId)
            }
            val togglePendingIntent = PendingIntent.getBroadcast(
                context, appWidgetId, toggleIntent,
                PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
            )
            // Muss FLAG_MUTABLE sein: das System fuegt bei jedem Klick auf ein Listenelement
            // das fillInIntent aus getViewAt() in diesen Template-Intent ein.
            views.setPendingIntentTemplate(R.id.widget_list, togglePendingIntent)

            val refreshIntent = Intent(context, LichtWidgetProvider::class.java).apply {
                action = ACTION_REFRESH
                putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId)
            }
            val refreshPendingIntent = PendingIntent.getBroadcast(
                context, appWidgetId + 1_000_000, refreshIntent,
                PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
            )
            views.setOnClickPendingIntent(R.id.refresh_button, refreshPendingIntent)

            appWidgetManager.updateAppWidget(appWidgetId, views)
            appWidgetManager.notifyAppWidgetViewDataChanged(appWidgetId, R.id.widget_list)
        }
    }
}
