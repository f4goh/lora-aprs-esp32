/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Afficheur.cpp
 * Author: ale
 * 
 * Created on 7 avril 2022, 19:54
 */

#include "Afficheur.h"

// Constructeur de Afficheur
// Sur la carte Ballon adresse 0x3c connecté sur SDA et SCL

Afficheur::Afficheur() :
SSD1306Wire(0x3c, SDA, SCL, GEOMETRY_128_64) {
    init();
    setFont(ArialMT_Plain_24);
}

Afficheur::~Afficheur() {
}

/**
 * Méthode pour afficher un message
 * @param message String le message à afficher
 */
void Afficheur::afficher(String message) {
    clear();
    drawStringMaxWidth(0, 0, 110, message);
    display();
}



/**
 * Méthode pour afficher une grandeur physique
 * @param char* message le libellé
 * @param valeur  la valeur à afficher
 * @param unite   l'unite de la grandeur
 */
void Afficheur::afficherFloat(String message, float valeur, String unite, int precision) {
    String valst(valeur,precision);
    clear();
    drawString(0,0, message );
    drawString(0,32, valst + unite);
    display();

}