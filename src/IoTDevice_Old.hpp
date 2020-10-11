/*
  IoTDevice.h - Library for manipulating IoT Device for Azure IoT Hub.
*/
#ifndef IoTDevice_h
#define IoTDevice_h

#include <WiFi.h>
#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>

#include <ArduinoJson.h>
#include <time.h>

#include <apps/sntp/sntp.h>
#include "DeviceConfiguration.h"

#include "OTA.hpp"
#include "Blink.hpp"
#include "Constants.hpp"

class IoTDevice
{
public:

  IoTDevice(){
    IoTDevice::connectionStatus = false;
  }

  void connect()
  {
    const char *ssid = config.get("core", "ssid", 128);
    const char *password = config.get("core", "password", 128);

    this->connectWifi(ssid, password);

    this->initDeviceTime();

    const char *connectionString = config.get("core", "connectionString", 256);

    this->connectHub(connectionString);
  }

  bool send(DynamicJsonDocument message)
  {
    String output;

    serializeJson(message, output);

    Serial.println("Sending message:");
    Serial.println(output);

    // Construct the iothub message from a string or a byte array
    IOTHUB_MESSAGE_HANDLE messageHandle  = IoTHubMessage_CreateFromString(output.c_str());

    IoTHubDeviceClient_LL_SendEventAsync(__hub_client_handle__, messageHandle, sendConfirmationCallback, NULL);

    // The message is copied to the sdk so the we can destroy it
    IoTHubMessage_Destroy(messageHandle);
  }

  bool isConnected()
  {
    return this->connectionStatus;
  }

  StaticJsonDocument<defaultTwinSize> getProperties()
  {
    return getDesiredProperties();
  }

private:
  Blink_LED led;
  bool connectionStatus;
  OTA DeviceOTA;
  DeviceConfiguration config;
  IOTHUB_CLIENT_LL_HANDLE __hub_client_handle__;
  int receiveContext;
  
  #ifdef PROTOCOL_MQTT
  IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;
  #endif // PROTOCOL_MQTT

  #ifdef PROTOCOL_HTTP
  IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = HTTP_Protocol;
  #endif // PROTOCOL_HTTP

  void initDeviceTime()
  {       
    Serial.println("Initializing Device Time");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "192.168.10.1");
    sntp_init();
    time_t ts = 0;

    // Before 1980 is uninitialized
    while (ts < 10 * 365 * 24 * 3600)
    {
      this->led.blink();
      this->led.blink();
      
      delay(1000);
      ts = time(NULL);
    }
    Serial.println("SNTP initialization complete");
    return;
  }

  void connectHub(const char *connectionString)
  {
    Serial.println("Connecting the client");
    __hub_client_handle__ = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol);

    // Used to initialize IoTHub SDK subsystem
    (void)IoTHub_Init();

    if (__hub_client_handle__  == NULL)
    {
        Serial.printf("Error AZ002: Failure createing Iothub device. Hint: Check you connection string.\n");
    }

    Serial.printf("Connected to Iothub.\n");
    Serial.printf("Connecting to the device...\n");

    //IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_TRUSTED_CERT, certificates);
  }

  void connectWifi(const char *ssid, const char *password)
  {
    Serial.print("Starting connecting WiFi: ");
    Serial.println(ssid);
    this->led.high();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    this->led.low();
  }

  /* -- ConfigureOptions --
   * Configures the LL Device client options.
   */
  void ConfigureOptions()
  {
      // Set any option that are neccessary.
      // For available options please see the iothub_sdk_options.md documentation
      int diagnosticsSamplingPercent = 0;
      IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diagnosticsSamplingPercent);

      // Setting the Trusted Certificate.
      //IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_TRUSTED_CERT, certificates);

#ifdef PROTOCOL_MQTT || defined PROTOCOL_MQTT_WS
        //Setting the auto URL Encoder (recommended for MQTT). Please use this option unless
        //you are URL Encoding inputs yourself.
        //ONLY valid for use with MQTT
        bool autoUrlEncode = true;
        IoTHubDeviceClient_LL_SetOption(__hub_client_handle__, OPTION_AUTO_URL_ENCODE_DECODE, &autoUrlEncode);
#endif

      if (IoTHubClient_LL_SetMessageCallback(__hub_client_handle__, IoTDevice::messageCallback, &this->receiveContext) != IOTHUB_CLIENT_OK)
      {
          Serial.printf("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
      }
      else 
      {
         Serial.printf("MessageCallback set.\n");
      }
      
      (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(__hub_client_handle__, IoTDevice::connection_status_callback, NULL);
    }

  static void connnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason){
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED){

    }
    else{
      Serial.println("Device is not connected");
    }
  }

  static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
  {
      (void)reason;
      (void)user_context;
      // This sample DOES NOT take into consideration network outages.
      if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
      {
          (void)printf("The device client is connected to iothub\r\n");
      }
      else
      {
          (void)printf("The device client has been disconnected\r\n");
      }
  }

  static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
  {
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
    {
    }
  }

  static IOTHUBMESSAGE_DISPOSITION_RESULT messageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
  {
    // Retrieve properties from the message
    MAP_HANDLE mapProperties = IoTHubMessage_Properties(message);
    if (mapProperties != NULL)
    {
        const char*const* keys;
        const char*const* values;
        size_t propertyCount = 0;
        if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK)
        {
            if (propertyCount > 0)
            {
                printf("Message Properties:\r\n");
                for (size_t index = 0; index < propertyCount; index++)
                {
                    printf("\tKey: %s Value: %s\r\n", keys[index], values[index]);
                }
                printf("\r\n");
            }
        }
    }

    return IOTHUBMESSAGE_ACCEPTED;
  }

  static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
  {
    char *temp = (char *)malloc(size + 1);

    if (temp == NULL)
    {
      return;
    }

    memcpy(temp, payLoad, size);
    temp[size] = '\0';

    Serial.print("Recieving digital twin update: ");
    Serial.println(temp);

    StaticJsonDocument<defaultTwinSize> doc;
    deserializeJson(doc, temp);

    if (doc.containsKey("desired") && doc.containsKey("reported"))
    {
      JsonObject desiredProperties = doc["desired"].as<JsonObject>();
      writeDesiredProperties(desiredProperties);
    }
    else{
      writeDesiredProperties(doc.as<JsonObject>());
    }

    free(temp);
  }

  static int deviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
  {
    Serial.printf("Try to invoke method %s", methodName);
    const char *responseMessage = "\"Successfully invoke device method\"";
    int result = 200;

    if (strcmp(methodName, "start") == 0)
    {
      Serial.printf("Start sending temperature and humidity data");
    }
    else if (strcmp(methodName, "stop") == 0)
    {
      Serial.printf("Stop sending temperature and humidity data");
    }
    else
    {
      Serial.printf("No method %s found", methodName);
      responseMessage = "\"No method found\"";
      result = 404;
    }

    *response_size = strlen(responseMessage) + 1;
    *response = (unsigned char *)strdup(responseMessage);

    return result;
  }

  void writeDesiredProperties(JsonObject doc)
  {
    doc.remove("$version");

    Serial.println("Updating DesiredProperties");
    serializeJson(doc, Serial);

    DynamicJsonDocument stateDoc(1024);
    stateDoc["state"] = doc;

    fs::File file = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "w");
    serializeJson(stateDoc, file);
    file.close();
  }

  StaticJsonDocument<defaultTwinSize> getDesiredProperties()
  {
    fs::File file = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "r");

    StaticJsonDocument<defaultTwinSize> doc;
    DeserializationError err = deserializeJson(doc, file);

    if (err.code() != err.Ok)
    {
      Serial.print("Unable to parse saved twin:");
      Serial.println(err.c_str());
    } 
    else if(!doc.containsKey("reported") || !doc["reported"].as<boolean>())
    {
      String output;
      serializeJson(doc["state"], output);
      const char* reportedState = output.c_str();

      file.close();

      IoTHubClient_LL_SendReportedState(__hub_client_handle__, (const unsigned char*)reportedState, output.length(), reportedStateCallback, NULL);

      doc["reported"] = true;

      fs::File writeFile = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "w");
      serializeJson(doc, writeFile);
      writeFile.close();
    }

    return doc["state"];
  }

  static void reportedStateCallback(int status_code, void* userContextCallback){

  }
};

#endif