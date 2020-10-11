#ifndef __ESP32_AzureIoT_OTA_DEVICED2C_H__
#define __ESP32_AzureIoT_OTA_DEVICED2C_H__

#include <ArduinoJson.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool IoTDevice_Send(DynamicJsonDocument message);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif