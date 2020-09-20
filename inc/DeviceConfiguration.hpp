#include <Arduino.h>

/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef DeviceConfiguration_h
#define DeviceConfiguration_h

#include <stdio.h>
#include <stdlib.h>   
#include <SPIFFSIniFile.h>

const char *configurationFileName = "config.ini";
const char *configurationFilePath = "/device";

class DeviceConfiguration
{
public:
    static void init()
    {
        if (!SPIFFS.begin(false, configurationFilePath))
        {
            Serial.println("Mount Failed");
            return;
        }

        Serial.println("Used " + String(SPIFFS.usedBytes()) + " over " + String(SPIFFS.totalBytes()) + "bytes.");
        Serial.println("File system mounted");
    }

    const char* get(const char *section, const char *key, const size_t max_size)
    {
        char* buffer = (char*) malloc(max_size + 1);

        const char* filename = (configurationFilePath + String("/") + configurationFileName).c_str();

        SPIFFSIniFile file(filename);

        if (!file.open())
        {
            Serial.print("Ini file ");
            Serial.print(configurationFileName);
            Serial.println(" does not exist");

            return "";
        }

        if (!file.getValue(section, key, buffer, max_size))
        {
            Serial.println("Section '" + String(section) + "' does not have an entry '" + String(key) + "'!");
            return "";
        }

        return buffer;
    }

private:
};

#endif