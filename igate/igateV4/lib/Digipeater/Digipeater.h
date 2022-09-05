/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Digipeater.h
 * Author: ale
 *
 * Created on 11 mai 2022, 12:43
 */

#ifndef DIGIPEATER_H
#define DIGIPEATER_H

#include <string.h>
#include <Arduino.h>


#define MAX_PATHS 5
#define HEAD_LEN 80
#define PATH_LEN 70
#define BODY_LEN 100
#define CALL_LEN 10
#define PDU_LEN 150


class Digipeater {
public:
    Digipeater(String _call,String _latitude,String _longitude, String _comment);
    Digipeater(const Digipeater& orig);
    virtual ~Digipeater();
    bool routing(char* msg);
    char * getMsg();
    void test();
    char * getBeacon();
    
    
private:
    bool split(char *str, char *partA, char *partB,char car);
    bool splitPath();
    bool addPath();
    int countChar(char *msg, char car);
    void check(char *msg);
    void clearMem();
    
    
    String beaconMsg;
    char callDigi[CALL_LEN];
    char head[HEAD_LEN];
    char body[BODY_LEN];
    char fromCall[CALL_LEN];
    char path[PATH_LEN];
    char pathTab[MAX_PATHS][CALL_LEN];
    char pdu[PDU_LEN];
    int  nbPaths;

};

#endif /* DIGIPEATER_H */

//F4GOH-9>APDR16,F4GOH-2,F4GOH-3,F4GOH-4*:=4753.00N/00016.72Eb/A=000399 lora aprs"+ "\n"