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


class Afficheur {
public:

    Afficheur();
    virtual ~Afficheur();

    void setup();

    void setPass(String message);
    void setCnxState(bool _wifiCnx,bool _aprsIsCnx);    
    void affiche(char* msg);
    void setRssi(int _rssi);
    void displayRssi(bool _disRssi);
    void setErrorCnx(bool _cnxError);
    
private:

    static void marshall(void * parametres);

    void scrollingAuto();

    static void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    static void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    static void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);

    FrameCallback frames[2] = {Afficheur::drawFrame1, Afficheur::drawFrame2};

    OverlayCallback overlays[1] = {Afficheur::msOverlay};
    int overlaysCount;
    int frameCount;
    TaskHandle_t TaskHandle_Aff;
    
    static Afficheur* anchor;

    String pass;    
    bool wifiCnx;
    bool aprsIsCnx;
    bool cnxError;
    String call,latitude,longitude,comment;
    int altitude;
    int rssi;
    uint8_t compteurTrame;
    bool disRssi;
    bool disAlti;
    
    SSD1306Wire display = SSD1306Wire(0x3c, SDA, SCL, GEOMETRY_128_64);
    OLEDDisplayUi ui = OLEDDisplayUi(&display);
};



#endif /* AFFICHEUR_H */
