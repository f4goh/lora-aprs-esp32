/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Aprsis.h
 * Author: ale
 *
 * Created on 8 avril 2022, 19:04
 */

#ifndef APRSIS_H
#define APRSIS_H

#include <WiFi.h>
#include <Arduino.h>
#define TOOL_NAME "ESP32-APRS-IS"
#define VERSION "1.0"
#define MSG_MAX_LENGTH 100
#define APRS_SERVER "euro.aprs2.net"
#define APRS_PORT 14580
#define BEACON_DELAY 15*60*1000  //15 minutes


class Aprsis {
public:
    Aprsis();
    Aprsis(const Aprsis& orig);
    virtual ~Aprsis();
    bool setup(const String _user,const String _passcode,const String _latitude,const String _longitude,const String _message);

    bool connect(const String server, const int port);
    bool connect(const String server, const int port, const String filter);
    bool connected();

    bool sendMessage(const String message);
    int available();

    String getMessage();
    String getAPRSMessage();
    bool getCnxState();
    

    static QueueHandle_t queueMsg;
    
private:
    TaskHandle_t TaskHandle_Ait;
    static Aprsis* anchor;
    static void marshall(void *);
    void aprsIsTask();
    bool _connect(const String server, const int port, const String login_line);
    bool sendBeacon();
    void connexion();
    
    String user;
    String passcode;
    String tool_name;
    String version;
    String longitude;
    String latitude;
    String message;
    WiFiClient client;
    char msg[MSG_MAX_LENGTH];
    bool cnxState;
};

#endif /* APRSIS_H */
