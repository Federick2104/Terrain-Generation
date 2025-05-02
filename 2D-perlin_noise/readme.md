# Come compilare ed eseguire il progetto

Questo progetto utilizza CMake per la compilazione. Assicurati di avere CMake e un compilatore C++ (come g++ o Clang) installati. Se sei su Linux (come Debian/Ubuntu), installa anche le dipendenze di SFML:

```bash
# Esempio per Debian/Ubuntu
sudo apt update
sudo apt install build-essential cmake libgl-dev libx11-dev libxrandr-dev libfreetype6-dev libudev-dev
```

## 1. Creare la directory di build

Dalla directory principale del progetto (`2D-perlin_noise`), esegui:

```bash
mkdir build
cd build
```

## 2. Configurare con CMake

Una volta dentro la directory `build`, esegui CMake per generare i file di build:

```bash
cmake ..
```

## 3. Compilare il progetto

Sempre dalla directory `build`, compila entrambi gli eseguibili (`TerrainGeneration` e `NoiseTest`):

```bash
make
```
*In alternativa, puoi compilare solo il target principale:*
```bash
make TerrainGeneration
```
*O solo il target di test:*
```bash
make NoiseTest
```

## 4. Eseguire il programma principale

Dopo la compilazione, puoi eseguire il visualizzatore del terreno dalla directory `build`:

```bash
./TerrainGeneration
```

## 5. Eseguire il test del noise (opzionale)

Se hai compilato `NoiseTest`, puoi eseguirlo dalla directory `build`:

```bash
./NoiseTest
```

# 2D Perlin Noise TerrainGenerator
### Main algorithm noise()
Questo codice implementa la funzione `noise()` dell’algoritmo **Perlin Noise** in 2D. Di seguito viene descritto in dettaglio ogni sua sezione.

---

## 1. Determinazione della cella unitaria

```cpp
int X = static_cast<int>(std::floor(x)) & 255;
int Y = static_cast<int>(std::floor(y)) & 255;
```

- ```std::floor(x)``` arrotonda verso il basso, ottenendo le coordinate intere della cella che contiene il punto ```(x, y)```.

- L'operazione ```& 255``` limita i valori nell'intervallo ```[0, 255]```, creando un effetto di wrapping della griglia, utile per ottenere noise ripetibile.

## 2. Calcolo delle Coordinate Relative nella Cella
```cpp
float x_rel = x - std::floor(x);
float y_rel = y - std::floor(y);
```

- Si calcolano le coordinate locali all'interno della cella, ottenendo valori frazionari in ```[0.0, 1.0]```.

- Questi valori saranno usati per interpolare tra i gradienti degli angoli della cella.

## 3. Calcolo dei Gradienti ai Quattro Angoli
```cpp
float n0 = grad(indice(X, Y), x_rel, y_rel);
float n1 = grad(indice(X+1, Y), x_rel-1, y_rel);
float n2 = grad(indice(X, Y+1), x_rel, y_rel-1);
float n3 = grad(indice(X+1, Y+1), x_rel-1, y_rel-1);
```

Questa parte calcola i "prodotti scalari" tra i vettori gradiente e i vettori che puntano dal vertice della cella al punto (x,y):

- ```indice(X,Y)``` determina quale gradiente usare in ogni angolo utilizzando la tabella di permutazione.

- ```grad()``` calcola il prodotto scalare tra il gradiente selezionato e il vettore distanza.

- Le coordinate relative vengono adattate per ogni angolo (es. ```x_rel-1``` per gli angoli a destra)

## 4. Interpolazione Lungo l'Asse X
```cpp
float ix0 = interpolare(n0, n1, x_rel);
float ix1 = interpolare(n2, n3, x_rel);
```

Questa parte interpola i valori di gradiente lungo l'asse X:

- ```ix0``` è l'interpolazione tra i valori in basso a sinistra e in basso a destra.

- ```ix1``` è l'interpolazione tra i valori in alto a sinistra e in alto a destra.

- La funzione ```interpolare()``` (non mostrata) dovrebbe utilizzare una curva smooth per un'interpolazione più naturale.

## 5. Interpolazione Finale Lungo l'Asse Y
```cpp
return interpolare(ix0, ix1, y_rel);
```
Questa parte interpola i due valori intermedi lungo l'asse Y per ottenere il valore finale del noise.

## 6. Funzionamento Complessivo

L'algoritmo genera un valore di rumore che:

1. È __deterministico__ (gli stessi input producono gli stessi output).

2. __Varia gradualmente__ (punti vicini hanno valori simili).

3. Non presenta pattern evidenti o ripetitivi (grazie alla randomizzazione della tabella di permutazione).

4. È __efficiente__ da calcolare (utilizza solo operazioni semplici).

Questo è il cuore dell'algoritmo Perlin Noise, che crea un "rumore coerente" utilizzato in innumerevoli applicazioni di computer grafica, generazione procedurale di terreni, texture, animazioni e molto altro


## Curiosità sul Perlin Noise

Curiosità sul Perlin Noise
La tua osservazione sul __range limitato__ [-0.5, 0.5] e coerenza di 0 quando non si randomizza la tabella di permutazione evidenzia alcuni aspetti affascinanti dell'algoritmo di Perlin Noise. Ecco alcune curiosità interessanti:

- Matematica dietro il __range limitato__: Quando la tabella di permutazione non è randomizzata, i gradienti seguono un pattern prevedibile. Matematicamente, questo porta a una convergenza verso la media durante l'interpolazione, limitando il range a [-0.5, 0.5] invece del teorico [-1, 1].

- __Invenzione accidentale__: Ken Perlin sviluppò l'algoritmo nel 1983 mentre lavorava agli effetti del film "Tron". Era frustrato dalla qualità artificiale delle texture procedurali dell'epoca.

- __Premio Oscar tecnico__: Perlin ricevette un Academy Award for Technical Achievement nel 1997 per questo algoritmo, evidenziando quanto fosse rivoluzionario per la computer grafica.

- __Approssimazione della natura__: Il Perlin Noise è così efficace perché simula la "correlazione locale" che si trova nei fenomeni naturali: valori vicini tendono ad essere correlati, ma con variazioni graduali.

- __Implementazione originale__: L'algoritmo originale di Perlin conteneva un bug che produceva un leggero pattern a scacchiera. Questo "difetto" divenne così distintivo che molte implementazioni successive lo mantennero intenzionalmente!

- __Improved Perlin Noise__: Nel 2002, Perlin stesso pubblicò una versione migliorata che eliminava il pattern a scacchiera, ma molti artisti preferiscono ancora la versione "classica" per il suo carattere distintivo.

- __Efficienza computazionale__: L'algoritmo è stato progettato specificamente per essere efficiente, utilizzando solo operazioni intere e interpolazioni semplici per funzionare bene anche su hardware limitato.

- __Dimensionalità__: Sebbene stia utilizzando la versione 2D, l'algoritmo può essere esteso a qualsiasi dimensione, rendendolo utile per animazioni (3D), simulazioni di fluidi (4D) e oltre.

- __Self-similarity__: A scale diverse, il Perlin Noise mostra proprietà di auto-similarità (simili ai frattali), che è una caratteristica fondamentale di molti fenomeni naturali.

- __Domini applicativi__: Oltre alla __computer grafica__, il Perlin Noise viene utilizzato in campi sorprendenti come __l'archeologia__ computazionale per simulare processi di erosione, in __musicologia__ per generare composizioni naturali, e persino in __ecologia__ per modellare la distribuzione delle popolazioni.

L'osservazione sulla tabella di permutazione non randomizzata ha effettivamente identificato una proprietà matematica fondamentale che distingue il semplice rumore dall'elegante "controlled randomness" che rende il Perlin Noise così utile e affascinante!