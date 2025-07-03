# Quick Start Guide  
This quick start guide will help you build the Yocto image, configure your Raspberry Pi 4, and run the sample LVGL GUI application.  

## Build and Flash the Yocto Image

To build the custom Linux image for Raspberry Pi 4, follow the instructions in the guide below:

[Yocto Setup Guide](../../docs/yocto-raspberrypi4-setup.md)

Once the image is ready (`core-image-base-raspberrypi4-64.wic.bz2`), flash it to an SD card using **balenaEtcher** or `dd`.

After inserting the SD card into your Raspberry Pi and powering it on, you can try basic commands like:

```bash
ifconfig   
ls /etc
free -h   
```

## Run or Build the LVGL Demo
To test the GUI layer, navigate to the LVGL demo documentation:

[this document](../lvgl-demo/README.md).
You can either:

- Use the precompiled binary located in lvgl-demo/bin/
- Or compile the source using the Yocto SDK environment

Once ready, copy the binary to your device and run:

```bash
chmod +x main
./main
```
You should see a "Hello, World!" label on a gray background.



Your system is now ready to run embedded GUI applications.
For configuration details (touchscreen, display overlays), SDK setup, and troubleshooting, please refer to the [full documentation](../../docs/yocto-raspberrypi4-setup.md).