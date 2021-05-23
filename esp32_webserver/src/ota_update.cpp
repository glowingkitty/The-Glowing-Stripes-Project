#include <Arduino.h>
#include <ArduinoOTA.h>
#include "HttpsOTAUpdate.h"
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include "stripe_config.h"
#include "serial.h"
#include "device_role.h"

StaticJsonDocument<850> led_strip_info = load_strip_config();
String stripe_id = led_strip_info["0"].as<String>();

DynamicJsonDocument firmware_updates_status(1024);

/////////////////////////////
/// OTA
/////////////////////////////

static const char *url = "https://raw.githubusercontent.com/glowingkitty/The-Glowing-Stripes-Project/master/esp32_webserver/firmware.bin"; //state url of your firmware image

static const char *server_certificate =   "-----BEGIN CERTIFICATE-----\n"
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
"+OkuE6N36B9K\n"
"-----END CERTIFICATE-----\n";

static HttpsOTAStatus_t otastatus;

bool ota_in_progress {false};

void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

void update_firmware_status(String stripe_id, String new_status){
    if(device_is_client()){
        HTTPClient http;   
        
        IPAddress serverIp = MDNS.queryHost("glow");
        http.begin("http://"+serverIp.toString()+"/new_firmware_update_status");
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST("{\"led_strip_id\":\""+stripe_id+"\",\"status\":\""+new_status+"\"}");
        if(httpResponseCode==200){
            Serial.println("Updated firmware status for "+stripe_id);
        }else{
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();  //Free resources
        return;

    } 
    
    // if this LED strip is the host, update the local list of firmware_updates_status
    firmware_updates_status[stripe_id] = new_status;
}

void check_ota_status(){
    // update webserver ESP32 firmware
    if (ota_in_progress){
        otastatus = HttpsOTA.status();
        if(otastatus == HTTPS_OTA_SUCCESS) { 
            Serial.println("Firmware update successfull. Rebooting device...");
            update_firmware_status(stripe_id,"not_updating");
            ESP.restart();
        } else if(otastatus == HTTPS_OTA_FAIL) { 
            Serial.println("Firmware update failed");
            update_firmware_status(stripe_id,"updated_failed");
            ESP.restart();
        }
    }

    // once LEDs ESP32 confirms update is complete, continue with webserver ESP32 update
    else if(led_esp_available()){
        String command = led_esp_received_message();
        if (command.startsWith("firmware_update_success")){
            Serial.println("Updating firmware for webserver ESP32...");
            HttpsOTA.onHttpEvent(HttpEvent);
            HttpsOTA.begin(url, server_certificate); 
            ota_in_progress = true;
        } else if (command.startsWith("firmware_update_failed")){
            // if error returned, skip webserver update and change firmware_update_status to error
            update_firmware_status(stripe_id,"updated_failed");
        }
    }
}