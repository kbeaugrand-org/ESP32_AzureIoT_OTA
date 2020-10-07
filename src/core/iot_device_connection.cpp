#include <WiFi.h>

#include <AzureIoTHub.h>
#include <azure_c_shared_utility/xlogging.h>
#include <apps/sntp/sntp.h>

#include "blink.h"
#include "device_configuration.h"
#include "iot_device_core.h"

#ifndef IOTDEVICE_PROTOCOL_MQTT
#define IOTDEVICE_PROTOCOL_MQTT
#endif

#define IOTDEVICE_DIAGNOSTIC_SAMPLING_PERCENTAGE 0

//Setting the auto URL Encoder (recommended for MQTT). Please use this option unless
//you are URL Encoding inputs yourself.
//ONLY valid for use with MQTT
#ifndef IOTDEVICE_AUTO_URL_ENCODE_DECODE && (defined PROTOCOL_MQTT || defined PROTOCOL_MQTT_WS)
#define IOTDEVICE_AUTO_URL_ENCODE_DECODE true
#endif 

int receiveContext;
bool clientConnected = false;

void IoTDevice_ConnectWifi(const char *ssid, const char *password)
{
  LogInfo("Starting connecting WiFi: %s", ssid);
  BlinkLed_High();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    LogInfo(".");
  }

  LogInfo("WiFi connected, IP address: %s", WiFi.localIP());

  BlinkLed_Low();
}

void IoTDevice_InitDeviceTime()
{       
  LogInfo("Initializing Device Time.");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "192.168.10.1");
  sntp_init();
  time_t ts = 0;

  // Before 1980 is uninitialized
  while (ts < 10 * 365 * 24 * 3600)
  {
    BlinkLed_Blink();
    BlinkLed_Blink();
    
    delay(1000);
    ts = time(NULL);
  }

  LogInfo("SNTP initialization complete.");
  return;
}

void IoTDevice_ConnectToHub(const char *connectionString)
{
#ifdef IOTDEVICE_PROTOCOL_MQTT
#include <iothubtransportmqtt.h>
IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
#endif // IOTDEVICE_PROTOCOL_MQTT

#ifdef IOTDEVICE_PROTOCOL_HTTP
#include <iothubtransporthttp.h>
IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = HTTP_Protocol;
#endif // IOTDEVICE_PROTOCOL_HTTP

  LogInfo("Connecting the client");
  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);

  // Used to initialize IoTHub SDK subsystem
  (void)IoTHub_Init();

  if (iotHubClientHandle  == NULL)
  {
      LogError("Error AZ002: Failure createing Iothub device. Hint: Check you connection string.");
  }

  LogInfo("Connecting to the device...");
}

bool IoTDevice_IsConnected(){
  return clientConnected;
}

bool IoTDevice_ConnectFromConfiguration() {
    DeviceConfiguration_Init();

    const char *ssid = DeviceConfiguration_Get("core", "ssid", 128);
    const char *password = DeviceConfiguration_Get("core", "password", 128);

    IoTDevice_ConnectWifi(ssid, password);

    IoTDevice_InitDeviceTime();

    const char *connectionString = DeviceConfiguration_Get("core", "connectionString", 256);

    IoTDevice_ConnectToHub(connectionString);
}

/* -- ConfigureOptions --
 * Configures the LL Device client options.
 */
void IoTDevice_ConfigureOptions()
{
    // Set any option that are neccessary.
    // For available options please see the iothub_sdk_options.md documentation
#ifdef IOTDEVICE_DIAGNOSTIC_SAMPLING_PERCENTAGE
    int diagnosticSamplingPercentage = IOTDEVICE_DIAGNOSTIC_SAMPLING_PERCENTAGE;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diagnosticSamplingPercentage);
#endif

#ifdef IOTDEVICE_TRUSTED_CERT
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_TRUSTED_CERT, IOTDEVICE_TRUSTED_CERT);
#endif

#ifdef IOTDEVICE_AUTO_URL_ENCODE_DECODE
    bool autoUrlEncodeDecode = IOTDEVICE_AUTO_URL_ENCODE_DECODE;
    IoTHubDeviceClient_LL_SetOption(iotHubClientHandle, OPTION_AUTO_URL_ENCODE_DECODE, &autoUrlEncodeDecode);
#endif
  
  (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(iotHubClientHandle, IoTDevice_ConnectionStatusCallback, NULL);
}

static void IoTDevice_ConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    clientConnected = false;

    switch (reason)
    {
    case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
        if (result == IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED)
        {
            LogInfo(">>> Connection status: timeout");
        }
        break;
    case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
        break;
    case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
        break;
    case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
        break;
    case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
        if (result == IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED)
        {
            LogInfo(">>> Connection status: disconnected");
        }
        break;
    case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
        break;
    case IOTHUB_CLIENT_CONNECTION_OK:
        if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
        {
            clientConnected = true;
            LogInfo(">>> Connection status: connected");
        }
        break;
    }
}