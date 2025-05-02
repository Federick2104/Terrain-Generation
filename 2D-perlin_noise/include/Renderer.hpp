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
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdlib>

#include "Noise.hpp"  // Per il noise grezzo (opzionale)
#include "Terrain.hpp" // Per i biomi
//MODALITA' DI VISUALIZZAZIONE
enum class ModalitaVisualizzazione{
    BIOMI, // biomi con colori differenti
    ALTEZZA, // grayscale
    COMBINATA //biomi + altezza
};

class Renderer {
public:
    Renderer(const Terrain& terreno, unsigned int cellSize); 
    void run(); //avvia il loop main

    void setModalita(ModalitaVisualizzazione modalita);

private:
    const Terrain& terreno; 
    sf::RenderWindow finestra; 
    sf::View camera;

    unsigned int cellSize; //dim cella pixel
    ModalitaVisualizzazione modAttuale = ModalitaVisualizzazione::COMBINATA; // Default mode

    sf::Vector2i mousePos; 
    bool isPanning = false; // For panning
    sf::Vector2i lastMousePos; // For panning

    void gestioneEventi();
    void drawTerreno();
    sf::Color determinaColore(float valore_noise) const;  //colore cella
    sf::Color determinaColoreBioma(Bioma bioma) const; //colore bioma
    void zoomView(float factor, sf::Vector2i mousePos);
    void panView(sf::Vector2i currentMousePos); // Declare panView
    void drawUI();

};