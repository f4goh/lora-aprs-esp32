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
configuration(new Preferences()),
dynModelLst{"portable", "stationary", "pedestrian", "automotive", "sea", "airborne1g", "airborne2g", "airborne4g", "wrist", "bike"}
{
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
    con->onCmd("second",_second_);
    con->onCmd("freq", _freq_);
    con->onCmd("symbol", _symbol_);
    con->onCmd("comment", _comment_);
    con->onCmd("setbat", _setbat_);
    con->onCmd("setalt", _setalt_);
    con->onCmd("setcs", _setcs_);
    con->onCmd("setcomp", _setcomp_);
    con->onCmd("navmod", _navmod_);
    con->onCmd("setsms", _setsms_);
    con->onCmd("setsmsmin", _setsmsmin_);
    con->onCmd("setsmsph", _setsmsph_);
    
    con->onCmd("show", _config_);
    con->onCmd("raz", _raz_);
    con->onCmd("exit", _exit_);

    con->onUnknown(_unknown);
    con->start();

    anchor->configuration->begin("tracker", false);

    if (anchor->configuration->getBool("config", false) == false) {
        anchor->configuration->putString("call", "nocall");
        anchor->configuration->putLong("freq", FREQUENCY);
        anchor->configuration->putChar("second", 0);
        anchor->configuration->putChar("symbol", '>');
        anchor->configuration->putBool("setbat", false);
        anchor->configuration->putString("comment", "test");
        anchor->configuration->putBool("setalt", false);
        anchor->configuration->putBool("setcs", false);
        anchor->configuration->putBool("setcomp", false);
        anchor->configuration->putBool("config", true);
        anchor->configuration->putUChar("navmod", 0); //default portable
        anchor->configuration->putInt("messageId",0);
        anchor->configuration->putChar("sms",0);
        anchor->configuration->putChar("smsmin",1);
        anchor->configuration->putString("smsphone","+33695979933");
        
    }
    anchor->configuration->end();
    this->run();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}

void Menu::_help_(ArgList& L, Stream& S) {
    S.println(F("Available commands"));
    S.println(F("Set new callsign                            : call f4goh-6"));
    S.println(F("Set frequency                               : freq 433775000"));
    S.println(F("Set car symbol                              : symbol >"));
    S.println(F("Set second txing into the minute            : second 20"));
    S.println(F("Set new comment                             : comment hello"));
    S.println(F("Set Battery measurement in comment (0 or 1) : setbat 1"));
    S.println(F("Set altitude feild in pdu (0 or 1)          : setalt 1"));
    S.println(F("Set Course/Speed feild in pdu (0 or 1)      : setcs 1"));
    S.println(F("Set compression position (0 or 1)           : setcomp 1"));    
    S.println(F("Set dynamic Platform Model                  : navmod portable"));
            for (uint8_t  n = 1; n < NB_DYN_MODES; n++) {
                S.printf("%53s%s\n\r", "navmod ",anchor->dynModelLst[n].c_str());
            }
    S.println(F("Set SMS (0 :disable, 1 :google, 2 :locator) : setsms 1"));
    S.println(F("Set SMS minutes txing interval              : setsmsmin 15"));
    S.println(F("Set SMS phone numer                         : setsmsph +336xxxxxxxx"));
    S.println(F("Show configuration                          : show"));
    S.println(F("Reset default configuration                 : raz"));
    S.println(F("Exit menu                                   : exit"));
}

//manque seconds


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
        anchor->configuration->begin("tracker", false);
        S.printf("call is %s\n\r", p.c_str());
        anchor->configuration->putString("call", p);
        anchor->configuration->end();

    }
}

void Menu::_second_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=anchor->acceptCmd(p,2,2);
    if (ret==true) {
                    anchor->configuration->begin("tracker", false);
                    S.printf("seconds is %s\n\r", p.c_str());
                    anchor->configuration->putChar("second",(int8_t) p.toInt());
                    anchor->configuration->end();                    
    }
}


void Menu::_freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 9, 9);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        S.printf("freq is %s\n\r", p.c_str());      
        anchor->configuration->putLong("freq", p.toInt());
        anchor->configuration->end();
    }
}

void Menu::_symbol_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        S.printf("symbol is %s\n\r", p.c_str());

        anchor->configuration->putChar("symbol", (char) p.c_str()[0]);
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
        anchor->configuration->begin("tracker", false);
        S.printf("comment is %s\n\r", p.c_str());
        anchor->configuration->putString("comment", p);
        anchor->configuration->end();
    }
}

void Menu::_setbat_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        anchor->configuration->putBool("setbat", (int8_t) p.toInt());
        Serial.printf("Battery measurement is : %s\n\r", anchor->configuration->getBool("setbat") ? "Enable" : "Disable");
        anchor->configuration->end();
    }
}


void Menu::_setalt_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        anchor->configuration->putBool("setalt", (int8_t) p.toInt());
        Serial.printf("Altitude is : %s\n\r", anchor->configuration->getBool("setalt") ? "Enable" : "Disable");
        anchor->configuration->end();
    }

}

void Menu::_setcs_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        anchor->configuration->putBool("setcs", (int8_t) p.toInt());
        Serial.printf("Course/Speed is : %s\n\r", anchor->configuration->getBool("setcs") ? "Enable" : "Disable");
        anchor->configuration->end();
    }

}


void Menu::_setcomp_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        anchor->configuration->putBool("setcomp", (int8_t) p.toInt());
        Serial.printf("Compression position is : %s\n\r", anchor->configuration->getBool("setcomp") ? "Enable" : "Disable");
        anchor->configuration->end();
    }
}

//portable, stationary, pedestrian, automotive, sea, airborne1g, airborne2g, airborne4g, wrist, bike

void Menu::_navmod_(ArgList& L, Stream& S) {
    String p;
    uint8_t  n;    
    bool ret;
    uint8_t model = 0;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 3, 10);
    if (ret == true) {
        ret = false;        
        for (n = 0; n < NB_DYN_MODES; n++) {
            if (p == anchor->dynModelLst[n]) {
                model = n;
                ret = true;
            }
        }
        if (ret == false) {
            S.printf("Error, must be : \n\r");
            for (n = 0; n < NB_DYN_MODES; n++) {
                S.printf("navmod %s\n\r", anchor->dynModelLst[n].c_str());
            }
        }        
    }
    p == anchor->dynModelLst[model];
    anchor->configuration->begin("tracker", false);
    S.printf("Dynamic Platform Model is %s\n\r", p.c_str());
    anchor->configuration->putUChar("navmod", model);
    anchor->configuration->end();
}

void Menu::_setsms_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        anchor->configuration->putUChar("sms", (int8_t) p.toInt());
        S.printf("SMS txing is : ");
        uint8_t mode=anchor->configuration->getUChar("sms");
        //S.printf("%d\n\r",mode);
        switch(mode){
            case 0 : S.printf("Disable\n\r"); break;
            case 1 : S.printf("Google mode\n\r"); break;
            case 2 : S.printf("Locator mode\n\r"); break;
        }
        anchor->configuration->end();
    }
}

void Menu::_setsmsmin_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret=anchor->acceptCmd(p,1,2);
    if (ret==true) {
                    anchor->configuration->begin("tracker", false);
                    S.printf("SMS minutes is every %s\n\r", p.c_str());
                    anchor->configuration->putChar("smsmin",(int8_t) p.toInt());
                    anchor->configuration->end();                    
    }
}

void Menu::_setsmsph_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 12, 12); 
    if (ret == true) {
        anchor->configuration->begin("tracker", false);
        S.printf("SMS phone  is %s\n\r", p.c_str());      
        anchor->configuration->putString("smsphone", p);
        anchor->configuration->end();
    }
}


void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("tracker", false);
    S.printf("Call is                   : %s\n\r",anchor->configuration->getString("call").c_str());
    S.printf("Symbol is                 : %c\n\r",anchor->configuration->getChar("symbol"));
    S.printf("Frequency is              : %d\n\r",anchor->configuration->getLong("freq"));
    S.printf("Transmit at second        : %d \n\r",anchor->configuration->getChar("second"));
    S.printf("Battery measurement is    : %s\n\r",anchor->configuration->getBool("setbat")? "Enable" : "Disable");
    S.printf("Altitude is               : %s\n\r",anchor->configuration->getBool("setalt")? "Enable" : "Disable");
    S.printf("Course/Speed is           : %s\n\r",anchor->configuration->getBool("setcs")? "Enable" : "Disable");
    S.printf("Compression is            : %s\n\r",anchor->configuration->getBool("setcomp")? "Enable" : "Disable");
    uint8_t  model=anchor->configuration->getUChar("navmod");
    S.printf("Dynamic Platform Model is : ");
    S.printf("%s \n\r", anchor->dynModelLst[model].c_str());
    S.printf("SMS feature is            : ");
    uint8_t mode = anchor->configuration->getUChar("sms");    
    switch (mode) {
        case 0: S.printf("Disable\n\r");
            break;
        case 1: S.printf("Google place URL\n\r");
            break;
        case 2: S.printf("Locator 8 digits\n\r");
            break;
    }
    S.printf("SMS minute interval is    : %d\n\r", anchor->configuration->getChar("smsmin"));
    S.printf("SMS phone is              : %s\n\r",anchor->configuration->getString("smsphone").c_str());
    S.printf("Compression is            : %s\n\r",anchor->configuration->getBool("setcomp")? "Enable" : "Disable");
    S.printf("Comment is                : %s\n\r",anchor->configuration->getString("comment").c_str());
    anchor->configuration->end();
}

void Menu::_raz_(ArgList& L, Stream& S) {
    anchor->configuration->begin("tracker", false);
    anchor->configuration->putBool("config", false);
    //anchor->configuration->clear();  
    ESP.restart();
}

String Menu::getDynModName(uint8_t model){
    return dynModelLst[model];
}


Menu* Menu::anchor = NULL;
