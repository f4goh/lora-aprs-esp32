/* 
 * File:   tracker.cpp
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
 * 
 */

#include <Arduino.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <power_management.h>
#include <Wire.h>
#include <Position.h>           // Trame Position APRS
#include <LoraAprs.h>
#include <console.h>
#include <Preferences.h>
//#include <nvs_flash.h>



#define POSITION_COMPRESSED false  //true = compressed format
#define FREQUENCY 433775000


#define INTERVAL_BETWEEN_TX 20 //in sec


#define LED 4 //used for ptt

PowerManagement powerManagement;

HardwareSerial serialGps(1); // GPS sur hardware serial 1
TinyGPSPlus gps;

Console con;
LoraAprs lora;
Preferences preferences;

Position pos("F4KMN-7","APLT00","WIDE1-1",48.010237, 0.206267, "Ballon", '/', 'O'); // icon ballon

bool exitFlag;
int8_t second;


static bool lectureGPS(unsigned long ms);
void sendLora();
void checkBattery();
void checkPosition();

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
    Serial.println(F("Set new callsign : call f4goh-6"));
    Serial.println(F("Transmit every minutes at (0-59) second : second 10"));
    Serial.println(F("Set frequency : freq 433775000"));
    Serial.println(F("Set car symbol : symbol >"));
    Serial.println(F("Set new comment : comment hello"));
    Serial.println(F("Set Battery measurement in comment (0 or 1) : setbat 1"));
    Serial.println(F("Set altitude feild in pdu (0 or 1) : setalt 1"));
    Serial.println(F("Set compression position (0 or 1) : setcomp 1"));
    Serial.println(F("Get pdu (to see APRS data) : getpdu"));
    Serial.println(F("Show configuration : shconfig"));
    Serial.println(F("Reset default configuration : raz"));
    Serial.println(F("Exit menu : exit"));
}

void _call_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,7,10);
    if (ret==true) {
                    S.printf("call is %s\n\r", p.c_str());
                    pos.setCallsign(p);
                    preferences.putString("call",p);                    
    }
}

void _second_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,2,2);
    if (ret==true) {
                    S.printf("seconds is %s\n\r", p.c_str());
                    second=(int8_t) p.toInt();
                    preferences.putChar("second",(int8_t) p.toInt());                    
    }
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

void _symbol_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,1,1);
    if (ret==true) {
                    S.printf("symbol is %s\n\r", p.c_str());
                    pos.setSymbol((char) p.c_str()[0]);
                    preferences.putChar("symbol",(char) p.c_str()[0]);                    
    } 
}

void _comment_(ArgList& L, Stream& S) {
    String arg;
    bool ret;
    String p = "";
    while (!(arg = L.getNextArg()).isEmpty()) {
        p = p + arg+" ";
    }
    ret = acceptCmd(p, 3, 30);
    if (ret == true) {
        S.printf("comment is %s\n\r", p.c_str());
        pos.setComment(p);
        preferences.putString("comment", p);
    }
}

void _setbat_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,1,1);
    if (ret==true) {                                        
                    preferences.putBool("setbat",(int8_t) p.toInt());
                    Serial.printf("Battery measurement is : %s\n\r",preferences.getBool("setbat")? "Enable" : "Disable");                    
    }  
}

void _config_(ArgList& L, Stream& S) {
    Serial.printf("Call is : %s\n\r",preferences.getString("call").c_str());
    Serial.printf("Symbol is : %c\n\r",preferences.getChar("symbol"));
    Serial.printf("Frequency is : %d\n\r",preferences.getLong("freq"));
    Serial.printf("Transmit at second : %d \n\r",preferences.getChar("second"));
    Serial.printf("Battery measurement is : %s\n\r",preferences.getBool("setbat")? "Enable" : "Disable");
    Serial.printf("Altitude is : %s\n\r",preferences.getBool("setalt")? "Enable" : "Disable");
    Serial.printf("Compression is : %s\n\r",preferences.getBool("setcomp")? "Enable" : "Disable");
    Serial.printf("Comment is : %s\n\r",preferences.getString("comment").c_str());
}

void _raz_(ArgList& L, Stream& S) {
  //preferences.putBool("config",true);
  preferences.clear();  
  ESP.restart();    
}

void _exit_(ArgList& L, Stream& S) {
    exitFlag=true;    
}

void _getpdu_(ArgList& L, Stream& S) {
   checkBattery();
   checkPosition();
   Serial.println(pos.getPduAprs(preferences.getBool("setcomp")));
}

void _setalt_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = acceptCmd(p, 1, 1);
    if (ret == true) {
        preferences.putBool("setalt", (int8_t) p.toInt());
        Serial.printf("Altitude is : %s\n\r", preferences.getBool("setalt") ? "Enable" : "Disable");
    }

}

void _setcomp_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = acceptCmd(p, 1, 1);
    if (ret == true) {
        preferences.putBool("setcomp", (int8_t) p.toInt());
        Serial.printf("Compression position is : %s\n\r", preferences.getBool("setcomp") ? "Enable" : "Disable");
    }
}


void _unknown(String& L, Stream& S) {
    S.print("Error");
    //S.println(L);
}

void setupConsole(){
  con.onCmd("help", _help_);
  con.onCmd("call", _call_);
  con.onCmd("second", _second_);
  con.onCmd("freq", _freq_);
  con.onCmd("symbol", _symbol_);
  con.onCmd("comment", _comment_);
  con.onCmd("setbat", _setbat_);
  con.onCmd("shconfig", _config_);
  con.onCmd("raz", _raz_);
  con.onCmd("exit", _exit_);
  con.onCmd("getpdu", _getpdu_);
  con.onCmd("setalt", _setalt_);
  con.onCmd("setcomp", _setcomp_);
  
  con.onUnknown(_unknown);

  
  preferences.begin("tracker", false);
    
  if (preferences.getBool("config",false)==false){
      preferences.putString("call","nocall");
      preferences.putLong("freq",FREQUENCY);
      preferences.putChar("second",0);
      preferences.putChar("symbol",'>');
      preferences.putBool("setbat",false);
      preferences.putString("comment","test");
      preferences.putBool("setalt",false);
      preferences.putBool("setcomp",false);
  }
  
  pos.setCallsign(preferences.getString("call"));
  lora.setFrequency(preferences.getLong("freq"));
  pos.setComment(preferences.getString("comment"));  
  pos.setSymbol(preferences.getChar("symbol"));    
  //preferences.end(); //inutile
  
}


void setup() {
  //nvs_flash_erase(); // erase the NVS partition and...
  //nvs_flash_init(); // initialize the NVS partition.
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
  
  lora.setup();
  
  setupConsole();
 
  
}


void menu(){
    char car;    
    if (Serial.available()>0){
        car=Serial.read();
        if (car=='m'){
            exitFlag=false;
            Serial.print(F("help command for info"));
            while(exitFlag==false){
                con.run();
            }
        }
    }
}



void loop() {
    
    menu();
    
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

            if (gps.time.second() == preferences.getChar("second")) {
                if (gps.location.isValid()) {
                    checkBattery();
                    sendLora();
                }
            }

        }
        else{
            Serial.println(F("Wait fix GPS")); 
        }

    }     
}

void checkBattery() {
    bool BatteryIsConnected;
    String batteryVoltage = "";
    String batteryChargeCurrent = "";
    if (preferences.getBool("setbat") == true) {        
        BatteryIsConnected = powerManagement.isBatteryConnect();
        if (BatteryIsConnected) {
            batteryVoltage = String(powerManagement.getBatteryVoltage(), 2);
            batteryChargeCurrent = String(powerManagement.getBatteryChargeDischargeCurrent(), 0);
            pos.setComment(" -  _Bat.: " + batteryVoltage + "V - Cur.: " + batteryChargeCurrent + "mA");
            
        } else {
            pos.setComment("Powered by USB");
        }
    } else {
        pos.setComment(preferences.getString("comment"));
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

void checkPosition() {
    pos.setLatitude(gps.location.lat());
    pos.setLongitude(gps.location.lng());

    if (preferences.getBool("setalt") == true) {
        pos.setAltitude(gps.altitude.meters());
    } else {
        pos.setAltitude(0);
    }
}


void sendLora() {    
    checkPosition();
    Serial.println(pos.getPduAprs(preferences.getBool("setcomp")));
    digitalWrite(LED, HIGH);
    lora.send(pos.getPduAprs(preferences.getBool("setcomp")), pos.getPduLength());
    digitalWrite(LED, LOW);
}

