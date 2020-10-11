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

void OTA_Start(String host, int port, String bin);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif