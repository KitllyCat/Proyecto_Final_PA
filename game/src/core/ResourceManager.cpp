/*
 * ResourceManager.cpp
 * Implementación del gestor de recursos
 */

#include "ResourceManager.h"
#include <fstream>
#include <sys/stat.h>

using namespace std;
using json = nlohmann::json;

// ========================================
// SINGLETON
// ========================================

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

ResourceManager::ResourceManager() {
    logInfo("ResourceManager inicializado");
}

ResourceManager::~ResourceManager() {
    clearAll();
    logInfo("ResourceManager destruido");
}

// ========================================
// TEXTURAS
// ========================================

bool ResourceManager::loadTexture(const string& id, const string& filepath) {
    if (!fileExists(filepath)) {
        logError("Textura no encontrada: " + filepath);
        return false;
    }
    
    auto texture = make_unique<sf::Texture>();
    if (!texture->loadFromFile(filepath)) {
        logError("Error al cargar textura: " + filepath);
        return false;
    }
    
    textures[id] = move(texture);
    logInfo("Textura cargada: " + id + " <- " + filepath);
    return true;
}

sf::Texture* ResourceManager::getTexture(const string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second.get();
    }
    
    logError("Textura no encontrada: " + id);
    return nullptr;
}

void ResourceManager::unloadTexture(const string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        textures.erase(it);
        logInfo("Textura descargada: " + id);
    }
}

// ========================================
// FUENTES
// ========================================

bool ResourceManager::loadFont(const string& id, const string& filepath) {
    if (!fileExists(filepath)) {
        logError("Fuente no encontrada: " + filepath);
        return false;
    }
    
    auto font = make_unique<sf::Font>();
    if (!font->loadFromFile(filepath)) {
        logError("Error al cargar fuente: " + filepath);
        return false;
    }
    
    fonts[id] = move(font);
    logInfo("Fuente cargada: " + id + " <- " + filepath);
    return true;
}

sf::Font* ResourceManager::getFont(const string& id) {
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        return it->second.get();
    }
    
    logError("Fuente no encontrada: " + id);
    return nullptr;
}

void ResourceManager::unloadFont(const string& id) {
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        fonts.erase(it);
        logInfo("Fuente descargada: " + id);
    }
}

// ========================================
// MÚSICA
// ========================================

bool ResourceManager::loadMusic(const string& id, const string& filepath) {
    if (!fileExists(filepath)) {
        logError("Música no encontrada: " + filepath);
        return false;
    }
    
    auto musicTrack = make_unique<sf::Music>();
    if (!musicTrack->openFromFile(filepath)) {
        logError("Error al cargar música: " + filepath);
        return false;
    }
    
    music[id] = move(musicTrack);
    logInfo("Música cargada: " + id + " <- " + filepath);
    return true;
}

sf::Music* ResourceManager::getMusic(const string& id) {
    auto it = music.find(id);
    if (it != music.end()) {
        return it->second.get();
    }
    
    logError("Música no encontrada: " + id);
    return nullptr;
}

// ========================================
// SONIDOS
// ========================================

bool ResourceManager::loadSound(const string& id, const string& filepath) {
    if (!fileExists(filepath)) {
        logError("Sonido no encontrado: " + filepath);
        return false;
    }
    
    auto buffer = make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filepath)) {
        logError("Error al cargar sonido: " + filepath);
        return false;
    }
    
    soundBuffers[id] = move(buffer);
    logInfo("Sonido cargado: " + id + " <- " + filepath);
    return true;
}

sf::SoundBuffer* ResourceManager::getSoundBuffer(const string& id) {
    auto it = soundBuffers.find(id);
    if (it != soundBuffers.end()) {
        return it->second.get();
    }
    
    logError("Sonido no encontrado: " + id);
    return nullptr;
}

void ResourceManager::unloadSound(const string& id) {
    auto it = soundBuffers.find(id);
    if (it != soundBuffers.end()) {
        soundBuffers.erase(it);
        logInfo("Sonido descargado: " + id);
    }
}

// ========================================
// CONFIGURACIÓN JSON
// ========================================

bool ResourceManager::loadConfig(const string& filepath) {
    if (!fileExists(filepath)) {
        logError("Configuración no encontrada: " + filepath);
        return false;
    }
    
    try {
        ifstream file(filepath);
        gameConfig = make_unique<json>(json::parse(file));
        logInfo("Configuración cargada: " + filepath);
        return true;
    } catch (const exception& e) {
        logError("Error al parsear JSON: " + string(e.what()));
        return false;
    }
}

json* ResourceManager::getConfig() {
    return gameConfig.get();
}

// ========================================
// GESTIÓN DE MEMORIA
// ========================================

void ResourceManager::clearAll() {
    textures.clear();
    fonts.clear();
    music.clear();
    soundBuffers.clear();
    gameConfig.reset();
    
    logInfo("Todos los recursos liberados");
}

// ========================================
// UTILIDADES
// ========================================

bool ResourceManager::fileExists(const string& filepath) {
    struct stat buffer;
    return (stat(filepath.c_str(), &buffer) == 0);
}

void ResourceManager::logError(const string& message) {
    if (enableLogging) {
        cerr << "[ResourceManager ERROR] " << message << endl;
    }
}

void ResourceManager::logInfo(const string& message) {
    if (enableLogging) {
        cout << "[ResourceManager] " << message << endl;
    }
}