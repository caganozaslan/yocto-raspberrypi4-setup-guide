# Yocto Project Setup and Sample LVGL Application Build Guide

This repository is a comprehensive guide to building a fully customized Linux image for the Raspberry Pi 4 platform using the Yocto Project step by step. The created image not only includes basic operating system components but also provides a complete environment for embedded system development with Wi-Fi, SSH, and essential terminal tools (such as nano, cat, curl).

The project aims to create a high-quality graphical interface using the Waveshare 7" DSI LCD display. The image fully integrates Linux’s framebuffer (fbdev) graphics infrastructure and EVDEV touch support, providing all necessary components for developing real-time and interactive user interfaces on embedded devices.

Additionally, the LVGL graphics library used in the project is integrated into the image to develop high-performance, lightweight, and customizable GUI applications on embedded Linux. The guide explains in detail step by step how to compile LVGL-based applications and how to configure framebuffer and touch support.

Moreover, a comprehensive Frequently Asked Questions (FAQ) section has been added to quickly find solutions to common issues encountered in the project.


## Features

- Custom Linux image creation for Raspberry Pi 4 using Yocto Project
- Support for Wi-Fi connectivity with WPA/WPA2 security
- Secure remote access via OpenSSH server
- Essential terminal utilities included (nano, cat, curl, etc.)
- Integration of Waveshare 7" DSI LCD display with framebuffer (fbdev) support
- Full EVDEV touchscreen input support for accurate touch events
- Lightweight and high-performance GUI development using LVGL library
- Cross-compilation SDK for developing and testing embedded applications on host PC
- Detailed step-by-step documentation suitable for beginners and experienced developers
- Troubleshooting guide and comprehensive FAQ section to resolve common issues
- Support for building and flashing SD card images via CLI and GUI tools (balenaEtcher)
- Configurable kernel and device tree overlays for flexible hardware customization
- Designed to work seamlessly under Windows WSL2 with Ubuntu 20.04 environment

## Quick Start
If you don't want to spend time with the step-by-step guide, you can quickly start your project by following the [quick start guide](yocto-rpi4-starter-kit/quick-start.md) designed for intermediate and advanced developers.


## Documentation
You can access the prepared documentation for this project via the following links:


### EN
- [Step by Step Setup Guide](docs/yocto-raspberrypi4-setup.md)
- [FAQ](docs/FAQ.md)
- [LVGL Demo](yocto-rpi4-starter-kit/lvgl-demo/README.md)
- [Quick Start](yocto-rpi4-starter-kit/README.md)
- [Yocto Project Fast Setup](yocto-rpi4-starter-kit/yocto-project-rpi/README.md)
### TR
- [Adım Adım Kurulum Rehberi](docs/TR/yocto-raspberrypi4-setup_TR.md)
- [SSS](docs/TR/FAQ_TR.md)


### DE
- [Schritt-für-Schritt-Anleitung zur Einrichtung](docs/DE/yocto-raspberrypi4-setup_DE.md)
- [FAQ](docs/DE/FAQ_DE.md)