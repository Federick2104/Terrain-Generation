//Gestione della mappa 
/*
Crea la griglia dati, chiama Noise per popolare la griglia.
 - creo un vector per contenere x e y
 Mappare i valori di noise a tipi di terreno (acqua, erba, etc.)
 Fornire interfaccia per accedere ai dati del terreno
*/

#pragma once
#include <vector>
#include <Noise.hpp> // per il noise

enum class Bioma {
    Acqua, Sabbia, Erba, Collina , Montagna
}; //aggiungere altri biomi se necessario

class Terrain {
public:
    //costrttore per accettare dimensione tramite UI
    Terrain(const PerlinNoise& noise, int width, int height, float scale = 0.1f);
    Bioma getBioma(float x, float y) const; //restituisce il bioma in base a x e y
    float getAltezza(float x, float y) const; //per visualizzazione 3D futura magari

    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }

private:
    const PerlinNoise& generatore_noise;
    std::vector<std::vector<Bioma>> mappaBiomi;  //griglia di biomi

    int mapWidth;
    int mapHeight;
    float noiseScale;

    void generaMappaBiomi(); //precalcola la mappa di biomi
};

