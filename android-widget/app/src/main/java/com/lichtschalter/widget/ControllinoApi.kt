package com.lichtschalter.widget

import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL

/**
 * Kleiner blockierender HTTP-Client fuer den Controllino. Wird ausschliesslich von
 * Hintergrund-Threads aufgerufen (RemoteViewsFactory.onDataSetChanged() bzw. WorkManager-Worker),
 * nie vom Main-Thread - das ist bei beiden Aufrufern durch das Android-Framework garantiert.
 *
 * Spiegelt exakt die Endpunkte aus Provisorischer_Lichtschalter_freqtest.ino:
 * GET /state liefert "sz=1&ak=0&...", die Schaltbefehle sind dieselben Query-Strings
 * wie die Buttons in der Web-UI (z.B. "/?button1on").
 */
object ControllinoApi {

    data class LampInfo(val key: String, val label: String)

    // Reihenfolge = Reihenfolge im Widget.
    val TOGGLES = listOf(
        LampInfo("sz", "Schlafzimmer"),
        LampInfo("ak", "Ankleide"),
        LampInfo("bad", "Badspiegel"),
        LampInfo("bu", "Buero"),
        LampInfo("ku", "Kueche"),
        LampInfo("kl", "KuecheLED"),
        LampInfo("we", "Wohnzimmer Esstisch"),
        LampInfo("wt", "Wohnzimmer TV"),
        LampInfo("pump", "Umwaelzpumpe")
    )

    private val ON_PATH = mapOf(
        "sz" to "/?button1on",
        "ak" to "/?button2on",
        "bad" to "/?button3on",
        "bu" to "/?button4on",
        "ku" to "/?button5on",
        "kl" to "/?button5LEDon",
        "we" to "/?button6on",
        "wt" to "/?button7on",
        "pump" to "/?Pumpean"
    )
    private val OFF_PATH = mapOf(
        "sz" to "/?button1off",
        "ak" to "/?button2off",
        "bad" to "/?button3off",
        "bu" to "/?button4off",
        "ku" to "/?button5off",
        "kl" to "/?button5LEDoff",
        "we" to "/?button6off",
        "wt" to "/?button7off",
        "pump" to "/?Pumpestop"
    )

    fun togglePath(key: String, currentlyOn: Boolean): String? {
        return if (currentlyOn) OFF_PATH[key] else ON_PATH[key]
    }

    /** Ruft /state ab und liefert die key=value-Paare als Map, oder null bei Fehler/Timeout. */
    fun fetchState(ip: String): Map<String, String>? {
        return try {
            val text = get(ip, "/state")
            text.trim().split("&").mapNotNull { pair ->
                val idx = pair.indexOf('=')
                if (idx < 0) null else pair.substring(0, idx) to pair.substring(idx + 1)
            }.toMap()
        } catch (e: Exception) {
            null
        }
    }

    /** Feuert einen Schaltbefehl (z.B. "/?button1on") und ignoriert die Antwort, genau wie fetch() in der Web-UI. */
    fun sendCommand(ip: String, path: String): Boolean {
        return try {
            get(ip, path)
            true
        } catch (e: Exception) {
            false
        }
    }

    private fun get(ip: String, path: String): String {
        val connection = URL("http://$ip$path").openConnection() as HttpURLConnection
        connection.requestMethod = "GET"
        connection.connectTimeout = 3000
        connection.readTimeout = 3000
        try {
            connection.inputStream.use { stream ->
                BufferedReader(InputStreamReader(stream)).use { reader ->
                    return reader.readText()
                }
            }
        } finally {
            connection.disconnect()
        }
    }
}
