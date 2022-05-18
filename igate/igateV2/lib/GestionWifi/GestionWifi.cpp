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
     anchor = this;
}

GestionWifi::GestionWifi(const GestionWifi& orig) {
}

GestionWifi::~GestionWifi() {
}

bool GestionWifi::setup(const char* _ssid, const char* _password, wifiMode _wMode) {
    ssid = strdup(_ssid);
    password = strdup(_password);
    wMode = _wMode;
    bool retCnx;
    if (wMode == CLIENT) {
        Serial.println("Client Wifi");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        retCnx = connexion();
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
        BaseType_t retTsk = xTaskCreatePinnedToCore(GestionWifi::marshall, "gestion_wifi", 10000, NULL, 3, &TaskHandle_Gwt, 0);
        retCnx = ((retTsk == pdPASS) && retCnx) ? true : false;
    } else {
        Serial.println("Access point Wifi");
        WiFi.mode(WIFI_AP);
        retCnx = WiFi.softAP(ssid, password);
        if (retCnx) {
            Serial.print("[+] AP Created with IP Gateway ");
            Serial.println(WiFi.softAPIP());
            Serial.println("\n[*] Creating AP");
        }
    }
    return retCnx;
}

bool GestionWifi::connected(){
    return (WiFi.status() == WL_CONNECTED);
}

String GestionWifi::getIP(){
    return String(WiFi.localIP());
}



bool GestionWifi::connexion() {
    unsigned long startTime = millis();
    bool ret=false;
    
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < TIMEOUT)
    {
        delay(500);
        Serial.print(".");
        if ((millis() - startTime) >= TIMEOUT) {
            Serial.println("Resetting due to Wifi not connecting...");
            ESP.restart();
        }
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

void GestionWifi::marshall(void * parametres) {
    anchor->checkWifiTask();
}

void GestionWifi::checkWifiTask() {
    Serial.println("Tache Wifi en fonctionnement");
    while (1) {
        vTaskDelay(1);
        //cnxState = false;
        if (WiFi.status() != WL_CONNECTED) {            
            Serial.println("Reconnecting to WiFi...");
            WiFi.reconnect();
            connexion();
        }
    }
}


GestionWifi* GestionWifi::anchor = NULL;


