/* 
 * File:   Position.h
 * Author: philippe & Anthony (F4GOH)
 *
 * Created on 7 août 2021, 08:00
 * Modified on 13 août 2021, 18:00
 */

#ifndef POSITION_H
#define POSITION_H

#include <WString.h>
#include <stdio.h>
#include <math.h>
#include <Arduino.h>
              //String _callsign = "",
              //


class Position {
public:
    Position (String _callsign,
              String _destination,
              String _path,
             const float _latitude, 
             const float _longitude,
             String _comment = "", 
             const char _symboleTable = '/',
             const char _symbole= '>'
             );
    
    Position(const Position& orig);
    virtual ~Position();
    
    char* getPduAprs(bool compressed = false);
    void setLatitude(const float _latitude);
    void setLongitude(const float _longitude);
    void setComment(String _comment);
    void setCallsign(String _callsign);
    void setDestination(String _destination);
    void setPath(String _path);
    void setAltitude(const float _alt);
    void setSymbol(const char _symbole);
    
    
    int getPduLength();
    
    
protected:
    void latitude_to_str();
    void longitude_to_str();
    void convBase91(int x, char* base91);
    char    slat[9];
    char    slong[10];
    String  callsign;
    String  destination;
    String  path;
    float   latitude;   
    float   longitude;
    char    symboleTable;
    char    symbole;
    char    pdu[150];
    int     pduLength;
    
    
private:

    int     alt;
    String  comment;
   
    void latitude_to_comp_str();
    void longitude_to_comp_str();    
};

#endif /* POSITION_H */