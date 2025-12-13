#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "../core/ResourceManager.h"
#include "Scene.h"

using namespace std;
using namespace sf;

class SceneManager {
public:
    SceneManager(ResourceManager& res);
    // carga la escena inicial
    bool loadInitialScene(const string& scenePath);

    // carga cualquier escena (ruta relativa)
    //bool loadScene(const string& scenePath);
    
    bool loadScene(const string& path, int startStep=0);


    // update / draw / events
    void update(float dt);
    void handleEvent(const Event& ev);
    void draw(RenderWindow& window);

    // devuelve el path de la escena actual
    string currentScenePath() const;

private:
    ResourceManager& resources;
    unique_ptr<Scene> currentScene;
    string currentPath;
};

#endif
