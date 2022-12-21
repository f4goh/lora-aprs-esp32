/* 
 * File:   Menu.h
 * Author: philippe SIMIER Touchard Washington Le Mans
 *
 * Created on 22 avril 2022, 17:11
 */

#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <console.h>
#include <Preferences.h>        // Classe pour écrire et lire la configuration

#define FREQUENCY 433775000
#define TIMEOUT_MENU 10
#define NB_DYN_MODES 10

class Menu {
    
public:
    Menu();
    Menu(const Menu& orig);
    virtual ~Menu();
    
    void run();
    void setup();
    String getDynModName(uint8_t model);

 private:
    // Méthodes associées aux commandes
    static void _call_(ArgList& L, Stream& S);
    static void _second_(ArgList& L, Stream& S);
    static void _freq_(ArgList& L, Stream& S);
    static void _symbol_(ArgList& L, Stream& S);
    static void _comment_(ArgList& L, Stream& S);
    static void _setbat_(ArgList& L, Stream& S);    
    static void _setalt_(ArgList& L, Stream& S);
    static void _setcs_(ArgList& L, Stream& S);
    static void _setcomp_(ArgList& L, Stream& S);
    static void _config_(ArgList& L, Stream& S);
    static void _raz_(ArgList& L, Stream& S);
    static void _exit_(ArgList& L, Stream& S);
    static void _help_(ArgList& L, Stream& S);
    static void _unknown(String& L, Stream& S);
    static void _navmod_(ArgList& L, Stream& S);
    static void _setsms_(ArgList& L, Stream& S);
    static void _setsmsmin_(ArgList& L, Stream& S);
    static void _setsmsph_(ArgList& L, Stream& S);

    bool exitFlag;
    Console *con;
    Preferences *configuration;
    static Menu* anchor;    
    bool acceptCmd(String cmd, int longMin, int longMax);
    String dynModelLst[NB_DYN_MODES];
};

#endif /* MENU_H */
