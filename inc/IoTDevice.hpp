#include <Arduino.h>

/*
  IoTDevice.h - Library for manipulating IoT Device for Azure IoT Hub.
*/
#ifndef IoTDevice_h
#define IoTDevice_h

#include <WiFi.h>
#include <AzureIotHub.h>
#include <Esp32MQTTClient.h>
#include <ArduinoJson.h>

#include "OTA.hpp"
#include "Blink.hpp"
#include "DeviceConfiguration.hpp"
#include "Constants.hpp"
#include "IoTDeviceState.hpp"

class IoTDevice
{
public:

  IoTDevice(){
    IoTDeviceState::reset();
  }

  void connect()
  {
    const char *ssid = config.get("core", "ssid", 128);
    const char *password = config.get("core", "password", 128);

    this->connectWifi(ssid, password);

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

private:

  bool connectionStatus;
  OTA DeviceOTA;
  Blink_LED led;
  DeviceConfiguration config;

  void connectHub(const char *connectionString)
  {
    this->connectionStatus = Esp32MQTTClient_Init((const uint8_t *)connectionString, true);

    Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
    Esp32MQTTClient_SetMessageCallback(MessageCallback);
    Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
    Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  }

  void connectWifi(const char *ssid, const char *password)
  {
    Serial.println("Starting connecting WiFi.");
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

  static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
  {
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
    {
    }
  }

  static void MessageCallback(const char *payLoad, int size)
  {
    Serial.println("Message callback:");
    Serial.println(payLoad);
  }

  static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
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

  static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
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

    fs::File file = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "w");
    serializeJson(doc, file);
    file.close();

    StaticJsonDocument<10> state = IoTDeviceState::getState();
    state["desiredStateDate"] = millis();
    IoTDeviceState::setState(state);
  }

  static StaticJsonDocument<defaultTwinSize> getDesiredProperties()
  {
    fs::File file = SPIFFS.open((configurationFilePath + String("/") + twinFilePath).c_str(), "r");

    StaticJsonDocument<defaultTwinSize> doc;
    DeserializationError err = deserializeJson(doc, file);

    StaticJsonDocument<10> state = IoTDeviceState::getState();

    if (err.code() != err.Ok)
    {
      Serial.print("Unable to get parse saved twin:");
      Serial.println(err.c_str());
    } 
    else if(state["reportedStateDate"] <= state["desiredStateDate"])
    {
      String output;
      serializeJson(doc, output);
      const char* reportedState = output.c_str();

      Esp32MQTTClient_ReportState(reportedState);
      state["reportedStateDate"] = millis();

      IoTDeviceState::setState(state);
    }

    file.close();

    return doc;
  }
};

#endif