/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#include <stdio.h>
#include <stdlib.h>   
#include <SPIFFS.h>
#include <SPIFFSIniFile.h>

#include "logging.h"
#include "constants.h"
#include "device_configuration.h"

#ifndef CONFIGURATION_FILENAME
#define CONFIGURATION_FILENAME "config.ini"
#endif

void DeviceConfiguration_Init() {
    
    if (!SPIFFS.begin(false, CONFIGURATION_FILE_PATH))
    {
        Log_Error("Mount Failed");
        return;
    }

    Log_Info("Used %s over %s bytes.", SPIFFS.usedBytes(), SPIFFS.totalBytes());
    Log_Info("File system mounted");
}

const char*  DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size)
{
    char* buffer = (char*) malloc(max_size + 1);
    const char* filename = (CONFIGURATION_FILE_PATH + String("/") + CONFIGURATION_FILENAME).c_str();
    SPIFFSIniFile file(filename);

    if (!file.open())
    {
        Log_Error("Ini file %s does not exist.", CONFIGURATION_FILENAME);
        return "";
    }

    if (!file.getValue(section, key, buffer, max_size))
    {
        Log_Error("Section '%s' does not have an entry '%s'!", section, key);
        return "";
    }

    return buffer;
}