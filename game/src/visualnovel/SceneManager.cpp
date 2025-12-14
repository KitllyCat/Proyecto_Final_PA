#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include "../json.hpp"

using json = nlohmann::json;

SceneManager::SceneManager(ResourceManager& res)
: resources(res), 
  currentScene(nullptr), 
  currentMusicPath(""),
  screenSize(1920, 1080)  // ← NUEVO: Tamaño por defecto
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
    cout << "[SceneManager] Cargando escena: " << path << " (step " << startStep << ")" << endl;
    
    currentPath = path;
    
    currentScene = make_unique<Scene>();
    bool success = currentScene->loadFromFile(path, resources, startStep);
    
    if (!success) {
        cerr << "[SceneManager ERROR] No se pudo cargar: " << path << endl;
        currentScene.reset();
        currentPath.clear();
        return false;
    }
    
    // NUEVO: Configurar tamaño de pantalla para transiciones
    currentScene->setScreenSize(screenSize);
    
    // Registrar callback de música
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
            cerr << "[SceneManager] No se pudo abrir JSON para leer música: " << scenePath << endl;
            return;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Verificar si tiene campo "music"
        if (j.contains("music")) {
            string musicPath = j["music"];
            
            // Si está vacío, detener música actual
            if (musicPath.empty()) {
                stopMusic();
                return;
            }
            
            // Cargar nueva música (solo si es diferente)
            loadMusic(musicPath);
        } else {
            // Si no tiene campo "music", detener música
            cout << "[SceneManager] Escena sin música definida, deteniendo audio" << endl;
            stopMusic();
        }
    }
    catch (const exception& e) {
        cerr << "[SceneManager ERROR] Al leer música del JSON: " << e.what() << endl;
    }
}

void SceneManager::loadMusic(const string& musicPath) {
    // Si ya está sonando la misma música, no recargar
    if (musicPath == currentMusicPath && sceneMusic.getStatus() == Music::Playing) {
        cout << "[SceneManager] Música ya está sonando: " << musicPath << endl;
        return;
    }
    
    // Detener música anterior
    stopMusic();
    
    // Intentar cargar nueva música
    if (!sceneMusic.openFromFile(musicPath)) {
        cerr << "[SceneManager ERROR] No se pudo cargar música: " << musicPath << endl;
        return;
    }
    
    currentMusicPath = musicPath;
    sceneMusic.setLoop(true);
    sceneMusic.setVolume(70.f);
    sceneMusic.play();
    
    cout << "[SceneManager] ♪ Reproduciendo: " << musicPath << endl;
}

void SceneManager::stopMusic() {
    if (sceneMusic.getStatus() == Music::Playing) {
        sceneMusic.stop();
        cout << "[SceneManager] Música detenida" << endl;
    }
    currentMusicPath.clear();
}

void SceneManager::update(float dt) {
    if (!currentScene) return;
    currentScene->update(dt);

    // Si la escena terminó, cargar siguiente
    if (currentScene->isFinished()) {
        string next = currentScene->getNextScene();
        
        if (!next.empty()) {
            // Validar que no sea la misma escena (evitar bucle)
            if (next == currentPath) {
                cerr << "[SceneManager WARNING] Escena intenta cargarse a sí misma: " 
                     << next << endl;
                return;
            }
            
            cout << "[SceneManager] Transición: " << currentPath << " → " << next << endl;
            
            // Resolver path relativo si es necesario
            string fullPath = next;
            if (next.find('/') == string::npos && next.find('\\') == string::npos) {
                fullPath = "data/scenes/" + next;
            }
            
            loadScene(fullPath);
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

string SceneManager::currentScenePath() const { 
    return currentPath; 
}