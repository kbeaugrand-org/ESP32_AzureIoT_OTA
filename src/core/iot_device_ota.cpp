#include <Update.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <Update.h>

#include "logging.h"
#include "iot_device_ota.h"
#include "iot_device_twin.h"

WiFiClient client;
int contentLength;
bool isValidContentType;

// Utility to extract header value from headers
static const char *getHeaderValue(String header, const char *headerName)
{   
    int prefixLength = strlen(headerName);
    String headerValue = header.substring(prefixLength);

    char *result = (char *) malloc(headerValue.length()+1);
    strcpy(result, headerValue.c_str());

    return (const char *)result;
}

static void OTA(const char *host, int port, const char *bin)
{
    Log_Debug("Connecting to: %s", host);

    if (client.connect(host, port))
    {
        // Connection Succeed.
        // Fecthing the bin
        Log_Debug("Fetching Bin: %s", bin);

        // Get the contents of the bin file
        client.print(String("GET ") + String((char *) bin) + " HTTP/1.1\r\n" +
                     "Host: " + String((char *) host) + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 5000)
            {
                Log_Error("Client Timeout !");
                client.stop();
                return;
            }
        }

        while (client.available())
        {
            // read line till /n
            String line = client.readStringUntil('\n');

            // remove space, to check if the line is end of headers
            line.trim();
            // if the the line is empty,
            // this is end of headers
            // break the while and feed the
            // remaining `client` to the
            // Update.writeStream();
            if (!line.length())
            {
                //headers ended
                break; // and get the OTA started
            }
            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                   Log_Error("Got a non 200 status code from server. Exiting OTA Update.");
                   break;
                }
            }
            // extract headers here
            // Start with content length
            if (line.startsWith("Content-Length: "))
            {
                const char *contentLengthValue = getHeaderValue(line, "Content-Length: ");
                contentLength = atol(contentLengthValue);
                Log_Debug("Got %d bytes from server", contentLength);
            }
            // Next, the content type
            if (line.startsWith("Content-Type: "))
            {
                const char *contentType = getHeaderValue(line, "Content-Type: ");
                Log_Debug("Got %s payload.", contentType);

                if (strcmp(contentType, "application/octet-stream") == 0)
                {
                    isValidContentType = true;
                }
            }
        }
    }
    else
    {
        Log_Error("Connection to %s failed. Please check your setup", host);
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Log_Debug("contentLength : %d, isValidContentType : %d", contentLength, isValidContentType);

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);
        // If yes, begin
        if (canBegin)
        {
            Log_Info("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
            size_t written = Update.writeStream(client);
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
            }
        }
        else
        {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            Log_Error("Not enough space to begin OTA");
            client.flush();
        }
    }
    else
    {
        Log_Error("There was no content in the response");
        client.flush();
    }
}


void IoTDevice_StartOTA()
{
    DynamicJsonDocument desiredProperties = IoTDevice_GetDesiredProperties();

    const char *hostname;
    const char *binPath;
    int port;


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

    if(!desiredProperties["firmware"]["source"].containsKey("hostname"))
    {
        Log_Error("Missing hostname from firmware.source device twin desired properties.");
        return;
    }
    else
    {
        hostname = desiredProperties["firmware"]["source"]["hostname"];
    }
    
    if(!desiredProperties["firmware"]["source"].containsKey("binPath"))
    {
        Log_Error("Missing binPath from firmware.source device twin desired properties.");
        return;
    }
    else
    {
        binPath = desiredProperties["firmware"]["source"]["binPath"];
    }

    if(!desiredProperties["firmware"]["source"].containsKey("port"))
    {
        Log_Error("Missing port from firmware.source device twin desired properties.");
        return;
    }
    else
    {
        port = desiredProperties["firmware"]["source"]["port"];
    }

    OTA(hostname, port, binPath);
}