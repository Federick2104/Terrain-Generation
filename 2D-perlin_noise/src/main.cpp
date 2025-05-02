#include <SFML/Graphics.hpp>
#include "../include/Noise.hpp"
#include "../include/Terrain.hpp"
#include "../include/Renderer.hpp"
#include <iostream>

int main() {
    try {
        // Create Perlin noise generator
        PerlinNoise noise(12345678910);  // Seed for noise generation
        
        // Create terrain with dimensions
        Terrain terreno(noise, 1024, 1024, 0.05f);
        // Griglia di 256x256 con scala di 0.05f (256*0.05 = 12.8)

        /*
            Small scale (0.05): Large, continuous regions of the same biome
            Large scale (1.0): Fragmented biomes with frequent transitions
        */


        // Create renderer with cell size of 3 pixels
        Renderer renderer(terreno, 3);
        
        std::cout << "Terrain generation complete. Starting renderer..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  B: Biome view mode" << std::endl;
        std::cout << "  A: Height view mode" << std::endl;
        std::cout << "  C: Combined view mode" << std::endl;
        std::cout << "  Esc: Exit" << std::endl;
        
        // Run the visualization
        renderer.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}