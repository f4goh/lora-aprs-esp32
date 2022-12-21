/* 
 * File:   tracker.cpp V3
 * Author: F4GOH
 *
 * Created on 12 mars 2022, 17:02
 * board = ttgo-t-beam T22_V1.1
 * 
 * F4KMN-7>APLT00,WIDE1-1,qAO,F4KMN-1:!4753.41N/00016.61EO/A=000258  -  _Bat.: 4.13V - Cur.: -102mA
 * 
 * https://github.com/akoro/Console
 * 
 * https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

 * AXP202X_Library
 * ===============
 * #ID: 6657
 * Arduino library for X-Power AXP202 chip. Arduino library for X-Power AXP202 chip. Tested with ESP32
 * pio lib install 6657
 * 
 * LoRa
 * ====
 * #ID: 1167
 * An Arduino library for sending and receiving data using LoRa radios. Supports Semtech SX1276/77/78/79 based boards/shields.
 * pio lib install 1167
 * Library Manager: LoRa @ 0.8.0 has been installed!
 * TinyGPSPlus
 * ===========
 * #ID: 1655
 * A new, customizable Arduino NMEA parsing library
 * pio lib install 1655
 * Library Manager: TinyGPSPlus @ 1.0.3 has been installed!
 * Console
 * =======
 * #ID: 6893
 * Small user console class for embedded systems based on arduino framework
 * pio lib install 6893
 * Library Manager: Console @ 1.2.1 has been installed!
 * 
 * #ID: 5746
 * SparkFun u-blox Arduino Library
 * pio lib install 5746
 * SparkFun u-blox Arduino Library @ 1.8.11 has been installed!
 * 
 * todo:
 * add optional ssd1306 display
 * ajout fonction envoi message en fonction d'un temps horaire plus large
 * 
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <power_management.h>
#include <Wire.h>
#include <Position.h>           // Trame Position APRS
#include <LoraAprs.h>
#include <Preferences.h>
#include <Menu.h>  
#include "SparkFun_Ublox_Arduino_Library.h"
#include <Message.h>
#include <esp_task_wdt.h>

//https://github.com/lora-aprs/LoRa_APRS_iGate/blob/master/src/LoRa_APRS_iGate.cpp
//watch dog


#define POSITION_COMPRESSED false  //true = compressed format
#define FREQUENCY 433775000


#define INTERVAL_BETWEEN_TX 20 //in sec


#define LED 4 //used for ptt
#define BP 38 //used to force tx

PowerManagement powerManagement;

HardwareSerial serialGps(1); // GPS sur hardware serial 1
TinyGPSPlus gps;

Console con;
LoraAprs lora;
//Afficheur *afficheur;
Preferences configuration;
Menu *leMenu;

SFE_UBLOX_GPS myGPS;


Position pos("F4KMN-7","APLT00","WIDE1-1",48.010237, 0.206267, "Ballon", '/', 'O'); // icon ballon

Message mes("F4GOH-9","APLT00","WIDE1-1","+336xxxxxxxx");

bool smsFlag;  //evite de d'envoyer plusieurs fois la trame dans la meme minute


static bool lectureGPS(unsigned long ms);
void sendLoraPosition();
void sendLoraSms();
void checkBattery();
void checkPosition();
void checkPdu();
void updateDynamicPlatformModel(uint8_t model);
void saveDynamicPlatformModel();


void setup() {
  //nvs_flash_erase(); // erase the NVS partition and...
  //nvs_flash_init(); // initialize the NVS partition.
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(BP,INPUT);
  
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
  
  lora.setup();
  //afficheur = new Afficheur;
    
  leMenu = new Menu;  // Menu de configuration
  leMenu->setup();
    
  //setupConsole();
  configuration.begin("tracker", false);
  pos.setCallsign(configuration.getString("call"));
  lora.setFrequency(configuration.getLong("freq"));
  pos.setComment(configuration.getString("comment"));  
  pos.setSymbol(configuration.getChar("symbol"));    
  //second=(int8_t) configuration.getChar("second");
  mes.setPhone(configuration.getString("smsphone"));
  
  if (myGPS.begin(serialGps) == true) //Connect to the Ublox module using serial
  {
        uint8_t model=configuration.getUChar("navmod");
        if (model>0) model++; //see enum dynModel in SparkFun_Ublox_Arduino_Library.h
        updateDynamicPlatformModel(model);
  }
  else
  {
    Serial.println(F("Ublox GPS not detected. Please check wiring. Run in portable mode."));
  }
  smsFlag=false;
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);
}

// force txing with 'p' key on serial terminal or push button (middle of reset and off button)
void checkPdu() {
    char car;
    bool tx = false;
    if (digitalRead(BP) == 0) {
        tx = true;
    }
    if (Serial.available() > 0) {
        car = Serial.read();
        if (car == 'p') {
            tx = true;
        }
    }
    if (tx) {
        checkBattery();
        sendLoraSms();        
    }
}



void loop() {
    esp_task_wdt_reset();
    checkPdu();
    
    // display time and sync txing
    if (lectureGPS(1000)) {
        digitalWrite(LED, digitalRead(LED)^1);
        if (gps.time.isValid()) {
            if (gps.time.hour() < 10) Serial.print(F("0"));
            Serial.print(gps.time.hour());
            Serial.print(F(":"));
            if (gps.time.minute() < 10) Serial.print(F("0"));
            Serial.print(gps.time.minute());
            Serial.print(F(":"));
            if (gps.time.second() < 10) Serial.print(F("0"));
            Serial.println(gps.time.second());

            if (gps.time.second() == configuration.getChar("second")) {  //position
            //if (gps.time.second()%15 == 0) {
                if (gps.location.isValid()) {
                    checkBattery();
                    sendLoraPosition();
                }
            }
            if (configuration.getUChar("sms") > 0 && smsFlag==false) {  //sms
                if (gps.time.minute()%configuration.getChar("smsmin")==0) {                    
                    sendLoraSms();
                    smsFlag=true;                    
                }
            }
            if (smsFlag==true && gps.time.minute()%configuration.getChar("smsmin")!=0){ //flipflop sms
                smsFlag=false;
            }
        }
        else{
            Serial.println(F("Wait fix GPS")); 
        }

    }     
}

// build APRS comment data if setbat enable otherwise comment default
void checkBattery() {
    bool BatteryIsConnected;
    String batteryVoltage = "";
    String batteryChargeCurrent = "";
    if (configuration.getBool("setbat") == true) {        
        BatteryIsConnected = powerManagement.isBatteryConnect();
        if (BatteryIsConnected) {
            batteryVoltage = String(powerManagement.getBatteryVoltage(), 2);
            batteryChargeCurrent = String(powerManagement.getBatteryChargeDischargeCurrent(), 0);
            pos.setComment("Bat:" + batteryVoltage + "V/" + batteryChargeCurrent + "mA");
            
        } else {
            pos.setComment("Powered by USB");
        }
    } else {
        pos.setComment(configuration.getString("comment"));
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

//build position
void checkPosition() {
    pos.setLatitude(gps.location.lat());
    pos.setLongitude(gps.location.lng());    
    pos.enableAltitude(configuration.getBool("setalt"));
    pos.enableCourseSpeed(configuration.getBool("setcs"));
    pos.setAltitude(gps.altitude.meters());
    pos.setCourse((int)gps.course.deg());
    pos.setSpeed((int)gps.speed.knots());
    char * loc;
    loc=pos.latlng2loc(HUIT_DIGITS);
    Serial.println(loc);
    mes.setPosition(gps.location.lat(),gps.location.lng());
    mes.setComment(String(loc));
}

void sendLoraPosition() {    
    checkPosition();
    char *pdu;   
    digitalWrite(LED, LOW);
    Serial.println(pos.getPduAprs(configuration.getBool("setcomp"))); 
    lora.send(pos.getPduAprs(configuration.getBool("setcomp")), pos.getPduLength());
    digitalWrite(LED, HIGH);
}

void sendLoraSms() {    
    checkPosition();
    char *pdu;   
    digitalWrite(LED, LOW);
    uint8_t mode=configuration.getUChar("sms");        
    switch(mode){
            case 0 : Serial.printf("Disable SMS error\n\r"); break;
            case 1 : pdu=mes.getPduSMS(GOOGLE); break;
            case 2 : pdu=mes.getPduSMS(FREE_TXT); break;
    }
    Serial.println(pdu);
    lora.send(pdu, mes.getPduLength());
    digitalWrite(LED, HIGH);
}


void updateDynamicPlatformModel(uint8_t model) {
    
    //writing
    if (myGPS.setDynamicModel((dynModel) model) == false) // Set the dynamic model to PORTABLE
    {
        Serial.println(F("***!!! Warning: setDynamicModel failed !!!***"));
    } else {
        Serial.println(F("Dynamic platform model changed successfully!"));
    }

    
    // Let's read the new dynamic model to see if it worked
    uint8_t newDynamicModel = myGPS.getDynamicModel();
    if (newDynamicModel == 255) {
        Serial.println(F("***!!! Warning: getDynamicModel failed !!!***"));
    } else {
        if (newDynamicModel>0) newDynamicModel--;
        Serial.print(F("The new dynamic model is: "));
        Serial.println(leMenu->getDynModName(newDynamicModel));
    }

}

//not used
void saveDynamicPlatformModel(){
    myGPS.saveConfigSelective(VAL_CFG_SUBSEC_NAVCONF);
}