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
 * Livebox-0E00

 */

#include "Afficheur.h"


// Address 0x3c on SDA and SCL


Afficheur::Afficheur()
//
{
  anchor = this;
  call="F4KMN";
  rssi=0;
  compteurTrame=0;
  pass="";
  wifiCnx=false;
  aprsIsCnx=false;
  overlaysCount = 1;
  frameCount = 2;
  disRssi=false;
  cnxError=false;
 }


Afficheur::~Afficheur() {
    
}

void Afficheur::setup() {
    
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);
  
  ui.setOverlays(overlays, overlaysCount);
  ui.setFrames(frames, frameCount);
  //ui.disableAutoTransition();
  ui.init();
  display.flipScreenVertically();
  
  xTaskCreatePinnedToCore(Afficheur::marshall, "afficheur", 10000, NULL, 2, &TaskHandle_Aff, 1); // creation de la tache 

}

void Afficheur::setErrorCnx(bool _cnxError){
    cnxError=_cnxError;
}



/**
 * test function
 * @param msg String message to display
 */

void Afficheur::setPass(String message) {
    pass=message;    
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
 * parse data to display
 * @param msg String message to display
 */

void Afficheur::affiche(char* msg) {
   //f4goh-7>APLT00,WIDE1-1:!4753.41N/00016.61E> Bat:3.98V/-102mA
   //ou
   //f4goh-7>APLT00,WIDE1-1:!4753.41N/00016.61E>/A=000246  Bat:3.84V/-107mA
   String  s=String(msg);
   int index,index2;   
   index=s.indexOf('>');
   call=s.substring(0, index);
   index=s.indexOf(':');
   latitude=s.substring(index+2, index+10);
   longitude=s.substring(index+11, index+20);
   
   index2=s.indexOf("/A=");
   if (index2>0){
       altitude=s.substring(index2+3,index2+9).toInt()/3.28;
       Serial.println(altitude);
       comment=s.substring(index2+10, s.length());
       disAlti=true;
   }
   else{
       comment=s.substring(index+22, s.length());
       disAlti=false;
   }
   
   Serial.println(call);
   Serial.println(latitude);
   Serial.println(longitude);
   Serial.println(comment); 
   compteurTrame++;
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
}

/**
 * display page 1
 */

void Afficheur::drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    String msg;
    if (anchor->call == "F4KMN") {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawXbm(x + 0, y + 20, TW_width, TW_height, TW_Logo_bits);
        display->setFont(ArialMT_Plain_24);
        display->drawString(45 + x, y+25, anchor->call);
    } else {
        //display->setFont(ArialMT_Plain_10);
        display->setFont(ArialMT_Plain_16);
        display->setTextAlignment(TEXT_ALIGN_RIGHT);
        display->drawString(128 + x, 10 + y, String(anchor->compteurTrame));
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 10 + y, anchor->call);
        display->drawString(0 + x, 26 + y, anchor->comment);
        if (anchor->disAlti){
            msg = String(anchor->altitude) + " m";
            display->drawString(0 + x, 26 + 16 + y, msg);
            if (anchor->disRssi) {
                display->setTextAlignment(TEXT_ALIGN_RIGHT);
                display->setFont(ArialMT_Plain_10);
                msg = String(anchor->rssi) + " dBm";
                display->drawString(128 + x, 26 + 20 + y, msg);
            }
        } else {
            if (anchor->disRssi) {
                msg = "RSSI: " + String(anchor->rssi) + " dBm";
                display->drawString(0 + x, 26 + 16 + y, msg);
            }
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
        display->drawString(0 + x, 16 + y, anchor->latitude);
        display->drawString(0 + x, 16 + 24 + y, anchor->longitude);
    }
}

Afficheur* Afficheur::anchor = NULL;

