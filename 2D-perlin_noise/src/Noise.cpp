//Implementazione del Noise

#include "Noise.hpp"
#include <iomanip> // Per std::setprecision del TEST
#include <iostream>
#include <algorithm>  // Per std::shuffle
#include <numeric>    // Per std::iota
#include <random>     // Per std::default_random_engine
#include <cmath>     // Per std::floor

PerlinNoise::PerlinNoise(unsigned int seed) {
    // 1. Inizializzo la tabella_permutazione con numeri da 0 a 255
    tabella_permutazione.resize(256);
    std::iota(tabella_permutazione.begin(), tabella_permutazione.end(), 0);

    // 2. Mescolo la tabella_permutazione usando il seed
    std::default_random_engine engine(seed);
    std::shuffle(tabella_permutazione.begin(), tabella_permutazione.end(), engine);

    // 3. duplico la tabella_permutazione (per evitare overflow a causa degli indici)
    tabella_permutazione.insert(tabella_permutazione.end(),
                                tabella_permutazione.begin(),
                                tabella_permutazione.end());
    /* COSA FURBA 
    Stai dicendo a tabella_permutazione di inserire alla fine .end()
    una copia di sé stessa .begin() -> .end().
    [0,1,2] => [0,1,2,0,1,2]
    */

    /*  4. Inizializzo gradienti (8 direzione fondamentali di un piano 2D)
       Ogni direzione ha un vettore di gradiente associato
       che rappresenta la direzione del gradiente in quel punto
       (x,y) sono le coordinate del gradiente
       (1,0) è la direzione orizzontale a destra ... eccetera */
       // (1 1) è la direzione diagonale in alto a destra  

    gradienti = {
        //destra         sinistra      in alto       in basso
        {1.0f, 0.0f} , {-1.0f, 0.0f}, {0.0f, 1.0f} , {0.0f, -1.0f}, 
        //diagonali  
        {1.0f, 1.0f} , {1.0f, -1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}
    };

    /* Questi gradienti determinano la direzione e l'intensità della variazione 
    del rumore attorno a ciascun punto della griglia. */

}
    float PerlinNoise::interpolare(float a, float b, float t) const {
        return a + smussare(t) * (b - a);
    }
    /* La funzione di interpolazione lineare (LERP) mescola i valori a e b
    in base al parametro t. La funzione smussare(t) viene utilizzata per
    applicare una curva di smussamento al valore t, rendendo la transizione
    più fluida. La funzione smussare(t) è una funzione di fading che
    rende la transizione più fluida. */

    float PerlinNoise::smussare(float t) const {
        return t * t * t * (t * (t * 6 - 15) + 10); //curva di easing
    }
    /* La funzione smussare(t) applica una curva di smussamento al valore t
    per rendere la transizione più fluida. La formula 6t⁵-15t⁴+10t³
    è una funzione di fading che rende la transizione più fluida.
    La funzione smussare(t) restituisce un valore compreso tra 0 e 1
    che rappresenta la quantità di smussamento applicata al valore t. */

    float PerlinNoise::grad(int hash, float x, float y) const {
       const auto& g = gradienti[hash % gradienti.size()]; 
        
        return g.first * x + g.second * y; 
    }
    /* Uso l'hash (via tabella_permutazione) per scegliere quale gradiente usare.
    - Misuro quanto il punto (x,y) si "allinea" alla direzione del gradiente.
    - Questo valore è la base su cui costruisci il rumore finale 
      (dopo farai interpolazioni tra i vari vertici della cella).
    */

    int PerlinNoise::indice(int x, int y) const {
        return tabella_permutazione[(x + tabella_permutazione[y]) % 256];
    }

    /*Questa doppia permutazione serve a:
    Mescolare bene i valori (ottenere una distribuzione pseudo-casuale ma continua),
    - Evitare pattern ripetuti,
    - Rendere il rumore più "naturale".
    In sostanza, è un trucco per far sì che anche se (x, y) 
    cambiano un po', il valore hash cambi "dolcemente", senza salti netti 
    (importantissimo per evitare artefatti brutti nel Perlin Noise!). */

    // (x, y) → permutazione → gradiente → rumore
    // -------- FUNZIONE PRINCIPALE --------

    float PerlinNoise::noise(float x, float y) {
        // 1. Trovo le coordinate cella unitaria (coordinate intere)
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        // 2. Coordinate relative all'angolo della cella (0.0 a 1.0)
        float x_rel = x - floor(x);
        float y_rel = y - floor(y);

        // 3. Calcola gradienti ai 4 angoli
        float n0 = grad(indice(X, Y), x_rel, y_rel);
        float n1 = grad(indice(X+1, Y), x_rel-1, y_rel);
        float n2 = grad(indice(X, Y+1), x_rel, y_rel-1);
        float n3 = grad(indice(X+1, Y+1), x_rel-1, y_rel-1);

        // 4. Interpola lungo X
        float ix0 = interpolare(n0, n1, x_rel);
        float ix1 = interpolare(n2, n3, x_rel);

        // 5. Interpola lungo Y
        return interpolare(ix0, ix1, y_rel);
    }


// Funzione di test standalone
/*

cmake .. && make NoiseTest
*/

#if defined(TEST_NOISE_STANDALONE)



void testNoise() {
    std::cout << "\n=== TEST PERLIN NOISE ===\n";

    // 1. Istanza con seed fisso per risultati riproducibili
    PerlinNoise pn(12345);

    // 2. Test punti notevoli
    std::cout << "Valore a (0.0, 0.0): " << pn.noise(0.0f, 0.0f) << "\n";
    std::cout << "Valore a (0.5, 0.5): " << pn.noise(0.5f, 0.5f) << "\n";  // Centro cella, dovrebbe essere ~0.0
    std::cout << "Valore a (1.0, 1.0): " << pn.noise(1.0f, 1.0f) << "\n";  // Angolo cella

    // 3. Test pattern 2D
    std::cout << "\nPattern 5x5 (scale 0.2):\n";
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            float val = pn.noise(x * 0.2f, y * 0.2f);
            std::cout << std::fixed << std::setprecision(2) << val << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    testNoise();
    return 0;
}

#endif