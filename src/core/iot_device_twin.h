#ifndef __ESP32_AzureIoT_OTA_DEVICETWIN_H__
#define __ESP32_AzureIoT_OTA_DEVICETWIN_H__

#include <ArduinoJson.h>

#include "constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif