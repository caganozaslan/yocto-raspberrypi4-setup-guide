# Schritt-für-Schritt Yocto-Setup für Raspberry Pi 4

Dieses Dokument bietet eine detaillierte Schritt-für-Schritt-Anleitung zur Erstellung eines benutzerdefinierten Betriebssystem-Images mit dem Yocto-Projekt für ein Embedded-Linux-System auf einem Raspberry Pi 4. Das entwickelte System ist so konfiguriert, dass LVGL (Light and Versatile Graphics Library) für die Unterstützung der grafischen Benutzeroberfläche, SSH- und Wi-Fi-Konnektivität für den Netzwerkzugang, Touchscreen-Unterstützung und grundlegende Terminal-basierte Werkzeuge enthalten sind. Alle Schritte werden in einer WSL (Windows Subsystem for Linux)-Umgebung mit installiertem Ubuntu durchgeführt. Der Prozess umfasst außerdem das Kompilieren und Ausführen von Anwendungen auf der Zielhardware mithilfe des Yocto SDK.

## Arbeitsumgebung

Um dieses Dokument korrekt zu befolgen, müssen Sie dieselbe Arbeitsumgebung verwenden und die notwendigen Anforderungen erfüllen.

- Betriebssystem: Windows 10/11  
- WSL: WSL2  
- Hardware: Raspberry Pi 4B  
- Linux-Distribution: Ubuntu 20.04  
- Display: Waveshare 7-Zoll kapazitives Touch-Display DSI LCD (800x480)

## Anforderungen

- 16 GB oder mehr RAM  
- 150 GB oder mehr freier Speicherplatz  
- Mindestens 8 GB RAM für WSL zugewiesen  
- Mindestens 32 GB SWAP-Speicher für WSL zugewiesen  
- UART-zu-USB Konverter  
- SD-Karte (mindestens 8 GB, 16 GB oder mehr empfohlen)  
- SD-zu-USB Adapter

### Überprüfung von Arbeitsspeicher und SWAP-Speicher

Um zu sehen, wie viel RAM und SWAP-Speicher WSL zur Verfügung hat, geben Sie im WSL-Terminal den folgenden Befehl ein:

**free -h**

Wenn die ausgegebenen Werte unter den erforderlichen Grenzwerten liegen, können Sie den folgenden Anweisungen folgen, um die Einstellungen anzupassen.

#### Konfiguration von Arbeitsspeicher und SWAP-Speicher

Die WSL-Konfigurationsdatei befindet sich auf der Windows-Seite, in Ihrem Benutzerordner:

```bash
C:\Users\<IHR_BENUTZERNAME>\.wslconfig
```
Falls die Datei nicht existiert, erstellen Sie sie mit dem Editor (Notepad) und setzen Sie den Inhalt wie folgt:
```bash
[wsl2]
memory=12GB
processors=4
swap=32GB
localhostForwarding=true
```
> **memory:** Maximale Menge an RAM, die WSL zugewiesen wird  
> **processors:** Maximale Anzahl der zu verwendenden CPU-Kerne  
> **swap:** Größe des virtuellen Speichers, der innerhalb von WSL erstellt wird  
> **localhostForwarding:** Erforderlich für die Weiterleitung von Localhost-Ports während der Entwicklung

Nach Anwendung dieser Einstellungen starten Sie WSL neu, damit die aktualisierte Konfiguration wirksam wird.



## Einrichtung des Yocto-Projekts und Build-Prozess des Images

An diesem Punkt wird der Prozess zur Einrichtung einer Yocto-Umgebung von Grund auf für den Raspberry Pi 4 Schritt für Schritt dokumentiert. Dies umfasst, wie die benötigten Meta-Layer hinzugefügt, die notwendigen Dateien konfiguriert und schließlich das System erfolgreich gebaut wird, um eine `.wic`-Image-Datei zu erzeugen.

Beginnen wir damit, die Pakete, die wir in unserem Projekt verwenden werden, in der WSL-Umgebung zu installieren.


```bash
sudo apt update && sudo apt install -y \
    gawk wget git-core diffstat unzip texinfo \
    gcc build-essential chrpath socat cpio python3 \
    python3-pip python3-pexpect xz-utils debianutils \
    iputils-ping libsdl2-dev xterm zstd libssl-dev \
    locales tmux libncurses5 libncurses5-dev libncursesw5-dev \
    libtinfo5 libtinfo-dev libpixman-1-dev libwayland-dev \
    libdrm-dev libxkbcommon-dev libinput-dev \
    bzip2 curl
```
Die aufgeführten Pakete erfüllen alle Anforderungen unseres Projekts. Zusätzlich wurden gebräuchliche Pakete für WSL-Umgebungen aufgenommen, um potenzielle zukünftige Projekte zu unterstützen.

### Erstellen des Yocto-Arbeitsverzeichnisses

Wir erstellen nun das Verzeichnis, in dem unser Yocto-Projekt entwickelt wird. Das Arbeiten unter einer festen Verzeichnisstruktur ist wichtig, um die Projektorganisation zu gewährleisten. Um dieses Dokument leichter folgen zu können, können Sie das Verzeichnis mit dem gleichen Namen anlegen. Wenn Sie einen anderen Namen bevorzugen, passen Sie die entsprechenden Abschnitte bitte entsprechend an.

```bash
mkdir -p ~/yocto-project-rpi
cd ~/yocto-project-rpi
```

### Herunterladen des Yocto-Kerns und der Layers

In diesem Projekt wird die **Kirkstone**-Version für den Kern und die Layers verwendet. Bitte verwenden Sie dieselbe Version, um diesem Dokument genau folgen zu können.

Herunterladen des Poky-Core-Layers:


```bash
git clone -b kirkstone git://git.yoctoproject.org/poky
```

Alle Layers müssen sich im Verzeichnis des Poky-Kerns befinden.

```bash
cd poky
```

Herunterladen des Raspberry Pi Support-Layers und des OpenEmbedded-Layers für zusätzliche Pakete:

```bash
git clone -b kirkstone https://github.com/agherzan/meta-raspberrypi.git
git clone -b kirkstone https://github.com/openembedded/meta-openembedded.git
```

So aktivieren Sie BitBake:

```bash
source oe-init-build-env
```
Um die Layers in Yocto einzubinden, führen Sie die folgenden Befehle der Reihe nach aus:

```bash
bitbake-layers add-layer ../meta-raspberrypi
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia
```
Um sicherzustellen, dass alle Layers korrekt hinzugefügt wurden, können Sie den folgenden Befehl ausführen:

```bash
bitbake-layers show-layers
```

Sie sollten eine Ausgabe ähnlich der folgenden in der Layer-Liste sehen. (Die Reihenfolge kann variieren.)

```
layer                 path                                                             priority
=========================================================================================================
meta                  /home/arda/yocto-project-rpi/poky/meta                                   5
meta-poky             /home/arda/yocto-project-rpi/poky/meta-poky                              5
meta-yocto-bsp        /home/arda/yocto-project-rpi/poky/meta-yocto-bsp                         5
meta-oe               /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-oe              5
meta-python           /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-python          5
meta-networking       /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-networking      5
meta-multimedia       /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-multimedia      5
meta-raspberrypi      /home/arda/yocto-project-rpi/poky/meta-raspberrypi                        9
```

### Konfigurieren der Yocto-Build-Einstellungen

Wir haben alle notwendigen Layers für unser Projekt in Yocto eingebunden, aber um eine optimierte Linux-Distribution zu erstellen, werden wir nur die Werkzeuge, die wir benötigen, aus diesen Layers in unser Image aufnehmen.

Um die `local.conf`-Einstellungen zu konfigurieren:

(Vorausgesetzt, Sie befinden sich im Verzeichnis `~/yocto-project-rpi/poky`.)

```bash
nano build/conf/local.conf
```

Um das Zielgerät festzulegen, suchen Sie die Zeile **MACHINE ??= "qemux86-64"** in der Datei und ersetzen Sie sie durch Folgendes:


```bash
MACHINE = "raspberrypi4-64"
```

Um den GPU-Speicher zu konfigurieren und UART-Unterstützung zu aktivieren, fügen Sie die folgenden Zeilen am Ende der Datei hinzu:

```bash
ENABLE_UART = "1"
GPU_MEM = "128"
```
Diese Zeilen aktivieren den UART-Seriellport und weisen der GPU 128 MB Videospeicher zu. Dies wird für Touchscreen-Displays und HDMI-Ausgabe empfohlen.

Für die Unterstützung von DSI-Displays fügen Sie die folgende Zeile am Ende der Datei hinzu:

```bash
RPI_EXTRA_CONFIG += " \
dtoverlay=vc4-fkms-v3d \
"
```

Um den SSH-Server, Wi-Fi-Verbindungstools, Texteditor (nano), Systembefehle (htop, wget, curl) usw. zu aktivieren, fügen Sie die folgenden Zeilen am Ende der Datei hinzu:

```bash
IMAGE_INSTALL:append = " \
    linux-firmware-bcm43455 \
    linux-firmware \
    iw wpa-supplicant openssh nano bash htop \
    coreutils util-linux libstdc++ libstdc++-dev \
    libgcc wget curl iproute2 iputils net-tools \
"
```

Um erweiterte Funktionen zu aktivieren, fügen Sie die folgenden Zeilen am Ende der Datei hinzu:

```bash
IMAGE_FEATURES:append = " \
    package-management \
    ssh-server-openssh \
    tools-debug \
"
```

Wenn Ihr Projekt eine grafische Benutzeroberfläche enthält, fügen Sie die folgenden Zeilen für LVGL, Framebuffer, Touchscreen und Testwerkzeuge am Ende der Datei hinzu:

```bash
EXTRA_IMAGE_INSTALL:append = " \
    lvgl \
    lv-drivers \
    lv-lib-png \
    libsdl2 \
    libevdev \
    tslib \
    fbset \
    evtest \
    nano gdb strace procps coreutils less file which \
"
```

Wenn Sie Ihre LVGL-Anwendungen mit dem SDK bauen möchten, können Sie die notwendigen Pakete für die LVGL-Anwendungskompilierung in das SDK aufnehmen, indem Sie die folgenden Zeilen hinzufügen:

```bash
TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-cmake \
    nativesdk-pkgconfig \
    nativesdk-libsdl2 \
"

TOOLCHAIN_TARGET_TASK:append = " \
    libevdev \
"
```

Ändern Sie keinen anderen Teil der Datei außer den angegebenen Zeilen. Nachdem Sie die Bearbeitung abgeschlossen haben, speichern Sie die Datei und beenden Sie den Editor (STRG + O » STRG + X).

Nach Abschluss aller Konfigurationen können wir nun die Image-Datei erstellen. Der Build-Prozess kann einige Zeit in Anspruch nehmen, insbesondere beim ersten Build, der in der Regel zwischen 1 und 3 Stunden dauert.


```bash
bitbake core-image-base
```

#### Unterbrechen des Build-Prozesses

Falls Sie den BitBake-Build-Prozess absichtlich oder unbeabsichtigt stoppen müssen, können Sie den Build dort fortsetzen, wo er unterbrochen wurde. Vor dem Neustart des Builds können Sie den Cache löschen, um mögliche Fehler zu beseitigen. Um den Build neu zu starten:

Öffnen Sie WSL erneut und aktivieren Sie BitBake:

```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```

Temporäre Dateien und Caches bereinigen:

```bash
bitbake -c cleansstate core-image-base
```

Starten Sie den Neuaufbau des Images:

```bash
bitbake core-image-base
```

### Schreiben des Images auf die SD-Karte

Nach Abschluss des Builds befindet sich die erzeugte `.wic.bz2`-Datei im Verzeichnis **tmp/deploy/images/raspberrypi4-64/**.

Um die Datei zu entpacken:

```bash
cd tmp/deploy/images/raspberrypi4-64/
bunzip2 -f core-image-base-raspberrypi4-64.wic.bz2
```

Nach etwa 30 Sekunden wird die `.wic`-Datei entpackt sein. Um das Verzeichnis mit dem Image in Windows einfach zu öffnen:

```bash
explorer.exe .
```

To write the `.wic` image using balenaEtcher, follow these steps:

1. Download and install [balenaEtcher](https://www.balena.io/etcher/) on your computer.  
2. Launch the application.  
3. Click on "Flash from file" and select the previously extracted `.wic` image file.  
4. Click on "Select target" and choose your SD card.  
5. Click on "Flash!" to start the writing process.  
6. Once the writing is complete, Etcher will automatically perform a verification. When you see the "Flash Complete!" message, the process is finished.


## config.txt-Einstellungen

Die `config.txt` ist eine grundlegende Konfigurationsdatei, die die Hardwareeinstellungen des Raspberry Pi während des Bootvorgangs definiert. Diese Datei legt Optionen wie GPU-Speicher, Bildschirmauflösung, Videoausgabe, Overlays (Treibermodule) und andere Hardwareparameter fest. Von Yocto erzeugte Images enthalten diese Datei, aber bei Verwendung benutzerdefinierter Hardware (z. B. DSI-Displays, Touchpanels oder zusätzlicher Sensoren) sind manuelle Anpassungen möglicherweise erforderlich.

In unserem Projekt sind Anpassungen für Anzeige- und Touchscreen-Einstellungen notwendig. Wenn Sie Ihre SD-Karte unter Windows öffnen, können Sie auf das **boot**-Verzeichnis zugreifen und dessen Inhalt bearbeiten. Suchen Sie die Datei **config.txt** im **boot**-Ordner und:

- Um UART auf dem Raspberry Pi 4 zu aktivieren, stellen Sie sicher, dass die Zeile `enable_uart=1` in der Datei enthalten ist.  
- Um die Ausgabe über DSI zu aktivieren, muss eine der folgenden Zeilen vorhanden sein: `dtoverlay=vc4-fkms-v3d` oder `dtoverlay=vc4-kms-v3d`.  
  - Wenn Ihre Oberfläche den Framebuffer verwendet, nutzen Sie `dtoverlay=vc4-fkms-v3d`.  
  - Wenn Sie ein modernes GPU-beschleunigtes Qt- oder Wayland-basiertes System verwenden, nutzen Sie `dtoverlay=vc4-kms-v3d`.  
  - **Verwenden Sie niemals beide Zeilen gleichzeitig!**  
- Um die Touchscreen-Funktionalität zu aktivieren, suchen Sie die Zeile `dtparam=i2c_arm` und ändern Sie sie in `dtparam=i2c_arm=on`.  
  - Fügen Sie anschließend folgende Zeile am Ende der Datei hinzu: `dtoverlay=rpi-ft5406`

Speichern und schließen Sie die Datei. Ihre SD-Karte ist nun bereit, in das Raspberry-Pi-Gerät eingelegt zu werden.

> ⚠️ **WARNUNG:**  
> Bevor Sie die SD-Karte aus dem Computer entfernen, führen Sie immer die Funktion **„Speichermedium sicher entfernen“** aus.  
> Andernfalls kann das Image beschädigt werden und das System möglicherweise nicht starten.

## Raspberry Pi 4 Konfiguration

### Einrichtung einer UART-Verbindung auf dem Raspberry Pi 4

Um direkten seriellen Terminalzugriff auf den Raspberry Pi 4 herzustellen, können Sie einen USB-zu-UART-Konverter verwenden. Diese Methode ermöglicht die direkte Kommunikation mit dem Gerät über ein Terminal, selbst wenn kein Display oder keine Netzwerkverbindung verfügbar ist.


---

#### 1. Hardware verbinden

- Verbinden Sie Ihren USB-zu-UART-Konverter wie folgt:  
  - **Konverter TX** → Raspberry Pi **RX**-Pin  
  - **Konverter RX** → Raspberry Pi **TX**-Pin  
  - **Konverter GND** → Beliebiger **GND**-Pin am Raspberry Pi

> 📌 **Hinweis:** Das GPIO-Pinlayout des Raspberry Pi 4 finden Sie im [offiziellen GPIO-Diagramm](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-pinout).

---


#### 2. COM-Port auf Ihrem Computer identifizieren

Um herauszufinden, an welchem Port Ihr USB-zu-UART-Konverter erkannt wurde:
  - Klicken Sie mit der rechten Maustaste auf das Startmenü → Öffnen Sie den **Geräte-Manager**
  - Unter „Anschlüsse (COM & LPT)“ finden Sie das Gerät mit der Bezeichnung `"USB Serial Port (COMx)"`
  - Notieren Sie sich die `COMx`-Portnummer

---

#### 3. PuTTY herunterladen und installieren

- Laden Sie [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) für serielle Kommunikation herunter und installieren Sie es  
- Sie können die Standard-Installationsoptionen verwenden

---

#### 4. Über die serielle Schnittstelle mit PuTTY verbinden

- Starten Sie PuTTY  
- Im Tab „Session“:
  - **Verbindungstyp**: *Serial*  
  - **Serielle Schnittstelle**: `COMx` (z. B. `COM3`)  
  - **Geschwindigkeit (Baudrate)**: `115200`  
- Klicken Sie auf die Schaltfläche **Open**

> Wenn das Terminalfenster nach dem Öffnen leer bleibt, drücken Sie die Eingabetaste. Der Anmeldebildschirm sollte erscheinen.

---

#### 5. Als root anmelden

Wenn der Anmeldebildschirm erscheint, melden Sie sich wie folgt an:

```plaintext
raspberrypi login: root
```

Es wird kein Passwort benötigt. In Yocto-basierten Systemen ist der root-Benutzer standardmäßig passwortlos. Aus Sicherheitsgründen können Sie später ein Passwort hinzufügen.

### WLAN-Einrichtung und SSH-Verbindung

Um Internetzugang und Remote-Verbindungen auf dem Raspberry Pi 4 zu ermöglichen, muss WLAN konfiguriert und eine SSH-Verbindung hergestellt werden. Die folgenden Schritte erklären, wie man WLAN manuell konfiguriert und sich über SSH mit einem Yocto-System verbindet.

---

#### 1. Erstellen Sie das wpa_supplicant-Konfigurationsverzeichnis (falls es nicht existiert)


```bash
mkdir -p /etc/wpa_supplicant
```

---

#### 2. Erstellen Sie die WLAN-Konfigurationsdatei

```bash
nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Schreiben Sie den folgenden Inhalt in die Datei:

```
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1

network={
    ssid="IHR WLAN-NAME"
    psk="IHR WLAN-PASSWORT"
}
```

> Aktualisieren Sie die Felder `ssid` und `psk` entsprechend den Informationen Ihres eigenen WLAN-Netzwerks.

---

#### 3. Starten Sie die WLAN-Verbindung

```bash
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf
udhcpc -i wlan0
```
Am Ende dieses Schritts wird die WLAN-Verbindung gestartet und eine IP-Adresse angefordert.  
Wenn an dieser Stelle ein Fehler auftritt, überprüfen Sie bitte die SSID und das Passwort in Ihrer Konfigurationsdatei.

---

#### 4. Überprüfen Sie die IP-Adresse

```bash
ip addr show wlan0
```

Die Ausgabe dieses Befehls zeigt eine IP-Adresse im Format `192.168.1.XX/24`. Dies ist die IP-Adresse Ihres Geräts.

---

#### 5. Verbindung über SSH herstellen

Sie können über SSH von einem Computer im selben Netzwerk aus eine Verbindung herstellen.  
Geben Sie dazu den folgenden Befehl im WSL-Terminal ein:

```bash
ssh root@192.168.1.x
```

> In Yocto-Systemen kann sich der Benutzer `root` ohne Passwort anmelden.

Wenn Ihr WSL-Terminal **root@raspberrypi4-64:~#** anzeigt, wurde die SSH-Verbindung erfolgreich hergestellt.  
Sie können die Verbindung testen, indem Sie verschiedene Befehle ausprobieren.

## SDK-Erstellung und Anwendungs-Kompilierung

Mit dem von Yocto erzeugten SDK (Software Development Kit) können Sie Anwendungen für Zielgeräte wie den Raspberry Pi 4 ganz einfach auf Ihrem Host-System (z. B. Ubuntu unter WSL) kompilieren.  
Das SDK enthält den Compiler, Bibliotheken und alle Entwicklungstools, die mit dem Zielsystem kompatibel sind.  
So können Sie Ihre Anwendung auf Ihrem Computer kompilieren, anstatt direkt auf dem Zielgerät, und ausführbare Binärdateien erzeugen.

In diesem Abschnitt erfahren Sie Schritt für Schritt, wie Sie das SDK installieren und aktivieren.

Aktivieren Sie zunächst BitBake:


```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```
Starten Sie den SDK-Build-Prozess mit BitBake:

```bash
bitbake core-image-base -c populate_sdk
```

> Hinweis: Sie sollten den SDK-Build-Befehl entsprechend dem Image-Typ ausführen, den Sie beim Erstellen des Yocto-Images verwendet haben.  
> Zum Beispiel: `bitbake core-image-sato -c populate_sdk`

Der SDK-Erstellungsprozess wird einige Zeit in Anspruch nehmen.  
Schließen Sie das Terminal nicht – warten Sie, bis der Vorgang abgeschlossen ist.

Navigieren Sie anschließend zu dem Verzeichnis, in dem das SDK generiert wurde:


```bash
cd tmp/deploy/sdk/
ls
```

Wenn das SDK erfolgreich generiert wurde, sehen Sie eine `.sh`-Datei im Verzeichnis.  
Für unser Projekt lautet der Name:  
`poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh`  
Dies ist die SDK-Installationsdatei. Sie können diese Datei ausführen, um die Installation abzuschließen.


```bash
chmod +x poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
./poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
```

Während der Installation werden Sie aufgefordert, das Installationsverzeichnis des SDK anzugeben.  
Lassen Sie das Feld leer, um das Standardverzeichnis zu verwenden. Wenn Sie einen benutzerdefinierten Pfad möchten, geben Sie den gewünschten Ordnernamen ein.  
Standardmäßig wird es in folgendes Verzeichnis installiert: `/opt/poky/4.0.26/`

Nach Abschluss der Installation, um das SDK zu verwenden:


```bash
source /opt/poky/4.0.26/environment-setup-cortexa72-poky-linux
```
geben Sie den Befehl ein. Die notwendigen Include-Pfade für die Kompilierung werden nun gesetzt, und Sie können Anwendungen erstellen, die mit Ihrem Image kompatibel sind.  
Um zu überprüfen, ob das SDK aktiv ist:


```bash
echo $CC
```
Die Ausgabe sollte in der Form `aarch64-poky-linux-gcc...` erscheinen.  
Wenn Sie dies sehen, wurden alle Schritte erfolgreich abgeschlossen.

> Hinweis: Jedes Mal, wenn Sie WSL neu starten, müssen Sie die SDK-Umgebung erneut aktivieren.  
> Außerdem kann BitBake nicht aktiviert werden, während das SDK aktiv ist.

## Einrichtung der LVGL-Umgebung

Wir werden eine geeignete Umgebung vorbereiten, um LVGL-Anwendungen zu erstellen und zu kompilieren.  
Anschließend können wir durch Anpassung der Konfigurationsdateien unsere eigene Software nach Wunsch kompilieren.

Erstellen Sie zunächst das Verzeichnis, in dem wir unsere Arbeiten durchführen werden:


```bash
mkdir lvgl-project-rpi
cd lvgl-project-rpi
```

Laden wir das LVGL-Linux-Projekt mit der bereits vorbereiteten Basisstruktur herunter:

```bash
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
git submodule update --init --recursive
```

Um zu testen, ob die Umgebung korrekt funktioniert, können Sie die Demo-Anwendung kompilieren, ohne irgendwelche Einstellungen zu ändern:

```bash
cmake -B build -S .
make -C build -j
```

Die Beispielanwendung befindet sich im Verzeichnis **lvgl-project-rpi\lv_port_linux\build\bin**.  
Da diese Anwendung mit Ihrem SDK kompiliert wurde, kann sie nicht unter Windows ausgeführt werden.

## Kompilierung unserer eigenen LVGL-Anwendung

Weitere Informationen zur Verwendung von LVGL sowie Beispielanwendungen finden Sie unter folgenden Quellen:

- 📘 [LVGL Offizielle Dokumentation](https://docs.lvgl.io/latest/en/html/index.html)  
- 💻 [LVGL PC-Simulator (Entwicklung und Test)](https://github.com/lvgl/lv_port_pc_visual_studio)  

Diese Ressourcen helfen Ihnen, die Grundlagen der Widget-Nutzung, des Ereignissystems und der Stilstruktur zu erlernen. Anschließend können Sie Ihre eigene Anwendung entwickeln.

Wir haben unser benutzerdefiniertes Beispiel durch Tests im Windows-Simulator entwickelt.  
Unsere einfache Benutzeroberfläche besteht aus zwei Dateien: **my_app.cpp** und **my_app.h**.  
Um diese Oberfläche zu kompilieren, platzieren Sie Ihre Dateien im `src`-Verzeichnis.

In der für den LVGL-Simulator konfigurierten Datei `main.c` werden standardmäßig die Funktionen  
`lv_demo_widgets()` und `lv_demo_widgets_start_slideshow()` aufgerufen, um die LVGL-Demooberfläche zu starten.  
Um Ihre eigene Oberfläche auszuführen, müssen Sie diese Aufrufe durch Ihre eigene `my_app()`-Funktion ersetzen.

Gehen Sie dazu wie folgt vor:

---

Binden Sie die Datei `my_app.h` ein:

- Fügen Sie in `main.c` die folgende Zeile unterhalb der anderen `#include`-Anweisungen ein:

```c
#include "my_app.h"
```

---

Suchen Sie die folgenden Zeilen und **kommentieren Sie sie aus**:

```c
// lv_demo_widgets();
// lv_demo_widgets_start_slideshow();
```

Fügen Sie stattdessen Ihre eigene Funktion hinzu:

```c
my_app();
```

### lv_conf.h-Konfiguration

Die Datei `lv_conf.h` enthält umfangreiche Einstellungen, um festzulegen, was beim Build einbezogen wird.  
Im Standardzustand sind die meisten Standard-Widgets aktiviert.  
Stellen Sie sicher, dass die von Ihrer Anwendung verwendeten LVGL-Widgets wie folgt definiert sind: `#define LV_USE_CHECKBOX   1`.  
Wenn der Wert 0 ist, ändern Sie ihn auf 1.  
Um den Ressourcenverbrauch zu reduzieren, können Sie nicht verwendete Widgets auf 0 setzen.

### ⚙️ Bearbeiten der Datei CMakeLists.txt

Um Ihre benutzerdefinierte Datei `my_app.c` zu kompilieren, müssen Sie einige kleine, aber wichtige Änderungen an der Datei `CMakeLists.txt` vornehmen.  
Diese Datei teilt dem Build-System mit, welche Quelltexte verwendet werden, wie das Programm heißt und welche Bibliotheken eingebunden werden sollen.

---

Suchen Sie zunächst die folgende Zeile in der Datei `CMakeLists.txt`:



```cmake
add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

Diese Zeile gibt den Namen des zu erstellenden Programms (`lvglsim`) sowie die zu kompilierenden Dateien an.  
Hier ist `src/main.c` Ihre Haupt-Quelldatei. Da Sie Ihre Benutzeroberfläche in `my_app.c` geschrieben haben, sollten Sie auch `src/my_app.c` zu dieser Zeile hinzufügen.

Aktualisieren Sie sie wie folgt:

```cmake
add_executable(lvglsim src/main.c src/my_app.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

> Hinweis: Wenn Ihre Anwendungsdatei `my_app.cpp` heißt, verwenden Sie `.cpp`. Handelt es sich um eine C-Datei, verwenden Sie `.c`.

### Anzeige-Backend-Konfiguration

Um LVGL unter Linux auszuführen, wird ein Anzeige-Backend verwendet, das bestimmt, wie die Grafiken auf dem Bildschirm dargestellt werden. Zum Beispiel:

- Während der Entwicklung und beim Testen wird häufig **SDL** verwendet. Es öffnet ein Anzeigefenster.
- Auf echter Hardware wird typischerweise **Framebuffer (FBDEV)** verwendet, um die Ausgabe direkt auf dem Bildschirm darzustellen.

Welches Backend verwendet wird, ist in der Datei `lv_conf.h` festgelegt.  
Sie sollten nur eine der folgenden Optionen aktivieren:

```c
#define LV_USE_SDL 1
#define LV_USE_LINUX_FBDEV 0
```

In diesem Beispiel ist also **SDL aktiviert** und FBDEV ist **deaktiviert**.

Um zu überprüfen und zu ändern, welches System aktiv ist, öffnen Sie einfach die Datei und ändern Sie die `1`- und `0`-Werte am Anfang der jeweiligen Zeilen.  
Es sollte immer nur ein System gleichzeitig aktiviert sein – mehrere aktive Backends können zu Konflikten führen.

Sobald Sie das Anzeigesystem gewählt haben (z. B. SDL oder FBDEV), wird es in der Datei `main.c` initialisiert.  
Suchen Sie im Code nach der folgenden Zeile:


```c
driver_backends_init_backend(...);
```

Diese Zeile initialisiert automatisch das ausgewählte Backend.  
Die Datei `CMakeLists.txt` enthält bereits die notwendigen Einstellungen, um das ausgewählte Backend in den Build einzubeziehen.  
Sie müssen also lediglich das gewünschte Backend in der Datei `lv_conf.h` auswählen – den Rest übernimmt das System für Sie.

Vergessen Sie nicht, das Projekt neu zu kompilieren, damit Ihre Änderungen wirksam werden:

```bash
cmake -B build -S .
make -C build -j
```

Den kompilierten Output finden Sie im Verzeichnis **build/bin**.  
Für weiterführende Konfigurationsmöglichkeiten können Sie die LVGL-Entwicklerdokumentation konsultieren.

Ihre kompilierte Anwendung ist nun bereit, auf Ihrem Raspberry Pi ausgeführt zu werden.  
Sie können die Datei per SSH auf das Gerät übertragen und dort ausführen.




