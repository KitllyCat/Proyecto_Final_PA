/*
 * ResourceManager.h
 * Gestiona la carga y acceso centralizado a recursos del juego:
 * - Texturas (sprites, backgrounds, UI)
 * - Fuentes
 * - Música y efectos de sonido
 * - Configuraciones JSON
 * 
 * Patrón Singleton: solo existe una instancia en toda la aplicación
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include "../json.hpp"
using json = nlohmann::json;

class ResourceManager {
public:
    // Singleton: obtener instancia única
    static ResourceManager& getInstance();
    
    // Eliminar constructores de copia (patrón Singleton)
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // === CARGA DE RECURSOS ===
    
    // Texturas (sprites, backgrounds, UI)
    bool loadTexture(const std::string& id, const std::string& filepath);
    sf::Texture* getTexture(const std::string& id);
    
    // Fuentes
    bool loadFont(const std::string& id, const std::string& filepath);
    sf::Font* getFont(const std::string& id);
    
    // Música (streaming, archivos grandes)
    bool loadMusic(const std::string& id, const std::string& filepath);
    sf::Music* getMusic(const std::string& id);
    
    // Efectos de sonido (cargados en memoria)
    bool loadSound(const std::string& id, const std::string& filepath);
    sf::SoundBuffer* getSoundBuffer(const std::string& id);
    
    // === CONFIGURACIÓN JSON ===
    bool loadConfig(const std::string& filepath);
    json* getConfig();
    
    // === GESTIÓN DE MEMORIA ===
    void unloadTexture(const std::string& id);
    void unloadFont(const std::string& id);
    void unloadSound(const std::string& id);
    void clearAll(); // Libera todos los recursos
    
    // === UTILIDADES ===
    bool fileExists(const std::string& filepath);
    void logError(const std::string& message);
    void logInfo(const std::string& message);
    
private:
    // Constructor privado (Singleton)
    ResourceManager();
    ~ResourceManager();
    
    // === CONTENEDORES DE RECURSOS ===
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::map<std::string, std::unique_ptr<sf::Music>> music;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    
    // Configuración del juego
    std::unique_ptr<json> gameConfig;
    
    // Sistema de logs
    bool enableLogging = true;
};