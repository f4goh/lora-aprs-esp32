/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Afficheur.cpp
 * Author: ale
 * 
 * Created on 7 avril 2022, 19:54
 */

#include "Afficheur.h"


// Address 0x3c on SDA and SCL


Afficheur::Afficheur() :
frameCount(1),
overlaysCount(1),
display(SSD1306Wire(0x3c, SDA, SCL, GEOMETRY_128_64)),
ui(OLEDDisplayUi(&display)),
frames{Afficheur::drawFrame1, Afficheur::drawFrame2,Afficheur::drawFrame3},
overlays{Afficheur::msOverlay},
rssi(0),
compteurTrame(0),
wifiCnx(false),
aprsIsCnx(false),
disRssi(false),
cnxError(false)
{
  anchor = this;
  memset(&report, '\0', sizeof(aprs));
  strcpy(report.callSign,CALLSIGN);
  
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);
  
  ui.setOverlays(overlays, overlaysCount);
  ui.setFrames(frames, frameCount);
  //ui.disableAutoTransition();
  //ui.disableIndicator();
  ui.disableAllIndicators();
  ui.init();
  
  display.flipScreenVertically();
  
  xTaskCreatePinnedToCore(Afficheur::marshall, "afficheur", 10000, NULL, 2, &TaskHandle_Aff, 1); // creation de la tache 
 }


Afficheur::~Afficheur() {
    
}


void Afficheur::setErrorCnx(bool _cnxError){
    cnxError=_cnxError;
}


/**
 * update connexion state
 * @param 2 bool
 */


void Afficheur::setCnxState(bool _wifiCnx,bool _aprsIsCnx) {
    wifiCnx=_wifiCnx;
    aprsIsCnx=_aprsIsCnx;
}


void Afficheur::marshall(void * parametres) {
    anchor->scrollingAuto();
}

/**
 * update rssi value
 * @param  bool
 */

void Afficheur::displayRssi(bool _disRssi){
  disRssi=_disRssi;  
}




/**
 * set new RSSI
 * @param _rssi int 
 */


void Afficheur::setRssi(int _rssi){
    rssi=_rssi;    
}

/**
 * task to scroll display between two pages 
 */


void Afficheur::scrollingAuto() {
    Serial.println("Tache Afficheur en fonctionnement");
    while (1) {
        vTaskDelay(1); //indispensable car sinon guru ?!
        ui.update();
    }
}

/**
 * display overlay on 2 pages wifi enable and tcp/ip enable
 */

void Afficheur::msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  String text;
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  text=anchor->wifiCnx ? "EN." : "DIS.";
  text="WIFI "+text;
  display->drawString(0, 0, text);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  text=anchor->aprsIsCnx ? "EN." : "DIS.";
  text="TCP/IP "+text;
  display->drawString(128, 0, text); 
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128, 60, text); 
  text=VERSION;
  
}

/**
 * display page 1
 */

void Afficheur::drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    String msg;
    if (strcmp(anchor->report.callSign, CALLSIGN)==0) {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawXbm(x + 0, y + 20, TW_width, TW_height, TW_Logo_bits);
        display->setFont(ArialMT_Plain_24);
        display->drawString(45 + x, y+25, anchor->report.callSign);
    } else {
        //display->setFont(ArialMT_Plain_10);
        display->setFont(ArialMT_Plain_16);
        display->setTextAlignment(TEXT_ALIGN_RIGHT);
        display->drawString(128 + x, 10 + y, String(anchor->compteurTrame));
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 10 + y,  anchor->report.callSign);
        display->drawString(0 + x, 26 + y, anchor->report.comment);
        if (anchor->disRssi) {
                msg = "RSSI: " + String(anchor->rssi) + " dBm";
                display->drawString(0 + x, 26 + 16 + y, msg);
        }
    }
}

/**
 * display page 2
 */


void Afficheur::drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->setFont(ArialMT_Plain_24);
    if (anchor->cnxError) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 20 + y, "ERROR");
    } else {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 16 + y, anchor->report.latitude);
        display->drawString(0 + x, 16 + 24 + y, anchor->report.longitude);
    }
}

/**
 * display page 3 altitude course speed
 */

void Afficheur::drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    String msg;
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    if (anchor->report.withAltitude) {
        msg = "Altitude " + String(anchor->report.altitudeMeters) + " m";
        display->drawString(0 + x, 10 + y, msg);
    }
    if (anchor->report.withCourseSpeed) {
        msg = "Course " + String(anchor->report.course) + " °";
        display->drawString(0 + x, 26 + y, msg);
        msg = "Speed " + String(anchor->report.course) + " kts";
        display->drawString(0 + x, 26 + 16 + y, msg);
    }
}

/**
 * refresh data to display
 * @param msg char * message parsed to display
 */

void Afficheur::refresh(aprs _report) {
    compteurTrame++;
    memcpy(&report, &_report, sizeof (aprs));
    if ((report.withCourseSpeed) || (report.withAltitude)) {
        frameCount = 3;
    } else {
        frameCount = 2;
    }
    ui.setFrames(frames, frameCount);
    Serial.println(report.callSign);
    Serial.println(report.latitude);
    Serial.println(report.longitude);
    Serial.println(report.symTableId);
    Serial.println(report.symCode);
    Serial.println(report.course);
    Serial.println(report.speed);
    Serial.println(report.altitude);
    Serial.println(report.altitudeMeters);
    Serial.println(report.comment);
}

Afficheur* Afficheur::anchor = NULL;

