/* 
 * File:   Menu.cpp
 * Author: philippe SIMIER Touchard Washington le Mans
 * 
 * Created on 22 avril 2022, 17:11
 */

#include "Menu.h"

Menu::Menu() :
exitFlag(false),
con(new Console()),
configuration(new Preferences()) {
    anchor = this;
}

Menu::Menu(const Menu& orig) {
}

Menu::~Menu() {

    delete configuration;
    anchor = NULL;
}

void Menu::run() {

    char car;
    int cpt = 0;
    exitFlag = false;
    Serial.println(F("Press m key to enter menu"));
    while ((cpt < TIMEOUT_MENU) && !exitFlag) {

        Serial.print(".");
        cpt++;

        if (Serial.available() > 0) {
            car = Serial.read();
            if (car == 'm') {
                Serial.println(F("help command for info"));
                while (exitFlag == false) {
                    con->run();
                }
            }
        }
        delay(1000);
    }
    Serial.println(F("\nConsole exit"));
}

void Menu::setup() {

    con->onCmd("help", _help_);
    con->onCmd("ssid", _ssid_);
    con->onCmd("pass", _pass_);
    con->onCmd("call", _call_);
    con->onCmd("freq", _freq_);
    con->onCmd("latitude", _latitude_);
    con->onCmd("longitude", _longitude_);
    con->onCmd("comment", _comment_);
    con->onCmd("digi", _digi_);
    con->onCmd("show", _config_);
    con->onCmd("raz", _raz_);
    con->onCmd("exit", _exit_);
    con->onCmd("internet", _internet_);
    con->onCmd("server", _server_);
    con->onCmd("port", _port_);
    con->onUnknown(_unknown);
    con->start();

    anchor->configuration->begin("igate", false);

    if (anchor->configuration->getBool("config", false) == false) {
        anchor->configuration->putString("ssid", "nossid");
        anchor->configuration->putString("pass", "nopassword");
        anchor->configuration->putString("call", "nocall");
        anchor->configuration->putLong("freq", FREQUENCY);
        anchor->configuration->putString("latitude", "0000.00N");
        anchor->configuration->putString("longitude", "00000.00E");
        anchor->configuration->putString("comment", "LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32");
        anchor->configuration->putBool("internet", false);
        anchor->configuration->putString("server", "euro.aprs2.net");
        anchor->configuration->putInt("port", 14580);
        anchor->configuration->putBool("digi", false);
        anchor->configuration->putBool("config", true);
    }
    anchor->configuration->end();
    this->run();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}

void Menu::_help_(ArgList& L, Stream& S) {
    S.println(F("Available commands"));
    S.println(F("Set ssid                                       : ssid mywifi"));
    S.println(F("Set password                                   : pass toto"));
    S.println(F("Set new callsign                               : call f4goh-6"));
    S.println(F("Set latitude in degrees minutes decimal        : latitude 4753.41N"));
    S.println(F("Set longitude in degrees minutes decimal       : longitude 00016.61E"));
    S.println(F("Set new comment                                : comment hello"));
    S.println(F("Set frequency                                  : freq 433775000"));
    S.println(F("Enable wifi for igate (aprs.fi)                : internet 1"));
    S.println(F("Enable local wifi Access Point for AprsDroid   : internet 0"));
    S.println(F("When disable igate, local wifi AP are enable"));
    S.println(F("Set server                                     : server euro.aprs2.net"));
    S.println(F("Set port                                       : port 14580"));
    S.println(F("Set Digipeater enable (0 or 1)                 : digi 1"));
    S.println(F("Show configuration                             : show"));
    S.println(F("Reset default configuration                    : raz"));
    S.println(F("Exit menu                                      : exit"));
}

bool Menu::acceptCmd(String cmd, int longMin, int longMax) {

    if (cmd.length() >= longMin && cmd.length() <= longMax) {
        return true;
    } else {
        Serial.println("Erreur");
        return false;
    }
}

void Menu::_unknown(String& L, Stream& S) {
    S.print(L);
    S.println(" : command not found");
}

void Menu::_call_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 7, 10);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("call is %s\n\r", p.c_str());
        anchor->configuration->putString("call", p);
        anchor->configuration->end();

    }
}

void Menu::_ssid_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 3, 20);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("ssid is %s\n\r", p.c_str());
        anchor->configuration->putString("ssid", p);
        anchor->configuration->end();
    }
}

void Menu::_pass_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 3, 20);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("password is %s\n\r", p.c_str());
        anchor->configuration->putString("pass", p);
        anchor->configuration->end();
    }
}

void Menu::_freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 9, 9);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("freq is %s\n\r", p.c_str());      
        anchor->configuration->putLong("freq", p.toInt());
        anchor->configuration->end();
    }
}

void Menu::_latitude_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 8, 8);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("latitude is %s\n\r", p.c_str());
        anchor->configuration->putString("latitude", p);
        anchor->configuration->end();
    }
}

void Menu::_longitude_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 9, 9);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("longitude is %s\n\r", p.c_str());
        anchor->configuration->putString("longitude", p);
        anchor->configuration->end();
    }
}

void Menu::_comment_(ArgList& L, Stream& S) {
    String arg;
    bool ret;
    String p = "";
    while (!(arg = L.getNextArg()).isEmpty()) {
        p = p + arg + " ";
    }
    ret = anchor->acceptCmd(p, 3, 30);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("comment is %s\n\r", p.c_str());
        anchor->configuration->putString("comment", p);
        anchor->configuration->end();
    }
}

void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("igate", false);
    Serial.printf("Ssid is             : %s\n\r", anchor->configuration->getString("ssid").c_str());
    Serial.printf("password is         : %s\n\r", anchor->configuration->getString("pass").c_str());
    Serial.printf("Call is             : %s\n\r", anchor->configuration->getString("call").c_str());
    Serial.printf("Frequency is        : %d\n\r", anchor->configuration->getLong("freq"));
    Serial.printf("latitude is         : %s\n\r", anchor->configuration->getString("latitude").c_str());
    Serial.printf("longitude is        : %s\n\r", anchor->configuration->getString("longitude").c_str());
    Serial.printf("Internet is         : %s\n\r", anchor->configuration->getBool("internet") ? "Enable" : "Disable");
    Serial.printf("TCP Access point is : %s\n\r", anchor->configuration->getBool("internet") ? "Disable" : "Enable");
    Serial.printf("Server is           : %s\n\r", anchor->configuration->getString("server").c_str());
    Serial.printf("Port is             : %d\n\r", anchor->configuration->getInt("port"));
    Serial.printf("Digipeater is       : %s\n\r", anchor->configuration->getBool("digi") ? "Enable" : "Disable");
    Serial.printf("Comment is          : %s\n\r", anchor->configuration->getString("comment").c_str());
    anchor->configuration->end();
}

void Menu::_raz_(ArgList& L, Stream& S) {
    anchor->configuration->begin("igate", false);
    anchor->configuration->putBool("config", false);
    //anchor->configuration->clear();  
    ESP.restart();
}

void Menu::_internet_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        anchor->configuration->putBool("internet", (int8_t) p.toInt());
        Serial.printf("Internet is : %s\n\r", anchor->configuration->getBool("internet") ? "Enable" : "Disable");
        Serial.printf("TCP Access point is : %s\n\r", anchor->configuration->getBool("internet") ? "Disable" : "Enable");
        anchor->configuration->end();
    }
}

void Menu::_server_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 3, 30);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("server is %s\n\r", p.c_str());
        anchor->configuration->putString("server", p);
        anchor->configuration->end();

    }
}

void Menu::_port_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 2, 5);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        S.printf("port is %s\n\r", p.c_str());      
        anchor->configuration->putInt("port", p.toInt());
        anchor->configuration->end();
    }
}

void Menu::_digi_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("igate", false);
        anchor->configuration->putBool("digi", (int8_t) p.toInt());
        Serial.printf("Digipeater is : %s\n\r", anchor->configuration->getBool("digi") ? "Enable" : "Disable");
        anchor->configuration->end();
    }
}


Menu* Menu::anchor = NULL;


