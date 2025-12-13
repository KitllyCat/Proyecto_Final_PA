// main.cpp - Remoria (actualizado con autosave)
// Requisitos: SFML 2.5.1 y nlohmann::json (json.hpp) en include path
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "src/core/ResourceManager.h"
#include "src/visualnovel/SceneManager.h"
#include "src/save/SaveManager.h"

using namespace std;
using namespace sf;
using json = nlohmann::json;

int main() {
    json config;
    const string configPath = "data/game_config.json";
    ifstream cfgFile(configPath);

    if (!cfgFile.is_open()) {
        cout << "Notice: No se pudo abrir game_config.json, usando defaults\n";
        config["window"]["width"] = 1920;
        config["window"]["height"] = 1080;
        config["window"]["title"] = "Remoria";
        config["start_scene"] = "prologue.json";
    } else {
        cfgFile >> config;
        cfgFile.close();
    }

    int WIDTH = config["window"].value("width", 1920);
    int HEIGHT = config["window"].value("height", 1080);
    string TITLE = config["window"].value("title", "Remoria");
    string startScene = config.value("start_scene", "prologue.json");

    RenderWindow window(VideoMode(WIDTH, HEIGHT), TITLE, Style::Close);
    window.setFramerateLimit(60);

    ResourceManager resources;
    SceneManager sceneManager(resources);

    // ============================
    // AUTOSAVE LOAD
    // ============================
    string sceneId;
    int stepIndex = 0;

    if (SaveManager::getInstance().load(sceneId, stepIndex)) {
        cout << "Autosave cargado: " << sceneId << " step " << stepIndex << endl;
        sceneManager.loadScene("data/scenes/" + sceneId + ".json", stepIndex);
    } else {
        sceneManager.loadScene("data/scenes/" + startScene, 0);
    }

    Clock clock;
    while (window.isOpen()) {
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();

            sceneManager.handleEvent(ev);
        }

        float dt = clock.restart().asSeconds();
        sceneManager.update(dt);

        window.clear(Color(10, 10, 10));
        sceneManager.draw(window);
        window.display();
    }

    return 0;
}
