/* 
 * File:   main.cpp
 * Author: ale
 * Igate not finished
 * Created on 6 avril 2022, 13:14
 * bluetooth Version FOR APRS Droid
 * LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
 * todo :
 * display
  */
#include <Arduino.h>
#include <LoraAprs.h>
#include <Afficheur.h>          // Afficheur SSD1306
#include <AprsDroid.h>
#include <console.h>
#include <Preferences.h>


#define LED 25 //green led 
#define FREQUENCY 433775000

LoraAprs lora;
Afficheur *afficheur;
AprsDroid *ad;
Console con;
Preferences preferences;

bool exitFlag;


bool acceptCmd(String cmd,int longMin,int longMax){
    if (cmd.length()>=longMin && cmd.length()<=longMax){
        return true;
    }else{
        Serial.println(F("erreur"));
        return  false;
    }        
}

void _help_(ArgList& L, Stream& S) {
    Serial.println(F("Configuation exemple"));
    Serial.println(F("Set frequency : freq 433775000"));
    Serial.println(F("Show configuration : shconfig"));
    Serial.println(F("Reset default configuration : raz"));
    Serial.println(F("Exit menu : exit"));
}


void _freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,9,9);
    if (ret==true) {
                    S.printf("freq is %s\n\r", p.c_str());
                    lora.setFrequency(p.toInt());
                    preferences.putLong("freq",p.toInt());                    
    }
}


void _config_(ArgList& L, Stream& S) {
    Serial.printf("Frequency is : %d\n\r",preferences.getLong("freq"));
}

void _raz_(ArgList& L, Stream& S) {
  preferences.putBool("config",false);
  preferences.clear();  
  ESP.restart();    
}

void _exit_(ArgList& L, Stream& S) {
    exitFlag=true;    
}


void _unknown(String& L, Stream& S) {
    S.print("Error");
    //S.println(L);
}

void setupConsole(){
  con.onCmd("help", _help_);
  con.onCmd("freq", _freq_);
  con.onCmd("shconfig", _config_);
  con.onCmd("raz", _raz_);
  con.onCmd("exit", _exit_);
  
  con.onUnknown(_unknown);

  
  preferences.begin("bluetooth", false);
    
  if (preferences.getBool("config",false)==false){
      preferences.putLong("freq",FREQUENCY);
  }
  
  //preferences.end(); //inutile
  
}

void menu(){
    char car;    
    if (Serial.available()>0){
        car=Serial.read();
        if (car=='m'){
            exitFlag=false;
            Serial.println(F("help command for info"));
            while(exitFlag==false){
                con.run();
            }
            Serial.println(F("Exit menu"));
        }
    }
}



void setup() {
    //nvs_flash_erase(); // erase the NVS partition and...
    //nvs_flash_init(); // initialize the NVS partition.
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    lora.setup();
    afficheur = new Afficheur;
    afficheur->afficher("BT");


    bool ret;

    Serial.println("bluetooth gate");
    ad = new AprsDroid;
    ret = ad->setup("LoRa Droid");
    if (ret) {
        Serial.println("Access point BT enable");
    }
    setupConsole();
   
}


void loop() {
    delay(1);
    char msg[MSG_MAX_LENGTH];
    menu();
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //lora -> bluetooth
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        ad->send(msg); //via bt
    }
    if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { //bluetooth-> lora
        Serial.println("msg bt queue");
        Serial.println(msg);
        digitalWrite(LED, HIGH);
        lora.send(msg, ad->getPduLength());
        digitalWrite(LED, LOW);
    }
}
