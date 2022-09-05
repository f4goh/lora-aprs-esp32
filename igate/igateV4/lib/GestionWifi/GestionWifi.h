/* 
 * File:   GestionWifi.h
 * Author: ale
 *
 * Created on 9 avril 2022, 09:04
 * https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#10
 * https://randomnerdtutorials.com/esp32-wifimulti/
 */

#ifndef GESTIONWIFI_H
#define GESTIONWIFI_H

#include <WiFi.h>
#include <Arduino.h>
#define TIMEOUT 10*1000
//#define TIME_RECONNECT 10*1000

enum wifiMode{
    ACCES_POINT,
    CLIENT
};

class GestionWifi {
public:
    GestionWifi();
    GestionWifi(const GestionWifi& orig);
    virtual ~GestionWifi();
    
    bool setup(const char* _ssid,const char* _password, wifiMode _wMode);
    bool connexion();
    
    bool connected();
    String getIP();

private:
    static void WiFiEvent(WiFiEvent_t event,WiFiEventInfo_t info);
    
    TaskHandle_t TaskHandle_Gwt;
    static GestionWifi* anchor;
    //static void marshall(void *);
    //void checkWifiTask();
    char *ssid;
    char *password;
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    wifiMode wMode;
    bool wifi_connected;
     

};

#endif /* GESTIONWIFI_H */

