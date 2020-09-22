#include <Arduino.h>

/*
  IoTDeviceState.h - Library for manipulating IoT Device for Azure IoT Hub.
*/
#ifndef IoTDeviceState_h
#define IoTDeviceState_h

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <fs.h>

#include "Constants.hpp"

const char* stateFile = (configurationFilePath + String("/") + stateFilePath).c_str();

class IoTDeviceState
{

public: 
  static void reset() {
    if(SPIFFS.exists(stateFile))
    { 
      Serial.println("Removing state file");
      SPIFFS.remove(stateFile);
    }
  }
  static DynamicJsonDocument getState()
  {
    if(!SPIFFS.exists(stateFile))
    {    
      DynamicJsonDocument doc(1024);
      doc["desiredStateDate"] = millis();
      doc["reportedStateDate"] = millis();

      setState(doc);

      return doc;
    }

    fs::File readFile = SPIFFS.open(stateFile, "r");
  
    DynamicJsonDocument doc(10);
    DeserializationError err = deserializeJson(doc, readFile);

    if (err.code() != err.Ok)
    {
      Serial.print("Unable to get state file:");
      Serial.println(err.c_str());
      Serial.println(readFile.readString());
    }

    readFile.close();

    return doc;
  }

  static void setState(DynamicJsonDocument doc)
  {    
    const char* stateFile = (configurationFilePath + String("/") + stateFilePath).c_str();
    String output;

    fs::File writeFile = SPIFFS.open(stateFile, "w");
    serializeJson(doc, output);

    Serial.print("Writing state file:");
    Serial.println(output);
    writeFile.print(output);
    writeFile.close();
  }
};

#endif