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

enum class Biome {
    Acqua, Sabbia, Erba, Montagna
}; //aggiungere altri biomi se necessario

class Terrain {
public:
    Terrain(const PerlinNoise& noise);
    Bioma getBioma(float x, float y) const; //restituisce il bioma in base a x e y
    float getAltezza(float x, float y) const; //per visualizzazione 3D futura magari

private:
    const PerlinNoise& generatore_noise;
    std::vector<std::vector<Bioma>> mappaBiomi;  //griglia di biomi

    void generaMappaBiomi(); //precalcola la mappa di biomi
}
