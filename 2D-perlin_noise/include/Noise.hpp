// Dichiarazione del Noise
/*
Contiene funzioni che generano valori di noise di coordinata (x,y).
*/
#pragma once
#include <vector>
#include <utility> //per std::pair

class PerlinNoise {
//Costruttore: accetta un "seed" per inizializzare la tabella di permutazione
public: 
    PerlinNoise(unsigned long seed);

// Metodo principale: restituisce un valore tra -1.0 e 1.0 per le coordinate (x,y)
    float noise(float x, float y) const;

private:
    std::vector<int> tabella_permutazione; //numeri unici da 0-255 mescolati casualmente
    std::vector<std::pair<float, float>> gradienti; //assegna una pendenza a ognu punto della griglia
    
    void normalizzaGradienti(); //normalizza i gradienti per noise uniforme

    //LERP, unisce/mescola i valori di noise in base a t
    float interpolare(float a, float b, float t) const;
    //FADE Funzione di fading (6t⁵-15t⁴+10t³) 
    float smussare(float t) const; 
    // Per accedere alla tabella di permutazione 
    int indice(int x, int y) const;  
    // calcola il gradiente
    float grad(int hash, float x, float y) const; 
};