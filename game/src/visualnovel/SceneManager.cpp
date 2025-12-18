#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include "../json.hpp"
using json = nlohmann::json;

SceneManager::SceneManager(ResourceManager& res)
: resources(res), 
  currentScene(nullptr), 
  currentMusicPath(""),
  screenSize(1920, 1080)
{
}

SceneManager::~SceneManager() {
    stopMusic();
}

void SceneManager::setScreenSize(Vector2u size) {
    screenSize = size;
    if (currentScene) {
        currentScene->setScreenSize(size);
    }
}

bool SceneManager::loadInitialScene(const string& scenePath) {
    return loadScene(scenePath);
}

bool SceneManager::loadScene(const string& path, int startStep) {
    cout << "[System] Cargando escena: " << path << " (step " << startStep << ")" << endl;
    currentPath = path;
    currentScene = make_unique<Scene>();
    bool success = currentScene->loadFromFile(path, resources, startStep);
    if (!success) {
        cerr << "[System ERROR] No se pudo cargar: " << path << endl;
        currentScene.reset();
        currentPath.clear();
        return false;
    }
    //Configurar tamaño de pantalla
    currentScene->setScreenSize(screenSize);
    //Registrar callback de musica
    currentScene->setMusicChangeCallback([this](const string& musicPath) {
        this->loadMusic(musicPath);
    });
    loadMusicFromJSON(path);
    return true;
}

void SceneManager::loadMusicFromJSON(const string& scenePath) {
    try {
        ifstream file(scenePath);
        if (!file.is_open()) {
            cerr << "[System] No se pudo abrir JSON para leer música: " << scenePath << endl;
            return;
        }
        json j;
        file >> j;
        file.close();
        if (j.contains("music")) {
            string musicPath = j["music"];
            if (musicPath.empty()) {
                stopMusic();
                return;
            }
            loadMusic(musicPath);
        } else {
            stopMusic();
        }
    }
    catch (const exception& e) {
        cerr << "[System ERROR] Al leer música del JSON: " << e.what() << endl;
    }
}

void SceneManager::loadMusic(const string& musicPath) {
    if (musicPath == currentMusicPath && sceneMusic.getStatus() == Music::Playing) {
        return;
    }
    stopMusic();
    if (!sceneMusic.openFromFile(musicPath)) {
        cerr << "[System ERROR] No se pudo cargar música: " << musicPath << endl;
        return;
    }
    
    currentMusicPath = musicPath;
    sceneMusic.setLoop(true);
    sceneMusic.setVolume(70.f);
    sceneMusic.play();
    cout << "[Music] ♪ Reproduciendo~: " << musicPath << endl;
}

void SceneManager::stopMusic() {
    if (sceneMusic.getStatus() == Music::Playing) {
        sceneMusic.stop();
    }
    currentMusicPath.clear();
}

void SceneManager::update(float dt) {
    if (!currentScene) return;
    currentScene->update(dt);
    if (currentScene->isFinished()) {
        string next = currentScene->getNextScene();
        if (!next.empty()) {
            if (next == currentPath) {
                cerr << "[System WARNING] Escena intenta cargarse a sí misma: " << next << endl;
                return;
            }
            string fullPath = next;
            if (next.find('/') == string::npos && next.find('\\') == string::npos) {
                fullPath = "data/scenes/" + next;
            }
            loadScene(fullPath);
        }
    }
}

void SceneManager::handleEvent(const Event& ev) {
    if (!currentScene){return;}
    currentScene->handleEvent(ev);
}

void SceneManager::draw(RenderWindow& window) {
    if (!currentScene){return;}
    currentScene->draw(window);
}

string SceneManager::currentScenePath() const { 
    return currentPath; 
}
