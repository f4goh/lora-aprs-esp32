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
 * todo:
 * check server alive every 20 seconds
 */

#include "Aprsis.h"

Aprsis::Aprsis()
{
    tool_name=TOOL_NAME;
    version=VERSION;
    anchor = this;
    timeNow = 0;
}

Aprsis::Aprsis(const Aprsis& orig) {
}

Aprsis::~Aprsis() {
}


bool Aprsis::setup(const String _user, const String _latitude,const String _longitude,const String _message,const String _server,const int _port) {
  user=_user;
  passcode=passCode(user);
  longitude =_longitude;
  latitude=_latitude;
  message=_message;
  serverAprs=_server;
  portAprs=_port;
  BaseType_t ret=xTaskCreatePinnedToCore(Aprsis::marshall, "gestion_aprsis", 10000, NULL, 2, &TaskHandle_Ait, 1);
  return (ret==pdPASS) ? true : false;
}



bool Aprsis::isConnected() {
  return cnxState;
}

void Aprsis::setRssi(int _rssi){
    rssi=_rssi;    
}


void Aprsis::marshall(void * parametres) {
    anchor->aprsIsTask();
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

    void onAck(AcAckHandler cb, void* arg = 0);             //ack received

    
    void onPoll(AcConnectHandler cb, void* arg = 0);        //every 125ms when connected

void Aprsis::aprsIsTask() {
    // asyncronous tcp client 
    AsyncClient client;

    String msgBeacon = anchor->user + ">APLG01:=" + anchor->latitude + "L" + anchor->longitude + "&" + anchor->message + "\n\r";
   /*
    A Quoi ça sert ?
    client.onPoll([](void* arg, AsyncClient * c) {
        Serial.printf("p:%lu\n\r", millis());
    });
    */
    
    client.onAck([](void* arg, AsyncClient * c, size_t len, uint32_t time) {
        Serial.printf("a:%u:%u\n\r", len, time);

    });

    client.onDisconnect([](void* arg, AsyncClient * c) {
        Serial.printf("Disconnect tcp \n\r");
        c->close();
        anchor->cnxState = false;
    });

    client.onError([](void* arg, AsyncClient * c, int8_t error) {
        Serial.printf("Error: %s\n\r", c->errorToString(error));
        //digitalWrite(led4, HIGH);
        c->close();
    });
    client.onTimeout([](void* arg, AsyncClient * c, uint32_t time) {
        Serial.printf("Timeout\n\r");
        Serial.println(time);
    });

    client.onConnect([](void* arg, AsyncClient * c) {
        anchor->cnxState = true;
        Serial.println("tcp APRS Server Connected");
        //digitalWrite(led4, LOW);    
        String login = "user " + anchor->user + " pass " + anchor->passcode + " vers " + anchor->tool_name + " " + anchor->version + "\n\r";
        c->write(login.c_str());

    });
    client.onData([](void* arg, AsyncClient * c, void* data, size_t len) {
        anchor->timeNow=millis();
        Serial.printf("Data received with length: %d\n\r", len);
        String line((char*) data);
        if (line.startsWith("#")) {
            line = line.substring(0, line.indexOf('\n'));
                    Serial.println(line);
            if (line.indexOf("logresp") != -1) {
                if (line.indexOf("unverified") == -1) {
                    Serial.println("User verified with passcode!");
                }
            }
        }
    });

    int cpt = BEACON_DELAY;
    String strMsg;

    Serial.print("APRS IS Task running  core : ");
    Serial.println(xPortGetCoreID());


    while (true) {
        //char missatge[100];
        vTaskDelay(1);
        if (millis() - anchor->timeNow > RECONNECT_SERVER_DELAY) {
            client.close(); //pb de connection Ethernet sur point d'accès WiFi, il faut se reconnecter
            anchor->timeNow=millis();
        }
        if (!client.connected()) {
            Serial.println("Try to connect");
            if (!client.connect(anchor->serverAprs.c_str(), anchor->portAprs)) {
                Serial.println("Something went wrong on connecting! Is the server reachable?");
                vTaskDelay(RECONNECT_DELAY);
            }
            vTaskDelay(CONNECT_DELAY);
        }
        if (client.connected()) {
            if (cpt == BEACON_DELAY) { //envoie la position balise périodiquement
                client.write(msgBeacon.c_str());
                Serial.println(msgBeacon);
                cpt = 0;
            }
            cpt++;
            if (xQueueReceive(queueMsg, msg, 0) == pdPASS) { //0 au lieu de portMAX_DELAY = non bloquant
                Serial.println("msg aprsis queue");
                Serial.println(msg);
                strMsg = String(msg) + " RSSI:" + String(rssi) + " dBm\n\r";
                client.write(strMsg.c_str()); //non testé
                //client.add(missatge,sizeof(missatge));  //autre version non testé
                //client.send();

            }
        }
    }
}




QueueHandle_t Aprsis::queueMsg = xQueueCreate(3, sizeof (msg));
Aprsis* Aprsis::anchor = NULL;

/*
 * pour plus tard peut être...
 vTaskSuspend( xHandle );
 vTaskResume( xHandle );
  */
