/* 
 * File:   main.cpp
 * Author: F4GOH - KF4GOH
 * Created on 6 avril 2022, 13:14
  * bluetooth Version FOR APRS Droid
 * LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
 * todo :
 * digipeater
 */
#include <Arduino.h>
#include <LoraAprs.h>
#include <Afficheur.h>          // Afficheur SSD1306
#include <AprsDroid.h>
#include <Preferences.h>
#include <Menu.h>               // Classe commandes via la console


#define LED 25 //green led 
#define FREQUENCY 433775000

LoraAprs lora;
Afficheur *afficheur;
AprsDroid *ad;
Console con;
Preferences configuration;
Menu *leMenu;


void erreur(){
    Serial.print("Bluetooth Sartup");    
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
    //afficheur->afficher("BT");

    leMenu = new Menu;  // Menu de configuration
    leMenu->setup();
    
    
    bool ret;
    configuration.begin("bluetooth", false);
    lora.setFrequency(configuration.getLong("freq"));
    Serial.println("bluetooth gate");
    ad = new AprsDroid;
    ret = ad->setup("LoRa Droid");
    if (ret) {
        Serial.println("Access point BT enable");
         afficheur->setCnxState(ret);
    }
    else{
        erreur();
    }
    //setupConsole();
   
}


void loop() {
    delay(1);
    char msg[MSG_MAX_LENGTH];
    //menu();
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //lora -> bluetooth
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        afficheur->setRssi(lora.packetRssi());
        afficheur->displayRssi(true);
        afficheur->affiche(msg);
        ad->send(msg); //via bt
    }
    if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { //bluetooth-> lora
        Serial.println("msg bt queue");
        Serial.println(msg);
        afficheur->displayRssi(false);
        afficheur->affiche(msg);
        digitalWrite(LED, HIGH);
        lora.send(msg, ad->getPduLength());
        digitalWrite(LED, LOW);
    }
}
