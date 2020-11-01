/*
  Device_OTA.h - Library for flashing device OTA (Over The Air).
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICEOTA_H__
#define __ESP32_AzureIoT_OTA_DEVICEOTA_H__

#include <Arduino.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief    Starts the device Firmware update OTA from the device twin desired properties.
*
*            This is a blocking call.
*/
void IoTDevice_StartOTA();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif