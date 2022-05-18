/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GestionWifi.h
 * Author: ale
 *
 * Created on 9 avril 2022, 09:04
 */

#ifndef GESTIONWIFI_H
#define GESTIONWIFI_H

#include <WiFi.h>
#include <Arduino.h>
#define TIMEOUT 5*1000
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
    TaskHandle_t TaskHandle_Gwt;
    static GestionWifi* anchor;
    static void marshall(void *);
    void checkWifiTask();
    char *ssid;
    char *password;
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    wifiMode wMode;
    

};

#endif /* GESTIONWIFI_H */

