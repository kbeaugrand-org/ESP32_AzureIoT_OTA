#ifndef __ESP32_AzureIoT_OTA_DEVICETWIN_H__
#define __ESP32_AzureIoT_OTA_DEVICETWIN_H__

#include <ArduinoJson.h>

#include "constants.h"

StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties();

#endif