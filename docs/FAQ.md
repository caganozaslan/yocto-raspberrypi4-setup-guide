# Frequently Asked Questions
This document contains frequently asked questions and problems encountered during the process of building and running a Yocto image for Raspberry Pi 4.

## Bitbake Build Stuck Issue
During the Yocto build process, the bitbake command may hang for a long time or suddenly freeze and make no progress. This is a common problem especially for users running under WSL (Windows Subsystem for Linux). Usually, system resources run out and the build is interrupted because WSL's default RAM and SWAP settings are insufficient.

### Checking and Adjusting Memory and SWAP Size
To see how much RAM and SWAP space WSL has, enter the command free -h in the WSL terminal. If RAM is below 12 GB and SWAP below 32 GB, you can configure the necessary settings by following the steps below.

The WSL configuration file is located on the Windows side, in your user folder:

```bash
C:\Users\<YOUR_USERNAME>\.wslconfig
```
If the file does not exist, create it with Notepad and set its content as follows:

```bash
[wsl2]
memory=12GB
processors=4
swap=32GB
localhostForwarding=true
```

> **memory:** Maximum RAM allocated to WSL

> **processors:** Maximum number of CPU cores used

> **swap:** Size of virtual memory created inside WSL

> **localhostForwarding:** Required for localhost port forwarding during development

After making these settings, restart WSL to apply the changes.

> Note: If your system has less RAM, you can set memory=8GB and swap=48GB.

## Black Screen Issue
One of the most common issues when creating a Yocto image for Raspberry Pi 4 is the black screen problem. To diagnose the black screen, perform the following checks in order:

### Backlight Check

When you look behind your screen, do you see white backlight LEDs lit? Even if nothing is displayed on the screen, the backlight should turn on when the screen is powered. If the backlight is off, the possible reasons include:

- Power connections to the screen may be disconnected.
- Power connections may be reversed.
- The screen may be defective.

If the backlight is off, definitely check the physical connections and ensure they are connected correctly. If you connected the pins incorrectly, even if you fix the wiring later, the screen may have become unusable.

### Is the Framebuffer (fbdev) Present?

In Yocto-based embedded systems, the framebuffer (fbdev) infrastructure must work properly for graphical interfaces to appear on the screen. The framebuffer is a low-level interface where the operating system passes graphical data to the hardware. If the framebuffer device (/dev/fb0) does not exist on the system, the screen cannot display images. This usually stems from kernel configuration, device tree settings, or missing overlays. Check the following:


### Is the Framebuffer Working?
To check if the framebuffer actually works, we can write random pixels to the screen. Run the following command in the terminal:

```bash
cat /dev/urandom > /dev/fb0
```
If no errors appear and you see noise or random pixels on the screen, the framebuffer works correctly. In this case, the problem might be that the console is not activated or is running on the wrong virtual terminal.

Try these commands one by one to switch console screens:

```bash
chvt 1
```

```bash
chvt 2
```

```bash
chvt 3
```

If the console still does not appear, make sure your Yocto image creation process was done correctly.

### Does the Framebuffer Device Exist?

Connect via UART and run the following command in the terminal:

```bash
ls /dev/fb*
```

If you get the output No such file or directory, it means the framebuffer does not exist. If there is no output at all, it means the framebuffer is not loading correctly.

Many framebuffer errors stem from missing configurations in the **config.txt** file. Make sure the line **dtoverlay=vc4-fkms-v3d** exists in your **config.txt**. If the problem persists, try removing that line and adding **dtoverlay=vc4-kms-v3d** instead.

> Note: Ensure that both lines do not appear simultaneously.


#### Are HDMI and DSI Connected Simultaneously?
If both HDMI and DSI screens are connected on a Raspberry Pi 4 at the same time, the display priority may conflict, and the screen can stay completely black. The system by default tries to send video through HDMI while the DSI screen remains inactive.

- Physically check if the HDMI screen is connected, and if so, disconnect it.
- If you cannot remove the HDMI connection or the problem persists, add the following line to **config.txt** to disable HDMI output in software:

```bash
hdmi_blanking=2
```

## LVGL Application Starts but Does Not Appear on Screen
If you can get a stable image on the screen but your LVGL application does not show up, the problem can have several causes.

### Desktop Environment Running
If an X11, Wayland, or any desktop environment (e.g., Weston, GNOME, LXDE) is running on the system, framebuffer access is locked by the desktop manager. The application may appear to be running in the background but shows nothing on the screen. If a desktop environment is running, you must disable it. Use the commands below:

```bash
systemctl disable weston
systemctl disable xserver
systemctl disable graphical.target
```

> If you use an uncommon desktop environment, stop it by running its respective disable commands.

For testing, run:

```bash
cat /dev/urandom > /dev/fb0
```

If you see noise on the screen, the framebuffer is now free and ready to use.

### Is Your LVGL Configuration Compatible with Framebuffer?

If no desktop environment is active on your system, you cannot use display backends such as SDL or DRM, which require a desktop. If your LVGL application uses a display backend other than framebuffer and you have no desktop environment, it will not show images.

- Make sure in **lv_conf.h** that framebuffer is enabled and all other display backends are disabled:

```bash
#define LV_USE_FBDEV 1
```
Ensure this line is present and set to 1.

```bash
#define LV_USE_SDL 0
#define LV_USE_DRM 0
```
Ensure these lines set other display backends to 0.

In your LVGL main file, make sure the display is created as follows:

```bash
  lv_display_t * disp = lv_linux_fbdev_create();  
  lv_linux_fbdev_set_file(disp, "/dev/fb0");
```
> Note: Usually /dev/fb0 is used by default, but in some cases, the DSI screen might be assigned fb1, so you need to set /dev/fb1 in that case.


## Touchscreen Not Working
Touchscreen issues usually come from two main causes: physical connection errors and missing drivers. First, check that physical connections are correct, then verify that drivers and software components are properly installed.

### Physical Connection Errors

- The Waveshare screens used in our example and many other touchscreens transmit touch data via I2C lines. Ensure that the screen’s I2C pins are connected to the correct pins on the Raspberry Pi (or your device).
- Connections can loosen over time. Disconnect and reconnect firmly to ensure a stable physical connection.
- If your screen lacks I2C or SPI pins, it probably sends touch data through the display cable itself. In this case, ensure the display cable is undamaged and tightly connected.

### Missing LVGL Driver

To use touch support in LVGL applications, the EVDEV driver must be correctly configured. Check your project’s main file for the following:

```bash
#include "lvgl/src/drivers/evdev/lv_evdev.h"
```
Ensure this line is included.

```bash
lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/touchscreen0");
```

Ensure this line is present and declared correctly. You can add the following debug code right below to help troubleshoot:

```bash
 if(indev == NULL) {
        printf("Touch input error!\n");
    }
```