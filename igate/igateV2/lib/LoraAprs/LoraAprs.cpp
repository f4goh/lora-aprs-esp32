/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LoraAprs.cpp
 * Author: ale
 * 
 * Created on 19 mars 2022, 18:04
 */

#include "LoraAprs.h"

LoraAprs::LoraAprs(long _frequency,int _spreadingFactor,long _signalBandwith, int _codingRate,int _txPower):
frequency(_frequency),
spreadingFactor(_spreadingFactor),
signalBandwith(_signalBandwith),
codingRate(_codingRate),
txPower(_txPower)        
{
    anchor = this;
    txing=false;
}

LoraAprs::LoraAprs(const LoraAprs& orig) {
}

LoraAprs::~LoraAprs() {
}

void LoraAprs::setup(){
   Serial.println("Set SPI pins!");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
 
  Serial.println("Set LoRa pins!");
  setPins(LORA_CS, LORA_RST, LORA_IRQ);

  
  Serial.println("frequency: ");
  Serial.println(String(frequency));
  if (!begin(frequency)) {
    Serial.println("Starting LoRa failed!");    
    while (true) {
    }
  }
  setSpreadingFactor(spreadingFactor);
  setSignalBandwidth(signalBandwith);
  setCodingRate4(codingRate);
  enableCrc();
  
  setTxPower(txPower);//Config.lora.power);
  Serial.println("LoRa init done!");
  //msg.reserve(100);
  //Serial.println(msg.size()));
  xTaskCreatePinnedToCore(LoraAprs::marshall, "lora_rx", 10000, NULL, 3, &TaskHandle_Rx, 1); // creation de la tache 
}

void LoraAprs::send(char* msg,int length){
    txing=true;    
    beginPacket();
    // Header:
    write('<');
    write(0xFF);
    write(0x01);
    // APRS Data:
    write((const uint8_t *) msg,length);
    endPacket();
    txing=false;
}

void LoraAprs::marshall(void * parametres) {
    anchor->receive();
}

bool LoraAprs::checkMessage() {
      
  if (!parsePacket()) {
    return false;
  }
  // read header:
  char dummy[4];
  readBytes(dummy, 3);
  if (dummy[0] != '<') {
    // is no APRS message, ignore message
    while (available()) {
      read();
    }
    return false;
  }
  // read APRS data:
  int n=0;
  while (available() && n<MSG_MAX_LENGTH-1) {
    msg[n] = (char)read();
    n++;
  }
  msg[n]='\0';
  return true;
}


void LoraAprs::receive() {

    Serial.println("Tache RX en fonctionnement");

    while (1) {
        vTaskDelay(1); //indispensable car sinon guru ?!
        if (!txing) {
            if (checkMessage()) {
                //Serial.println(getMessage());                
                xQueueSend(queueRx, msg, 0);   //0 non bloquant au lieu de portMAX_DELAY
            }
        }

    }
}

QueueHandle_t LoraAprs::queueRx = xQueueCreate(3, sizeof (msg));
LoraAprs* LoraAprs::anchor = NULL;

