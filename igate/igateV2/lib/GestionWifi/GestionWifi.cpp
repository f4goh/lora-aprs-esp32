/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GestionWifi.cpp
 * Author: ale
 * 
 * Created on 9 avril 2022, 09:04
 */

#include "GestionWifi.h"

GestionWifi::GestionWifi() {
}

GestionWifi::GestionWifi(const GestionWifi& orig) {
}

GestionWifi::~GestionWifi() {
}

bool GestionWifi::setup(char* _ssid, char* _password) {
    ssid = _ssid;
    password = _password;
    WiFi.begin(ssid, password);
    return connexion();
}

bool GestionWifi::reconnexion(){
    WiFi.reconnect();
    return connexion();
}

void GestionWifi::deconnexion(){
    WiFi.disconnect();
}


bool GestionWifi::connexion() {
    unsigned long startTime = millis();
    bool ret=false;
    
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= TIMEOUT)
    {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        ip = WiFi.localIP();
        subnet = WiFi.subnetMask();
        gateway = WiFi.gatewayIP();
        Serial.println();
        Serial.println("WiFi connecté");
        Serial.print("Adresse IP : ");
        Serial.println(ip);
        Serial.print("Adresse IP de la gateway : ");
        Serial.println(gateway);
        ret = true;
    }
    return ret;
}




