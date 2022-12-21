/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LoraAprs.h
 * Author: ale
 *
 * Created on 19 mars 2022, 18:04
 * TRX SX1278 pinout is in pins_arduino.h file (board environement)
 * /home/ale/.platformio/packages/framework-arduinoespressif32/variants/tbeam
 * // SPI LoRa Radio
 * #define LORA_SCK 5      // GPIO5  - SX1276 SCK
 * #define LORA_MISO 19    // GPIO19 - SX1276 MISO
 * #define LORA_MOSI 27    // GPIO27 - SX1276 MOSI
 * #define LORA_CS 18      // GPIO18 - SX1276 CS
 * #define LORA_RST 23     // GPIO23 - SX1276 RST
 * #define LORA_IRQ 26     // GPIO26 - SX1276 IO0
 * #define LORA_IO0 LORA_IRQ  // alias
 * #define LORA_IO1 33     // GPIO33 - SX1276 IO1 -> wired on pcb AND connected to header pin LORA1
 * #define LORA_IO2 32     // GPIO32 - SX1276 IO2 -> wired on pcb AND connected to header pin LORA2
 */

#ifndef LORAAPRS_H
#define LORAAPRS_H

#include <LoRa.h>
#include <Arduino.h>


#define FREQUENCY 433775000
#define SPREADING_FACTOR 12
#define SIGNAL_BANDWIDTH 125000
#define CODING_RATE_4 5
#define TX_POWER 20


class LoraAprs : public LoRaClass {
public:
    LoraAprs(long _frequency=FREQUENCY,
            int _spreadingFactor=SPREADING_FACTOR,
            long _signalBandwith=SIGNAL_BANDWIDTH,
            int _codingRate=CODING_RATE_4,
            int _txPower=TX_POWER);
    LoraAprs(const LoraAprs& orig);
    virtual ~LoraAprs();
    void setup();
    void send(char* msg,int length);



private:
  long frequency;
  int spreadingFactor;
  long signalBandwith;
  int codingRate;
  int txPower;
  
};

#endif /* LORAAPRS_H */

