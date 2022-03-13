/* 
 * File:   Position.cpp
 * Author: philippe & Anthony (F4GOH)
 * 
 * Created on 7 août 2021, 08:00
 * Modified on 13 août 2021, 18:00
 */

#include "Position.h"

/**
 * 
 * @param _latitude
 * @param _longitude      double longitude 
 * @param _comment        String le commentaire
 * @param _symboleTable   char pour désigner la table des symboles
 * @param _symbole        char pour désigner un symbole dans la table 
 */
Position::Position(String _callsign,String _destination,String _path, const float _latitude, const float _longitude, String _comment, const char _symboleTable, const char _symbole) :
callsign(_callsign),
destination(_destination),
path(_path),        
latitude(_latitude),
longitude(_longitude),         
symboleTable(_symboleTable),
symbole(_symbole),
alt(0),
comment(_comment){
    
}

Position::Position(const Position& orig) {
}

Position::~Position() {
}

void Position::setLatitude(const float _latitude) {
    latitude = _latitude;
}

void Position::setLongitude(const float _longitude) {
    longitude = _longitude;
}

void Position::setComment(String _comment) {
    comment = _comment;
}

void Position::setCallsign(String _callsign){
  callsign=_callsign;  
}

void Position::setDestination(String _destination){
  destination=_destination;  
}

void Position::setPath(String _path){
  path=_path;  
}

void Position::setSymbol(const char _symbole){
   symbole=_symbole; 
}

void Position::setAltitude(const float _alt) {
    alt = (int)(3.2809 * _alt);
}


/**
 * @brief Fabrique le PDU APRS position 
 *        si compressed est true la position est compressée (base91)
 * @param bool compressed indique si la position doit être compressée
 * @return char* Le pdu APRS position 
 */
char* Position::getPduAprs(bool compressed) {
    char com[43];
    char scom[33];
    char salt[10];
    
    //exemple
    //F4KMN-7>APLT00,WIDE1-1: !4753.41N/00016.61EO/A=000193 -  _Bat.: 4.19V - Cur.: 495mA !w>$!
    
    char headerArray[50];
    String header;
    header=callsign+">"+destination+","+path;
    header.toCharArray(headerArray,50);
    
    if (alt != 0){
        snprintf(salt, sizeof (salt), "/A=%06d", alt);
        comment.toCharArray(scom, 33);
        snprintf(com, sizeof(com), "%s %s", salt, scom);
    }else{
        comment.toCharArray(com, 43);
    }
    
    if (compressed) {

        latitude_to_comp_str();
        longitude_to_comp_str();

        snprintf(pdu, sizeof (pdu), "%s:!%c%s%s%c  T%s", headerArray,symboleTable, slat, slong, symbole, com);
    }
    else {

        latitude_to_str();
        longitude_to_str();

        snprintf(pdu, sizeof (pdu), "%s:!%s%c%s%c%s", headerArray,slat, symboleTable, slong, symbole, com);
    }
    pduLength=strlen(pdu);
    
    return pdu;
}

int Position::getPduLength(){
   return pduLength; 
}

void Position::latitude_to_str() {
    char hemi; // Hemisphere: N or S
    int ideg; // whole number of degrees
    double dmin; // Minutes after removing degrees
    char smin[8]; // Decimal Minutes in format mm.mm 
    float latAbs = 0.0;

    if (latitude < 0.0) {
        latAbs = (-latitude);
        hemi = 'S';
    } else {
        latAbs = latitude;
        hemi = 'N';
    }

    ideg = (int) latAbs;
    dmin = (latAbs - ideg) * 60.;

    snprintf(smin, sizeof (smin), "%05.2f", dmin);
    /* Due to roundoff, 59.99 could come out as "60.00" */
    if (smin[0] == '6') {
        smin[0] = '0';
        ideg++;
    }

    snprintf(slat, sizeof (slat), "%02d%s%c", ideg, smin, hemi);
}

void Position::longitude_to_str() {
    char hemi; // Hemisphere: W or E
    int ideg; // whole number of degrees. 
    double dmin; // Minutes after removing degrees. */
    char smin[8]; // Minutes in format mmm.mm */
    float longAbs = 0.0;

    if (longitude < 0.0) {
        longAbs = (-longitude);
        hemi = 'W';
    } else {
        longAbs = longitude;
        hemi = 'E';
    }

    ideg = (int) longAbs;
    dmin = (longAbs - ideg) * 60.;

    snprintf(smin, sizeof (smin), "%05.2f", dmin);
    /* Due to roundoff, 59.9999 could come out as "60.00" */
    if (smin[0] == '6') {
        smin[0] = '0';
        ideg++;
    }

    snprintf(slong, sizeof (slong), "%03d%s%c", ideg, smin, hemi);
}

void Position::latitude_to_comp_str() {

    int y;
    y = (int) round(380926. * (90. - latitude));
    convBase91(y, slat);
}

void Position::longitude_to_comp_str() {

    int x;
    x = (int) round(190463. * (180. + longitude));
    convBase91(x, slong);
}

void Position::convBase91(int x, char* base91) {

    //int c[4];

    for (int i = 3; i >= 0; i--) {
        base91[i] = (x % 91) + 33;
        x /= 91;
    }
    base91[4] = '\0';
}