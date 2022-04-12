/* 
 * File:   Afficheur.h
 * From Author: philippe SIMIER Lycée Touchard Washington
 * Modified by Anthony Le Cren
 * Created on 11 février 2022, 11:15
 * Classe Afficheur pour le TTGO
 * spécialisation de SSD1306Wire
 */

#ifndef AFFICHEUR_H
#define AFFICHEUR_H

#include <Arduino.h>
#include <SSD1306Wire.h>


class Afficheur : public SSD1306Wire
{
public:
    
    Afficheur();
    virtual ~Afficheur();
    
    void afficher(String message);
    void afficherFloat(String message, float valeur, String unite, int precision=1);
    
private:

};

#endif /* AFFICHEUR_H */