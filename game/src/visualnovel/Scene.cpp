/*
 * Scene.cpp
 * Implementación de la clase Scene
 */

#include "Scene.h"
#include <iostream>

using namespace std;
using namespace sf;

// ========================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================

Scene::Scene()
    : currentDialogueIndex(0),
      resourcesLoaded(false),
      musicPlaying(false)
{
}

Scene::~Scene() {
    unloadResources();
}

// ========================================
// CONFIGURACIÓN
// ========================================

void Scene::setId(const string& sceneId) {
    id = sceneId;
}

void Scene::setBackgroundPath(const string& path) {
    backgroundPath = path;
}

void Scene::setMusicPath(const string& path) {
    musicPath = path;
}

// ========================================
// PERSONAJES
// ========================================

void Scene::addCharacter(const CharacterData& character) {
    characters.push_back(character);
}

void Scene::removeCharacter(const string& characterId) {
    for (auto it = characters.begin(); it != characters.end(); ++it) {
        if (it->id == characterId) {
            characters.erase(it);
            break;
        }
    }
}

void Scene::updateCharacterExpression(const string& characterId, const string& frame1, const string& frame2) {
    for (auto& character : characters) {
        if (character.id == characterId) {
            character.animator.loadFrames(frame1, frame2);
            cout << "[Scene] Expresion actualizada: " << characterId << endl;
            break;
        }
    }
}

// ========================================
// DIÁLOGOS
// ========================================

void Scene::addDialogue(const DialogueData& dialogue) {
    dialogues.push_back(dialogue);
}

void Scene::clearDialogues() {
    dialogues.clear();
    currentDialogueIndex = 0;
}

bool Scene::hasMoreDialogues() const {
    return currentDialogueIndex < dialogues.size();
}

DialogueData Scene::getCurrentDialogue() const {
    if (currentDialogueIndex < dialogues.size()) {
        return dialogues[currentDialogueIndex];
    }
    return DialogueData{"", "", ""};
}

void Scene::advanceDialogue() {
    if (currentDialogueIndex < dialogues.size()) {
        currentDialogueIndex++;
    }
}

void Scene::resetDialogues() {
    currentDialogueIndex = 0;
}

// ========================================
// OPCIONES/CHOICES
// ========================================

void Scene::addChoice(const ChoiceData& choice) {
    choices.push_back(choice);
}

void Scene::clearChoices() {
    choices.clear();
}

bool Scene::hasChoices() const {
    return !choices.empty();
}

vector<ChoiceData> Scene::getChoices() const {
    return choices;
}

// ========================================
// CARGA DE RECURSOS
// ========================================

bool Scene::loadResources() {
    cout << "[Scene] Cargando recursos para escena: " << id << endl;
    
    // Cargar background
    if (!backgroundPath.empty()) {
        if (!backgroundTexture.loadFromFile(backgroundPath)) {
            cerr << "[Scene ERROR] No se pudo cargar background: " << backgroundPath << endl;
            return false;
        }
        backgroundSprite.setTexture(backgroundTexture);
        cout << "[Scene] Background cargado: " << backgroundPath << endl;
    }
    
    // Cargar música
    if (!musicPath.empty()) {
        if (!sceneMusic.openFromFile(musicPath)) {
            cerr << "[Scene WARNING] No se pudo cargar música: " << musicPath << endl;
            // No es crítico, continuar
        } else {
            sceneMusic.setLoop(true);
            cout << "[Scene] Musica cargada: " << musicPath << endl;
        }
    }
    
    // Cargar personajes
    for (auto& character : characters) {
        if (!character.animator.loadFrames(character.frame1Path, character.frame2Path)) {
            cerr << "[Scene ERROR] No se pudo cargar personaje: " << character.id << endl;
            return false;
        }
        
        character.animator.setScale(character.scale, character.scale);
        character.animator.setPosition(character.position);
        character.animator.setFlipHorizontal(character.flipHorizontal);
        character.animator.play();
        
        cout << "[Scene] Personaje cargado: " << character.id << endl;
    }
    
    resourcesLoaded = true;
    cout << "[Scene] Recursos cargados correctamente" << endl;
    return true;
}

void Scene::unloadResources() {
    stopMusic();
    characters.clear();
    resourcesLoaded = false;
    cout << "[Scene] Recursos descargados" << endl;
}

// ========================================
// MÚSICA
// ========================================

void Scene::playMusic() {
    if (!musicPath.empty() && sceneMusic.getStatus() != Music::Playing) {
        sceneMusic.play();
        musicPlaying = true;
        cout << "[Scene] Musica iniciada" << endl;
    }
}

void Scene::stopMusic() {
    if (musicPlaying) {
        sceneMusic.stop();
        musicPlaying = false;
    }
}

void Scene::setMusicVolume(float volume) {
    sceneMusic.setVolume(volume);
}

// ========================================
// GETTERS
// ========================================

string Scene::getId() const {
    return id;
}

bool Scene::isLoaded() const {
    return resourcesLoaded;
}

// ========================================
// ACTUALIZACIÓN Y RENDERIZADO
// ========================================

void Scene::update(float deltaTime) {
    // Actualizar animaciones de personajes
    for (auto& character : characters) {
        character.animator.update(deltaTime);
    }
}

void Scene::draw(RenderWindow& window) {
    // Dibujar background
    if (resourcesLoaded && backgroundTexture.getSize().x > 0) {
        window.draw(backgroundSprite);
    }
    
    // Dibujar personajes
    for (auto& character : characters) {
        window.draw(character.animator);
    }
}
