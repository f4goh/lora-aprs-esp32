/* 
 * File:   main.cpp
 * Author: ale
 * Igate not finished
 * Created on 6 avril 2022, 13:14
 * todo :
 * check connexion
 * display msg
 * add console config
 *  F4KMN-1
  12443
 4753.41N
 00016.61E
 LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
 
 
 */
#include <Arduino.h>
#include <LoraAprs.h>
#include <Afficheur.h>          // Afficheur SSD1306
#include <GestionWifi.h>
#include <Aprsis.h>
#include <AprsDroidTcp.h>
#include <console.h>
#include <Preferences.h>



#define LED 25 //green led 

#define FREQUENCY 433775000
#define TIMEOUT_MENU 10 //10 secondes

LoraAprs lora;
Afficheur *afficheur;
Aprsis *is;
GestionWifi *gsWifi;
AprsDroidTcp *ad;
Console con;
Preferences preferences;


bool exitFlag;


        
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
    Serial.println(F("Set ssid : ssid mywifi"));
    Serial.println(F("Set password : pass toto"));
    Serial.println(F("Set new callsign : call f4goh-6"));
    Serial.println(F("Set latitude in degrees minutes decimal: latitude 4753.41N"));
    Serial.println(F("Set longitude in degrees minutes decimal: longitude 00016.61E"));
    Serial.println(F("Set new comment : comment hello"));
    Serial.println(F("Enable wifi for igate (aprs.fi): internet 1"));
    Serial.println(F("Enable local wifi Access Point for AprsDroid: internet 0"));
    Serial.println(F("When disable igate, local wifi AP are enable"));
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
                    preferences.putString("call",p);                    
    }
}

void _ssid_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,3,20);
    if (ret==true) {
                    S.printf("ssid is %s\n\r", p.c_str());
                    preferences.putString("ssid",p);                    
    }
}

void _pass_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,3,20);
    if (ret==true) {
                    S.printf("password is %s\n\r", p.c_str());
                    preferences.putString("pass",p);                    
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

void _latitude_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,8,8);
    if (ret==true) {
                    S.printf("latitude is %s\n\r", p.c_str());
                    preferences.putString("latitude",p);                    
    }
}

void _longitude_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=acceptCmd(p,9,9);
    if (ret==true) {
                    S.printf("longitude is %s\n\r", p.c_str());
                    preferences.putString("longitude",p);                    
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
        preferences.putString("comment", p);
    }
}


void _config_(ArgList& L, Stream& S) {
    Serial.printf("Ssid is : %s\n\r",preferences.getString("ssid").c_str());
    Serial.printf("password is : %s\n\r",preferences.getString("pass").c_str());
    Serial.printf("Call is : %s\n\r",preferences.getString("call").c_str());
    Serial.printf("Frequency is : %d\n\r",preferences.getLong("freq"));
    Serial.printf("latitude is : %s\n\r",preferences.getString("latitude").c_str());
    Serial.printf("longitude is : %s\n\r",preferences.getString("longitude").c_str());
    Serial.printf("Internet is : %s\n\r", preferences.getBool("internet") ? "Enable" : "Disable");
    Serial.printf("TCP Access point is : %s\n\r", preferences.getBool("internet") ? "Disable" : "Enable");
    Serial.printf("Comment is : %s\n\r",preferences.getString("comment").c_str());
}



void _raz_(ArgList& L, Stream& S) {
  preferences.putBool("config",false);
  preferences.clear();  
  ESP.restart();    
}

void _exit_(ArgList& L, Stream& S) {
    exitFlag=true;    
}


void _internet_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = acceptCmd(p, 1, 1);
    if (ret == true) {
        preferences.putBool("internet", (int8_t) p.toInt());
        Serial.printf("Internet is : %s\n\r", preferences.getBool("internet") ? "Enable" : "Disable");
        Serial.printf("TCP Access point is : %s\n\r", preferences.getBool("internet") ? "Disable" : "Enable");
    }
}

void _unknown(String& L, Stream& S) {
    S.print("Error");
    //S.println(L);
}

void menu() {
    char car;
    int cpt = 0;
    exitFlag = false;
    while ((cpt < TIMEOUT_MENU) && !exitFlag) {
        if (Serial.available() > 0) {
            car = Serial.read();
            if (car == 'm') {
                Serial.println(F("help command for info"));
                while (exitFlag == false) {
                    con.run();
                }
            }
        }
        Serial.print(".");
        delay(1000);
        cpt++;
    }
    Serial.println("\nConsole exit");
}

void setupConsole(){
  con.onCmd("help", _help_);
  con.onCmd("ssid", _ssid_);
  con.onCmd("pass", _pass_);
  con.onCmd("call", _call_);
  con.onCmd("freq", _freq_);
  con.onCmd("latitude", _latitude_);
  con.onCmd("longitude", _longitude_);
  con.onCmd("comment", _comment_);
  con.onCmd("shconfig", _config_);
  con.onCmd("raz", _raz_);
  con.onCmd("exit", _exit_);
  con.onCmd("internet", _internet_);

  con.onUnknown(_unknown);
    
  preferences.begin("igate", false);
    
  if (preferences.getBool("config",false)==false){
      preferences.putString("ssid","nossid");
      preferences.putString("pass","nopassword");
      preferences.putString("call","nocall");
      preferences.putLong("freq",FREQUENCY);
      preferences.putString("latitude","0000.00N");
      preferences.putString("longitude","00000.00E");
      preferences.putString("comment","LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32");
      preferences.putBool("internet",false);
      preferences.putBool("config",true);
  }
  
  //preferences.end(); //inutile
  
  menu();
}


String passCode(String call){
    call.toUpperCase();
    int id=call.indexOf('-');
    if (id!=-1){
        call=call.substring(0,id);    
    }
    int hash=0x73e2;
    int i=0;
    int len=call.length();
    while (i<len){
        hash^=(int) call.charAt(i)<<8;
        hash^=(int) call.charAt(i+1);
        i+=2;
    }
    hash&=0x7fff;    
    
    return String(hash);
}

void erreur(String msg){
    Serial.print("Wifi Sartup Error on ");
    Serial.println(msg);
    Serial.println("Check configuration");
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
    //afficheur->afficher("Test");
    
    
    setupConsole();
   
    bool ret;

    if (!preferences.getBool("internet")) {
        Serial.println("APRS Droid gate");
        gsWifi = new GestionWifi;
        ad = new AprsDroidTcp;
        ret = gsWifi->setup("APRS Droid", "totototo", ACCES_POINT);
        if (ret) {
            ret = ad->setup();
            if (ret) {
                Serial.println("Access point WIFI enable");
            }
            else erreur("TCP local access point");
        }

    } else {
        Serial.println("wifi igate on internet");
        String pass = passCode(preferences.getString("call"));
        afficheur->afficher(pass);
        Serial.println(pass);
        gsWifi = new GestionWifi;
        is = new Aprsis;
        ret = gsWifi->setup(preferences.getString("ssid").c_str(), preferences.getString("pass").c_str(), CLIENT);
        if (ret) {
            ret = is->setup(preferences.getString("call"), pass, preferences.getString("latitude"), preferences.getString("longitude"), preferences.getString("comment"));
            if (ret) {
                Serial.println("Connexion au serveur Aprs done");
            }
            else erreur("Internet igate server");
        }
    }
}


void loop() {
    delay(1);
    char msg[MSG_MAX_LENGTH];


    if (xQueueReceive(lora.queueRx, msg, 0) == pdPASS) { //lora -> internet ou tcp aprs droid
        Serial.println("msg queue");
        Serial.println(msg);
        Serial.print("with RSSI ");
        Serial.println(lora.packetRssi());
        if (!preferences.getBool("internet")) {
            ad->send(msg);
        } else {
            xQueueSend(is->queueMsg, msg, 0); //via internet
        }
    }
    if (!preferences.getBool("internet")) {
        if (xQueueReceive(ad->queueRx, msg, 0) == pdPASS) { //wifi tcp aprs droid-> lora
            Serial.println("msg bt queue");
            Serial.println(msg);
            digitalWrite(LED, HIGH);
            lora.send(msg, ad->getPduLength());
            digitalWrite(LED, LOW);
        }
    }
}
