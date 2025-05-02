#include "../include/Renderer.hpp"
#include <iostream>
#include <cstdint> // For uint8_t
#include <optional>
#include <string> // For std::string in drawUI
#include <algorithm> // For std::max, std::min


Renderer::Renderer(const Terrain& terreno, unsigned int cellSize)
    : terreno(terreno), cellSize(cellSize), isPanning(false) {


    // Create window with appropriate size
    int width = terreno.getWidth() * cellSize;
    int height = terreno.getHeight() * cellSize;

    finestra.create(sf::VideoMode({static_cast<unsigned int>(width),
                                  static_cast<unsigned int>(height)}),
                    "Terrain Generation Viewer");

    // Set up camera view
    camera = finestra.getDefaultView();

}

void Renderer::run() {
    finestra.setFramerateLimit(60);

    while (finestra.isOpen()) {
        gestioneEventi();

        finestra.clear(sf::Color::Black);
        finestra.setView(camera); // Apply the potentially panned/zoomed view

        drawTerreno();
        drawUI();

        finestra.display();
    }
}

void Renderer::setModalita(ModalitaVisualizzazione modalita) {
    modAttuale = modalita;
}

//// Event handling

void Renderer::gestioneEventi()
{
    while (auto eventOpt = finestra.pollEvent())
    {
        // 1) Window closed or Escape pressed
        if (eventOpt->is<sf::Event::Closed>() ||
            (eventOpt->is<sf::Event::KeyPressed>() && eventOpt->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
        {
            finestra.close();
            return; // Exit the function immediately after closing
        }

        // 2) Change visualization mode with keys B, A, C
        if (auto const* key = eventOpt->getIf<sf::Event::KeyPressed>())
        {
            switch (key->code)
            {
                case sf::Keyboard::Key::B:
                    setModalita(ModalitaVisualizzazione::BIOMI);
                    // std::cout << "Mode changed to: Biome View" << std::endl; // Debug removed
                    break;
                case sf::Keyboard::Key::A:
                    setModalita(ModalitaVisualizzazione::ALTEZZA);
                    // std::cout << "Mode changed to: Height View" << std::endl; // Debug removed
                    break;
                case sf::Keyboard::Key::C:
                    setModalita(ModalitaVisualizzazione::COMBINATA);
                    // std::cout << "Mode changed to: Combined View" << std::endl; // Debug removed
                    break;
                default:
                    // Ignore other keys
                    break;
            }
        }

        // 3) Zoom with Mouse Wheel
        if (auto const* wheel = eventOpt->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (wheel->wheel == sf::Mouse::Wheel::Vertical) // Check for vertical scroll
            {
                float zoomFactor = (wheel->delta > 0) ? 1.f / 1.1f : 1.1f; // Zoom in or out
                zoomView(zoomFactor, wheel->position); // Pass mouse position
            }
        }

        // 4) Start Panning (Middle Mouse Button)
        if (auto const* mb = eventOpt->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mb->button == sf::Mouse::Button::Middle)
            {
                // std::cout << "DEBUG: Panning started at pixel: (" << mb->position.x << ", " << mb->position.y << ")" << std::endl; // DEBUG removed
                isPanning = true;
                lastMousePos = mb->position; // Store initial position
                finestra.setMouseCursor(sf::Cursor(sf::Cursor::Type::SizeAll)); // Change cursor
            }
        }

        // 5) Stop Panning
        if (auto const* mb = eventOpt->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mb->button == sf::Mouse::Button::Middle)
            {
                // if (isPanning) { // Only print if we were actually panning // DEBUG removed
                //      std::cout << "DEBUG: Panning stopped" << std::endl; // DEBUG removed
                // } // DEBUG removed
                isPanning = false;
                finestra.setMouseCursor(sf::Cursor(sf::Cursor::Type::Arrow)); // Restore cursor
            }
        }

        // 6) Pan View on Mouse Move (if panning)
        // TO_DO: Panning logic is here but reported as not working. Needs further investigation.
        if (auto const* mm = eventOpt->getIf<sf::Event::MouseMoved>())
        {
            if (isPanning)
            {
                panView(mm->position); // Pass current position
                lastMousePos = mm->position; // Update last position for next move
            }
        }
    }
}

//// Drawing

void Renderer::panView(sf::Vector2i currentMousePos) {
    sf::Vector2f lastPosF = finestra.mapPixelToCoords(lastMousePos);
    sf::Vector2f currentPosF = finestra.mapPixelToCoords(currentMousePos);
    sf::Vector2f delta = lastPosF - currentPosF; // Calculate the difference in world coordinates

    // Add this debug output
    // if (delta.x != 0 || delta.y != 0) { // DEBUG removed
    //     std::cout << "DEBUG: panView called. Delta World Coords: (" << delta.x << ", " << delta.y << ")" << std::endl; // DEBUG removed
    // } else { // DEBUG removed
         // Optional: Log if delta is zero to see if that's the issue // DEBUG removed
         // std::cout << "DEBUG: panView called but delta is zero." << std::endl; // DEBUG removed
    // } // DEBUG removed


    camera.move(delta); // Move the camera by the delta
    finestra.setView(camera); // Apply the updated view
}

void Renderer::drawTerreno() {
    int width = terreno.getWidth();
    int height = terreno.getHeight();

    // Get the visible area based on the current camera view
    const sf::View& currentView = finestra.getView(); // Use the member 'camera' or get the current view
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    // Calculate the range of cells to draw based on the view
    // Add/subtract 1 cell buffer for safety when panning/zooming near edges
    int startX = std::max(0, static_cast<int>((viewCenter.x - viewSize.x / 2.0f) / cellSize) - 1);
    int startY = std::max(0, static_cast<int>((viewCenter.y - viewSize.y / 2.0f) / cellSize) - 1);
    int endX = std::min(width, static_cast<int>((viewCenter.x + viewSize.x / 2.0f) / cellSize) + 1);
    int endY = std::min(height, static_cast<int>((viewCenter.y + viewSize.y / 2.0f) / cellSize) + 1);

    sf::RectangleShape cella(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize))); // Create cell shape once

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            cella.setPosition({static_cast<float>(x * cellSize),
                             static_cast<float>(y * cellSize)});

            sf::Color colore;

            switch (modAttuale) {
                case ModalitaVisualizzazione::BIOMI:
                    colore = determinaColoreBioma(terreno.getBioma(x, y));
                    break;
                case ModalitaVisualizzazione::ALTEZZA:
                    colore = determinaColore(terreno.getAltezza(x, y));
                    break;
                case ModalitaVisualizzazione::COMBINATA:
                    colore = determinaColoreBioma(terreno.getBioma(x, y));
                    // TO_DO: Refine brightness adjustment based on height. Ensure noise range is handled correctly.
                    float altezza = terreno.getAltezza(x, y); // Noise value is typically -1 to 1
                    // Map noise from [-1, 1] to a brightness factor [0.4, 1.0] (example range)
                    float factor = 0.7f + 0.3f * (altezza + 1.0f) / 2.0f; // Adjust factor calculation
                    colore.r = static_cast<uint8_t>(std::min(255.0f, static_cast<float>(colore.r) * factor));
                    colore.g = static_cast<uint8_t>(std::min(255.0f, static_cast<float>(colore.g) * factor));
                    colore.b = static_cast<uint8_t>(std::min(255.0f, static_cast<float>(colore.b) * factor));
                    break;
            }

            cella.setFillColor(colore);
            finestra.draw(cella);
        }
    }
}

sf::Color Renderer::determinaColore(float valore_noise) const {
    // TO_DO: Improve grayscale mapping. Noise is -1 to 1, map it to 0-255.
    // Map noise from [-1, 1] to [0, 1] then scale to [0, 255]
    uint8_t intensity = static_cast<uint8_t>((valore_noise + 1.0f) / 2.0f * 255.0f);
    return sf::Color(intensity, intensity, intensity);
}

sf::Color Renderer::determinaColoreBioma(Bioma bioma) const {
    // Use the correct biome enums from Terrain.hpp
    switch (bioma) {
        case Bioma::Acqua:
            return sf::Color(65, 105, 225);  // RoyalBlue
        case Bioma::Sabbia:
            return sf::Color(238, 214, 175);  // AntiqueWhite variant
        case Bioma::Erba:
            return sf::Color(34, 139, 34);    // ForestGreen
        case Bioma::Collina:
            return sf::Color(160, 82, 45);    // Sienna
        case Bioma::Montagna:
            return sf::Color(139, 137, 137);  // Gray75 variant
        default:
            return sf::Color::Magenta;        // Error color
    }
}

void Renderer::zoomView(float factor, sf::Vector2i mousePos) {
    // Convert pixel position to world coordinates before zoom
    sf::Vector2f beforeCoords = finestra.mapPixelToCoords(mousePos, camera); // Specify the view

    camera.zoom(factor);
    // Don't set the view yet, calculate offset first

    // Convert the same pixel position to world coordinates after zoom
    sf::Vector2f afterCoords = finestra.mapPixelToCoords(mousePos, camera); // Specify the view

    // Calculate the offset needed to keep the point under the mouse stationary
    sf::Vector2f offsetCoords = beforeCoords - afterCoords;

    camera.move(offsetCoords);
    finestra.setView(camera); // Apply the final view with zoom and offset
}


void Renderer::drawUI() {
    // TO_DO: Implement more UI elements if needed (e.g., coordinates, zoom level).
    // Save current view and set to default for UI
    sf::View previousView = finestra.getView();
    finestra.setView(finestra.getDefaultView()); // Use the window's default view for UI

    // --- UI Drawing Code ---
    // Example: Display current mode (Requires sf::Font and sf::Text setup)
    /*
    sf::Font font;
    // TO_DO: Load font properly, handle errors
    if (!font.loadFromFile("path/to/your/font.ttf")) {
         std::cerr << "Error loading font for UI" << std::endl;
    } else {
        sf::Text modeText;
        modeText.setFont(font);
        modeText.setCharacterSize(16); // pixels
        modeText.setFillColor(sf::Color::White);
        modeText.setPosition(10.f, 10.f); // Top-left corner

        std::string modeStr;
        switch (modAttuale) {
            case ModalitaVisualizzazione::BIOMI:    modeStr = "Mode: Biomes (B)"; break;
            case ModalitaVisualizzazione::ALTEZZA:  modeStr = "Mode: Height (A)"; break;
            case ModalitaVisualizzazione::COMBINATA: modeStr = "Mode: Combined (C)"; break;
        }
        modeText.setString(modeStr);
        finestra.draw(modeText);
    }
    */
    // --- End UI Drawing Code ---


    // Restore previous view used for the terrain
    finestra.setView(previousView);
}