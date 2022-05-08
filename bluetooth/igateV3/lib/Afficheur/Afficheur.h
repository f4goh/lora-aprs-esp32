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

class Afficheur {
public:

    Afficheur();
    virtual ~Afficheur();

    void setCnxState(bool _btCnx);    
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

    
    int frameCount; // Nombre de pages à afficher
    int overlaysCount;
    SSD1306Wire display;
    OLEDDisplayUi ui;
    FrameCallback frames[2];
    OverlayCallback overlays[1]; //affichage commun par dessus toutes les pages
    String call;
    int rssi;
    uint8_t compteurTrame;
    bool btCnx;
    bool disRssi;
    bool cnxError;
    
    static Afficheur* anchor;
    
    TaskHandle_t TaskHandle_Aff;

    String latitude,longitude,comment;
    int altitude;
    bool disAlti;
};

#endif /* AFFICHEUR_H */
