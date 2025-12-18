// main.cpp - Remoria v0.6.8
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

bool isMaximized = false;
Vector2u windowedSize(1920, 1080);

void toggleWindowedFullscreen(RenderWindow& window, bool& isMaximized, const string& title, const Image& icon) {
    if (!isMaximized) {
        VideoMode desktop = VideoMode::getDesktopMode();
        window.create(desktop,title,Style::Resize | Style::Close);
        isMaximized = true;
    } else {
        window.create(VideoMode(windowedSize.x, windowedSize.y),title,Style::Resize | Style::Close);
        isMaximized = false;
    }
    window.setFramerateLimit(60);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    // Resolución lógica fija
    View view(FloatRect(0.f, 0.f, 1920.f, 1080.f));
    window.setView(view);
}

enum class GameState {
    Intro,
    Menu,
    TransitionToGame,
    Playing
};

int main() {
    SetConsoleOutputCP(CP_UTF8);//Admite utf8 en consola
    cout<<"INICIANDO GAME ENIGNE..."<<endl;
	//Carga de json de config
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
    //Renderizar la ventana
    RenderWindow window(VideoMode(1920, 1080), config["window"].value("title", "Remoria~"), Style::Resize | Style::Close);
	window.setFramerateLimit(60);
	//Asignar icono del juego
	Image icon;
    if (!icon.loadFromFile("assets/images/icon.png")) {
        cout << "[System ERROR]: No se pudo cargar el icono del juego\n";
    } else {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
	//Inicia el Core
    ResourceManager resources;
    SceneManager sceneManager(resources);
    sceneManager.setScreenSize(window.getSize());
	//Inicia Mainmenu
    MainMenu menu(resources, window.getSize());
    CreditsScreen creditsScreen(resources, window.getSize());
	//Inicia Intro
    IntroScreen intro(resources, window.getSize());
    intro.addLogo("assets/images/intro/company_logo.png", 3.0f, Color::Black);
    intro.addLogo("assets/images/intro/made_with.png", 2.5f, Color(34, 3, 12));
	//Inicia las transiciones
    TransitionManager transition;
    transition.setScreenSize(window.getSize());
    //GameState, estado actual del juego :3
    GameState state = GameState::Intro;

    bool showingCredits = false;
    string sceneToLoad;
    int stepToLoad = 0;
    Clock clock;
    cout<<"GAME ENIGNE INICIADO!!!"<<endl;
	//Sfml abre la ventana en loop
    while (window.isOpen()) {
        Event ev; //declaracion de ev(para todo el enigne)
        while (window.pollEvent(ev)) {
        	if (ev.type == Event::Closed){
        		window.close();
			}
        	if (ev.type == Event::KeyPressed) {
			    if (ev.key.code == Keyboard::F) {
			        toggleWindowedFullscreen(window,isMaximized,config["window"].value("title", "Remoria~"),icon);
			        sceneManager.setScreenSize(window.getSize());
			        transition.setScreenSize(window.getSize());
			    }
			}
            if (state == GameState::Intro) {
                intro.handleEvent(ev);
            } else if (state == GameState::Menu) {
                if (showingCredits){
                	creditsScreen.handleEvent(ev);
				} else {
					menu.handleEvent(ev, window);
				}
            } else if (state == GameState::Playing) {
                sceneManager.handleEvent(ev);
            }
        }
        float dt = clock.restart().asSeconds();
		//Estado de Updates
        if (state == GameState::Intro) {
            intro.update(dt);
            if (intro.isFinished()) {
                menu.playMusic();
                state = GameState::Menu;
            }
        } else if (state == GameState::Menu) {
            menu.update(dt, window);
			//Muestra creditos como capa de arriba
            if (!showingCredits && menu.creditsRequested()) {
                cout << "[Main] Mostrando créditos" << endl;
                creditsScreen.reset();
                menu.resetCreditsRequest();
                showingCredits = true;
            } else if (showingCredits) {
                creditsScreen.update(dt);
                if (creditsScreen.backRequested()) {
                    cout << "[Main] Cerrando créditos" << endl;
                    creditsScreen.reset();
                    showingCredits = false;
                }
            } else if (menu.startNewGameRequested()) { //Inicia juego...
                transition.start(TransitionManager::Type::FADE_TO_BLACK, 1.f);
                sceneToLoad = "data/scenes/prologue.json";
                stepToLoad = 0;
                state = GameState::TransitionToGame;
            } else if (menu.continueRequested()) {
                string sceneId;
                if (SaveManager::getInstance().load(sceneId, stepToLoad)) {
                    transition.start(TransitionManager::Type::FADE_TO_BLACK, 1.f);
                    sceneToLoad = "data/scenes/" + sceneId + ".json";
                    state = GameState::TransitionToGame;
                }
            }
        } else if (state == GameState::TransitionToGame) {
            transition.update(dt);
            if (transition.isComplete()) {
                menu.stopMusic();
                sceneManager.loadScene(sceneToLoad, stepToLoad);
                transition.reset();
                state = GameState::Playing;
            }
        } else if (state == GameState::Playing) {
            sceneManager.update(dt);
        }
		//Empieza a dibujar en pantalla
        window.clear(Color::Black);
        if (state == GameState::Intro) {
            intro.draw(window);
        } else if (state == GameState::Menu) {
            menu.draw(window);
            if (showingCredits)
                creditsScreen.draw(window);
        } else if (state == GameState::TransitionToGame) {
            menu.draw(window);
            transition.draw(window);
        } else if (state == GameState::Playing) {
            sceneManager.draw(window);
        }
        window.display();
    }
    return 0;
}
//end main.cpp - Remoria v0.6.8