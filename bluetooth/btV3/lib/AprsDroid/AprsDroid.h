/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AprsDroid.h
 * Author: ale
 *
 * Created on 11 avril 2022, 11:28
 */




#ifndef APRSDROID_H
#define APRSDROID_H

#include <Arduino.h>
#include <BluetoothSerial.h>

#define MSG_MAX_LENGTH 100

class AprsDroid {
public:
    AprsDroid();
    AprsDroid(const AprsDroid& orig);
    virtual ~AprsDroid();
    bool setup(const String _name);
    void send(char* msg);
    int getPduLength();
    static QueueHandle_t queueRx;

private:
    TaskHandle_t TaskHandle_Rxbt;
    void receive();
    static AprsDroid* anchor;
    static void marshall(void *);
    BluetoothSerial bt;
    String name;
    char msg[MSG_MAX_LENGTH];
    int     pduLength;

};

#endif /* APRSDROID_H */

  
