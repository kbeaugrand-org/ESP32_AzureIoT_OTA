#ifndef IOT_DEVICE_D2C_H
#define IOT_DEVICE_D2C_H

#include "logging.h"

#include "iot_device_d2c.h"
#include "iot_device_core.h"
#include "blink.h"

static void IoTDevice_SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    BlinkLed_Blink();
  }
  else
  {
    Log_Error("Message was not send: %s", result);
  }
}

bool IoTDevice_Send(const char *message)
{
  Log_Info("Sending message: %s", message);

  // Construct the iothub message from a string or a byte array
  IOTHUB_MESSAGE_HANDLE messageHandle  = IoTHubMessage_CreateFromString(message);
  
  IoTHubDeviceClient_LL_SendEventAsync(__hub_client_handle__, messageHandle, IoTDevice_SendConfirmationCallback, NULL);

  // The message is copied to the sdk so the we can destroy it
  IoTHubMessage_Destroy(messageHandle);

  IoTHubDeviceClient_LL_DoWork(__hub_client_handle__);       
  ThreadAPI_Sleep(3);
}

#endif