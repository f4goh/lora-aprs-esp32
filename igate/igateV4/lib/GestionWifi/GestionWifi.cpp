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
        WiFi.disconnect();
        delay(100);
        WiFi.onEvent(GestionWifi::WiFiEvent);
        WiFi.begin(ssid, password);
        retCnx = connexion();
        //WiFi.setAutoReconnect(true);
        //WiFi.persistent(true);
        
        //BaseType_t retTsk = xTaskCreatePinnedToCore(GestionWifi::marshall, "gestion_wifi", 10000, NULL, 3, &TaskHandle_Gwt, 0);
        //retCnx = ((retTsk == pdPASS) && retCnx) ? true : false;
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
/*
 * WiFi Events
0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
2  SYSTEM_EVENT_STA_START                < ESP32 station start
3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
25 SYSTEM_EVENT_MAX
 */


void GestionWifi::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {

    switch (event) {
            Serial.printf("[WiFi-event] event: %d\n", event);
        case SYSTEM_EVENT_WIFI_READY:
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            WiFi.setHostname("LoRa_Igate");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            anchor->wifi_connected = false;
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            Serial.print("WiFi lost connection. Reason: ");
            Serial.println(info.disconnected.reason);
            anchor->wifi_connected = false;
            Serial.println("Trying to Reconnect");
            WiFi.begin(anchor->ssid, anchor->password);
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("ETH MAC: ");
            Serial.print(WiFi.macAddress());
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            anchor->wifi_connected = true;
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
 }


bool GestionWifi::connected(){
    return (WiFi.status() == WL_CONNECTED);
}

String GestionWifi::getIP(){
    return String(WiFi.localIP());
}

// est-ce nécéssaire ?

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
/*

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
*/

GestionWifi* GestionWifi::anchor = NULL;


