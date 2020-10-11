/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__

#ifdef __cplusplus
extern "C"
{
#endif

void DeviceConfiguration_Init();

const char* DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif