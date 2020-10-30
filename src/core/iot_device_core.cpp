#include "AzureIoTHub.h"

#include "iot_device_core.h"

char *IoTDevice_GetErrorText(IOTHUB_CLIENT_RESULT result)
{
  switch (result)
  {
  case IOTHUB_CLIENT_INVALID_ARG:
    return "IOTHUB_CLIENT_INVALID_ARG"; 
  case IOTHUB_CLIENT_ERROR:
    return "IOTHUB_CLIENT_ERROR";    
  case IOTHUB_CLIENT_INVALID_SIZE:
    return "IOTHUB_CLIENT_INVALID_SIZE";    
  case IOTHUB_CLIENT_INDEFINITE_TIME:
    return "IOTHUB_CLIENT_INDEFINITE_TIME";
  default: 
    return "";
  }
}