/* 
 * File:   main.cpp
 * Author: ale
 * Igate not finished
 * Created on 6 avril 2022, 13:14
 * todo :
 * check connexion
 * display msg
 * add console config
 */
#include <Arduino.h>
//#include <LoRa.h>
#include <LoraAprs.h>
#include <Position.h> 
#include <Afficheur.h>          // Afficheur SSD1306
//#include <ESP32Time.h>          // RTC pour esp32
#include <Aprsis.h>



#define LED 25 //green led 

LoraAprs lora;

Afficheur *afficheur;

Aprsis is;

//ESP32Time rtc;

Position pos("F4GOH-5","APLT00","WIDE1-1",48.010237, 0.206267, "Ballon", '/', 'O'); // pour test

#define POSITION_COMPRESSED false  //true = compressed format
#define FREQUENCY 433775000



void sendLora();

void setup() {
    //nvs_flash_erase(); // erase the NVS partition and...
    //nvs_flash_init(); // initialize the NVS partition.
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    lora.setup();
    afficheur = new Afficheur;
    afficheur->afficher("Test");
    
      
    bool ret = is.setup("Livebox-0E00", "FzhSJGFouVGNLYKAMa", "F4KMN-1", "12443","4753.41N","00016.61E","LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32");
    if (ret) {
          Serial.println("Connexion au serveur Aprs done");
    }
  
  //rtc.setTime(data.seconde, data.minute, data.heure, day, month, year);  
    
}

void loop() {
    delay(1);
    
    char c;
    char msg[MSG_MAX_LENGTH];
    
    if (Serial.available()>0){
        c=Serial.read();
        if (c=='t'){
           sendLora(); 
        }
    }
    
    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //0 ou portMAX_DELAY
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        xQueueSend(is.queueMsg, msg, 0);
    }

}



void sendLora() {        
    Serial.println(pos.getPduAprs(false));
    digitalWrite(LED, HIGH);
    lora.send(pos.getPduAprs(false), pos.getPduLength());
    digitalWrite(LED, LOW);
}
