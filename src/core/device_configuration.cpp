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

static SPIFFSIniFile *__DeviceConfigurationFile__;

static void DeviceConfiguration_OpenAndValidate()
{
    char configFileName[128]; 

    strcpy(configFileName, CONFIGURATION_FILE_PATH);
    strcat(configFileName, CONFIGURATION_FILENAME);

    __DeviceConfigurationFile__ = new SPIFFSIniFile(configFileName);

    Log_Debug("Opening file %s", configFileName);

    if (!__DeviceConfigurationFile__->open())
    {
        Log_Error("Ini file %s does not exist", configFileName);
    }

    char *buffer = (char *) malloc(512);

    if (!__DeviceConfigurationFile__->validate(buffer, 512))
    {
        Log_Error("Ini file %s contains errors\n %s", configFileName, buffer);
        free(buffer);
    }
}

static const char *DeviceConfiguration_GetErrorText(){
    SPIFFSIniFile::error_t error= __DeviceConfigurationFile__->getError();
        char *errorText;

        switch (error)
        {
        case SPIFFSIniFile::error_t::errorNoError:
            return "No Error";
            break;
        case SPIFFSIniFile::error_t::errorFileNotFound:
            return "File not found";
            break;
        case SPIFFSIniFile::error_t::errorFileNotOpen:
            return "File not open";
            break;
        case SPIFFSIniFile::error_t::errorBufferTooSmall:
            return "Buffer too small";
            break;        
        case SPIFFSIniFile::error_t::errorSeekError:
            return "Seek error";
            break;        
        case SPIFFSIniFile::error_t::errorSectionNotFound:
            return "Section not found";
            break;        
        case SPIFFSIniFile::error_t::errorKeyNotFound:
            return "Key not found";
            break;      
        case SPIFFSIniFile::error_t::errorEndOfFile:
            return "End of file";
            break;      
        case SPIFFSIniFile::error_t::errorUnknownError:
            return "Unknown error";
            break;
        
        default:
            break;
        }
}

void DeviceConfiguration_Init() {
    Log_Debug("Mounting SPIFFS file system: %s", CONFIGURATION_FILE_PATH);

    if (!SPIFFS.begin(false, CONFIGURATION_FILE_PATH))
    {
        Log_Error("Mount Failed");
        return;
    }

    Log_Debug("Used %d over %d bytes", SPIFFS.usedBytes(), SPIFFS.totalBytes());
    Log_Info("File system mounted");

    DeviceConfiguration_OpenAndValidate();
}

const char *DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size)
{
    Log_Trace("Getting [%s][%s]", section, key);

    char *buffer = (char *) malloc(max_size + 1);

    if (!__DeviceConfigurationFile__->getValue(section, key, buffer, max_size))
    {
        Log_Error("Cannot get [%s][%s] due to error %s", section, key, DeviceConfiguration_GetErrorText());

        return "";
    }

    Log_Debug("[%s][%s] = %s", section, key, buffer);

    return buffer;
}

void DeviceConfiguration_WriteFileContent(const char *filePath, const char *content, size_t size)
{
    Log_Trace("%s opening for writing", filePath);

    fs::File file = SPIFFS.open(filePath, "w");

    if (file == NULL)
    {
        Log_Error("Error could not open file %s for writing\n", filePath);
        return;
    }

    size_t written = file.print(content);

    if (written != size)
    {
        Log_Error("Error when writing file. Only %d bytes, specified %d bytes", written, size);
    }

    file.close();
}

const char *DeviceConfiguration_GetFileContent(const char *filePath, size_t *size)
{
    Log_Trace("%s opening for reading.", filePath);

    fs::File file = SPIFFS.open(filePath, "r");

    if (file == NULL)
    {
        Log_Error("Error could not open file %s for reading\n", filePath);
        return NULL;
    }

    *size = file.available();
    char *content = (char *)malloc(*size + 1);
    
    if(content == NULL)
    {
        Log_Error("Not enough memory to allocate %d bytes.", size);
        file.close();
        return NULL;
    }
    
    *size = file.readBytes(content, *size);

    file.close();

    return content;
}