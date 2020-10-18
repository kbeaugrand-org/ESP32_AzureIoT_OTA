/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__

#ifdef __cplusplus
extern "C"
{
#endif

static const char* CONFIGURATION_FILENAME = "/config.ini";

void DeviceConfiguration_Init();

const char* DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

const char* DeviceConfiguration_GetFileContent(const char* filePath);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif