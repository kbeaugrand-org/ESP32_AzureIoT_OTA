#include <Arduino.h>

#include <apps/sntp/sntp.h>

/*
  IoTDevice.h - Library for manipulating IoT Device for Azure IoT Hub.
*/
#ifndef IoTDevice_h
#define IoTDevice_h

#include <WiFi.h>
#include <AzureIotHub.h>
#include <ArduinoJson.h>

#include "OTA.hpp"
#include "Blink.hpp"
#include "DeviceConfiguration.hpp"
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

    boolean send = Esp32MQTTClient_SendEvent(output.c_str());

    if (send)
    {
      this->led.blink();
      return true;
    }

    return false;
  }

  bool isConnected()
  {
    return this->connectionStatus;
  }

  StaticJsonDocument<defaultTwinSize> getProperties()
  {
    return getDesiredProperties();
  }

  // void setDefaultProperties(StaticJsonDocument<defaultTwinSize> defaults){
  //   this->writeDesiredProperties(defaults);
  // }

private:
  Blink_LED led;
  bool connectionStatus;
  OTA DeviceOTA;
  DeviceConfiguration config;

  void initDeviceTime()
  {       
    LogInfo("Initializing Device Time");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "192.168.10.1");
    sntp_init();
    time_t ts = 0;

    // Before 1980 is uninitialized
    while (ts < 10 * 365 * 24 * 3600)
    {
      this->led.blink();
      this->led.blink();
      
      ThreadAPI_Sleep(1000);
      ts = get_time(NULL);
    }
    LogInfo("SNTP initialization complete");
    return;
  }

  void connectHub(const char *connectionString)
  {
    LogInfo("Connecting the client");
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol);

    Esp32MQTTClient_Init((const uint8_t *)connectionString, true);
  
    Esp32MQTTClient_SetConnectionStatusCallback(connnectionStatusCallback);

    Esp32MQTTClient_SetSendConfirmationCallback(sendConfirmationCallback);
    Esp32MQTTClient_SetMessageCallback(messageCallback);
    Esp32MQTTClient_SetDeviceTwinCallback(deviceTwinCallback);
    Esp32MQTTClient_SetDeviceMethodCallback(deviceMethodCallback);
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

  static void connnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason){
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED){

    }
    else{
      LogError("Device is not connected");
    }
  }

  static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
  {
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
    {
    }
  }

  static void messageCallback(const char *payLoad, int size)
  {
    Serial.println("Message callback:");
    Serial.println(payLoad);
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
    LogInfo("Try to invoke method %s", methodName);
    const char *responseMessage = "\"Successfully invoke device method\"";
    int result = 200;

    if (strcmp(methodName, "start") == 0)
    {
      LogInfo("Start sending temperature and humidity data");
    }
    else if (strcmp(methodName, "stop") == 0)
    {
      LogInfo("Stop sending temperature and humidity data");
    }
    else
    {
      LogInfo("No method %s found", methodName);
      responseMessage = "\"No method found\"";
      result = 404;
    }

    *response_size = strlen(responseMessage) + 1;
    *response = (unsigned char *)strdup(responseMessage);

    return result;
  }

  static void writeDesiredProperties(JsonObject doc)
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

  static StaticJsonDocument<defaultTwinSize> getDesiredProperties()
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

      doc["reported"] = Esp32MQTTClient_ReportState(reportedState);

      fs::File writeFile = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "w");
      serializeJson(doc, writeFile);
      writeFile.close();
    }

    return doc["state"];
  }
};

#endif