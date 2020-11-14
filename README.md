# ESP32 - Azure IoT Device with OTA

This project aims to demonstrate how we can use ESP32 with Azure IoT Hub and implement OTA features through the platform.

## Prerequisites

### Arduino

Arduino IDE should be installed to get the possibility to develop with board with VSCode.
The Arduino IDE may be installed from: [https://www.arduino.cc/en/main/software#download](https://www.arduino.cc/en/main/software#download)

### VSCode

At time this document is written you may have to install:

* VS Code 1.51.0
* Arduino Extension 0.3.3
    > This may be installed with VS Code command

    ```bash
    ext install vscode-arduino
    ```


### ESP 32 & Hardware Adaptation

1. Install esp32 board support into your Arduino IDE.

    * Start Arduino and open Preferences window.

    * Enter `https://dl.espressif.com/dl/package_esp32_index.json` into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.

    * Open Boards Manager from Tools > Board menu and install esp32 platform 1.0.4 or later

    * Select your ESP32 board from Tools > Board menu after installation

2. Navigate to where your esp32 board package is located, typically in `C:\Users\<your username>\AppData\Local\Arduino15\packages` on Windows and `~/.arduino15/packages/` on Linux

    * Navigate deeper in to `hardware/esp8266/<board package version>/` where the `platform.txt` file lives.

    * Copy the [`platform.local.txt`](https://github.com/Azure/azure-iot-arduino/blob/master/examples/iothub_ll_telemetry_sample/esp32/platform.local.txt) file from the `esp32` folder in the sample into the same folder as the `platform.txt`.

    * Alternatively, or for later versions of the Board Package, add the define `-DDONT_USE_UPLOADTOBLOB` to `build.extra_flags=` in `platform.txt` or a `platform.local.txt` that you create.

### Azure IoT C SDK

1. Download and build the Azure IoT SDK on your Arduino libraries
    * Clone the Azure IoT SDK from github : [https://github.com/Azure/azure-iot-pal-arduino](https://github.com/Azure/azure-iot-pal-arduino)

        ```bash
        git clone --recursive https://github.com/Azure/azure-iot-pal-arduino
        ```

    * Build the SDK to your Arduino Libraries

        ```bash
        python3 make_sdk.py -o `C:\Users\<your username>\Documents\Arduino\libraries\` on Windows
        ```

## Getting Started

### Create an IoT hub

This section describes how to create an IoT hub using the [Azure portal](https://portal.azure.com).

1. Sign in to the [Azure portal](https://portal.azure.com).

1. From the Azure homepage, select the **+ Create a resource** button, and then enter *IoT Hub* in the **Search the Marketplace** field.

1. Select **IoT Hub** from the search results, and then select **Create**.

1. On the **Basics** tab, complete the fields as follows:

   * **Subscription**: Select the subscription to use for your hub.

   * **Resource Group**: Select a resource group or create a new one. To create a new one, select **Create new** and fill in the name you want to use. To use an existing resource group, select that resource group. For more information, see [Manage Azure Resource Manager resource groups](../articles/azure-resource-manager/management/manage-resource-groups-portal.md).

   * **Region**: Select the region in which you want your hub to be located. Select the location closest to you. Some features, such as [IoT Hub device streams](../articles/iot-hub/iot-hub-device-streams-overview.md), are only available in specific regions. For these limited features, you must select one of the supported regions.

   * **IoT Hub Name**: Enter a name for your hub. This name must be globally unique. If the name you enter is available, a green check mark appears.

> [!IMPORTANT]
> Because the IoT hub will be publicly discoverable as a DNS endpoint, be sure to avoid entering any sensitive or personally identifiable information when you name it.

   ![Create a hub in the Azure portal](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-hub/iot-hub-create-screen-basics.png?raw=true)

1. Select **Next: Size and scale** to continue creating your hub.

   ![Set the size and scale for a new hub using the Azure portal](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-hub/iot-hub-create-screen-size-scale.png?raw=true)

   You can accept the default settings here. If desired, you can modify any of the following fields:

    * **Pricing and scale tier**: Your selected tier. You can choose from several tiers, depending on how many features you want and how many messages you send through your solution per day. The free tier is intended for testing and evaluation. It allows 500 devices to be connected to the hub and up to 8,000 messages per day. Each Azure subscription can create one IoT hub in the free tier.

      If you are working through a Quickstart for IoT Hub device streams, select the free tier.

    * **IoT Hub units**: The number of messages allowed per unit per day depends on your hub's pricing tier. For example, if you want the hub to support ingress of 700,000 messages, you choose two S1 tier units.
    For details about the other tier options, see [Choosing the right IoT Hub tier](../articles/iot-hub/iot-hub-scaling.md).

    * **Azure Security Center**: Turn this on to add an extra layer of threat protection to IoT and your devices. This option is not available for hubs in the free tier. For more information about this feature, see [Azure Security Center for IoT](https://docs.microsoft.com/azure/asc-for-iot/).

    * **Advanced Settings** > **Device-to-cloud partitions**: This property relates the device-to-cloud messages to the number of simultaneous readers of the messages. Most hubs need only four partitions.

1. Select **Next: Tags** to continue to the next screen.

    Tags are name/value pairs. You can assign the same tag to multiple resources and resource groups to categorize resources and consolidate billing. For more information, see [Use tags to organize your Azure resources](../articles/azure-resource-manager/management/tag-resources.md).

    ![Assign tags for the hub using the Azure portal](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-hub/iot-hub-create-tabs.png?raw=true)

1. Select **Next: Review + create** to review your choices. You see something similar to this screen, but with the values you selected when creating the hub.

    ![Review information for creating the new hub](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-hub/iot-hub-create-review.png?raw=true)

1. Select **Create** to create your new hub. Creating the hub takes a few minutes.

### Register a new device in the IoT hub

In this section, you create a device identity in the identity registry in your IoT hub. A device cannot connect to a hub unless it has an entry in the identity registry. For more information, see the [IoT Hub developer guide](../articles/iot-hub/iot-hub-devguide-identity-registry.md#identity-registry-operations).

1. In your IoT hub navigation menu, open **IoT Devices**, then select **New** to add a device in your IoT hub.

    ![Create device identity in portal](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-device/create-identity-portal-vs2019.png?raw=true)

1. In **Create a device**, provide a name for your new device, such as **myDeviceId**, and select **Save**. This action creates a device identity for your IoT hub.

   ![Add a new device](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-device/create-a-device-vs2019.png?raw=true)

> [!IMPORTANT]
> Because the IoT hub will be publicly discoverable as a DNS endpoint, be sure to avoid entering any sensitive or personally identifiable information when you name it.

1. After the device is created, open the device from the list in the **IoT devices** pane. Copy the **Primary Connection String** to use later.

    ![Device connection string](https://github.com/MicrosoftDocs/azure-docs/blob/master/includes/media/iot-hub-include-create-device/device-details-vs2019.png?raw=true)

> [!NOTE]
> The IoT Hub identity registry only stores device identities to enable secure access to the IoT hub. It stores device IDs and keys to use as security credentials, and an enabled/disabled flag that you can use to disable access for an individual device. If your application needs to store other device-specific metadata, it should use an application-specific store. For more information, see [IoT Hub developer guide](../articles/iot-hub/iot-hub-devguide-identity-registry.md).

### Sets the IoT device Twin desired properties

In this section, you set the device twin desired properties.

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

#### Create a storage account

Every storage account must belong to an Azure resource group. A resourcegroup is a logical container for grouping your Azure services. When you create a storage account, you have the option to either create a new resource group, or use an existing resource group. This article shows howto create a new resource group.

A **general-purpose v2** storage account provides access to all of theAzure Storage services: blobs, files, queues, tables, and disks. The steps outlined here create a general-purpose v2 storage account, but the steps to create any type of storage account are similar. For more information about types of storage accounts and other storage account settings, see [Azure storage account overview](storage-account-overview.md).
To create a general-purpose v2 storage account in the Azure portal, follow these steps:

1. On the Azure portal menu, select **All services**. In the list of resources, type **Storage Accounts**. As you begin typing, the list filters based on your input. Select **Storage Accounts**.
1. On the **Storage Accounts** window that appears, choose **Add**.
1. On the **Basics** tab, select the subscription in which to create the storage account.
1. Under the **Resource group** field, select your desired resource group, or create a new resource group.  For more information on Azure resource groups, see [Azure Resource Manager overview](https://docs.microsoft.com/en-us/azure/azure-resource-manager/resource-group-overview).
1. Next, enter a name for your storage account. The name you choose must be unique across Azure. The name also must be between 3 and 24 characters in length, and may include only numbers and lowercase letters.
1. Select a location for your storage account, or use the default location.
1. Select a performance tier. The default tier is *Standard*.
1. Set the **Account kind** field to *Storage V2 (general-purpose v2)*.
1. Specify how the storage account will be replicated. The default replication option is *Read-access geo-redundant storage (RA-GRS)*. For more information about available replication options, see [Azure Storage redundancy](https://docs.microsoft.com/en-us/azure/storage/common/storage-redundancy).
1. Specify the access tier for blobs in the storage account. The default tier is *hot*. For more information about blob access tiers, see [Hot, cool, and archive access tiers for blobs](https://docs.microsoft.com/en-us/azure/storage/blobs/storage-blob-storage-tiers).
1. To use Azure Data Lake Storage, choose the **Advanced** tab, and then set **Hierarchical namespace** to **Enabled**. For more information, see [Azure Data Lake Storage Gen2 Introduction](https://docs.microsoft.com/en-us/azure/storage/blobs/data-lake-storage-introduction)
1. Select **Review + Create** to review your storage account settings and create the account.
1. Select **Create**.

The following image shows the settings on the Basics tab for a new storage account:

![https://docs.microsoft.com/en-us/azure/includes/media/storage-create-account-portal-include/account-create-portal.png](https://docs.microsoft.com/en-us/azure/includes/media/storage-create-account-portal-include/account-create-portal.png)

#### Create a container

1. Navigate to your new storage account in the Azure portal.
1. In the left menu for the storage account, scroll to the **Blob service** section, then select **Containers**.
1. Select the **+ Container** button.
1. Type a name for your new container. The container name must be lowercase, must start with a letter or number, and can include only letters, numbers, and the dash (-) character. For more information about container and blob names, see [Naming and referencing containers, blobs, and metadata](https://docs.microsoft.com/rest/api/storageservices/naming-and-referencing-containers--blobs--and-metadata).
1. Set the level of public access to the container. 
    `Blob (anonymous read access for blobs only)`
1. Select **OK** to create the container.

![./src/.assets/create-container.png](./src/.assets/create-container.png)

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