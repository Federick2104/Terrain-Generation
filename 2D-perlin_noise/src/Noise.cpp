//Implementazione del Noise

#include "Noise.hpp"
#include <iomanip> // Per std::setprecision del TEST
#include <iostream>
#include <algorithm>  // Per std::shuffle
#include <numeric>    // Per std::iota
#include <random>     // Per std::default_random_engine
#include <cmath>     // Per std::floor std::sqrt

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
        // Diagonali (norma sqrt(2) ≈ 1.414)
        {1.0f, 1.0f},    // Destra-alto
        {1.0f, -1.0f},   // Destra-basso
        {-1.0f, 1.0f},   // Sinistra-alto
        {-1.0f, -1.0f},  // Sinistra-basso
        
        // Assiali (norma 1.0)
        {1.0f, 0.0f},    // Destra
        {-1.0f, 0.0f},   // Sinistra
        {0.0f, 1.0f},    // Alto
        {0.0f, -1.0f}    // Basso
    };
    /* Questi gradienti determinano la direzione e l'intensità della variazione 
    del rumore attorno a ciascun punto della griglia. */


    // 5. NORMALIZZA I GRADIENTI SCOMMENTA PER AVERE UN RUMORE UNIFORME
    normalizzaGradienti();  

}

    void PerlinNoise::normalizzaGradienti(){
        for (auto& g : gradienti) {
            float norma = std::sqrt(g.first * g.first + g.second * g.second);
            if (norma > 0.0f) { //controllo divisione per 0
                g.first /= norma;
                g.second /= norma;
            }
        }
    }

    float PerlinNoise::interpolare(float a, float b, float t) const {
        float t_smussato = smussare(t);
        return a + t_smussato * (b - a);
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
        int positiveY = y & 255;
        int positiveX = x & 255;
        return tabella_permutazione[(positiveX + tabella_permutazione[positiveY]) % 256];
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
        /* 1. Trovo le COORDINATE CELLA UNITARIA (coordinate intere)
       Questa parte calcola le coordinate intere della cella in cui si trova il punto (x,y).
        - std::floor(x) arrotonda verso il basso per ottenere l'angolo inferiore
           sinistro della cella.
        - & 255 (operazione bitwise AND con 255) limita il valore a 0-255,
         creando un effetto di "wrapping" della griglia che rende il noise
         ripetibile.
        */ 

        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        /* 2. COORDINATE RELATIVE all'angolo della cella (0.0 a 1.0)
        Questa parte calcola le coordinate relative all'interno della cella, 
         normalizzate tra 0.0 e 1.0.
        - Sottraendo il valore intero, otteniamo la parte frazionaria 
        della coordinata.
        - Questi valori servono per interpolare tra i valori di gradiente
        */
        float x_rel = x - std::floor(x);
        float y_rel = y - std::floor(y);

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


#if defined(TEST_NOISE_STANDALONE)

void testNoise() {
    unsigned int seed = 12345;
    PerlinNoise pn(seed);  // Seed fisso

    std::cout << std::fixed << std::setprecision(3);
    
    // Test che verifica le proprietà del Perlin Noise invece di valori specifici
    std::cout << "\n====== PERLIN NOISE PROPERTIES TEST ======\n";
    std::cout << "Seed => [ " << seed << " ] " << std::endl;
    // 1. Verifica che i valori siano nell'intervallo [-1,1]
    float min_val = 1.0f, max_val = -1.0f;
    for (float x = 0; x < 10; x += 0.5f) {
        for (float y = 0; y < 10; y += 0.5f) {
            float val = pn.noise(x, y);
            min_val = std::min(min_val, val);
            max_val = std::max(max_val, val);
        }
    }
    std::cout << "Range: [" << min_val << ", " << max_val << "] (dovrebbe essere circa [-1, 1])" << std::endl;
    
    // 2. Verifica che il rumore sia coerente (punti vicini hanno valori simili)
    float diff = std::abs(pn.noise(1.0f, 1.0f) - pn.noise(1.01f, 1.01f));
    std::cout << "Coerenza: diff tra punti vicini = " << diff << " (dovrebbe essere piccola)" << std::endl;
    
}

int main() {
    testNoise();
    return 0;
}

#endif