/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Message.cpp
 * Author: ale
 * 
 * Created on 5 décembre 2022, 18:18
 */

#include "Message.h"

Message::   Message(String _callsign,String _destination,String _path,String _phone):
callsign(_callsign),
destination(_destination),
path(_path),
phone(_phone),
configuration(new Preferences())
{

}

Message::Message(const Message& orig) {
}

Message::~Message() {
    delete configuration;
}

int Message::getPduLength() {
    return pduLength;
}

void Message::setPosition(float _latitude, float _longitude) {
    latitude=_latitude;
    longitude=_longitude;
}

void Message::setComment(String _comment) {
    comment=_comment;
    if (comment.length() > 67) {
        comment = comment.substring(0, 67); //67 char max
    }
}

void Message::setPhone(String _phone){
    phone=_phone;
}

char* Message::getPduSMS(msgMode mode){
    String header;
    int     messageId;    
    if (mode==GOOGLE){        
        comment="https://www.google.fr/maps/place/"; //https://www.google.fr/maps/place/48.5643,0.69195        
    }
   
    header = callsign + ">" + destination + "," + path + "::SMSGTE   :@" + phone + " " + comment;
    char messageArray[150];
    header.toCharArray(messageArray, 150);

    configuration->begin("tracker", false);
    messageId=configuration->getInt("messageId");
    messageId++;
    if (messageId > MAX_MESSAGES_ID) {
        messageId = 0;
    }
    if (mode == GOOGLE) {
        snprintf(pdu, sizeof (pdu), "%s%2.4f,%2.4f{%d", messageArray, latitude,longitude,messageId);
    } else {
        snprintf(pdu, sizeof (pdu), "%s{%d", messageArray, messageId);
    }

    configuration->putInt("messageId", messageId);
    configuration->end();
    //header.toCharArray(pdu,150);
    pduLength = strlen(pdu);
    return pdu;
}


//https://www.google.fr/maps/place/48.5643,0.69195
/*
 2 messages identiques avec id différent envoyé
 
 2 messages différents avec id identique non reçu
 
 */