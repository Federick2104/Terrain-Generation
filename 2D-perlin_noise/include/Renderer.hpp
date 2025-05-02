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
//MODALITA' DI VISUALIZZAZIONE
enum class ModalitaVisualizzazione{
    BIOMI, // biomi con colori differenti
    ALTEZZA, // grayscale
    COMBINATA //biomi + altezza
};

class Renderer {
public:
    Renderer(const Terrain& terreno, unsigned int cellSize = 4); 
    void run(); //avvia il loop main

    void setModalita(ModalitaVisualizzazione modalita);
    void salvaImmagine(const std::string& nome_file); 

private:
    const Terrain& terreno; 
    sf::RenderWindow finestra; 
    sf::View camera;

    unsigned int cellSize; //dim cella pixel
    ModalitaVisualizzazione modAttuale = ModalitaVisualizzazione::BIOMI; 

    sf::Vector2i mousePos;
    bool pannelloAttivo = false; 

    //UI elems
    sf::Font font;
    sf::Text infoText;

    void gestioneEventi();
    void drawTerreno();
    sf::Color determinaColore(float valore_noise) const;  //colore cella
    sf::Color determinaColoreBioma(Bioma bioma) const; //colore bioma
    void zoomView(float factor, sf::Vector2i mousePos);
    void panView(sf::Vector2i delta);
    void updateInfoText(sf::Vector2i mousePos);
    void drawUI();

};