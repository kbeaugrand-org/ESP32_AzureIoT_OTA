/**
 * A simple Azure IoT example for sending telemetry.
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SPIFFSIniFile.h>
#include <ESP32_AzureIoT_OTA.h>

#define DEFAULT_TELEMETRY_FREQUENCY 5000

#define FIRMWARE_VERSION "0.0.2"

int sensorPin = 34;   

// Cette fonction convertit la valeur analogique lue en température en °C
double Thermistor() {
  int RawADC = analogRead(sensorPin);
  double Temp;

  Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;            // conversion de degrés Kelvin en °C
  
  return Temp;
}

DynamicJsonDocument CheckPropertiesState()
{
  bool hasChanged = false;

  DynamicJsonDocument propertiesState = IoTDevice_GetReportedProperties(); 
  DynamicJsonDocument desiredState = IoTDevice_GetDesiredProperties();   

  if (propertiesState["firmware"]["version"] != FIRMWARE_VERSION)
  {
    propertiesState["firmware"]["version"] = FIRMWARE_VERSION;
    hasChanged = true;
  }  

  if (propertiesState["telemetry"]["frequency"].as<int>() != desiredState["telemetry"]["frequency"].as<int>())
  {
    propertiesState["telemetry"]["frequency"].set(desiredState["telemetry"]["frequency"].as<int>());
    hasChanged = true;
  }

  if(hasChanged){
    IoTDevice_ReportState(propertiesState);
  }

  if (propertiesState["firmware"]["version"] != desiredState["firmware"]["version"])
  {
    IoTDevice_StartOTA();
  }

  return propertiesState;
}

void setup()
{
  Serial.begin(115200);

  IoTDevice_ConnectFromConfiguration();
  randomSeed(analogRead(0));

  // Wait for device connection
  while(!IoTDevice_IsConnected()) { }
}

void loop()
{ 
  DynamicJsonDocument propertiesState = CheckPropertiesState();
  int waitTime = propertiesState["telemetry"]["frequency"] | DEFAULT_TELEMETRY_FREQUENCY;

  double temp = Thermistor();

  DynamicJsonDocument doc(1024);

  doc["sensor"] = "temp";
  doc["unit"]   = "C";
  doc["value"] = temp;

  String output;

  serializeJson(doc, output);
  IoTDevice_Send(output.c_str());
  
  Log_Info("Waiting for next value: %ds", waitTime);
  delay(waitTime);
}