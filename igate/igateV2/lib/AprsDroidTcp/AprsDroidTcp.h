/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AprsDroidTcp.h
 * Author: ale
 *
 * Created on 11 avril 2022, 11:28
 */




#ifndef APRSDROIDTCP_H
#define APRSDROIDTCP_H

#include <Arduino.h>
#include <WiFi.h>
#include<WiFiServer.h>


#define MSG_MAX_LENGTH 100
#define SERVER_PORT 14580


class AprsDroidTcp {
public:
    AprsDroidTcp();
    AprsDroidTcp(const AprsDroidTcp& orig);
    virtual ~AprsDroidTcp();
    bool setup();
    void send(char* msg);
    int getPduLength();
    static QueueHandle_t queueRx;

private:
    TaskHandle_t TaskHandle_Rxbt;
    void CheckForConnections();
    void receive();
    static AprsDroidTcp* anchor;
    static void marshall(void *);
    
    WiFiServer server= WiFiServer(SERVER_PORT);
    WiFiClient remoteClient;
    
  
    
    char msg[MSG_MAX_LENGTH];
    int     pduLength;

};

#endif /* APRSDROIDTCP_H */

  
