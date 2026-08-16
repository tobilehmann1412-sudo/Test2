# Lichtschalter Widget (Android)

Echtes Homescreen-Widget für die Controllino-Hausautomation
(`Provisorischer_Lichtschalter_freqtest.ino`). Zeigt Lampen/Pumpe als Liste mit
AN/AUS-Status, tippen schaltet direkt um, plus die vier Dimmwerte als Info-Zeilen.

## Wichtig: kann hier nicht gebaut/getestet werden

Dieses Projekt wurde ohne Android SDK geschrieben (in der Sandbox stand nur
Java/Gradle zur Verfügung, kein `ANDROID_HOME`). Der Code folgt Standard-Android-
Mustern (App-Widget mit `RemoteViewsService`-Collection, `WorkManager` für
Hintergrund-Refresh, Konfigurations-Activity), wurde aber **nicht kompiliert**.
Beim ersten Öffnen in Android Studio können also noch kleine Korrekturen nötig
sein (z. B. falls sich API-Namen zwischen den hier verwendeten Bibliotheksversionen
minimal unterscheiden).

## Bauen

1. Diesen Ordner (`android-widget/`) in Android Studio öffnen (Datei → Öffnen).
2. Android Studio bietet beim ersten Öffnen an, den Gradle-Wrapper zu erzeugen –
   das bestätigen (es liegt bewusst kein `gradlew`/`gradle-wrapper.jar` im Repo,
   da Binärdateien hier nicht sinnvoll erzeugt werden konnten).
3. Gradle-Sync abwarten, dann per USB-Debugging oder Emulator installieren.
4. Auf dem Homescreen lange drücken → Widgets → "Lichtschalter" auf den
   Startbildschirm ziehen.
5. Es öffnet sich automatisch ein Dialog zur Eingabe der IP-Adresse des
   Controllino (Standard: `192.168.2.200`).

## Voraussetzungen/Einschränkungen

- **Nur im selben WLAN nutzbar.** Der Controllino ist nur im lokalen Netz
  (192.168.2.x) erreichbar, es gibt keine Anbindung über das Internet/Mobilfunk.
  Ohne VPN oder Port-Forwarding funktioniert das Widget außerhalb des Heimnetzes
  nicht (Port-Forwarding auf ein ungesichertes HTTP-Gerät wird hier bewusst nicht
  empfohlen).
- **Aktualisierung:** Android erlaubt Widgets grundsätzlich kein Live-Polling im
  Sekundentakt (Akku-/Hintergrundlimits). Deshalb:
  - Tippen auf das Sync-Icon oben rechts lädt sofort neu.
  - Tippen auf eine Lampe schaltet sofort um und aktualisiert danach die Zeile.
  - Im Hintergrund aktualisiert `WorkManager` zusätzlich alle ~15 Minuten
    (das kürzeste von Android garantierte periodische Intervall).
- **Live-Status wie in der Web-UI:** Das Widget zeigt dieselben Werte wie
  `GET /state` im Sketch. Badspiegel ist wie in der Web-UI nur "zuletzt
  gesendeter Befehl", da das Relais keine Rückmeldung gibt. Die fünf extern per
  Relais-Board geschalteten Rollos sind hier (noch) gar nicht enthalten – dazu
  müssten zusätzliche `/state`-Felder im Sketch ergänzt werden, siehe unten.
- **HTTP statt HTTPS:** Der Controllino spricht nur Klartext-HTTP, daher ist
  `usesCleartextTraffic="true"` gesetzt. Das ist für ein privates Heimnetz-Gerät
  okay, aber kein allgemeines Sicherheitsmuster.

## Aufbau

- `LichtWidgetProvider.kt` – App-Widget-Grundgerüst, Klick-Routing, plant den
  15-Minuten-Hintergrund-Refresh.
- `LichtRemoteViewsService.kt` – baut die Liste; ruft bei jedem Refresh
  `GET /state` ab und übersetzt die Werte in Zeilen.
- `ControllinoApi.kt` – der eigentliche (blockierende) HTTP-Client, exakt an die
  Endpunkte aus dem Sketch angelehnt.
- `RefreshWorker.kt` – zwei `WorkManager`-Worker: periodischer Refresh und
  einzelner Schaltbefehl (inkl. Refresh danach).
- `WidgetConfigureActivity.kt` – Eingabe der Controllino-IP beim Hinzufügen.
- `Prefs.kt` – speichert die IP pro Widget-Instanz.

## Rollos mit ins Widget holen

Aktuell zeigt das Widget nur Lampen/Pumpe. Wenn du die vier Rollos mit
echtem Status (Schlafzimmer, Küche rechts, WZR, WZL) auch im Widget willst,
lässt sich das mit den bereits im Sketch vorhandenen `/state`-Feldern
(`rsz`, `rkr`, `rwzl`, `rwzr`) ergänzen – sag Bescheid, dann baue ich das nach.
