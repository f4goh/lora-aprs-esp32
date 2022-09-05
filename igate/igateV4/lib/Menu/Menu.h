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


class Menu {
    
public:
    Menu();
    Menu(const Menu& orig);
    virtual ~Menu();
    
    void run();
    void setup();

    // Méthodes associées aux commandes
    static void _call_(ArgList& L, Stream& S);
    static void _ssid_(ArgList& L, Stream& S);
    static void _pass_(ArgList& L, Stream& S);
    static void _freq_(ArgList& L, Stream& S);
    static void _latitude_(ArgList& L, Stream& S);
    static void _longitude_(ArgList& L, Stream& S);
    static void _comment_(ArgList& L, Stream& S);
    static void _config_(ArgList& L, Stream& S);
    static void _raz_(ArgList& L, Stream& S);
    static void _internet_(ArgList& L, Stream& S);
    static void _server_(ArgList& L, Stream& S);
    static void _port_(ArgList& L, Stream& S);

    static void _exit_(ArgList& L, Stream& S);
    static void _help_(ArgList& L, Stream& S);
    static void _unknown(String& L, Stream& S);
    static void _digi_(ArgList& L, Stream& S);

private:
    
    bool exitFlag;
    Console *con;
    Preferences *configuration;
    static Menu* anchor;
    bool acceptCmd(String cmd, int longMin, int longMax);
    
};

#endif /* MENU_H */
