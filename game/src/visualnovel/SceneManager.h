/*
 * SceneManager.h
 * Gestor principal de escenas para Remoria
 * - Carga escenas desde JSON
 * - Gestiona diálogos
 * - Maneja transiciones
 * - Controla el flujo de la historia
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "Scene.h"
#include "../ui/DialogueBox.h"
#include "../json.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

class SceneManager : public Drawable {
public:
    SceneManager();
    ~SceneManager();
    
    // === CARGA DE ESCENAS ===
    bool loadSceneFromJSON(const string& jsonPath);
    bool loadSceneById(const string& sceneId);      // Cargar desde data/storydata/scenes/
    void unloadCurrentScene();
    
    // === CONTROL DE DIÁLOGOS ===
    void advanceDialogue();                         // Avanzar al siguiente diálogo
    void skipDialogue();                            // Saltar texto typewriter
    bool isDialogueComplete() const;                // ¿Diálogo actual completo?
    bool hasMoreDialogues() const;                  // ¿Hay más diálogos?
    
    // === CONTROL DE ESCENAS ===
    void nextScene(const string& nextSceneId);      // Cambiar a siguiente escena
    string getCurrentSceneId() const;
    
    // === OPCIONES/CHOICES ===
    bool hasChoices() const;                        // ¿Hay opciones disponibles?
    vector<ChoiceData> getChoices() const;
    void selectChoice(int choiceIndex);             // Seleccionar una opción
    
    // === CONFIGURACIÓN ===
    void setDialogueBox(DialogueBox* dialogueBox); // Vincular DialogueBox
    void setTextSpeed(float speed);
    void setMusicVolume(float volume);
    
    // === ACTUALIZACIÓN ===
    void update(float deltaTime);
    void handleInput(const Event& event);           // Manejar eventos (ENTER, clicks)
    
    // === ESTADO ===
    bool isSceneLoaded() const;
    bool isWaitingForInput() const;
    
private:
    // Implementación de sf::Drawable
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    
    // Métodos internos
    bool parseSceneJSON(const json& sceneData);
    void loadNextDialogue();
    void showChoices();
    
    // === ESCENA ACTUAL ===
    unique_ptr<Scene> currentScene;
    DialogueBox* dialogueBox;                       // Referencia externa
    
    // === ESTADO DEL MANAGER ===
    bool sceneLoaded;
    bool waitingForInput;
    bool showingChoices;
    
    // === CONFIGURACIÓN ===
    float textSpeed;
    float musicVolume;
    
    // === RUTAS ===
    string scenesBasePath;                          // data/storydata/scenes/
};
