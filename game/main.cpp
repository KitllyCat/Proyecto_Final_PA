// main.cpp - Remoria (Menu + Autosave)
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <windows.h>
#include "src/core/ResourceManager.h"
#include "src/visualnovel/SceneManager.h"
#include "src/save/SaveManager.h"
#include "MainMenu.h"

using namespace std;
using namespace sf;
using json = nlohmann::json;

enum class GameState {
    Menu,
    Playing
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
	json config;
    ifstream cfg("data/game_config.json");

    if (!cfg.is_open()) {
        config["window"]["width"] = 1920;
        config["window"]["height"] = 1080;
        config["window"]["title"] = "Remoria";
        config["start_scene"] = "prologue";
    } else {
        cfg >> config;
        cfg.close();
    }

    int WIDTH  = config["window"].value("width", 1920);
    int HEIGHT = config["window"].value("height", 1080);
    string TITLE = config["window"].value("title", "Remoria");

    RenderWindow window(VideoMode(WIDTH, HEIGHT), TITLE, Style::Close);
	window.setFramerateLimit(60);
	
	ResourceManager resources;
	SceneManager sceneManager(resources);
	
	// NUEVO: Configurar tamaño de pantalla para transiciones
	sceneManager.setScreenSize(window.getSize());
	
	MainMenu menu(resources, window.getSize());
	menu.playMusic();

    GameState state = GameState::Menu;

    Clock clock;
    while (window.isOpen()) {
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();

            if (state == GameState::Menu)
                menu.handleEvent(ev, window);
            else
                sceneManager.handleEvent(ev);
        }

        float dt = clock.restart().asSeconds();

        if (state == GameState::Menu) {
            menu.update(dt);

            // NUEVA PARTIDA
            if (menu.startNewGameRequested()) {
                cout << "[Main] Iniciando nueva partida..." << endl;
                
                // ✅ DETENER MÚSICA DEL MENÚ
                menu.stopMusic();
                
                SaveManager::getInstance().clear();
                sceneManager.loadScene("data/scenes/prologue.json", 0);
                state = GameState::Playing;
            }
            // CONTINUAR
            else if (menu.continueRequested()) {
                cout << "[Main] Cargando partida guardada..." << endl;
                
                // ✅ DETENER MÚSICA DEL MENÚ
                menu.stopMusic();
                
                string sceneId;
                int step = 0;
                if (SaveManager::getInstance().load(sceneId, step)) {
                    sceneManager.loadScene(
                        "data/scenes/" + sceneId + ".json",
                        step
                    );
                    state = GameState::Playing;
                }
            }
            // CRÉDITOS (opcional)
            else if (menu.creditsRequested()) {
                cout << "[Main] Mostrando créditos (pendiente)" << endl;
            }
        }
        else {
            sceneManager.update(dt);
        }

        window.clear(Color::Black);

        if (state == GameState::Menu)
            menu.draw(window);
        else
            sceneManager.draw(window);

        window.display();
    }

    return 0;
}