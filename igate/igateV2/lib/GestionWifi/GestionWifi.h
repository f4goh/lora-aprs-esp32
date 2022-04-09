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
#define TIMEOUT 5000


class GestionWifi {
public:
    GestionWifi();
    GestionWifi(const GestionWifi& orig);
    virtual ~GestionWifi();
    
    bool setup(char* _ssid,char* _password);
    bool reconnexion();
    void deconnexion();    
    bool connexion();
    

private:
    char *ssid;
    char *password;
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;

};

#endif /* GESTIONWIFI_H */

