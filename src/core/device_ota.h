/*
  Device_OTA.h - Library for flashing device OTA (Over The Air).
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICEOTA_H__
#define __ESP32_AzureIoT_OTA_DEVICEOTA_H__

#include <Arduino.h>

void OTA_Start(String host, int port, String bin);

#endif