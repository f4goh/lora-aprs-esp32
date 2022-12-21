/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DecodeAprs.h
 * Author: ale
 *
 * Created on 10 mai 2022, 15:31
 */

#ifndef DECODEAPRS_H
#define DECODEAPRS_H

//#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <Afficheur.h>

#define BODY_LEN 100

class DecodeAprs : public Afficheur 
{
public:
    DecodeAprs();
    DecodeAprs(const DecodeAprs& orig);
    virtual ~DecodeAprs();
    bool parse(char* msg);
    
private:
    bool split(char *str, char car);
    bool extractCall(char *msg);
    int countChar(char *msg, char car);
    void clearMem();
    bool checkAprsAndTimeStamp();
    bool parsePositionId();
    bool checkAltitude();
    bool checkCourseSpeed();
    bool parseComment();    
    char body[BODY_LEN];
    aprs report;
    
    
};

#endif /* DECODEAPRS_H */

