#include <Arduino.h>

/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__

void DeviceConfiguration_Init();

const char* DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

#endif