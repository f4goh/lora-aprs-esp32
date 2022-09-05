/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DecodeAprs.cpp
 * Author: ale
 * 
 * Created on 10 mai 2022, 15:31
 *  #F4GOH-9>APDR16,F4GOH-1*:=4753.42N/00016.61Eb/A=000399 lora aprs

    #lien avec -2
#msg="F4GOH-9>APDR16,F4GOH-2*,WIDE2-1:=4753.00N/00016.72Eb/A=000399 lora aprs" + "\n"


    #lien avec -2 trait vert + bleu
#msg="F4GOH-9>APDR16,F4GOH-2,F4GOH-3*:=4753.00N/00016.72Eb/A=000399 lora aprs" + "\n"

    #lien avec -2 trait bleu
#msg="F4GOH-9>APDR16,F4GOH-2,F4GOH-3,WIDE1*:=4753.00N/00016.72Eb/A=000399 lora aprs" + "\n"

    #lien avec -2 trait bleu
msg="F4GOH-9>APDR16,F4GOH-2,F4GOH-3*,WIDE3-1:=4753.00N/00016.72Eb/A=000399 lora aprs" + "\n"

    #lien total trait bleu
#msg="F4GOH-9>APDR16,F4GOH-2,F4GOH-3,F4GOH-4,WIDE2*:=4753.00N/00016.72Eb/A=000399 lora aprs"+ "\n"

    #lien total trait vert depart + bleu
#msg="F4GOH-9>APDR16,F4GOH-2,F4GOH-3,F4GOH-4*:=4753.00N/00016.72Eb/A=000399 lora aprs"+ "\n"

 */

#include "DecodeAprs.h"

DecodeAprs::DecodeAprs() :
Afficheur()
{    
    clearMem();
    report.ta=POSITION;   //defaut car seul cas traité pour le moment
}

DecodeAprs::DecodeAprs(const DecodeAprs& orig) {
}

DecodeAprs::~DecodeAprs() {
}

void DecodeAprs::clearMem() {    
    memset(body, '\0', BODY_LEN);
    memset(&report, '\0', sizeof(aprs));
    
}


bool DecodeAprs::split(char *msg, char car) {
    char *pch;
    int taille;
    bool ret=false;
    pch = strchr(msg, car);    
    if (pch != NULL) {        
        taille=strlen(msg)-(pch-msg)-1;        
        strncpy(body,pch+1,taille); 
        ret=true;
    }
    return ret;
}

bool DecodeAprs::extractCall(char *msg){    
    int cpt=0;
    while ((msg[cpt]!='>') && (cpt<CALL_LEN)){
        report.callSign[cpt]=msg[cpt];
        cpt++;
    }
    report.callSign[cpt]='\0';
    if (cpt==CALL_LEN){
        return false;
    }
    return true;
}

int DecodeAprs::countChar(char *msg, char car) {
    int n;
    int cpt = 0;
    for (n = 0; n < strlen(msg); n++) {
        if (msg[n] == car) {
            cpt++;
        }
    }
    Serial.print("count");
    Serial.println(cpt);
    return cpt;
}

bool DecodeAprs::checkAprsAndTimeStamp(){
   if ((body[0]=='/') || (body[0]=='@')) {
       report.withTimeStamp=true;
       strncpy(report.timeStamp,body+1,TIMESTAMP_LEN);
       Serial.println(report.timeStamp);
       strcpy(body,body+TIMESTAMP_LEN+2);       
       return true;
   }
   if ((body[0]=='!') || (body[0]=='=')) {
       report.withTimeStamp=false;
       strcpy(body,body+1);
       return true;
   }
   return false;
}

bool DecodeAprs::parsePositionId(){
    if (body[12]=='T') {  //manque le cas '[' a revoir
        if (body[0]=='/'){
        Serial.println("Position compressed"); //non converti pour le moment
        report.symTableId = body[0];
        report.symCode = body[9];
        strncpy(report.latitude,body+1,4);
        strncpy(report.longitude,body+5,4);
        strcpy(body,body+10);
        return true;
        }
    }
    if ((body[7] == 'N') || (body[7] == 'S')) {
        if ((body[17] == 'E') || (body[17] == 'W')) {
            Serial.println("Position Normal");
            strncpy(report.latitude,body,LATITUDE_LEN);
            strncpy(report.longitude,body+9,LONGITUDE_LEN);
            report.symTableId = body[8];
            report.symCode = body[18];
            strcpy(body,body+LATITUDE_LEN+LONGITUDE_LEN+2);
            return true;
        }
    }
    return false;
}

bool DecodeAprs::checkAltitude() {
    if ((body[0] == '/') && (body[1] == 'A') && (body[2] == '=')) {
        Serial.println("Alitude found");
        strncpy(report.altitude, body + 3, ALTITUDE_LEN);
        strcpy(body, body + ALTITUDE_LEN + 3);
        report.altitudeMeters=atoi(report.altitude)/3.28;
        report.withAltitude=true;
        return true;
    }
    report.withAltitude=false;
    return false;
}

bool DecodeAprs::checkCourseSpeed() {
    if (body[2] == 'T') {                       // a revoir
        if ((body[0] != ' ') && (body[1] != ' ')) {
            Serial.println("Compressed speed course");
            report.course[0] = body[0];
            report.speed[0] = body[1];
            report.withCourseSpeed=true;
            return true;
        }
    }
    if (body[3] == '/') {
        Serial.println("Normal course speed found");
        strncpy(report.course, body, COURSE_LEN);
        strncpy(report.speed, body + 4, SPEED_LEN);
        strcpy(body, body + COURSE_LEN + SPEED_LEN + 1);
        report.withCourseSpeed=true;
        return true;
    }
    report.withCourseSpeed=false;
    return false;
}
bool DecodeAprs::parseComment(){
   if (strlen(body) > 0){
     strcpy(report.comment,body);
     Serial.println("comment found");
     return true;
   }
   return false;
}



bool DecodeAprs::parse(char* msg) {
    Serial.println(strlen(msg));
    clearMem();
    bool ret = false;
    if (strlen(msg) > 0) {
        if (extractCall(msg)==false){
           Serial.println("bad Call"); 
           return false;
        }
        split(msg, ':');
        if (strlen(body) > 0) {
            Serial.println(body);
            if (checkAprsAndTimeStamp() == false) {
                Serial.println("Bad APRS data ID");
                return false;
            }
            if (parsePositionId() == false) {
                Serial.println("Bad APRS coordinates");
                return false;
            }
            //check wx here for futur
            if (checkCourseSpeed() == false) {
                Serial.println("no course speed found");
            }
            if (checkAltitude() == false) {
                Serial.println("no altitude found");
            }
            if (parseComment() == false) {
                Serial.println("no comment found");
            }
            refresh(report);            
        }       
    }
    return ret;
}






