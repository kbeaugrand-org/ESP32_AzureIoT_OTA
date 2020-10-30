/**
 * A simple Azure IoT example for sending telemetry.
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SPIFFSIniFile.h>
#include <ESP32_AzureIoT_OTA.h>

#define DEFAULT_TELEMETRY_FREQUENCY 5000

int sensorPin = 33;   

typedef struct TELEMETRY_MSG {
  long random;
  char *sensor;
  int data[];
} TelemetryMessage;

// Cette fonction convertit la valeur analogique lue en température en °C
double Thermistor(int RawADC) {
   double Temp;
   Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
   Log_Trace("log(10000.0 * ((1024.0 / %d - 1))) = %d", RawADC, Temp);
   Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
   Log_Trace("1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp ) = %d", Temp);
   Temp = Temp - 273.15; // conversion de degrés Kelvin en °C
   Log_Trace("Temp - 273.15 = %d", Temp);
   return Temp;
}

DynamicJsonDocument CheckPropertiesState()
{
  bool hasChanged = false;

  DynamicJsonDocument propertiesState = IoTDevice_GetReportedProperties(); 
  DynamicJsonDocument desiredState = IoTDevice_GetDesiredProperties();     
  
  if (desiredState["telemetry"]["frequency"].as<int>() != propertiesState["telemetry"]["frequency"].as<int>())
  {
    propertiesState["telemetry"]["frequency"].set(desiredState["telemetry"]["frequency"].as<int>());
    hasChanged = true;
  }

  if(hasChanged){
    IoTDevice_ReportState(propertiesState);
  }

  return propertiesState;
}

void setup()
{
  Serial.begin(115200);

  IoTDevice_ConnectFromConfiguration();
  pinMode(sensorPin, INPUT);
  randomSeed(analogRead(0));

  // Wait for device connection
  while(!IoTDevice_IsConnected()) { }
}

void loop()
{ 
  int readVal = analogRead(sensorPin);
  double temp = Thermistor(readVal);

  DynamicJsonDocument doc(1024);

  doc["sensor"] = "temp";

  String output;

  serializeJson(doc, output);
  IoTDevice_Send(output.c_str());

  DynamicJsonDocument propertiesState = CheckPropertiesState();
  
  int waitTime = propertiesState["telemetry"]["frequency"] | DEFAULT_TELEMETRY_FREQUENCY;

  Log_Info("Waiting for next value: %ds", waitTime);

  delay(waitTime);
}