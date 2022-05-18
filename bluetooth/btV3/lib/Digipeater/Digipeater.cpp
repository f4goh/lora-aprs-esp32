/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Digipeater.cpp
 * Author: ale
 * 
 * Created on 11 mai 2022, 12:43
 */

#include "Digipeater.h"

Digipeater::Digipeater(String _call,String _latitude,String _longitude, String _comment) {
    beaconMsg=_call+">APDR16:!"+_latitude+"I"+_longitude+"#"+_comment;
    memset(callDigi, '\0', CALL_LEN);
    nbPaths=0;    
    _call.toCharArray(callDigi,CALL_LEN);
    clearMem();
}

Digipeater::Digipeater(const Digipeater& orig) {
}

Digipeater::~Digipeater() {
}

void Digipeater::clearMem() {
    memset(head, '\0', HEAD_LEN);
    memset(body, '\0', BODY_LEN);
    memset(fromCall, '\0', CALL_LEN);
    memset(path, '\0', PATH_LEN);
    memset(pdu, '\0', PDU_LEN);
    for (int n = 0; n < MAX_PATHS; n++) {
        memset(pathTab[n], '\0', CALL_LEN);
    }
}

bool Digipeater::split(char *msg, char *partA, char *partB,char car) {
    char *pch;
    int tailleA,tailleB;
    bool ret=false;
    pch = strchr(msg, car);    
    if (pch != NULL) {
        tailleA=pch-msg;
        tailleB=strlen(msg)-(pch-msg)-1;
        strncpy(partA,msg,tailleA);
        strncpy(partB,pch+1,tailleB); 
        ret=true;
    }
    return ret;
}

int Digipeater::countChar(char *msg, char car) {
    int n;
    int cpt = 0;
    for (n = 0; n < strlen(msg); n++) {
        if (msg[n] == car) {
            cpt++;
        }
    }
    return cpt;
}

bool Digipeater::splitPath(){
    int n;
    int cpt = 0;
    char temp[CALL_LEN] = {0};
    int m = 0;
    if (countChar(path, ',') >= MAX_PATHS - 1) {
        Serial.print("Full : ");
        Serial.println(cpt);
        return false;
    }
    for (n = 0; n < strlen(path) + 1; n++) {
        if ((path[n] != ',') && (path[n] != '\0')) {
            temp[m] = path[n];
            m++;
        } else {
            strncpy(pathTab[cpt], temp, CALL_LEN);
            memset(temp, '\0', CALL_LEN);
            m = 0;
            cpt++;
        }
    }
    nbPaths = cpt;
    Serial.println(cpt);
    for (n = 0; n < cpt; n++) {
        Serial.println(pathTab[n]);
    }
    return true;
}

bool Digipeater::routing(char* msg) {
    clearMem();
    Serial.println(strlen(msg));    
    if (strlen(msg) == 0) {
        return false;
    }
    if (split(msg, head, body, ':') == false) {
        return false;
    }
    Serial.println(head);
    Serial.println(body);
    if (strlen(head) == 0) {
        return false;
    }
    if (split(head, fromCall, path, '>') == false) {
        return false;
    }
    Serial.println(fromCall);
    Serial.println(path);
    if (splitPath() == false) {
        return false;
    }
    if (addPath()==false){
         Serial.println("Bad Path");
    }    
    return true;
}

bool Digipeater::addPath() {
    int n;
    if (strcmp(fromCall, callDigi) == 0) {
        Serial.println("Own call");  
        return false;
    }    
    for (n = 0; n < nbPaths; n++) {
        if (strcmp(pathTab[n], callDigi) == 0) {
            Serial.println("Call inside PATH");
            return false;

        }
    }    
    if (strncmp(pathTab[nbPaths - 1], "WIDE", 4) != 0) {
            Serial.println("No WIDE");
            return false;
    }
    if (pathTab[nbPaths - 1][5] == '*') {
            Serial.println("End PATH");
            return false;
    }    
    strcpy(pathTab[nbPaths], pathTab[nbPaths - 1]);
    memset(pathTab[nbPaths - 1], '\0', CALL_LEN);
    strcpy(pathTab[nbPaths - 1], callDigi);
    nbPaths++;
    pathTab[nbPaths - 1][6]--;
    if (pathTab[nbPaths - 1][6] == '0') {
            pathTab[nbPaths - 1][5] = '*';
            pathTab[nbPaths - 1][6] = '\0';
    }
    Serial.println(nbPaths);
    for (n = 0; n < nbPaths; n++) {
         Serial.println(pathTab[n]);
    }
    return true;
}


char * Digipeater::getMsg(){
   int n;
   memset(pdu, '\0', PDU_LEN);
   snprintf(pdu, PDU_LEN, "%s>", fromCall);
   for (n = 0; n < nbPaths-1; n++) {
       strcat(pdu, pathTab[n]);
       strcat(pdu, ",");
   }
   strcat(pdu, pathTab[nbPaths-1]);
   strcat(pdu, ":");
   strcat(pdu, body);
   Serial.println(pdu);
   return pdu;
}

char * Digipeater::getBeacon(){
    memset(pdu, '\0', PDU_LEN);
    beaconMsg.toCharArray(pdu,PDU_LEN);
    return pdu;
}


void Digipeater::check(char *msg){        
    char *nr;
    if (routing(msg)){
         nr=getMsg();
         Serial.println(msg);
         Serial.println(nr);
    };
}

void Digipeater::test(){
    
    char msg1[]="F4GOH-9>APDR16,F4GOH-2,WIDE1-1:=4753.42N/00016.61Eb/A=000399 lora aprs";
    check(msg1);
    char msg2[]="F4GOH-9>APDR16,WIDE2-2:=4753.42N/00016.61Eb/A=000399 lora aprs";
    check(msg2);
    //char msg3[]="F4GOH-9>APDR16,F4GOH-2,F4GOH-3,F4GOH-4,WIDE2*:=4753.42N/00016.61Eb/A=000399 lora aprs";
    //check(msg3);
}
