#include <WiFi.h>

#include <AzureIoTHub.h>
#include <apps/sntp/sntp.h>

#include "blink.h"
#include "logging.h"
#include "device_configuration.h"
#include "iot_device_core.h"
#include "iot_device_connection.h"

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

#ifdef IOTDEVICE_PROTOCOL_MQTT
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>
IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
#endif // IOTDEVICE_PROTOCOL_MQTT

#ifdef IOTDEVICE_PROTOCOL_HTTP
#include <AzureIoTProtocol_HTTP.h>
#include <iothubtransporthttp.h>
IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = HTTP_Protocol;
#endif // IOTDEVICE_PROTOCOL_HTTP

int receiveContext;
bool clientConnected = false;

void IoTDevice_ConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    clientConnected = false;

    switch (reason)
    {
    case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
        if (result == IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED)
        {
            Log_Info(">>> Connection status: timeout");
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
            Log_Info(">>> Connection status: disconnected");
        }
        break;
    case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
        break;
    case IOTHUB_CLIENT_CONNECTION_OK:
        if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
        {
            clientConnected = true;
            Log_Info(">>> Connection status: connected");
        }
        break;
    }
}

static void IoTDevice_ConnectWifi(const char *ssid, const char *password)
{
  Log_Info("Starting connecting WiFi: %s", ssid);
  BlinkLed_High();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Log_Info(".");
  }

  Log_Info("WiFi connected, IP address: %s", WiFi.localIP());

  BlinkLed_Low();
}

static void IoTDevice_InitDeviceTime()
{       
  Log_Info("Initializing Device Time.");
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

  Log_Info("SNTP initialization complete.");
  return;
}

static void IoTDevice_ConnectToHub(const char *connectionString)
{
#ifdef IOTDEVICE_PROTOCOL_MQTT
  Log_Info("Loading the MQTT_Protocol Transport provider");
#endif // IOTDEVICE_PROTOCOL_MQTT

#ifdef IOTDEVICE_PROTOCOL_HTTP
  Log_Info("Loading the HTTP_Protocol Transport provider");
#endif // IOTDEVICE_PROTOCOL_HTTP

  Log_Info("Connecting the client");
  __hub_client_handle__ = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);

  // Used to initialize IoTHub SDK subsystem
  (void)IoTHub_Init();

  if (__hub_client_handle__  == NULL)
  {
      Log_Error("Error AZ002: Failure creating Iothub device. Hint: Check you connection string.");
  }

  Log_Info("Connecting to the device...");
}

/* -- ConfigureOptions --
 * Configures the LL Device client options.
 */
static void IoTDevice_ConfigureOptions()
{
    // Set any option that are neccessary.
    // For available options please see the iothub_sdk_options.md documentation
#ifdef IOTDEVICE_DIAGNOSTIC_SAMPLING_PERCENTAGE
    int diagnosticSamplingPercentage = IOTDEVICE_DIAGNOSTIC_SAMPLING_PERCENTAGE;
    IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diagnosticSamplingPercentage);
#endif

#ifdef IOTDEVICE_TRUSTED_CERT
    IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_TRUSTED_CERT, IOTDEVICE_TRUSTED_CERT);
#endif

#ifdef IOTDEVICE_AUTO_URL_ENCODE_DECODE
    bool autoUrlEncodeDecode = IOTDEVICE_AUTO_URL_ENCODE_DECODE;
    IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_AUTO_URL_ENCODE_DECODE, &autoUrlEncodeDecode);
#endif
  
  (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(__hub_client_handle__, IoTDevice_ConnectionStatusCallback, NULL);
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