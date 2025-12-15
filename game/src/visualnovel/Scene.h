#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "json.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../core/ResourceManager.h"
#include "DialogueBox.h"
#include "../graphics/SpriteAnimator.hpp"
#include "../graphics/TransitionManager.h"
#include "../save/SaveManager.h"

using namespace std;
using namespace sf;
using json = nlohmann::json;

typedef function<void(const string&)> MusicChangeCallback;

// ÚNICO struct SceneStep con Choice ACTUALIZADA
struct SceneStep {
    string type;
    string speaker;
    string text;
    string bg_path;
    string music_path;
    string sfx_path;
    float sfx_volume;
    string effect;
    float duration;
    
    // Choice con TODOS los campos necesarios
    struct Choice { 
        string text;
        string goto_scene;
        int goto_step;
        string flag;
        string require_flag;
    };
    
    vector<Choice> choices;
};

class Scene {
public:
    Scene();
    bool loadFromFile(const string& path, ResourceManager& res, int startIndex=0);

    void setMusicChangeCallback(MusicChangeCallback callback);
    void setScreenSize(Vector2u size);

    void update(float dt);
    void handleEvent(const Event& ev);
    void draw(RenderWindow& window);

    bool isFinished() const;
    string getNextScene() const;
    void reset();

private:
    ResourceManager* resources;
    vector<SceneStep> steps;
    size_t currentIndex;

    // Background
    Sprite bgSprite;

    // Character (optional)
    Sprite characterSprite;
    unique_ptr<SpriteAnimator> characterAnimator;
    Vector2f characterPosition;
    bool hasCharacter;

    // Dialogue
    unique_ptr<DialogueBox> dialogue;

    // Control
    bool waitingChoice;
    bool finished;
    string nextScene;
    string basePath;
    string scenePath;
    
    // Callback para música
    MusicChangeCallback onMusicChange;
    
    // SFX
    vector<Sound> activeSounds;
    vector<SoundBuffer*> soundBuffers;
    
    // Sistema de transiciones
    TransitionManager transition;
    bool waitingTransition;

    // Helpers
    void startStep(const SceneStep& s);
    void advanceStep();
    string dirname(const string& path);
    
    void playSFX(const string& path, float volume = 100.f);
    void cleanupFinishedSounds();

    static bool pathLooksLikeAssets(const string& p);
};

#endif