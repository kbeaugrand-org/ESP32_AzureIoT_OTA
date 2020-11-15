#include <ArduinoJson.h>

#include <AzureIoTHub.h>
#include <SPIFFS.h>

#include "logging.h"
#include "iot_device_core.h"
#include "iot_device_method.h"

struct MethodCallback {
    const char* methodName;
    IOT_DEVICE_METHOD_CALLBACK callback;
};

static MethodCallback methodCallbacks[IOT_DEVICE_MAX_METHOD_COUNT];
static int registeredCount = 0;

static int IoTDevice_DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
    (void)userContextCallback;
    (void)payload;
    (void)size;

    bool methodFound = false;

    int result;

    Log_Debug("Direct method lookup: %s", method_name);

    for(int i = 0; i < registeredCount; i++) 
    {
        Log_Trace("Trying %s", methodCallbacks[i].methodName);

        if (strcmp(methodCallbacks[i].methodName, method_name) == 0)
        {
            Log_Debug("Method callback for %s found. Executing", method_name);
        
            methodFound = true;
            result = methodCallbacks[i].callback(payload, size, response, response_size);        
        }        
    }

    if (!methodFound)
    {
        Log_Info("Method %s not registered!", method_name);
        const char deviceMethodResponse[] = "{ }";
        *response_size = sizeof(deviceMethodResponse)-1;
        *response = (unsigned char *)malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);
        result = -1;
    }

    return result;
}

int OnSystemInfoInvoked(const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size)
{    
    String output;

    DynamicJsonDocument doc(1024);

    doc["chipRevision"] = ESP.getChipRevision();
    doc["cpuFreqMHz"] = ESP.getCpuFreqMHz();
    doc["cycleCount"] = ESP.getCycleCount();
    doc["sdkVersion"] = ESP.getSdkVersion();

    doc["flashChip"]["size"] = ESP.getFlashChipSize();
    doc["flashChip"]["speed"] = ESP.getFlashChipSpeed();

    doc["heap"]["size"] = ESP.getHeapSize(); //total heap size
    doc["heap"]["free"] = ESP.getFreeHeap(); //available heap
    doc["heap"]["minFree"] = ESP.getMinFreeHeap(); //lowest level of free heap since boot
    doc["heap"]["maxAlloc"] = ESP.getMaxAllocHeap();  //largest block of heap that can be allocated at once

    doc["spi"]["size"] = ESP.getPsramSize();
    doc["spi"]["free"] = ESP.getFreePsram();
    doc["spi"]["minFree"] = ESP.getMinFreePsram();
    doc["spi"]["maxAlloc"] = ESP.getMaxAllocPsram();

    doc["sketch"]["MD5"] = ESP.getSketchMD5();
    doc["sketch"]["size"] = ESP.getSketchSize();
    doc["sketch"]["space"] = ESP.getFreeSketchSpace();

    doc["fs"]["usedBytes"] =  SPIFFS.usedBytes();
    doc["fs"]["totalBytes"] = SPIFFS.totalBytes();

    serializeJsonPretty(doc, output);

    const char *deviceMethodResponse = output.c_str();
    *response_size = strlen(deviceMethodResponse);
    *response = (unsigned char *)malloc(*response_size);
    (void)memcpy(*response, deviceMethodResponse, *response_size);

    return 200;
}

void IoTDevice_RegisterDeviceMethodCallback(){
    (void)IoTHubClient_LL_SetDeviceMethodCallback(__hub_client_handle__, IoTDevice_DeviceMethodCallback, NULL);
    
    IoTDevice_RegisterForMethodInvoke("systemInfo", OnSystemInfoInvoked);
}

void IoTDevice_RegisterForMethodInvoke(const char *methodName, IOT_DEVICE_METHOD_CALLBACK callback) {
    Log_Debug("Registering method %s at index: %d", methodName, registeredCount);

    if(registeredCount >= IOT_DEVICE_MAX_METHOD_COUNT)
    {
        Log_Error("Cannot regiter more than %d methods. Increase the IOT_DEVICE_MAX_METHOD_COUNT variable.", IOT_DEVICE_MAX_METHOD_COUNT);
        return;
    }

    MethodCallback item = {
        .methodName = methodName,
        .callback = callback
    };

    methodCallbacks[registeredCount] = item;
    registeredCount++;
}