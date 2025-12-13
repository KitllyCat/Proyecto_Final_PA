#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager(ResourceManager& res)
: resources(res), currentScene(nullptr)
{
}

bool SceneManager::loadInitialScene(const string& scenePath) {
    return loadScene(scenePath);
}

/*bool SceneManager::loadScene(const string& scenePath) {
    cout << "SceneManager: cargando escena: " << scenePath << endl;
    auto s = make_unique<Scene>();
    if (!s->loadFromFile(scenePath, resources)) {
        cout << "SceneManager: fallo cargando escena: " << scenePath << endl;
        return false;
    }
    currentScene = move(s);
    currentPath = scenePath;
    return true;
}*/

bool SceneManager::loadScene(const string& path, int startStep) {
    currentScene = make_unique<Scene>();
    return currentScene->loadFromFile(path, resources, startStep);
}


void SceneManager::update(float dt) {
    if (!currentScene) return;
    currentScene->update(dt);

    // si la escena terminó y pide ir a otra escena, cargarla
    if (currentScene->isFinished()) {
        string next = currentScene->getNextScene();
        if (!next.empty()) {
            // resolver path: si next es ruta relativa, asumimos en data/scenes/
            // ajustar según estructura del proyecto
            string prefix = "data/scenes/";
            string full = next;
            // si next ya contiene '/' asumimos es ruta completa, sino la añadimos
            if (next.find('/') == string::npos && next.find('\\') == string::npos) {
                full = prefix + next;
            }
            loadScene(full);
        } else {
            // por ahora si la escena termino sin next, la dejamos en estado final
        }
    }
}

void SceneManager::handleEvent(const Event& ev) {
    if (!currentScene) return;
    currentScene->handleEvent(ev);
}

void SceneManager::draw(RenderWindow& window) {
    if (!currentScene) return;
    currentScene->draw(window);
}

string SceneManager::currentScenePath() const { return currentPath; }
