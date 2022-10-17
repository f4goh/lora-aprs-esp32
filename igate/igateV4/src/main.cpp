/* 
 * File:   main.cpp
 * Author: F4GOH - KF4GOH
  * Created on 6 avril 2022, 13:14
 * todo :
 * improve parser
 * check wifi reconnection
 * 
 * * LoRa
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
 * 
  LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
 *  
 */
#include <Arduino.h>
#include <LoraAprs.h>
#include <GestionWifi.h>
#include <Aprsis.h>
#include <AprsDroidTcp.h>
#include <Preferences.h>
#include <Menu.h>               // Classe commandes via la console
#include <DecodeAprs.h>
#include <Digipeater.h>
#include <ESP32Time.h>

#define BEACON_INTERVAL 15


#define LED 25 //green led 

LoraAprs lora;
//Afficheur *afficheur;
Aprsis *is;
GestionWifi *gsWifi;
AprsDroidTcp *ad;
Preferences configuration;
Menu *leMenu;
DecodeAprs *afficheur;
Digipeater *digi;

ESP32Time rtc;


void erreur(String msg){
    Serial.print("Wifi Sartup Error on ");
    Serial.println(msg);
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
    gsWifi = new GestionWifi;

    
    configuration.begin("igate", false);        
    digi = new Digipeater(configuration.getString("call"),configuration.getString("latitude"),configuration.getString("longitude"),configuration.getString("comment"));
    
    
    
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
        //String pass = is->passCode(configuration.getString("call"));                
        //Serial.println(pass); 
        Serial.println(configuration.getString("ssid").c_str());
        Serial.println(configuration.getString("pass").c_str());
        ret = gsWifi->setup(configuration.getString("ssid").c_str(), configuration.getString("pass").c_str(), CLIENT);
        if (ret) {
            is = new Aprsis;
            ret = is->setup(configuration.getString("call"), configuration.getString("latitude"), 
                            configuration.getString("longitude"), configuration.getString("comment"),
                            configuration.getString("server"),configuration.getInt("port"));
            if (ret) {
                Serial.println("Connexion au serveur Aprs done");
            }
            else erreur("Internet igate server");
        }
        else{
           erreur("Wifi error"); 
        }
    }
     
   Serial.println(ESP.getFreeHeap());
   
}


void loop() {
    char * pdu;
    char msg[MSG_MAX_LENGTH];
    char car;
    if (Serial.available()>0){
        car=Serial.read();
        if (car=='b'){  //test beacon
            sendBeacon();
        }
        /* //test parser
        if (car=='p'){
                //char msgp[]="F4GOH-9>APDR16,F4GOH-2*,WIDE2-1:=/5L!!<*e7> sTlora aprs";
                //char msgp[]="F4GOH-9>APDR16,F4GOH-2*,WIDE2-1:@092345z4753.00N/00016.72Eb/A=000399 lora aprs";
                //char msgp[]="f4goh-7>APLT00,WIDE1-1:!4753.41N/00016.61E>000/000/A=000252Bat:4.14V/ 0mA";
             char msgp[]="F4GOH-9>APDR16,F4GOH-2*,WIDE2-1:=4753.00N/00016.72Eb088/036/A=000399lora aprs";
             afficheur->parse(msgp); 
        }
        */

    }
    
    //connexion flags upper left and upper right EN. or DIS.
    if (!configuration.getBool("internet")) {
       afficheur->setCnxState(gsWifi->connected(), ad->getCnxState());
    } else {
         afficheur->setCnxState(gsWifi->connected(), is->isConnected());
    }

  
    //Mode lora -> internet ou tcp aprs droid
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { 
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        afficheur->setRssi(lora.packetRssi());
        afficheur->displayRssi(true);
        afficheur->parse(msg);
        if (!configuration.getBool("internet")) {
            ad->send(msg); //via access point
        } else {
            is->setRssi(lora.packetRssi());
            xQueueSend(is->queueMsg, msg, 0); //via internet
        }
        if (configuration.getBool("digi")) { //digi
            if (digi->routing(msg)) {
                pdu = digi->getMsg();
                Serial.println(pdu);
                lora.send(pdu, strlen(pdu));
            }
        }
    }

    //beaconing every 15 min
    if (configuration.getBool("digi")) {
        if (((rtc.getMinute() % BEACON_INTERVAL) == 0) && (rtc.getSecond()==0)) {
            sendBeacon();
        }
    }

    //Mode wifi tcp aprs droid -> lora
    if (!configuration.getBool("internet")) {
        if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { 
            afficheur->displayRssi(false);
            afficheur->parse(msg);
            Serial.println("msg bt queue");
            Serial.println(msg);
            digitalWrite(LED, HIGH);
            lora.send(msg, ad->getPduLength());
            digitalWrite(LED, LOW);
        }
    }
     
    delay(1);    
}
