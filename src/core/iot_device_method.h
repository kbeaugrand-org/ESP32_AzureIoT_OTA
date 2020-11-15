#ifndef __ESP32_AzureIoT_DEVICE_METHOD_H__
#define __ESP32_AzureIoT_DEVICE_METHOD_H__

#include <stddef.h>

typedef int(*IOT_DEVICE_METHOD_CALLBACK)(const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size);


#ifndef IOT_DEVICE_MAX_METHOD_COUNT
#define IOT_DEVICE_MAX_METHOD_COUNT 10
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief   Register for Cloud To Device Method invoke.
*
* @param    methodName      Pointer to a character string containing the message to send.
* @param    callback        Pointer to a method callback.
*
*/
void IoTDevice_RegisterForMethodInvoke(const char *methodName, IOT_DEVICE_METHOD_CALLBACK callback);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif