/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Aprsis.cpp
 * Author: ale
 * 
 * Created on 8 avril 2022, 19:04
 */

#include "Aprsis.h"

Aprsis::Aprsis()
{
    tool_name=TOOL_NAME;
    version=VERSION;
    anchor = this;
}

Aprsis::Aprsis(const Aprsis& orig) {
}

Aprsis::~Aprsis() {
}


bool Aprsis::setup(const String _user, const String _passcode,const String _latitude,const String _longitude,const String _message,const String _server,const int _port) {
  user=_user;
  passcode=_passcode;
  longitude =_longitude;
  latitude=_latitude;
  message=_message;
  serverAprs=_server;
  portAprs=_port;
  BaseType_t ret=xTaskCreatePinnedToCore(Aprsis::marshall, "gestion_aprsis", 10000, NULL, 2, &TaskHandle_Ait, 1);
  return (ret==pdPASS) ? true : false;
}

bool Aprsis::connect(const String server, const int port) {    
  const String login = "user " + user + " pass " + passcode + " vers " + tool_name + " " + version + "\n\r";
  Serial.println("connexion Serveur APRS en cours...");
  Serial.print(login);
  return _connect(server, port, login);
}

bool Aprsis::connect(const String server, const int port, const String filter) {
  const String login = "user " + user + " pass " + passcode + " vers " + tool_name + " " + version + " filter " + filter + "\n\r";
  return _connect(server, port, login);
}

bool Aprsis::_connect(const String server, const int port, const String login_line) {
  if (!client.connect(server.c_str(), port)) {
    Serial.println("Something went wrong on connecting! Is the server reachable?");
    return false;
  }
  Serial.println("connexion au serveur aprs");
  sendMessage(login_line);
  while (true) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line.indexOf("logresp") != -1) {
      if (line.indexOf("unverified") == -1) {
        return true;
      } else {
        Serial.print("User can not be verified with passcode!");
        return false;
      }
    }
  }  
  return true;
}

bool Aprsis::connected() {
  return client.connected();
}

void Aprsis::setRssi(int _rssi){
    rssi=_rssi;    
}

bool Aprsis::sendMessage(const String message) {
  if (!connected()) {
    Serial.println("pas de connexion");
    return false;
  }
  client.println(message);
  return true;
}

bool Aprsis::sendBeacon() {
  String  msgBeacon;
  //F4KMN-1>APLG01:=4753.41NL00016.61E&LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32 + crlf
  msgBeacon=user+">APLG01:="+latitude+"L"+longitude+"&"+message+ "\n\r";
  Serial.print(msgBeacon);
  return sendMessage(msgBeacon);
}


int Aprsis::available() {
  return client.available();
}

String Aprsis::getMessage() {
  String line;
  if (client.available() > 0) {
    line = client.readStringUntil('\n');
  }
  return line;
}


String Aprsis::getAPRSMessage() {
  String line;
  if (client.available() > 0) {
    line = client.readStringUntil('\n');
  }
  if (line.startsWith("#")) {
    Serial.println(line);
    return "";
  }
  if (line.length() == 0) {
    return "";
  }    
  return msg;
}


void Aprsis::connexion() {
    bool ret;
    if (!connected()) {
        Serial.println("Re init Connexion APRS..");
        client.stop();
    }
    //ret = connect(APRS_SERVER, APRS_PORT);
    ret = connect(serverAprs, portAprs);
    if (!ret) {
        Serial.println("pas de connexion au serveur aprs");
    } else {
        Serial.println("Connexion APRS ok");
        sendBeacon();
    }
}

void Aprsis::marshall(void * parametres) {
    anchor->aprsIsTask();
}




void Aprsis::aprsIsTask(){
    String strMsg;
    int cpt=BEACON_DELAY;
    Serial.println("Tache APRS-IS en fonctionnement");
    while (1) {
        vTaskDelay(1);
        if (cpt == BEACON_DELAY) { //envoie la position balise périodiquement
            connexion();
            cpt = 0;
        }
        cpt++;
        if (!connected()) {
            connexion();
            vTaskDelay(RECONNECT_DELAY);
        } else {
            if (xQueueReceive(queueMsg, msg, 0) == pdPASS) { //0 au lieu de portMAX_DELAY = non bloquant
                Serial.println("msg aprsis queue");
                Serial.println(msg);
                strMsg = String(msg) + " RSSI:" + String(rssi) + " dBm\n\r";
                if (!sendMessage(strMsg)) {
                    Serial.println("error sending pdu");
                }
            }
            strMsg = getMessage();
            if (strMsg.length() > 0) {
                Serial.println(strMsg);
            }
        }
    }
}

String Aprsis::passCode(String call){
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



QueueHandle_t Aprsis::queueMsg = xQueueCreate(3, sizeof (msg));
Aprsis* Aprsis::anchor = NULL;

/*
 * pour plus tard peut être...
 vTaskSuspend( xHandle );
 vTaskResume( xHandle );
  */
