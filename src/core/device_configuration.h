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

/**
* @brief   Initializes the device configuration.
*
*            This is a blocking call.
*
*/
void DeviceConfiguration_Init();

/**
* @brief   Gets the configuration values from the device configuration.
*
*            This is a blocking call.
*
* @param    section    Pointer to a character string containing the Ini Section lookup.
* @param    key        Pointer to a character string containing the Ini Key lookup.
* @param    max_size   The buffer size to allocate for getting the ini configuration value.
*
* @return   A pointer to a character string containing the configuration value.
*/
const char* DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

/**
* @brief   Gets a file content from the device storage.
*
*            This is a blocking call.
*
* @param    section    Pointer to a character string containing the file path.

* @return   A pointer to a character string containing the file content.
*/
const char* DeviceConfiguration_GetFileContent(const char* filePath);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif