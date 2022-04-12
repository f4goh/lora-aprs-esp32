/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AprsDroidTcp.cpp
 * Author: ale
 * 
 * Created on 11 avril 2022, 11:28
 */

#include "AprsDroidTcp.h"

AprsDroidTcp::AprsDroidTcp() {
    anchor = this;
}

AprsDroidTcp::AprsDroidTcp(const AprsDroidTcp& orig) {
}

AprsDroidTcp::~AprsDroidTcp() {
}

bool AprsDroidTcp::setup() {
    server.begin();
    BaseType_t ret = xTaskCreatePinnedToCore(AprsDroidTcp::marshall, "aprsdroid", 50000, NULL, 3, &TaskHandle_Rxbt, tskNO_AFFINITY);
    return (ret == pdPASS) ? true : false;
}

void AprsDroidTcp::CheckForConnections()
{
  if (server.hasClient())
  {
    // If we are already connected to another computer, 
    // then reject the new connection. Otherwise accept
    // the connection. 
    if (remoteClient.connected())
    {
      Serial.println("Connection rejected");
      server.available().stop();
    }
    else
    {
      Serial.println("Connection accepted");
      remoteClient = server.available();
      
    }
  }
}

void AprsDroidTcp::send(char* msg) {
    if (remoteClient.connected()) {
        remoteClient.println(msg);
    }
}

int AprsDroidTcp::getPduLength(){
   return pduLength; 
}


void AprsDroidTcp::marshall(void * parametres) {
    anchor->receive();
}


void AprsDroidTcp::receive() {

    Serial.println("Tache Wifi Droid en fonctionnement");
    int ptr = 0;    
    char car;
    while (1) {
        vTaskDelay(1); //indispensable car sinon guru ?!
        CheckForConnections();
        while (remoteClient.connected() && remoteClient.available()) {
            if (ptr < MSG_MAX_LENGTH - 1) {
                car = (char) remoteClient.read();
                //Serial.print(car);
                if (car != '\n') {
                    msg[ptr++] = car;                    
                } else {
                    msg[ptr] = '\0';
                    pduLength=ptr;
                    ptr = 0;
                    Serial.println("AprsDroidTcp incomming");
                    Serial.println(msg);
                    xQueueSend(queueRx, msg, 0);
                }
            } else {
                msg[ptr] = '\0';
                ptr = 0;
            }
        }
    }
}


QueueHandle_t AprsDroidTcp::queueRx = xQueueCreate(3, sizeof (msg));
AprsDroidTcp* AprsDroidTcp::anchor = NULL;
