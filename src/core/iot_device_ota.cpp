#include <Update.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include <HTTPClient.h>
#include <Update.h>

#include "logging.h"
#include "device_configuration.h"
#include "iot_device_core.h"
#include "iot_device_ota.h"
#include "iot_device_twin.h"

HTTPClient http;
int contentLength;

static void OTA(const char *uri)
{
    const char *certPath = DeviceConfiguration_Get("core", "caPath", 256);
    size_t certificatesLen;

    const char *root_ca = DeviceConfiguration_GetFileContent(certPath, &certificatesLen);

    Log_Debug("Begin request for: %s", uri);

    http.begin(uri, root_ca); 
    int statusCode = http.GET();

    if (statusCode <= 0)
    {
        Log_Error("status code is :%d. Exiting OTA!", statusCode);
        http.end();
        return;
    }
       
    int contentLength = http.getSize();

    Log_Debug("Trying to start OTA with %d bytes", contentLength);

    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin)
    {
        Log_Info("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
        // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
        size_t written = Update.writeStream(http.getStream());
        
        if (written == contentLength)
        {
            Log_Info("Written: %d successfully", written);
        }
        else
        {
            Log_Error("Written only: %d/%d. Retry?", written, contentLength);
        }
        
        if (Update.end())
        {
            Log_Info("OTA done!");
            if (Update.isFinished())
            {
                Log_Info("Update successfully completed. Rebooting.");
                ESP.restart();
            }
            else
            {
                Log_Info("Update not finished? Something went wrong!");
            }
        }
        else
        {
            Log_Error("Error Occurred. Error #%d", Update.getError());
            http.end();
        }
    }
    else
    {
        // not enough space to begin OTA
        // Understand the partitions and
        // space availability
        Log_Error("Not enough space to begin OTA");
        http.end();
    }
}


void IoTDevice_StartOTA()
{
    DynamicJsonDocument desiredProperties = IoTDevice_GetDesiredProperties();

    if(!desiredProperties.containsKey("firmware"))
    {
        Log_Error("Cannot create firmware update since firmware is not set to device twin.");
        return;
    }

    if(!desiredProperties["firmware"].containsKey("source"))
    {
        Log_Error("Missing source from firmware device twin desired properties.");
        return;
    }

    OTA(desiredProperties["firmware"]["source"]);
}