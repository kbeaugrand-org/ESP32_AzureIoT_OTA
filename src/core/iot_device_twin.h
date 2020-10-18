#ifndef __ESP32_AzureIoT_OTA_DEVICETWIN_H__
#define __ESP32_AzureIoT_OTA_DEVICETWIN_H__

#include <ArduinoJson.h>

#include "constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief   Gets the device desired properties.
*
* @return  A Json Document representing the desired properties on device twin.
*/
StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif