/* 
 * File:   main.cpp
 * Author: F4GOH - KF4GOH
 * Created on 6 avril 2022, 13:14
  * bluetooth Version FOR APRS Droid
 * LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
 * 
 * LoRa
 * ====
 * #ID: 1167
 * An Arduino library for sending and receiving data using LoRa radios. Supports Semtech SX1276/77/78/79 based boards/shields.
 * pio lib install 1167
 * Library Manager: LoRa @ 0.8.0 has been installed!
 *  
 * Console
 * =======
 * #ID: 6893
 * Small user console class for embedded systems based on arduino framework
 * pio lib install 6893
 * Library Manager: Console @ 1.2.1 has been installed!
 * 
 * Library : ESP32Time @ 1.0.4
 * installation : pio lib install 11703
 * 
 * Bibliothèque : Oled 
 * installation : pio lib install 2978
 * 
 * todo :
 * improve parser
 */
#include <Arduino.h>
#include <LoraAprs.h>
#include <Afficheur.h>          // Afficheur SSD1306
#include <AprsDroid.h>
#include <DecodeAprs.h>
#include <Preferences.h>
#include <Menu.h>               // Classe commandes via la console
#include <Digipeater.h>
#include <ESP32Time.h>


#define LED 25 //green led 
#define FREQUENCY 433775000
#define BEACON_INTERVAL 15

LoraAprs lora;
DecodeAprs *afficheur;
AprsDroid *ad;
Console con;
Preferences configuration;
Menu *leMenu;
Digipeater *digi;

ESP32Time rtc;

void erreur(){
    Serial.print("Bluetooth Sartup");    
    Serial.println("Check configuration");
    afficheur->setErrorCnx(true);
    while(1){
        
    }    
}

void sendBeacon(){
     char * pdu;
     pdu=digi->getBeacon();
     Serial.println(pdu);
     lora.send(pdu,strlen(pdu));
}


void setup() {
    //nvs_flash_erase(); // erase the NVS partition and...
    //nvs_flash_init(); // initialize the NVS partition.
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    rtc.setTime(30, 59, 15, 17, 1, 2021);  // 17th Jan 2021 15:59:30 date bidon
    lora.setup();
    afficheur = new DecodeAprs;

    leMenu = new Menu;  // Menu de configuration
    leMenu->setup();
    configuration.begin("bluetooth", false);
    digi = new Digipeater(configuration.getString("call"),configuration.getString("latitude"),configuration.getString("longitude"),configuration.getString("comment"));
    
    bool ret;
    
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
   
}


void loop() {
    char * pdu;
    char msg[MSG_MAX_LENGTH];
    char car;
    if (Serial.available() > 0) {
        car = Serial.read();
        if (car == 'b') { //test beacon
            sendBeacon();
        }
    }
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //lora -> bluetooth
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        afficheur->setRssi(lora.packetRssi());
        afficheur->displayRssi(true);
        afficheur->parse(msg);
        ad->send(msg); //via bt
         if (configuration.getBool("digi")) { //digi
            if (digi->routing(msg)) {
                pdu = digi->getMsg();
                Serial.println(pdu);
                lora.send(pdu, strlen(pdu));
            }
        }
    }
    if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { //bluetooth-> lora
        Serial.println("msg bt queue");
        Serial.println(msg);
        afficheur->displayRssi(false);
        afficheur->parse(msg);
        digitalWrite(LED, HIGH);
        lora.send(msg, ad->getPduLength());
        digitalWrite(LED, LOW);
    }
    
      if (configuration.getBool("digi")) {
        if (((rtc.getMinute() % BEACON_INTERVAL) == 0) && (rtc.getSecond()==0)) {
            sendBeacon();
        }
    }
    delay(1);
}
