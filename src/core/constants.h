#ifndef __ESP32_AzureIoT_OTA_CONSTANTS_H__
#define __ESP32_AzureIoT_OTA_CONSTANTS_H__

#ifdef __cplusplus
extern "C"
{
#endif

static const char* CONFIGURATION_FILE_PATH = "/device";

static const char* TWIN_FILE_PATH = "properties.json";

static const int TWIN_SIZE = 1024;

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif