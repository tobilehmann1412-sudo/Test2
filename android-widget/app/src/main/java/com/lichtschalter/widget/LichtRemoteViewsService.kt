package com.lichtschalter.widget

import android.appwidget.AppWidgetManager
import android.content.Context
import android.content.Intent
import android.view.View
import android.widget.RemoteViews
import android.widget.RemoteViewsService

class LichtRemoteViewsService : RemoteViewsService() {
    override fun onGetViewFactory(intent: Intent): RemoteViewsFactory {
        val appWidgetId = intent.getIntExtra(
            AppWidgetManager.EXTRA_APPWIDGET_ID,
            AppWidgetManager.INVALID_APPWIDGET_ID
        )
        return LichtRemoteViewsFactory(applicationContext, appWidgetId)
    }
}

private class LichtRemoteViewsFactory(
    private val context: Context,
    private val appWidgetId: Int
) : RemoteViewsService.RemoteViewsFactory {

    private data class Row(
        val title: String,
        val subtitle: String?,
        val status: String,
        val statusColor: Int,
        val togglePath: String?
    )

    private var rows: List<Row> = emptyList()

    override fun onCreate() {}

    // Wird vom System auf einem Hintergrund-Thread aufgerufen, bevor getCount()/getViewAt()
    // gelesen werden - hier ist ein blockierender HTTP-Request also sicher.
    override fun onDataSetChanged() {
        val ip = Prefs.loadIp(context, appWidgetId)
        val state = ControllinoApi.fetchState(ip)
        rows = if (state == null) {
            listOf(Row(context.getString(R.string.error_no_connection), null, "", 0, null))
        } else {
            buildRows(state)
        }
    }

    private fun buildRows(state: Map<String, String>): List<Row> {
        val list = ArrayList<Row>()
        val on = 0xFF22C55E.toInt()
        val off = 0xFF475569.toInt()
        val info = 0xFF94A3B8.toInt()

        for (t in ControllinoApi.TOGGLES) {
            val isOn = state[t.key] == "1"
            val subtitle = if (t.key == "sz") state["szl"]?.let { "LED: $it/1023" } else null
            list.add(
                Row(
                    title = t.label,
                    subtitle = subtitle,
                    status = if (isOn) "AN" else "AUS",
                    statusColor = if (isOn) on else off,
                    togglePath = ControllinoApi.togglePath(t.key, isOn)
                )
            )
        }

        state["wel"]?.let { list.add(Row("Wohnzimmer LED Esstisch", null, "$it/1023", info, null)) }
        state["wtl"]?.let { list.add(Row("Wohnzimmer LED TV", null, "$it/1023", info, null)) }
        state["fl"]?.let { list.add(Row("Flur", null, "$it/1023", info, null)) }

        return list
    }

    override fun onDestroy() {}

    override fun getCount(): Int = rows.size

    override fun getViewAt(position: Int): RemoteViews {
        val row = rows[position]
        val views = RemoteViews(context.packageName, R.layout.widget_item)
        views.setTextViewText(R.id.item_title, row.title)

        if (row.subtitle != null) {
            views.setViewVisibility(R.id.item_subtitle, View.VISIBLE)
            views.setTextViewText(R.id.item_subtitle, row.subtitle)
        } else {
            views.setViewVisibility(R.id.item_subtitle, View.GONE)
        }

        views.setTextViewText(R.id.item_status, row.status)
        if (row.statusColor != 0) {
            views.setTextColor(R.id.item_status, row.statusColor)
        }

        if (row.togglePath != null) {
            val fillInIntent = Intent().putExtra(EXTRA_TOGGLE_PATH, row.togglePath)
            views.setOnClickFillInIntent(R.id.item_root, fillInIntent)
        }

        return views
    }

    override fun getLoadingView(): RemoteViews? = null
    override fun getViewTypeCount(): Int = 1
    override fun getItemId(position: Int): Long = position.toLong()
    override fun hasStableIds(): Boolean = true
}
