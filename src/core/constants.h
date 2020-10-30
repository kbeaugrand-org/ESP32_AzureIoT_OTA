#ifndef __ESP32_AzureIoT_OTA_CONSTANTS_H__
#define __ESP32_AzureIoT_OTA_CONSTANTS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief   A pointer to a character string containing the device storage mounting point path.
*
*/
static const char *CONFIGURATION_FILE_PATH = "/device";

/**
* @brief   A integer representing the default twin size.
*
*/
static const int TWIN_SIZE = 2048;

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif