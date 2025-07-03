# Häufig gestellte Fragen (FAQ)
Dieses Dokument enthält häufig gestellte Fragen und Probleme, die beim Erstellen und Ausführen eines Yocto-Images für den Raspberry Pi 4 auftreten können.

## Bitbake-Build hängt fest
Während des Yocto-Build-Prozesses kann es vorkommen, dass der `bitbake`-Befehl für lange Zeit hängen bleibt oder plötzlich einfriert.  
Dies ist ein häufiges Problem, insbesondere bei Benutzern, die WSL (Windows Subsystem for Linux) verwenden.  
In der Regel sind die Systemressourcen erschöpft und der Build wird unterbrochen, da die Standard-RAM- und SWAP-Einstellungen von WSL unzureichend sind.

### Überprüfen und Anpassen von RAM- und SWAP-Größe
Um zu sehen, wie viel RAM und SWAP WSL zur Verfügung stehen hat, geben Sie im WSL-Terminal den Befehl `free -h` ein.  
Wenn der RAM unter 12 GB und der SWAP unter 32 GB liegt, können Sie die folgenden Schritte ausführen, um die erforderlichen Einstellungen vorzunehmen.

Die WSL-Konfigurationsdatei befindet sich auf der Windows-Seite im Benutzerordner:

```bash
C:\Users\<YOUR_USERNAME>\.wslconfig
```
Wenn die Datei nicht existiert, erstellen Sie sie mit dem Editor (Notepad) und fügen Sie folgenden Inhalt ein:

```bash
[wsl2]
memory=12GB
processors=4
swap=32GB
localhostForwarding=true
```

> **memory:** Maximal zugewiesener Arbeitsspeicher für WSL  
> **processors:** Maximale Anzahl verwendeter CPU-Kerne  
> **swap:** Größe des innerhalb von WSL erzeugten virtuellen Speichers  
> **localhostForwarding:** Erforderlich für das Port-Forwarding während der Entwicklung

Nachdem Sie diese Einstellungen vorgenommen haben, starten Sie WSL neu, um die Änderungen zu übernehmen.

> Hinweis: Wenn Ihr System weniger RAM hat, können Sie `memory=8GB` und `swap=48GB` festlegen.

## Schwarzer-Bildschirm-Problem
Eines der häufigsten Probleme beim Erstellen eines Yocto-Images für den Raspberry Pi 4 ist der schwarze Bildschirm.  
Um den schwarzen Bildschirm zu diagnostizieren, führen Sie die folgenden Überprüfungen der Reihe nach durch:

### Hintergrundbeleuchtung prüfen

Wenn Sie hinter den Bildschirm schauen – leuchten weiße Hintergrundbeleuchtungs-LEDs?  
Auch wenn auf dem Bildschirm nichts angezeigt wird, sollte die Hintergrundbeleuchtung bei Stromversorgung eingeschaltet sein.  
Wenn sie aus ist, können folgende Ursachen vorliegen:

- Die Stromverbindungen zum Bildschirm sind getrennt.  
- Die Stromverbindungen sind verpolt.  
- Der Bildschirm ist defekt.

Wenn die Hintergrundbeleuchtung aus ist, prüfen Sie unbedingt die physischen Verbindungen und stellen Sie sicher, dass sie korrekt angeschlossen sind.  
Wenn Sie die Pins falsch verbunden haben, kann es sein, dass der Bildschirm unbrauchbar wurde – auch wenn Sie die Verdrahtung später korrigieren.

### Ist das Framebuffer-Gerät (fbdev) vorhanden?

In Yocto-basierten Embedded-Systemen muss die Framebuffer-Infrastruktur (fbdev) ordnungsgemäß funktionieren, damit grafische Oberflächen auf dem Bildschirm erscheinen.  
Der Framebuffer ist eine Low-Level-Schnittstelle, über die das Betriebssystem Grafikdaten an die Hardware überträgt.  
Wenn das Framebuffer-Gerät (`/dev/fb0`) im System nicht vorhanden ist, kann der Bildschirm keine Bilder anzeigen.  
Dies liegt meist an Kernel-Konfigurationen, fehlerhaften Device-Tree-Einstellungen oder fehlenden Overlays.  
Prüfen Sie daher Folgendes:


### Funktioniert der Framebuffer?

Um zu überprüfen, ob der Framebuffer tatsächlich funktioniert, können wir zufällige Pixel auf den Bildschirm schreiben.  
Führen Sie dazu den folgenden Befehl im Terminal aus:


```bash
cat /dev/urandom > /dev/fb0
```
Wenn keine Fehler erscheinen und Sie Rauschen oder zufällige Pixel auf dem Bildschirm sehen, funktioniert der Framebuffer korrekt.  
In diesem Fall liegt das Problem möglicherweise darin, dass die Konsole nicht aktiviert ist oder auf dem falschen virtuellen Terminal läuft.

Versuchen Sie nacheinander die folgenden Befehle, um zwischen den Konsolen zu wechseln:

```bash
chvt 1
```

```bash
chvt 2
```

```bash
chvt 3
```

Wenn die Konsole weiterhin nicht angezeigt wird, stellen Sie sicher, dass der Yocto-Image-Erstellungsprozess korrekt durchgeführt wurde.

### Existiert das Framebuffer-Gerät?

Stellen Sie eine Verbindung über UART her und führen Sie folgenden Befehl im Terminal aus:

```bash
ls /dev/fb*
```

Wenn Sie die Ausgabe „No such file or directory“ erhalten, bedeutet das, dass der Framebuffer nicht existiert.  
Wenn überhaupt keine Ausgabe erscheint, wird der Framebuffer nicht korrekt geladen.

Viele Framebuffer-Probleme entstehen durch fehlende Konfigurationen in der Datei **config.txt**.  
Stellen Sie sicher, dass die Zeile **dtoverlay=vc4-fkms-v3d** in Ihrer **config.txt** vorhanden ist.  
Wenn das Problem weiterhin besteht, entfernen Sie diese Zeile und fügen Sie stattdessen **dtoverlay=vc4-kms-v3d** hinzu.

> Hinweis: Achten Sie darauf, dass nicht beide Zeilen gleichzeitig vorhanden sind.

#### Sind HDMI und DSI gleichzeitig angeschlossen?
Wenn sowohl HDMI- als auch DSI-Bildschirme gleichzeitig an einem Raspberry Pi 4 angeschlossen sind, kann es zu Konflikten bei der Anzeigepriorität kommen, und der Bildschirm bleibt vollständig schwarz.  
Das System versucht standardmäßig, das Videosignal über HDMI auszugeben, während der DSI-Bildschirm inaktiv bleibt.

- Prüfen Sie, ob ein HDMI-Bildschirm physisch angeschlossen ist, und trennen Sie ihn gegebenenfalls.  
- Wenn Sie die HDMI-Verbindung nicht entfernen können oder das Problem weiterhin besteht, fügen Sie die folgende Zeile zur **config.txt** hinzu, um die HDMI-Ausgabe softwareseitig zu deaktivieren:


```bash
hdmi_blanking=2
```

## LVGL-Anwendung startet, erscheint aber nicht auf dem Bildschirm

Wenn Sie ein stabiles Bild auf dem Bildschirm sehen, aber Ihre LVGL-Anwendung nicht angezeigt wird, kann dies mehrere Ursachen haben.

### Desktop-Umgebung läuft

Wenn ein X11-, Wayland- oder eine andere Desktop-Umgebung (z. B. Weston, GNOME, LXDE) auf dem System läuft, wird der Zugriff auf den Framebuffer vom Desktop-Manager blockiert.  
Die Anwendung scheint im Hintergrund zu laufen, zeigt aber nichts auf dem Bildschirm an.  
Wenn eine Desktop-Umgebung aktiv ist, müssen Sie diese deaktivieren. Verwenden Sie dazu die folgenden Befehle:


```bash
systemctl disable weston
systemctl disable xserver
systemctl disable graphical.target
```

> Wenn Sie eine ungewöhnliche Desktop-Umgebung verwenden, stoppen Sie sie mit den entsprechenden Deaktivierungsbefehlen.

Führen Sie zum Testen folgenden Befehl aus:

```bash
cat /dev/urandom > /dev/fb0
```

Wenn Sie Rauschen auf dem Bildschirm sehen, ist der Framebuffer nun freigegeben und einsatzbereit.

### Ist Ihre LVGL-Konfiguration mit dem Framebuffer kompatibel?

Wenn auf Ihrem System keine Desktop-Umgebung aktiv ist, können Sie keine Anzeige-Backends wie SDL oder DRM verwenden, da diese eine Desktop-Umgebung voraussetzen.  
Wenn Ihre LVGL-Anwendung ein anderes Backend als den Framebuffer verwendet und keine Desktop-Umgebung vorhanden ist, werden keine Bilder angezeigt.

- Stellen Sie in der Datei **lv_conf.h** sicher, dass der Framebuffer aktiviert ist und alle anderen Anzeige-Backends deaktiviert sind:

```bash
#define LV_USE_FBDEV 1
```
Stellen Sie sicher, dass diese Zeile vorhanden ist und auf 1 gesetzt ist.

```bash
#define LV_USE_SDL 0
#define LV_USE_DRM 0
```
Stellen Sie sicher, dass diese Zeilen die anderen Anzeige-Backends auf 0 setzen.

Stellen Sie in Ihrer Hauptdatei der LVGL-Anwendung sicher, dass das Display wie folgt erstellt wird:

```bash
  lv_display_t * disp = lv_linux_fbdev_create();  
  lv_linux_fbdev_set_file(disp, "/dev/fb0");
```
> Hinweis: In der Regel wird `/dev/fb0` standardmäßig verwendet, aber in manchen Fällen kann der DSI-Bildschirm als `fb1` zugewiesen sein – in diesem Fall müssen Sie `/dev/fb1` angeben.

## Touchscreen funktioniert nicht

Touchscreen-Probleme haben in der Regel zwei Hauptursachen: physikalische Verbindungsfehler und fehlende Treiber.  
Überprüfen Sie zunächst, ob die physischen Verbindungen korrekt sind, und stellen Sie dann sicher, dass Treiber und Softwarekomponenten ordnungsgemäß installiert wurden.

### Physikalische Verbindungsfehler

- Die in unserem Beispiel verwendeten Waveshare-Bildschirme (und viele andere Touchscreens) übertragen Touch-Daten über I2C-Leitungen.  
  Stellen Sie sicher, dass die I2C-Pins des Bildschirms mit den richtigen Pins des Raspberry Pi (oder Ihres Geräts) verbunden sind.
- Verbindungen können sich im Laufe der Zeit lockern. Trennen Sie alle Kabel und schließen Sie sie erneut fest an, um eine stabile Verbindung sicherzustellen.
- Wenn Ihr Bildschirm keine I2C- oder SPI-Pins hat, werden die Touch-Daten wahrscheinlich über das Displaykabel selbst übertragen.  
  In diesem Fall stellen Sie sicher, dass das Displaykabel unbeschädigt und fest angeschlossen ist.

### Fehlender LVGL-Treiber

Um Touch-Unterstützung in LVGL-Anwendungen zu nutzen, muss der EVDEV-Treiber korrekt konfiguriert sein.  
Überprüfen Sie die Hauptdatei Ihres Projekts auf die folgenden Zeilen:


```bash
#include "lvgl/src/drivers/evdev/lv_evdev.h"
```
Stellen Sie sicher, dass diese Zeile enthalten ist.

```bash
lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/touchscreen0");
```

Stellen Sie sicher, dass diese Zeile vorhanden ist und korrekt deklariert wurde.  
Sie können den folgenden Debug-Code direkt darunter einfügen, um bei der Fehlersuche zu helfen:

```bash
 if(indev == NULL) {
        printf("Touch input error!\n");
    }
```