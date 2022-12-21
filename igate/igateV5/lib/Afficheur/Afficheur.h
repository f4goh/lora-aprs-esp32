/* 
 * File:   Afficheur.h
 * From Author: philippe SIMIER Lycée Touchard Washington
 * Modified by Anthony Le Cren
 * Created on 11 février 2022, 11:15
 * Classe Afficheur pour le TTGO
 * spécialisation de SSD1306Wire
 * https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/README.md
 * https://www.online-utility.org/image/convert/to/XBM
 */

#ifndef AFFICHEUR_H
#define AFFICHEUR_H

#include <Arduino.h>
#include <SSD1306Wire.h>
#include "OLEDDisplayUi.h"
#include "images.h"

#define CALLSIGN "F4KMN"
#define VERSION "V5"

typedef enum{
    POSITION,    
    WEATHER,
    MESSAGE    
} TYPE_APRS;


#define CALL_LEN 10
#define TIMESTAMP_LEN 6
#define LATITUDE_LEN 8
#define LONGITUDE_LEN 9
#define ALTITUDE_LEN 6
#define COURSE_LEN 3
#define SPEED_LEN 3
#define COMMENT_LEN 50


typedef struct{
    TYPE_APRS ta;
    char callSign[CALL_LEN+1];
    char timeStamp[TIMESTAMP_LEN+1];
    char longitude[LONGITUDE_LEN+1];
    char latitude[LATITUDE_LEN+1];
    char altitude[ALTITUDE_LEN+1];
    int  altitudeMeters;
    char course[COURSE_LEN+1];
    char speed[SPEED_LEN+1];
    char comment[COMMENT_LEN];
    char symTableId;
    char symCode;
    bool compressed;
    bool withTimeStamp;
    bool withAltitude;  
    bool withCourseSpeed;  
}aprs;


class Afficheur {
public:

    Afficheur();
    virtual ~Afficheur();

    void setCnxState(bool _wifiCnx,bool _aprsIsCnx);        
    void setRssi(int _rssi);
    void displayRssi(bool _disRssi);
    void setErrorCnx(bool _cnxError);
    void refresh(aprs report);
    
    
private:

    static void marshall(void * parametres);

    void scrollingAuto();

    static void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    static void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    static void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    static void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);

    
    int frameCount; // Nombre de pages à afficher
    int overlaysCount;
    SSD1306Wire display;
    OLEDDisplayUi ui;
    FrameCallback frames[3];
    OverlayCallback overlays[1]; //affichage commun par dessus toutes les pages    
    int rssi;
    uint8_t compteurTrame;
    bool wifiCnx;
    bool aprsIsCnx;
    bool disRssi;
    bool cnxError;
    
    static Afficheur* anchor;
    
    TaskHandle_t TaskHandle_Aff;

    bool disAlti;
    aprs report;
    
};

#endif /* AFFICHEUR_H */
