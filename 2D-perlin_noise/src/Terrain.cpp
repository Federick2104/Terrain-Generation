//Implementazione della mappa
#include "Terrain.hpp"

Terrain::Terrain(const PerlinNoise& noise, int width, int height, float scale)
    : generatore_noise(noise), mapWidth(width), mapHeight(height), noiseScale(scale) {

    generaMappaBiomi();

}

void Terrain::generaMappaBiomi(){
    //ridimensiona la mappa in base ad parametri dallla UI
    mappaBiomi.resize(mapHeight);
    for (auto& row : mappaBiomi) {
        row.resize(mapWidth);
    }


    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            float nx = x * noiseScale;
            float ny = y * noiseScale;
            float noiseValue = generatore_noise.noise(nx, ny);
            
            // Converti il valore di noise in bioma
            if (noiseValue < -0.0f) {
                mappaBiomi[y][x] = Bioma::Acqua;
            } else if (noiseValue < 0.1f) {
                mappaBiomi[y][x] = Bioma::Sabbia;
            } else if (noiseValue < 0.3f) {
                mappaBiomi[y][x] = Bioma::Erba;
            } else if (noiseValue < 0.55f) {
                mappaBiomi[y][x] = Bioma::Collina;
            } else {
                mappaBiomi[y][x] = Bioma::Montagna;
            }
        }
    }

}

Bioma Terrain::getBioma(float x, float y) const {
    // coordinate => indici di mappa
    int mapX = static_cast<int>(x) % mapWidth;
    int mapY = static_cast<int>(y) % mapHeight;

    //coordinate negative
    if (mapX < 0) mapX += mapWidth;
    if (mapY < 0) mapY += mapHeight;

    return mappaBiomi[mapY][mapX];

}

//implementazioni future 3D
float Terrain::getAltezza(float x, float y) const {
    //uso direttamente valore approssimazione noise per altezza
    float nx = x * noiseScale;
    float ny = y * noiseScale;

    return generatore_noise.noise(nx, ny);
}



