/*
 * SceneManager.cpp
 * Implementación del gestor de escenas
 */

#include "SceneManager.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace sf;

// ========================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================

SceneManager::SceneManager()
    : dialogueBox(nullptr),
      sceneLoaded(false),
      waitingForInput(false),
      showingChoices(false),
      textSpeed(50.0f),
      musicVolume(80.0f),
      scenesBasePath("data/storydata/scenes/")
{
    cout << "[SceneManager] Inicializado" << endl;
}

SceneManager::~SceneManager() {
    unloadCurrentScene();
}

// ========================================
// CARGA DE ESCENAS
// ========================================

bool SceneManager::loadSceneFromJSON(const string& jsonPath) {
    cout << "[SceneManager] Cargando escena desde: " << jsonPath << endl;
    
    // Leer archivo JSON
    ifstream file(jsonPath);
    if (!file.is_open()) {
        cerr << "[SceneManager ERROR] No se pudo abrir: " << jsonPath << endl;
        return false;
    }
    
    json sceneData;
    try {
        file >> sceneData;
    } catch (const exception& e) {
        cerr << "[SceneManager ERROR] Error al parsear JSON: " << e.what() << endl;
        return false;
    }
    
    return parseSceneJSON(sceneData);
}

bool SceneManager::loadSceneById(const string& sceneId) {
    string fullPath = scenesBasePath + sceneId + ".json";
    return loadSceneFromJSON(fullPath);
}

void SceneManager::unloadCurrentScene() {
    if (currentScene) {
        currentScene->unloadResources();
        currentScene.reset();
    }
    sceneLoaded = false;
    waitingForInput = false;
    showingChoices = false;
}

// ========================================
// PARSEO DE JSON
// ========================================

bool SceneManager::parseSceneJSON(const json& sceneData) {
    try {
        // Crear nueva escena
        currentScene = make_unique<Scene>();
        
        // ID de la escena
        if (sceneData.contains("id")) {
            currentScene->setId(sceneData["id"]);
        }
        
        // Background
        if (sceneData.contains("background")) {
            string bgPath = "assets/images/backgrounds/" + sceneData["background"].get<string>();
            currentScene->setBackgroundPath(bgPath);
        }
        
        // Música
        if (sceneData.contains("music")) {
            string musicPath = "assets/audio/music/" + sceneData["music"].get<string>();
            currentScene->setMusicPath(musicPath);
        }
        
        // Personajes
        if (sceneData.contains("characters")) {
            for (const auto& charData : sceneData["characters"]) {
                CharacterData character;
                character.id = charData["id"];
                
                // Sprites
                string spritesPath = "assets/images/characters/" + character.id + "/";
                character.frame1Path = spritesPath + charData["sprites"][0].get<string>();
                character.frame2Path = spritesPath + charData["sprites"][1].get<string>();
                
                // Posición
                character.position.x = charData["position"][0];
                character.position.y = charData["position"][1];
                
                // Escala
                character.scale = charData.value("scale", 6.0f);
                
                // Flip
                character.flipHorizontal = charData.value("flip", false);
                
                currentScene->addCharacter(character);
            }
        }
        
        // Diálogos
        if (sceneData.contains("dialogues")) {
            for (const auto& dialogueData : sceneData["dialogues"]) {
                DialogueData dialogue;
                dialogue.characterName = dialogueData["character"];
                dialogue.text = dialogueData["text"];
                dialogue.characterExpression = dialogueData.value("expression", "");
                
                currentScene->addDialogue(dialogue);
            }
        }
        
        // Opciones/Choices
        if (sceneData.contains("choices")) {
            for (const auto& choiceData : sceneData["choices"]) {
                ChoiceData choice;
                choice.text = choiceData["text"];
                choice.nextScene = choiceData["next_scene"];
                choice.flag = choiceData.value("flag", "");
                
                currentScene->addChoice(choice);
            }
        }
        
        // Cargar recursos de la escena
        if (!currentScene->loadResources()) {
            cerr << "[SceneManager ERROR] Fallo al cargar recursos de la escena" << endl;
            return false;
        }
        
        // Iniciar música
        currentScene->playMusic();
        currentScene->setMusicVolume(musicVolume);
        
        // Cargar primer diálogo
        loadNextDialogue();
        
        sceneLoaded = true;
        cout << "[SceneManager] Escena cargada: " << currentScene->getId() << endl;
        return true;
        
    } catch (const exception& e) {
        cerr << "[SceneManager ERROR] Error al parsear escena: " << e.what() << endl;
        return false;
    }
}

// ========================================
// CONTROL DE DIÁLOGOS
// ========================================

void SceneManager::advanceDialogue() {
    if (!dialogueBox || !currentScene) return;
    
    // Si el texto no está completo, mostrarlo todo
    if (!dialogueBox->isTextComplete()) {
        dialogueBox->skipToEnd();
        return;
    }
    
    // Avanzar al siguiente diálogo
    currentScene->advanceDialogue();
    
    if (currentScene->hasMoreDialogues()) {
        loadNextDialogue();
    } else {
        // No hay más diálogos, verificar si hay opciones
        if (currentScene->hasChoices()) {
            showChoices();
        } else {
            waitingForInput = true;
            cout << "[SceneManager] Fin de la escena" << endl;
        }
    }
}

void SceneManager::skipDialogue() {
    if (dialogueBox) {
        dialogueBox->skipToEnd();
    }
}

bool SceneManager::isDialogueComplete() const {
    return dialogueBox ? dialogueBox->isTextComplete() : false;
}

bool SceneManager::hasMoreDialogues() const {
    return currentScene ? currentScene->hasMoreDialogues() : false;
}

// ========================================
// MÉTODOS INTERNOS
// ========================================

void SceneManager::loadNextDialogue() {
    if (!dialogueBox || !currentScene) return;
    
    DialogueData dialogue = currentScene->getCurrentDialogue();
    
    dialogueBox->setCharacterName(dialogue.characterName);
    dialogueBox->setText(dialogue.text);
    dialogueBox->show();
    
    // TODO: Cambiar expresión del personaje si está especificada
    if (!dialogue.characterExpression.empty()) {
        // currentScene->updateCharacterExpression(...);
    }
    
    waitingForInput = false;
}

void SceneManager::showChoices() {
    showingChoices = true;
    cout << "[SceneManager] Mostrando opciones" << endl;
    
    // TODO: Implementar UI de opciones
    // Por ahora, las choices se pueden obtener con getChoices()
}

// ========================================
// CONTROL DE ESCENAS
// ========================================

void SceneManager::nextScene(const string& nextSceneId) {
    cout << "[SceneManager] Cambiando a escena: " << nextSceneId << endl;
    unloadCurrentScene();
    loadSceneById(nextSceneId);
}

string SceneManager::getCurrentSceneId() const {
    return currentScene ? currentScene->getId() : "";
}

// ========================================
// OPCIONES/CHOICES
// ========================================

bool SceneManager::hasChoices() const {
    return currentScene ? currentScene->hasChoices() : false;
}

vector<ChoiceData> SceneManager::getChoices() const {
    return currentScene ? currentScene->getChoices() : vector<ChoiceData>();
}

void SceneManager::selectChoice(int choiceIndex) {
    if (!currentScene) return;
    
    vector<ChoiceData> choices = currentScene->getChoices();
    if (choiceIndex >= 0 && choiceIndex < choices.size()) {
        ChoiceData selectedChoice = choices[choiceIndex];
        cout << "[SceneManager] Opcion seleccionada: " << selectedChoice.text << endl;
        
        // Cargar siguiente escena
        nextScene(selectedChoice.nextScene);
        showingChoices = false;
    }
}

// ========================================
// CONFIGURACIÓN
// ========================================

void SceneManager::setDialogueBox(DialogueBox* dialogueBoxPtr) {
    dialogueBox = dialogueBoxPtr;
}

void SceneManager::setTextSpeed(float speed) {
    textSpeed = speed;
    if (dialogueBox) {
        dialogueBox->setTextSpeed(speed);
    }
}

void SceneManager::setMusicVolume(float volume) {
    musicVolume = volume;
    if (currentScene) {
        currentScene->setMusicVolume(volume);
    }
}

// ========================================
// ACTUALIZACIÓN
// ========================================

void SceneManager::update(float deltaTime) {
    if (currentScene) {
        currentScene->update(deltaTime);
    }
    
    if (dialogueBox) {
        dialogueBox->update(deltaTime);
    }
}

void SceneManager::handleInput(const Event& event) {
    if (event.type == Event::KeyPressed) {
        // ENTER o SPACE para avanzar diálogo
        if (event.key.code == Keyboard::Return || event.key.code == Keyboard::Space) {
            if (!showingChoices) {
                advanceDialogue();
            }
        }
        
        // S para skip
        if (event.key.code == Keyboard::S) {
            skipDialogue();
        }
        
        // Números para seleccionar opciones
        if (showingChoices) {
            if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num9) {
                int choice = event.key.code - Keyboard::Num1;
                selectChoice(choice);
            }
        }
    }
}

// ========================================
// ESTADO
// ========================================

bool SceneManager::isSceneLoaded() const {
    return sceneLoaded;
}

bool SceneManager::isWaitingForInput() const {
    return waitingForInput;
}

// ========================================
// RENDERIZADO
// ========================================

void SceneManager::draw(RenderTarget& target, RenderStates states) const {
    // Dibujar escena actual (background + personajes)
    if (currentScene) {
        currentScene->draw(static_cast<RenderWindow&>(target));
    }
    
    // DialogueBox se dibuja por separado en main.cpp
}
