/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Message.h
 * Author: ale
 *
 * Created on 5 décembre 2022, 18:18
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <Preferences.h>

#define MAX_MESSAGES_ID  99999

enum msgMode {
  FREE_TXT,
  GOOGLE  
};



class Message {
public:
    Message(String _callsign,String _destination,String _path,String _phone);
    Message(const Message& orig);
    virtual ~Message();
    
    char* getPduSMS(msgMode mode);
    int getPduLength();
    void setPosition(float _latitude, float _longitude);
    void setComment(String _comment);
    void setPhone(String _phone);
    void setCallsign(String _callsign);
    
    
private:
    String  callsign;
    String  destination;
    String  path;
    String  phone;
    Preferences *configuration;
    char    pdu[150];
    int     pduLength;
    String comment;
    float latitude,longitude;
    
    
    
    
};

#endif /* MESSAGE_H */

