/*
 * Remoria - Visual Novel Engine
 * Main Entry Point
 *
 * Resolución nativa: 1920x1080
 * Resolución de sprites: 320x180 (escalado x6)
 */

#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "src/core/ResourceManager.h"
#include "json.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

int main() {

    // -----------------------------
    //  Cargar config del juego
    // -----------------------------
    json config;

    ifstream file("data/game_config.json");
    if (!file.is_open()) {
        cout << "ERROR: No se pudo abrir game_config.json\n";
        return 1;
    }

    file >> config;
    file.close();

    int WIDTH  = config["window"]["width"];
    int HEIGHT = config["window"]["height"];
    string TITLE = config["window"]["title"];
    string ICON_PATH = config["window"]["icon"];

    // -----------------------------
    //  Crear ventana
    // -----------------------------
    RenderWindow window(VideoMode(WIDTH, HEIGHT), TITLE);
    window.setFramerateLimit(60);

    // Cargar icono
    Image icon;
    if (icon.loadFromFile(ICON_PATH)) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    // ResourceManager global
    ResourceManager rm;

    // Cargar fuente de ejemplo
    Font& font = rm.getFont("assets/fonts/default.ttf");

    Text text("Bienvenido a REMORIA", font, 42);
    text.setPosition(50, 50);
    text.setFillColor(Color::White);

    // -----------------------------
    //  Bucle principal del juego
    // -----------------------------
    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color(20, 20, 20)); // fondo temporal oscuro
        window.draw(text);
        window.display();
    }

    return 0;
}
