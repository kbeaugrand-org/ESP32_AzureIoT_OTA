#include <WiFi.h>

#include <stdio.h>
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

IOTHUB_CLIENT_LL_HANDLE __hub_client_handle__;

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

static void IoTDevice_ConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *user_context)
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

void IoTDevice_ConnectWifi(const char *ssid, const char *password)
{
  Log_Info("Starting connecting WiFi: %s", ssid);
  BlinkLed_High();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    printf(".");
  }

  printf("\n");

  Log_Info("Connected on WiFi");
  Log_Debug("IP address: %s", WiFi.localIP().toString().c_str());

  BlinkLed_Low();
}

static void IoTDevice_InitDeviceTime()
{       
  Log_Info("Initializing Device Time");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);

  const char *ntpHostName = DeviceConfiguration_Get("ntp", "hostname", 256);

  if (String(ntpHostName).equals(""))
  {
    ntpHostName = "pool.ntp.org";
  }

  Log_Debug("Adding SNTP host: %s", ntpHostName);
  sntp_setservername(0, (char *) ntpHostName);
  
  sntp_init();
  time_t ts = 0;

  // Before 1980 is uninitialized
  while (ts < 10 * 365 * 24 * 3600)
  {
    BlinkLed_Blink();
    BlinkLed_Blink();

    Log_Info("Waiting for SNTP initialization");
    
    delay(1000);
    ts = time(NULL);
    
  }

  Log_Info("SNTP initialization complete.");
  return;
}

static void IoTDevice_ConfigureOption(const char *optionName,  const void *value)
{
  Log_Trace("Setting %s", optionName);
  Log_Debug("%s = %s", optionName, value);
  IOTHUB_CLIENT_RESULT result = IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, optionName, value);

  if (result == IOTHUB_CLIENT_OK){
    return;
  }

  const char * resultCode = IoTDevice_GetErrorText(result);

  Log_Error("Unable to set %s => %s", optionName, resultCode);
  
  free((char *)resultCode);
}

/* -- ConfigureOptions --
 * Configures the LL Device client options.
 */
static void IoTDevice_ConfigureOptions()
{
  // Set any option that are neccessary.
  // For available options please see the iothub_sdk_options.md documentation in the main C SDK
  // turn off diagnostic sampling
  int diag_off = 0;
  IoTDevice_ConfigureOption(OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diag_off);

#if not defined IOTDEVICE_PROTOCOL_HTTP and LOGGING_LOG_LEVEL == LOG_LEVEL_TRACE
  // Example sdk status tracing for troubleshooting
  bool traceOn = true;
  IoTDevice_ConfigureOption(OPTION_LOG_TRACE, &traceOn);
#endif // IOTDEVICE_PROTOCOL_HTTP

  // Setting the Trusted Certificate.
  const char *certPath = DeviceConfiguration_Get("core", "caPath", 256);
  size_t certificatesLen;

  const char *content = DeviceConfiguration_GetFileContent(certPath, &certificatesLen);
  
  IoTDevice_ConfigureOption(OPTION_TRUSTED_CERT, content);

#ifdef IOTDEVICE_PROTOCOL_MQTT
  //Setting the auto URL Encoder (recommended for MQTT). Please use this option unless
  //you are URL Encoding inputs yourself.
  //ONLY valid for use with MQTT
  bool urlEncodeOn = true;
  IoTDevice_ConfigureOption(OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);
#endif // IOTDEVICE_PROTOCOL_MQTT

  // Setting connection status callback to get indication of connection to iothub
  (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(__hub_client_handle__, IoTDevice_ConnectionStatusCallback, NULL);

  // Setting device twin callback
  (void)IoTHubDeviceClient_LL_SetDeviceTwinCallback(__hub_client_handle__, IoTDevice_DeviceTwinCallback, NULL);

  IoTDevice_RegisterDeviceMethodCallback();
}

static void IoTDevice_ConnectToHub(const char *connectionString)
{
#ifdef IOTDEVICE_PROTOCOL_MQTT
  Log_Info("Loading the MQTT_Protocol Transport provider");
#endif // IOTDEVICE_PROTOCOL_MQTT

#ifdef IOTDEVICE_PROTOCOL_HTTP
  Log_Info("Loading the HTTP_Protocol Transport provider");
#endif // IOTDEVICE_PROTOCOL_HTTP
// Used to initialize IoTHub SDK subsystem
    (void)IoTHub_Init();

    // Create the iothub handle here
    __hub_client_handle__ = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, protocol);
    LogInfo("Creating IoTHub Device handle\r\n");

    if (__hub_client_handle__ == NULL)
    {
        LogInfo("Error AZ002: Failure creating Iothub device. Hint: Check you connection string.\r\n");
        return;
    }

    IoTDevice_ConfigureOptions();
    IoTHubDeviceClient_LL_DoWork(__hub_client_handle__);
    ThreadAPI_Sleep(500);

    IoTHubDeviceClient_LL_GetTwinAsync(__hub_client_handle__, IoTDevice_DeviceTwinCallback, NULL);
}

bool IoTDevice_IsConnected(){
  IoTHubDeviceClient_LL_DoWork(__hub_client_handle__);
  return clientConnected && __twin_received__;
}

void IoTDevice_ConnectFromConfiguration() {
    DeviceConfiguration_Init();
    BlinkLed_Init();

    const char *ssid = DeviceConfiguration_Get("core", "ssid", 128);
    const char *password = DeviceConfiguration_Get("core", "password", 128);

    IoTDevice_ConnectWifi(ssid, password);

    IoTDevice_InitDeviceTime();

    const char *connectionString = DeviceConfiguration_Get("core", "connectionString", 256);
    
    IoTDevice_ConnectToHub(connectionString);
}