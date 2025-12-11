// main.cpp - Remoria (actualizado para no salir con return 1)
// Requisitos: SFML 2.5.1 y nlohmann::json (json.hpp) en include path
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "src/core/ResourceManager.h"
#include "src/visualnovel/SceneManager.h"

using namespace std;
using namespace sf;
using json = nlohmann::json;

int main() {
    // -------------------------
    //  Leer configuración
    // -------------------------
    json config;
    const string configPath = "data/game_config.json";
    ifstream cfgFile(configPath);
    if (!cfgFile.is_open()) {
        cout << "Notice: No se pudo abrir " << configPath << ". Usando valores por defecto.\n";
        // defaults
        config["window"]["width"]  = 1920;
        config["window"]["height"] = 1080;
        config["window"]["title"]  = "Remoria";
        config["start_scene"] = "data/scenes/prologue.json";
    } else {
        try {
            cfgFile >> config;
        } catch (exception& e) {
            cout << "Warning: error parseando " << configPath << " : " << e.what() 
                 << ". Se usarán valores por defecto.\n";
            config["window"]["width"]  = 1920;
            config["window"]["height"] = 1080;
            config["window"]["title"]  = "Remoria";
            config["start_scene"] = "data/scenes/prologue.json";
        }
        cfgFile.close();
    }

    // Extraer valores (con fallback)
    int WIDTH  = config.value("window", json::object()).value("width", 1920);
    int HEIGHT = config.value("window", json::object()).value("height", 1080);
    string TITLE = config.value("window", json::object()).value("title", string("Remoria"));
    string ICON_PATH = config.value("window", json::object()).value("icon", string("assets/images/icon.png"));
    string startScene = config.value("start_scene", string("data/scenes/prologue.json"));

    // -------------------------
    //  Crear ventana
    // -------------------------
    RenderWindow window(VideoMode((unsigned)WIDTH, (unsigned)HEIGHT), TITLE, Style::Close);
    window.setFramerateLimit(60);

    // Intentar cargar icono (silencioso si falla)
    Image icon;
    if (icon.loadFromFile(ICON_PATH)) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else {
        cout << "Notice: icon no encontrado en: " << ICON_PATH << "\n";
    }

    // -------------------------
    //  ResourceManager y SceneManager
    // -------------------------
    ResourceManager resources;

    // Carga de fuente por defecto (si existe) - no es fatal si falla
    const string defaultFontPath = "assets/fonts/default.ttf";
    try {
        resources.getFont(defaultFontPath);
    } catch (...) {
        cout << "Notice: fuente por defecto no encontrada en " << defaultFontPath << " (no es fatal)\n";
    }

    SceneManager sceneManager(resources);

    // Asegurar path completo de escena inicial
    string startPath = startScene;
    if (startScene.find("assets/") == string::npos &&
        startScene.find('/') == string::npos &&
        startScene.find('\\') == string::npos) {
        startPath = string("data/scenes/") + startScene;
    }

    if (!sceneManager.loadInitialScene(startPath)) {
        cout << "Warning: No se pudo cargar la escena inicial: " << startPath 
             << ". Se permanecerá en pantalla vacía.\n";
        // no salimos; el juego seguirá corriendo con pantalla vacía
    }

    // -------------------------
    //  Loop principal
    // -------------------------
    Clock clock;
    while (window.isOpen()) {
        // Poll events
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed) {
                window.close();
            }
            // Pasar eventos a SceneManager para que Dialogue/Choices los procesen
            sceneManager.handleEvent(ev);
        }

        // Update
        float dt = clock.restart().asSeconds();
        sceneManager.update(dt);

        // Draw
        window.clear(Color(10, 10, 10));
        sceneManager.draw(window);
        window.display();
    }

    return 0; // exit normal
}
