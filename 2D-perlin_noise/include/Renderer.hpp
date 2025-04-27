//Dichiarazione funzioni di visualizzazione della mappa
/*
Prende i dati e li "disegna" visualizzandoli a schermo usando SFML.
Simple and Fast Multimedia Library => www.sfml-dev.org
Creare finestre, disegnare immagini, modificare i pixel, 
- caricare Image nella texture (sf::Texture)
- disegnare la texture sulla finestra (sf::Sprite)
*/
#pragma once
#include <SFML/Graphics.hpp>
#include "Noise.hpp"  // Per il noise grezzo (opzionale)
#include "Terrain.hpp" // Per i biomi

class Renderer {
public:
    Renderer(const Terrain& terreno); 
    void run(); //avvia il loop main

private:
    const Terrain& terreno; //riferimento alla mappa
    sf::RenderWindow finestra; //finestra di rendering

    void gestioneEventi();
    void drawTerreno();
    sf::Color determinaCoolore(float valore_noise) const; 
}