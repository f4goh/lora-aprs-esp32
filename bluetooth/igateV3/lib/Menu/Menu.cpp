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
    con->onCmd("call", _call_);
    con->onCmd("freq", _freq_);
    con->onCmd("show", _config_);
    con->onCmd("raz", _raz_);
    con->onCmd("exit", _exit_);
    con->onUnknown(_unknown);
    con->start();

    anchor->configuration->begin("bluetooth", false);

    if (anchor->configuration->getBool("config", false) == false) {
        anchor->configuration->putString("call", "nocall");
        anchor->configuration->putLong("freq", FREQUENCY);
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
    S.println(F("Set callsign for digi                          : call f4goh-6"));
    S.println(F("Set frequency                                  : freq 433775000"));
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
        anchor->configuration->begin("bluetooth", false);
        S.printf("call is %s\n\r", p.c_str());
        anchor->configuration->putString("call", p);
        anchor->configuration->end();

    }
}

void Menu::_freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 9, 9);
    if (ret == true) {
        anchor->configuration->begin("bluetooth", false);
        S.printf("freq is %s\n\r", p.c_str());      
        anchor->configuration->putLong("freq", p.toInt());
        anchor->configuration->end();
    }
}



void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("bluetooth", false);
    Serial.printf("Call is             : %s\n\r", anchor->configuration->getString("call").c_str());
    Serial.printf("Frequency is        : %d\n\r", anchor->configuration->getLong("freq"));
    anchor->configuration->end();
}

void Menu::_raz_(ArgList& L, Stream& S) {
    anchor->configuration->begin("bluetooth", false);
    anchor->configuration->putBool("config", false);
    //anchor->configuration->clear();  
    ESP.restart();
}

Menu* Menu::anchor = NULL;




