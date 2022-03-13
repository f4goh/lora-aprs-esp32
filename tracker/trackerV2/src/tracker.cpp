/* 
 * File:   tracker.cpp
 * Author: F4GOH
 *
 * Created on 12 mars 2022, 17:02
 * board = ttgo-t-beam T22_V1.1
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
 * 
 * F4KMN-7>APLT00,WIDE1-1,qAO,F4KMN-1:!4753.41N/00016.61EO/A=000258  -  _Bat.: 4.13V - Cur.: -102mA
 * 
 * todo :
 * - Add lora high level tx/rx class
 * - Add serial configuration
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <power_management.h>
#include <Wire.h>
#include <Position.h>           // Trame Position APRS 

#define POSITION_COMPRESSED false  //true = compressed format
#define FREQUENCY 433775000
#define SPREADING_FACTOR 12
#define SIGNAL_BANDWIDTH 125000
#define CODING_RATE_4 5
#define TX_POWER 20

#define INTERVAL_BETWEEN_TX 20 //in sec


#define LED 4 //used for ptt

PowerManagement powerManagement;

HardwareSerial serialGps(1); // GPS sur hardware serial 1
TinyGPSPlus gps;

Position pos("F4KMN-7","APLT00","WIDE1-1",48.010237, 0.206267, "Ballon", '/', 'O'); // icon ballon


static bool lectureGPS(unsigned long ms);
void setup_lora();
void sendLora();
void checkBattery();


void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
    Wire.begin(SDA, SCL);
  if (!powerManagement.begin(Wire)) {
    Serial.println("AXP192 init done!");
  } else {
    Serial.println("AXP192 init failed!");
  }
  powerManagement.activateLoRa();
  powerManagement.activateOLED();
  powerManagement.activateGPS();
  powerManagement.activateMeasurement();
  

  serialGps.begin(9600, SERIAL_8N1, 34, 12);
  setup_lora();
  
}

void loop() {
    if (lectureGPS(1000)) {
        //displayInfo();
        if (gps.time.isValid()) {
            if (gps.time.hour() < 10) Serial.print(F("0"));
            Serial.print(gps.time.hour());
            Serial.print(F(":"));
            if (gps.time.minute() < 10) Serial.print(F("0"));
            Serial.print(gps.time.minute());
            Serial.print(F(":"));
            if (gps.time.second() < 10) Serial.print(F("0"));
            Serial.println(gps.time.second());

            if (gps.time.second() % INTERVAL_BETWEEN_TX == 0) {
                if (gps.location.isValid()) {
                    checkBattery();
                    sendLora();
                }
            }

        }

    }
}


void checkBattery(){
    static bool   BatteryIsConnected;
    static String batteryVoltage       = "";
    static String batteryChargeCurrent = "";
    BatteryIsConnected = powerManagement.isBatteryConnect();
  if (BatteryIsConnected) {
    batteryVoltage       = String(powerManagement.getBatteryVoltage(), 2);
    batteryChargeCurrent = String(powerManagement.getBatteryChargeDischargeCurrent(), 0);
    pos.setComment(" -  _Bat.: " + batteryVoltage + "V - Cur.: " + batteryChargeCurrent + "mA");
  }
  else{
      pos.setComment("Powered by USB");
  }        
}


/**
 * 
 * @param ms le temps de lecture en ms
 * @return  bool indique la présence de nouvelle donnée
 */
static bool lectureGPS(unsigned long ms) {
    bool newData = false;
    unsigned long start = millis();
    do {
        while (serialGps.available())
            if (gps.encode(serialGps.read())) {
                newData = true;
            }
    } while (millis() - start < ms);
    return newData;
}


void setup_lora() {
    //LoRa() : frequencyRx(433775000), frequencyTx(433775000), power(20), spreadingFactor(12), signalBandwidth(125000), codingRate4(5)
  Serial.println("Set SPI pins!");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
 
  Serial.println("Set LoRa pins!");
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  long freq = FREQUENCY;        //Config.lora.frequencyTx;
  Serial.println("frequency: ");
  Serial.println(String(freq));
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");    
    while (true) {
    }
  }
  LoRa.setSpreadingFactor(SPREADING_FACTOR); //Config.lora.spreadingFactor);
  LoRa.setSignalBandwidth(SIGNAL_BANDWIDTH); //Config.lora.signalBandwidth);
  LoRa.setCodingRate4(CODING_RATE_4); //Config.lora.codingRate4);
  LoRa.enableCrc();

  LoRa.setTxPower(TX_POWER);//Config.lora.power);
  Serial.println("LoRa init done!");  
 
}

void sendLora() {
    digitalWrite(LED,HIGH);
    pos.setLatitude(gps.location.lat());
    pos.setLongitude(gps.location.lng());
    pos.setAltitude(gps.altitude.meters());
    //pos.setComment("test");
    Serial.println(pos.getPduAprs(POSITION_COMPRESSED));
    LoRa.beginPacket();
    // Header:
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    // APRS Data:
    LoRa.write((const uint8_t *) pos.getPduAprs(POSITION_COMPRESSED), pos.getPduLength());
    LoRa.endPacket();
    digitalWrite(LED,LOW);
}