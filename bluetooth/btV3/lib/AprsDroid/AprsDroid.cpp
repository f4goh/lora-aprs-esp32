/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AprsDroid.cpp
 * Author: ale
 * 
 * Created on 11 avril 2022, 11:28
 */

#include "AprsDroid.h"

AprsDroid::AprsDroid() {
    anchor = this;
}

AprsDroid::AprsDroid(const AprsDroid& orig) {
}

AprsDroid::~AprsDroid() {
}

bool AprsDroid::setup(const String _name) {
    name = _name;
    bt.begin(name);
    BaseType_t ret = xTaskCreatePinnedToCore(AprsDroid::marshall, "aprsdroid", 50000, NULL, 3, &TaskHandle_Rxbt, tskNO_AFFINITY);
    return (ret == pdPASS) ? true : false;
}

void AprsDroid::send(char* msg){
    bt.println(msg);    
}

int AprsDroid::getPduLength(){
   return pduLength; 
}


void AprsDroid::marshall(void * parametres) {
    anchor->receive();
}


void AprsDroid::receive() {

    Serial.println("Tache RX Droid en fonctionnement");
    int ptr = 0;    
    char car;
    while (1) {
        vTaskDelay(1); //indispensable car sinon guru ?!
        if (bt.available()) {                       //tester avec le while
            if (ptr < MSG_MAX_LENGTH - 1) {
                car = (char) bt.read();
                if (car != '\n') {
                    msg[ptr++] = car;                    
                } else {
                    msg[ptr] = '\0';
                    pduLength=ptr;
                    ptr = 0;
                    Serial.println("AprsDroid incomming");
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


QueueHandle_t AprsDroid::queueRx = xQueueCreate(3, sizeof (msg));
AprsDroid* AprsDroid::anchor = NULL;
