/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONFIGURATION_H__

#ifdef __cplusplus
extern "C"
{
#endif

static const char *CONFIGURATION_FILENAME = "/config.ini";

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
const char *DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

/**
* @brief   Gets a file content from the device storage returing also the content size.
*
*          This is a blocking call.
*
* @param    filePath    Pointer to a character string containing the file path.
* @param    size        Pointer to a value that will represent the content length.
*
* @return   A pointer to a character string containing the file content.
*/
const char *DeviceConfiguration_GetFileContent(const char *filePath, size_t *size);

/**
* @brief   Writes a file content from the device storage.
*
*            This is a blocking call.
*
* @param    filePath    Pointer to a character string containing the file path.
* @param    content     Pointer to a character string containing the expectd file content.
* @param    size        The content size in bytes.
*/
void DeviceConfiguration_WriteFileContent(const char *filePath, const char *content, size_t size);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif