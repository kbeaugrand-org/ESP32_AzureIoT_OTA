# ESP32 - Azure IoT Device with OTA

This project aims to demonstrate how we can use ESP32 with Azure IoT Hub and implement OTA features through the platform.

## Prerequisites

### Arduino

Arduino IDE should be installed to get the possibility to develop with board with VSCode.
The Arduino IDE may be installed from: [https://www.arduino.cc/en/main/software#download](https://www.arduino.cc/en/main/software#download)

### VSCode

At time this document is written you may have to install:

* VS Code 1.48.2
    > This is needed because a bug with Serial Monitor
    > [https://github.com/microsoft/vscode-arduino/issues/1098](https://github.com/microsoft/vscode-arduino/issues/1098)

* Arduino Extension 0.30
    > This may be installed with VS Code command
    > ```ext install vscode-arduino```

### ESP 32 & Hardware Adaptation

1. Install esp32 board support into your Arduino IDE.

    * Start Arduino and open Preferences window.

    * Enter `https://dl.espressif.com/dl/package_esp32_index.json` into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.

    * Open Boards Manager from Tools > Board menu and install esp32 platform 1.0.2 or later

    * Select your ESP32 board from Tools > Board menu after installation

2. Navigate to where your esp32 board package is located, typically in `C:\Users\<your username>\AppData\Local\Arduino15\packages` on Windows and `~/.arduino15/packages/` on Linux

    * Navigate deeper in to `hardware/esp8266/<board package version>/` where the `platform.txt` file lives.

    * Copy the [`platform.local.txt`](https://github.com/Azure/azure-iot-arduino/blob/master/examples/iothub_ll_telemetry_sample/esp32/platform.local.txt) file from the `esp32` folder in the sample into the same folder as the `platform.txt`.

    * Alternatively, or for later versions of the Board Package, add the define `-DDONT_USE_UPLOADTOBLOB` to `build.extra_flags=` in `platform.txt` or a `platform.local.txt` that you create.

### Azure IoT C SDK

1. Download and build the Azure IoT SDK on your Arduino libraries
    * Clone the Azure IoT SDK from github : [https://github.com/Azure/azure-iot-pal-arduino](https://github.com/Azure/azure-iot-pal-arduino)
        > ```git clone --recursive https://github.com/Azure/azure-iot-pal-arduino```
    * Build the SDK to your Arduino Libraries
        > python3 make_sdk.py -o `C:\Users\<your username>\Documents\Arduino\libraries\` on Windows

## Getting Started
