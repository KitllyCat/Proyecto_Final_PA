/*
 * Scene.h
 * Representa una escena completa de la visual novel:
 * - Background
 * - Personajes
 * - Diálogos
 * - Música
 * - Opciones/Choices
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include "../graphics/SpriteAnimator.h"

using namespace std;
using namespace sf;

// === ESTRUCTURAS DE DATOS ===

struct CharacterData {
    string id;                          // Identificador único
    string frame1Path;                  // Ruta al frame 1
    string frame2Path;                  // Ruta al frame 2
    Vector2f position;                  // Posición en pantalla
    float scale;                        // Escala (generalmente 6)
    bool flipHorizontal;                // Voltear horizontalmente
    SpriteAnimator animator;            // Animador del personaje
};

struct DialogueData {
    string characterName;               // Nombre del personaje que habla
    string text;                        // Texto del diálogo
    string characterExpression;         // Expresión (happy, sad, etc.) - opcional
};

struct ChoiceData {
    string text;                        // Texto de la opción
    string nextScene;                   // ID de la siguiente escena
    string flag;                        // Flag que se activa (opcional)
};

// === CLASE SCENE ===

class Scene {
public:
    Scene();
    ~Scene();
    
    // === CONFIGURACIÓN ===
    void setId(const string& sceneId);
    void setBackgroundPath(const string& path);
    void setMusicPath(const string& path);
    
    // === PERSONAJES ===
    void addCharacter(const CharacterData& character);
    void removeCharacter(const string& characterId);
    void updateCharacterExpression(const string& characterId, const string& frame1, const string& frame2);
    
    // === DIÁLOGOS ===
    void addDialogue(const DialogueData& dialogue);
    void clearDialogues();
    bool hasMoreDialogues() const;
    DialogueData getCurrentDialogue() const;
    void advanceDialogue();
    void resetDialogues();
    
    // === OPCIONES/CHOICES ===
    void addChoice(const ChoiceData& choice);
    void clearChoices();
    bool hasChoices() const;
    vector<ChoiceData> getChoices() const;
    
    // === CARGA DE RECURSOS ===
    bool loadResources();               // Carga background, música, personajes
    void unloadResources();
    
    // === MÚSICA ===
    void playMusic();
    void stopMusic();
    void setMusicVolume(float volume);
    
    // === GETTERS ===
    string getId() const;
    bool isLoaded() const;
    
    // === ACTUALIZACIÓN Y RENDERIZADO ===
    void update(float deltaTime);
    void draw(RenderWindow& window);
    
private:
    // === DATOS DE LA ESCENA ===
    string id;
    string backgroundPath;
    string musicPath;
    
    // === RECURSOS CARGADOS ===
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Music sceneMusic;
    
    vector<CharacterData> characters;
    vector<DialogueData> dialogues;
    vector<ChoiceData> choices;
    
    // === ESTADO ===
    size_t currentDialogueIndex;
    bool resourcesLoaded;
    bool musicPlaying;
};
