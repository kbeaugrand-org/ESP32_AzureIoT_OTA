// #include <SPIFFSIniFile.h>
// #include "SPIFFS.h"

// const char certificates[] =
//     /* DigiCert Baltimore Root --Used Globally--*/
//     // This cert should be used when connecting to Azure IoT on the Azure Cloud available globally. When in doubt, use this cert.
//     "-----BEGIN CERTIFICATE-----\r\n"
//     "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
//     "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
//     "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
//     "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
//     "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
//     "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
//     "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
//     "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
//     "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
//     "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
//     "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
//     "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
//     "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
//     "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
//     "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
//     "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
//     "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
//     "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
//     "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
//     "-----END CERTIFICATE-----\r\n";

// void printErrorMessage(uint8_t e, bool eol = true)
// {
//   switch (e) {
//   case SPIFFSIniFile::errorNoError:
//     Serial.print("no error");
//     break;
//   case SPIFFSIniFile::errorFileNotFound:
//     Serial.print("file not found");
//     break;
//   case SPIFFSIniFile::errorFileNotOpen:
//     Serial.print("file not open");
//     break;
//   case SPIFFSIniFile::errorBufferTooSmall:
//     Serial.print("buffer too small");
//     break;
//   case SPIFFSIniFile::errorSeekError:
//     Serial.print("seek error");
//     break;
//   case SPIFFSIniFile::errorSectionNotFound:
//     Serial.print("section not found");
//     break;
//   case SPIFFSIniFile::errorKeyNotFound:
//     Serial.print("key not found");
//     break;
//   case SPIFFSIniFile::errorEndOfFile:
//     Serial.print("end of file");
//     break;
//   case SPIFFSIniFile::errorUnknownError:
//     Serial.print("unknown error");
//     break;
//   default:
//     Serial.print("unknown error value");
//     break;
//   }
//   if (eol)
//     Serial.println();
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(2, OUTPUT);  
  
//   Serial.println("Formating the SPIFFS partition");
//   SPIFFS.format();

//   // Launch SPIFFS file system 
//   if(!SPIFFS.begin(false, "/device")){
//     Serial.println("An Error has occurred while mounting SPIFFS");
//     return;
//   }

//   const size_t bufferLen = 80;
//   char buffer[bufferLen];

//   const char *certFileName = "/device/ca.pem";
//   File writerCA = SPIFFS.open(certFileName, "w");
//   writerCA.print(certificates);
//   writerCA.close();

//   const char *filename = "/device/config.ini";

//   File writerFile = SPIFFS.open(filename, "w");

//   writerFile.println("[core]");
//   writerFile.println("ssid = Bbox-62FFD20C");         
//   writerFile.println("password = WZMxHnSG33zyd3uMhH");     
//   writerFile.println("connectionString = HostName=dm-explore-iothub-weu.azure-devices.net;DeviceId=esp32_000001;SharedAccessKey=6ndjQZIXxi0d5E8rK9sU5RIi8FK5UMi7vtAKp8YTYZY=");      
//   writerFile.println("caPath = " + String(certFileName));       

//   writerFile.println("[ntp]");
//   writerFile.println("hostname = pool.ntp.org");
    
//   writerFile.close();
  
//   Serial.printf("Used %d over %d bytes\n", SPIFFS.usedBytes(), SPIFFS.totalBytes()); 
//   digitalWrite(2, HIGH);
// }
 
// void loop() {
  
// }