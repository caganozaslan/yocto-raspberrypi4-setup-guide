# LVGL Demo Information Document

This folder contains a sample LVGL project designed to run on a Raspberry Pi 4 device running a custom Linux distribution built with Yocto. This GUI application operates based on Framebuffer and includes EVDEV touch input support. The sample displays a single label with the text "Hello World" on a light-colored background. No functions have been created for touch input interaction.  

The purpose of this application is to provide a working compiled LVGL example, verify that the system is ready to run LVGL applications, and offer easy access to basic LVGL GUI source code. You can use this structure to build and compile your own LVGL applications.

The files **CMakeLists.txt**, **Makefile**, **lv_conf.h**, and **main.cpp** are fully configured to include all necessary settings. The file **demo.cpp** contains only the GUI components. If you wish to continue developing with the current structure, you may keep the basic LVGL logic inside the **main.cpp** file as shown in the example.

## Compiling the LVGL Application

To compile the example LVGL application, the required components must be present in your Yocto SDK and the SDK environment must be activated. For detailed information about SDK setup, please refer to [this guide](../../docs/yocto-raspberrypi4-setup.md).

Once the SDK environment is active, follow the steps below to compile:

To clean up any previous build artifacts:

```bash
rm -rf build 
```

Then run:
```bash
cmake -B build -S .
make -C build -j
```
Your compiled application will be generated as main under the **bin** directory.


> Note: The sample application is already precompiled and available under the bin folder.