# Step-by-Step Yocto Setup for Raspberry Pi 4

This document provides a detailed, step-by-step guide to creating a custom operating system image using the Yocto Project for an embedded Linux system running on a Raspberry Pi 4. The developed system is configured to include LVGL (Light and Versatile Graphics Library) for graphical interface support, SSH and Wi-Fi connectivity for network access, touchscreen support, and basic terminal-based tools. All steps are executed in a WSL (Windows Subsystem for Linux) environment with Ubuntu installed. The process also covers compiling and running applications on the target hardware using the Yocto SDK.

## Work Environment

To follow this document correctly, you must use the same working environment and meet the necessary requirements.

- Operating System: Windows 10/11  
- WSL: WSL2  
- Hardware: Raspberry Pi 4B  
- Linux Distribution: Ubuntu 20.04  
- Display: Waveshare 7inch Capacitive Touch Display DSI LCD (800x480)

## Requirements

- 16 GB or more RAM  
- 150 GB or more free disk space  
- At least 8 GB of RAM allocated for WSL  
- At least 32 GB of SWAP space allocated for WSL  
- UART to USB converter  
- SD Card (minimum 8 GB, 16 GB or more recommended)  
- SD to USB adapter

### Memory and SWAP Space Check

To see how much RAM and SWAP space WSL has, enter the following command in the WSL terminal:

**free -h**

If the output values are below the required thresholds, you can follow the instructions below to adjust the settings.

#### Configuring Memory and SWAP Space

The WSL configuration file is located on the Windows side, in your user folder:

```bash
C:\Users\<YOUR_USERNAME>\.wslconfig
```
If the file does not exist, create it using Notepad and set its contents as follows:
```bash
[wsl2]
memory=12GB
processors=4
swap=32GB
localhostForwarding=true
```
> **memory:** Maximum amount of RAM to allocate to WSL  
> **processors:** Maximum number of CPU cores to use  
> **swap:** Size of the virtual memory to be created inside WSL  
> **localhostForwarding:** Required for localhost port forwarding during development

After applying these settings, restart WSL to apply the updated configuration.



## Yocto Project Setup and Image Build Process

At this stage, the process of setting up a Yocto environment from scratch for the Raspberry Pi 4 is documented step by step. This includes how to add the required meta layers, configure the necessary files, and finally build the system successfully to generate a `.wic` image file.

Let's start by adding the packages we will use in our project to the WSL environment.


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
The listed packages meet all the requirements of our project. Additionally, commonly used packages for WSL environments have been included for potential future projects.

### Creating the Yocto Working Directory

We will now create the directory in which our Yocto project will be developed. Working under a fixed directory structure is important for maintaining project organization. To follow this document more easily, you can create the directory with the same name. If you prefer to use a different name, feel free to adjust the relevant sections accordingly.

```bash
mkdir -p ~/yocto-project-rpi
cd ~/yocto-project-rpi
```

### Downloading the Yocto Core and Layers

In this project, the **Kirkstone** release is used for the core and layers. Please use the same version to follow this document accurately.

Downloading the Poky core layer:


```bash
git clone -b kirkstone git://git.yoctoproject.org/poky
```

All layers must be located under the Poky core directory.

```bash
cd poky
```

Downloading the Raspberry Pi support layer and the OpenEmbedded layer for additional packages:

```bash
git clone -b kirkstone https://github.com/agherzan/meta-raspberrypi.git
git clone -b kirkstone https://github.com/openembedded/meta-openembedded.git
```

To activate BitBake:

```bash
source oe-init-build-env
```
To include the layers in Yocto, execute the following commands in order:

```bash
bitbake-layers add-layer ../meta-raspberrypi
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia
```
To ensure all layers have been added correctly, you can run the following command:

```bash
bitbake-layers show-layers
```

You should see an output similar to this in the layer list. (The order may vary.)

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

### Configuring Yocto Build Settings

We have included all the necessary layers in Yocto for our project, but to create an optimized Linux distribution, we will include only the tools we need from those layers into our image.

To configure the `local.conf` settings:

(Assumes you are in the `~/yocto-project-rpi/poky` directory.)

```bash
nano build/conf/local.conf
```

To set the target device, locate the line **MACHINE ??= "qemux86-64"** in the file and replace it with the following:


```bash
MACHINE = "raspberrypi4-64"
```

To configure the GPU memory and enable UART support, add the following lines to the end of the file:

```bash
ENABLE_UART = "1"
GPU_MEM = "128"
```
These lines enable the UART serial port and allocate 128MB of video memory to the GPU. This is recommended for touchscreen displays and HDMI output.

For DSI display support, add the following line to the end of the file:
```bash
RPI_EXTRA_CONFIG += " \
dtoverlay=vc4-fkms-v3d \
"
```

To enable SSH server, Wi-Fi connection tools, text editor (nano), system commands (htop, wget, curl), etc., add the following lines at the end of the file:

```bash
IMAGE_INSTALL:append = " \
    linux-firmware-bcm43455 \
    linux-firmware \
    iw wpa-supplicant openssh nano bash htop \
    coreutils util-linux libstdc++ libstdc++-dev \
    libgcc wget curl iproute2 iputils net-tools \
"
```

To enable advanced features, add the following lines to the end of the file:

```bash
IMAGE_FEATURES:append = " \
    package-management \
    ssh-server-openssh \
    tools-debug \
"
```

If your project includes a graphical interface, add the following lines to the end of the file for LVGL, Framebuffer, touchscreen, and testing tools:

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

If you plan to build your LVGL applications using the SDK, you can include the necessary packages for LVGL application compilation in the SDK by adding the following lines:

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

Do not modify any part of the file other than the specified lines. Once you're done editing, save and exit. (CTRL + O » CTRL + X)

After completing all configurations, we can now generate the image file. The build process may take some time, especially during the initial build, which typically ranges from 1 to 3 hours.

```bash
bitbake core-image-base
```

#### Interrupting the Build Process

If you need to stop the BitBake build process intentionally or unintentionally, you can resume the build from where it left off. Before restarting the build, you can clear the cache to eliminate any potential errors. To restart the build:

Reopen WSL and activate BitBake:

```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```

Clean up temporary files and caches:
```bash
bitbake -c cleansstate core-image-base
```

Start rebuilding the image:
```bash
bitbake core-image-base
```

### Writing the Image to the SD Card

After the build is complete, the generated `.wic.bz2` file will be located in the **tmp/deploy/images/raspberrypi4-64/** directory.

To extract the file:
```bash
cd tmp/deploy/images/raspberrypi4-64/
bunzip2 -f core-image-base-raspberrypi4-64.wic.bz2
```

After approximately 30 seconds, the `.wic` file will be extracted. To easily access the directory containing the image in Windows:

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


## config.txt Settings

`config.txt` is a fundamental configuration file that defines the hardware settings for Raspberry Pi during the boot process. This file sets options such as GPU memory, display resolution, video output, overlays (driver extensions), and other hardware parameters. Images generated by Yocto include this file, but manual edits may be required when using custom hardware (e.g., DSI displays, touch panels, or additional sensors).

In our project, adjustments are needed for display and touchscreen settings. When you open your SD card on Windows, you can access the **boot** directory and modify its contents. Locate the **config.txt** file inside the **boot** folder and:

- To enable the UART on Raspberry Pi 4, ensure the line `enable_uart=1` is present in the file.  
- To enable output via DSI, one of the following lines must be present: `dtoverlay=vc4-fkms-v3d` or `dtoverlay=vc4-kms-v3d`.  
  - If your interface uses the Framebuffer, use `dtoverlay=vc4-fkms-v3d`.  
  - If you are using a modern GPU-accelerated Qt or Wayland-based system, use `dtoverlay=vc4-kms-v3d`.  
  - **Do not include both lines at the same time!**  
- To enable touchscreen functionality, find the line `dtparam=i2c_arm` and update it to `dtparam=i2c_arm=on`.  
  - Then add the following line at the end of the file: `dtoverlay=rpi-ft5406`

Save and close the file. Your SD card is now ready to be inserted into the Raspberry Pi device.

> ⚠️ **WARNING:**  
> Before removing the SD card from your computer, always perform the **"Eject XX Storage Device"** operation.  
> Otherwise, the image may become corrupted and the system may fail to boot.


## Raspberry Pi 4 Configuration

### Setting Up UART Connection on Raspberry Pi 4

To establish direct serial terminal access to the Raspberry Pi 4, you can use a USB to UART converter. This method allows direct communication with the device via terminal even if no display or network connection is available.

---

#### 1. Connect the Hardware

- Connect your USB to UART converter as follows:  
  - **Converter TX** → Raspberry Pi **RX** pin  
  - **Converter RX** → Raspberry Pi **TX** pin  
  - **Converter GND** → Any **GND** pin on the Raspberry Pi

> 📌 **Note:** You can refer to the [official GPIO diagram](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-pinout) for the Raspberry Pi 4 GPIO pin layout.

---


#### 2. Identify the COM Port on Your Computer

To find out which port your USB-to-UART converter is recognized on:
  - Right-click the Start menu → Open **Device Manager**
  - Under "Ports (COM & LPT)", locate the device listed as `"USB Serial Port (COMx)"`
  - Note down the `COMx` port number

---

#### 3. Download and Install PuTTY

- Download and install [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) for serial communication
- You can proceed with the default installation settings

---

#### 4. Connect via Serial Using PuTTY

- Launch PuTTY  
- In the "Session" tab:
  - **Connection type**: *Serial*  
  - **Serial line**: `COMx` (e.g., `COM3`)  
  - **Speed (baud rate)**: `115200`  
- Click the **Open** button

> If the terminal window appears blank after opening, press Enter. The login screen should appear.

---

#### 5. Log In as root

When the login screen appears, log in using the following:

```plaintext
raspberrypi login: root
```

No password will be required. In Yocto-based systems, the root user is passwordless by default. You can add a password later for security purposes.

### Wi-Fi Setup and SSH Connection

To enable internet access and remote connectivity on the Raspberry Pi 4, Wi-Fi must be configured and an SSH connection established. The following steps explain how to manually configure Wi-Fi and connect via SSH on a Yocto system.

---

#### 1. Create the wpa_supplicant Configuration Directory (if it does not exist)

```bash
mkdir -p /etc/wpa_supplicant
```

---

#### 2. Create the Wi-Fi Configuration File

```bash
nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Write the following content into the file:

```
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1

network={
    ssid="YOUR_WIFI_NAME"
    psk="YOUR_WIFI_PASSWORD"
}
```

> Update the `ssid` and `psk` fields according to your own wireless network information.

---

#### 3. Start the Wi-Fi Connection

```bash
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf
udhcpc -i wlan0
```
At the end of this step, the wireless network connection will be initiated and an IP address will be requested. If you encounter an error at this point, please check the Wi-Fi SSID and password in your configuration file.

---

#### 4. Check the IP Address

```bash
ip addr show wlan0
```

The output of this command will show an IP address in the format `192.168.1.XX/24`. This is the IP address of your device.

---

#### 5. Connect via SSH

You can connect via SSH from a computer on the same network. To do this, enter the following command in the WSL terminal:

```bash
ssh root@192.168.1.x
```

> In Yocto systems, the `root` user can log in without a password.

If your WSL terminal shows **root@raspberrypi4-64:~#**, your SSH connection has been successfully established. You can test the connection by trying various commands.

## SDK Generation and Application Compilation

With the SDK (Software Development Kit) generated by Yocto, you can easily compile applications for target devices like the Raspberry Pi 4 on your host system (e.g., Ubuntu running under WSL). The SDK includes the compiler, libraries, and all development tools compatible with the target system. This allows you to compile your application on your computer rather than directly on the target device, and produce executable binary files.

In this section, you'll see step-by-step how to install and activate the SDK.

First, activate BitBake:


```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```

Start the SDK build process using BitBake:

```bash
bitbake core-image-base -c populate_sdk
```

> Note: You should run the SDK build command according to the image type you used when building the Yocto image. For example: `bitbake core-image-sato -c populate_sdk`

The SDK build process will take some time. Do not close the terminal; wait until it finishes.

Next, navigate to the directory where the SDK was generated:


```bash
cd tmp/deploy/sdk/
ls
```

If the SDK has been successfully generated, you will see a `.sh` file in the directory.  
For our project, it is named:  
`poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh`  
This is the SDK installer file. You can run this file to complete the installation.


```bash
chmod +x poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
./poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
```

During the installation, you will be prompted to enter the SDK install directory.  
Leave it blank to use the default directory. If you want a custom path, enter the desired folder name.  
By default, it will be installed to: `/opt/poky/4.0.26/`

After the installation is complete, to use the SDK:


```bash
source /opt/poky/4.0.26/environment-setup-cortexa72-poky-linux
```
enter the command. The necessary include paths for compilation will now be set, and you can build applications compatible with your image. To check if the SDK is active:


```bash
echo $CC
```
The output should be in the form of `aarch64-poky-linux-gcc...`.  
If you see this, all steps have been completed successfully.

> Note: Each time you restart WSL, you will need to re-activate the SDK environment.  
> Also, BitBake cannot be activated while the SDK is active.

## Setting Up the LVGL Environment

We will prepare a suitable environment to create and compile LVGL applications. Then, by adjusting the configuration files, we will be able to compile our own software as desired.

First, create the directory where we will carry out our work:


```bash
mkdir lvgl-project-rpi
cd lvgl-project-rpi
```

Let's download the LVGL Linux project with the basic structure already set up:

```bash
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
git submodule update --init --recursive
```

To test if the environment works correctly, you can compile the demo application without modifying any settings:

```bash
cmake -B build -S .
make -C build -j
```

The example application will be located in **lvgl-project-rpi\lv_port_linux\build\bin**.  
Since this application is compiled using your SDK, it cannot be run on your Windows system.

## Compiling Our Own LVGL Application

You can find more information on how to use LVGL and explore sample applications from the following sources:

- 📘 [LVGL Official Documentation](https://docs.lvgl.io/latest/en/html/index.html)  
- 💻 [LVGL PC Simulator (Development and Testing)](https://github.com/lvgl/lv_port_pc_visual_studio)  

These resources will help you learn the basics of using widgets, the event system, and style structures. You can then build your own application.

We developed our custom example by testing it on the Windows Simulator. Our simple interface consists of two files: **my_app.cpp** and **my_app.h**. To compile this interface, place your files inside the `src` directory.

In the `main.c` file configured for the LVGL simulator, the default `lv_demo_widgets()` and `lv_demo_widgets_start_slideshow()` functions are called to start the LVGL demo interface.  
To run your own interface, you need to replace these calls with your own `my_app()` function.

To do this, follow the steps below:


---

Include the `my_app.h` file:

- In `main.c`, add the following line below the other `#include` statements:

```c
#include "my_app.h"
```

---

Locate the following lines and **comment them out**:

```c
// lv_demo_widgets();
// lv_demo_widgets_start_slideshow();
```

Instead, add your own function:

```c
my_app();
```

### lv_conf.h Configuration

The `lv_conf.h` file contains extensive settings to determine what is included in the build.  
In its default state, most standard widgets are enabled.  
Make sure the LVGL widgets used in your application are defined like `#define LV_USE_CHECKBOX   1`.  
If the value is 0, update it to 1.  
To reduce resource usage, you can set unused widgets to 0.

### ⚙️ Editing the CMakeLists.txt File

To compile your custom `my_app.c` file, you need to make a few small but important changes to the `CMakeLists.txt` file.  
This file tells the build system which source files to use, what the program name is, and which libraries to link.

---

First, locate the following line in the `CMakeLists.txt` file:


```cmake
add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

This line specifies the name of the program to be created (`lvglsim`) and which files will be compiled.  
Here, `src/main.c` is your main source file. Since you've written your interface in `my_app.c`, you should also add `src/my_app.c` to this line.

Update it as follows:

```cmake
add_executable(lvglsim src/main.c src/my_app.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

> Note: If your application file is named `my_app.cpp`, use `.cpp`. If it's a C file, use `.c`.

### Display Backend Configuration

To run LVGL on Linux, a display backend is used to define how graphics are rendered on the screen. For example:

- During development and testing, **SDL** is commonly used. It opens a display window.
- On real hardware, **Framebuffer (FBDEV)** is typically used for direct screen output.

The backend to be used is specified in the `lv_conf.h` file.  
You should enable only one of the following options:

```c
#define LV_USE_SDL 1
#define LV_USE_LINUX_FBDEV 0
```

So in this example, **SDL is enabled**, and FBDEV is **disabled**.

To check and modify which system is active, simply open the file and change the `1` and `0` values at the beginning of the relevant lines.  
Only one system should be enabled at a time — enabling multiple backends may cause conflicts.

Once you choose the display system (e.g., SDL or FBDEV), it will be initialized in the `main.c` file.  
Look for the following line in the code:


```c
driver_backends_init_backend(...);
```

This line automatically initializes the selected backend.  
The `CMakeLists.txt` file already contains the necessary settings to include the selected backend in the build.  
So, you only need to choose the backend you want to use from the `lv_conf.h` file — the system will handle the rest for you.

Don't forget to recompile the project for your changes to take effect:

```bash
cmake -B build -S .
make -C build -j
```

You can find the compiled output in the **build/bin** directory.  
For more advanced configuration options, you can refer to the LVGL developer documentation.

Your compiled application is now ready to run on your Raspberry Pi device.  
You can transfer the file to the device via SSH and run it there.



