#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <memory>
#include "../core/ResourceManager.h"
#include "Scene.h"
using namespace std;
using namespace sf;

class SceneManager {
public:
    SceneManager(ResourceManager& res);
    ~SceneManager();
    //Carga la escena inicial
    bool loadInitialScene(const string& scenePath);
    //Carga cualquier escena
    bool loadScene(const string& path, int startStep = 0);
    //Update/Draw/Events
    void update(float dt);
    void handleEvent(const Event& ev);
    void draw(RenderWindow& window);
    //Devuelve el path de la escena actual
    string currentScenePath() const;
    void setScreenSize(Vector2u size);
private:
    ResourceManager& resources;
    unique_ptr<Scene> currentScene;
    string currentPath;
    //Sistema de musica
    Music sceneMusic;
    string currentMusicPath;
    //Helpers
    void loadMusicFromJSON(const string& scenePath);
    void loadMusic(const string& musicPath);
    void stopMusic();
    Vector2u screenSize;
};

#endif