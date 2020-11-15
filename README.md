# ESP32 - Azure IoT Device with OTA

This project aims to demonstrate how we can use ESP32 with Azure IoT Hub and implement OTA features through the platform.

## Prerequisites

To work with this library, you will have to follow some prerequisites sets that are described at the wiki: [https://github.com/kbeaugrand/ESP32_AzureIoT_OTA/wiki](https://github.com/kbeaugrand/ESP32_AzureIoT_OTA/wiki).

## Getting Started

### Install this repository as an Arduino Library

In this section, you install this repository as an Arduino Library on your computer.

1. Download the library from the releases on github: [https://github.com/kbeaugrand/ESP32_AzureIoT_OTA/releases](https://github.com/kbeaugrand/ESP32_AzureIoT_OTA/releases)

1. Extract the release to Arduino Library (see: [https://www.arduino.cc/en/Guide/Libraries#manual-installation](https://www.arduino.cc/en/Guide/Libraries#manual-installation))

### Configure the device settings

In this section you create a new sketch called "Device Initiator" that helps you to configure the device settings. This sketch will create a new partition scheme on the device storage, and create the device settings file on that storage.

> [!NOTE]
> Storing those settings enables the possibility to work with Firmware update Over The Air without storing the device identity with the Firmware.

1. Start a new sketch from an example
    * Open VS Code and press `F1` or `Ctrl + Shift + P` to open command palette
    * Select `Ardiuno: Examples` command
    * On Arduino Examples Window, select `Examples from Custom Libraries` > `ESP32_AzureIoT_OTA` > `ESP32_Device_Initiator`

1. Configure your ESP 32 board from VSCode
    * Open VS Code and press `F1` or `Ctrl + Shift + P` to open command palette
    * Select `Ardiuno: Board Config` command
    * On Arduino Board Configuration Window select the `Minimal SPIFFS` Partition Scheme

1. On `ESP32_Device_Initiator`, type your device connection settings

    ```c++
    const char *connectionString = "Type your Device Connection String";
    const char *ssid = "Type here your WiFi SSID";
    const char *password = "Type here your WiFi shared Key";
    ```

1. Upload this sketch to Arduino ESP32
    * `Alt + Cmd + U` or `Alt + Ctrl + U`

> [!NOTE]
> Once this sketch is uploaded on the ESP 32, the integrated LED will be high.

### Create the device Firmware

In this section, you create the device firmware code.

1. Start a new sketch from an example
    * Open VS Code and press `F1` or `Ctrl + Shift + P` to open command palette
    * Select `Ardiuno: Examples` command
    * On Arduino Examples Window, select `Examples from Custom Libraries` > `ESP32_AzureIoT_OTA` > `ESP32_Device_OTA`

1. Configure your ESP 32 board from VSCode
    * Open VS Code and press `F1` or `Ctrl + Shift + P` to open command palette
    * Select `Ardiuno: Board Config` command
    * On Arduino Board Configuration Window select the `Minimal SPIFFS` Partition Scheme

    > [!NOTE]
    > This sketch will use the device settings file previously created to get its credentials and connect to WiFi and IoT Hub.
    > You will have to update the device logic on this one.

1. Configure the Arduino build output for this sketch
    * Open the `.vscode/arduino.json`
    * Update the settings file with this line:

        ```json
        {
            "output": "./.build"
        }
        ```

1. Upload this sketch to Arduino ESP32
    * `Alt + Cmd + U` or `Alt + Ctrl + U`

### Upload the device firmware to a Cloud Storage

In this section, you create a cloud storage account and upload you device firmware to it.

#### Build the firmware binary

1. Open the sketch repository with VS Code
1. Update the `FIRMWARE_VERSION`

    ```cpp
    #define FIRMWARE_VERSION "0.0.2"
    ```

1. Build the firmware
    * `Alt + Cmd + R` or `Alt + Ctrl + R`

#### Upload the binary to your storage account container

1. Navigate to your new storage account container in the Azure portal.
1. Select the **Upload** button.
1. Click on the **Select file** button.
1. On the `Open`Window, select the firmware binary from the sketch build directory `./.build/`.The default name is `ESP32_Device_OTA.ino.bin`.

### Create a Firmware update OTA with device Twin

In this section, you update your device twin to set a firmware update from the cloud portal.

1. In your IoT hub navigation menu, open **IoT Devices**, then select the IoT device you created.
1. Select the **Device Twin** button.
1. Use the twin editor to update your device twin with this following statements

    ```json
    {
    "properties": {
        "desired": {
          "firmware": {
            "version": "0.0.2",
            "source": "https://<storage_account_name>.blob.core.windows.net/<container_name>/ESP32_Device_OTA.ino.bin"
          }
        }
    }
    ```

1. Select the **Save** button.
1. Select the **Refresh** button to review the reported properties and verify once the device successfully applied the firmware update

    ```json
    {
        "properties": {
            "reported": {
              "firmware": {
                "version": "0.0.2"
              }
        }
    }
    ```

> [!NOTE]
> On device Serial Monitor, you may see the device update done
> Open VS Code and press `F1` or `Ctrl + Shift + P` to open command palette
> Select `Arduino: Open Serial Monitor` command

```
> Debug: Begin request for: https://<storage_account_name>.blob.core.windows.net/<container_name>/ESP32_Device_OTA.ino.bin
> Debug: Trying to start OTA with 1040864 bytes
> Info: Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!
> Info: Written: 1040864 successfully
> Info: OTA done!
> Info: Update successfully completed. Rebooting.
```