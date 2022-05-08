/* 
 * File:   main.cpp
 * Author: F4GOH - KF4GOH
 * Created on 6 avril 2022, 13:14
 * todo :
 * digipeater
 */
#include <Arduino.h>
#include <LoraAprs.h>
#include <Afficheur.h>          // Afficheur SSD1306
#include <GestionWifi.h>
#include <Aprsis.h>
#include <AprsDroidTcp.h>
#include <Preferences.h>
#include <Menu.h>               // Classe commandes via la console


#define LED 25 //green led 

LoraAprs lora;
Afficheur *afficheur;
Aprsis *is;
GestionWifi *gsWifi;
AprsDroidTcp *ad;
Preferences configuration;
Menu *leMenu;



void erreur(String msg){
    Serial.print("Wifi Sartup Error on ");
    Serial.println(msg);
    Serial.println("Check configuration");
    afficheur->setErrorCnx(true);
    while(1){
        
    }    
}

void setup() {
    //nvs_flash_erase(); // erase the NVS partition and...
    //nvs_flash_init(); // initialize the NVS partition.
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    lora.setup();
    afficheur = new Afficheur;
    
    leMenu = new Menu;  // Menu de configuration
    leMenu->setup();
    gsWifi = new GestionWifi;

   
    bool ret;
    configuration.begin("igate", false);
    lora.setFrequency(configuration.getLong("freq"));
    //access point internet disable
    if (!configuration.getBool("internet")) {
        Serial.println("APRS Droid gate");
        ret = gsWifi->setup("APRS Droid", "totototo", ACCES_POINT);
        if (ret) {
            ad = new AprsDroidTcp;
            ret = ad->setup();
            if (ret) {
                Serial.println("Access point WIFI enable");
            }
            else erreur("TCP local access point");
        }

    } else { //tcp/ip on aprs.is internet enable
        Serial.println("wifi igate on internet");
        String pass = is->passCode(configuration.getString("call"));                
        Serial.println(pass);              
        ret = gsWifi->setup(configuration.getString("ssid").c_str(), configuration.getString("pass").c_str(), CLIENT);
        if (ret) {
            is = new Aprsis;
            ret = is->setup(configuration.getString("call"), pass, configuration.getString("latitude"), configuration.getString("longitude"), configuration.getString("comment"));
            if (ret) {
                Serial.println("Connexion au serveur Aprs done");
            }
            else erreur("Internet igate server");
        }
        else{
           erreur("Wifi error"); 
        }
    }
   
}

void loop() {
    //connexion flags upper left and upper right
    if (!configuration.getBool("internet")) {
       afficheur->setCnxState(gsWifi->getCnxState(), ad->getCnxState());
    } else {
         afficheur->setCnxState(gsWifi->getCnxState(), is->getCnxState());
    }
   
    char msg[MSG_MAX_LENGTH];
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //lora -> internet ou tcp aprs droid
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());        
        afficheur->setRssi(lora.packetRssi());
        afficheur->displayRssi(true);
        afficheur->affiche(msg);
        if (!configuration.getBool("internet")) {
            ad->send(msg);                     //via bt
        } else {
            is->setRssi(lora.packetRssi());
            xQueueSend(is->queueMsg, msg, 0); //via internet
        }
    }
    if (!configuration.getBool("internet")) {
        if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { //wifi tcp aprs droid-> lora
            afficheur->displayRssi(false);
            afficheur->affiche(msg);
            Serial.println("msg bt queue");
            Serial.println(msg);
            digitalWrite(LED, HIGH);
            lora.send(msg, ad->getPduLength());
            digitalWrite(LED, LOW);
        }
    }
    delay(1);  
}


