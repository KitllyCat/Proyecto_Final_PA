// main.cpp - Remoria (Menu + Créditos como Overlay)
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include "json.hpp"

#include "src/core/ResourceManager.h"
#include "src/visualnovel/SceneManager.h"
#include "src/save/SaveManager.h"
#include "src/graphics/TransitionManager.h"

#include "MainMenu.h"
#include "IntroScreen.h"
#include "CreditsScreen.h"

using namespace std;
using namespace sf;
using json = nlohmann::json;

enum class GameState {
    Intro,
    Menu,
    TransitionToGame,
    Playing
};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ================= CONFIG =================
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

    RenderWindow window(
        VideoMode(
            config["window"].value("width", 1920),
            config["window"].value("height", 1080)
        ),
        config["window"].value("title", "Remoria"),
        Style::Close
    );
    window.setFramerateLimit(60);
	
	Image icon;
	if (!icon.loadFromFile("assets/images/icon.png")) {
	    cout << "ERROR: No se pudo cargar el icono del juego\n";
	} else {
	    window.setIcon(
	        icon.getSize().x,
	        icon.getSize().y,
	        icon.getPixelsPtr()
	    );
	}
	
    // ================= CORE =================
    ResourceManager resources;
    SceneManager sceneManager(resources);
    sceneManager.setScreenSize(window.getSize());

    MainMenu menu(resources, window.getSize());
    CreditsScreen creditsScreen(resources, window.getSize());

    IntroScreen intro(resources, window.getSize());
    intro.addLogo("assets/images/intro/company_logo.png", 3.0f, Color::Black);
    intro.addLogo("assets/images/intro/made_with.png", 2.5f, Color(34, 3, 12));

    TransitionManager transition;
    transition.setScreenSize(window.getSize());

    // ================= STATE =================
    GameState state = GameState::Intro;

    bool showingCredits = false;  // ✅ FLAG CLARO Y ÚNICO

    string sceneToLoad;
    int stepToLoad = 0;

    Clock clock;

    // ================= LOOP =================
    while (window.isOpen()) {
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed)
                window.close();

            if (state == GameState::Intro) {
                intro.handleEvent(ev);
            }
            else if (state == GameState::Menu) {
                if (showingCredits)
                    creditsScreen.handleEvent(ev);
                else
                    menu.handleEvent(ev, window);
            }
            else if (state == GameState::Playing) {
                sceneManager.handleEvent(ev);
            }
        }

        float dt = clock.restart().asSeconds();

        // ================= UPDATE =================
        if (state == GameState::Intro) {
            intro.update(dt);
            if (intro.isFinished()) {
                menu.playMusic();
                state = GameState::Menu;
            }
        }
        else if (state == GameState::Menu) {
            menu.update(dt, window);

            // ---- CRÉDITOS COMO OVERLAY ----
            if (!showingCredits && menu.creditsRequested()) {
                cout << "[Main] Mostrando créditos" << endl;
                creditsScreen.reset();
                menu.resetCreditsRequest();   // 🔑 LIMPIAR FLAG
                showingCredits = true;
            }
            else if (showingCredits) {
                creditsScreen.update(dt);

                if (creditsScreen.backRequested()) {
                    cout << "[Main] Cerrando créditos" << endl;
                    creditsScreen.reset();
                    showingCredits = false;
                }
            }
            // ---- JUEGO ----
            else if (menu.startNewGameRequested()) {
                transition.start(TransitionManager::Type::FADE_TO_BLACK, 1.f);
                sceneToLoad = "data/scenes/prologue.json";
                stepToLoad = 0;
                state = GameState::TransitionToGame;
            }
            else if (menu.continueRequested()) {
                string sceneId;
                if (SaveManager::getInstance().load(sceneId, stepToLoad)) {
                    transition.start(TransitionManager::Type::FADE_TO_BLACK, 1.f);
                    sceneToLoad = "data/scenes/" + sceneId + ".json";
                    state = GameState::TransitionToGame;
                }
            }
        }
        else if (state == GameState::TransitionToGame) {
            transition.update(dt);
            if (transition.isComplete()) {
                menu.stopMusic();
                sceneManager.loadScene(sceneToLoad, stepToLoad);
                transition.reset();
                state = GameState::Playing;
            }
        }
        else if (state == GameState::Playing) {
            sceneManager.update(dt);
        }

        // ================= DRAW =================
        window.clear(Color::Black);

        if (state == GameState::Intro) {
            intro.draw(window);
        }
        else if (state == GameState::Menu) {
            menu.draw(window);

            if (showingCredits)
                creditsScreen.draw(window);
        }
        else if (state == GameState::TransitionToGame) {
            menu.draw(window);
            transition.draw(window);
        }
        else if (state == GameState::Playing) {
            sceneManager.draw(window);
        }

        window.display();
    }

    return 0;
}
