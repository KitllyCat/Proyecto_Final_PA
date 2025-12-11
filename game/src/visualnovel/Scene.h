#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <string>
#include <vector>
#include <memory>
#include "../core/ResourceManager.h"
#include "DialogueBox.h"
#include "../graphics/SpriteAnimator.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

struct SceneStep {
    string type;
    string speaker;
    string text;
    string bg_path;
    string effect;
    float duration;
    struct Choice { string text; string goto_scene; string flag; };
    vector<Choice> choices;
};

class Scene {
public:
    Scene();
    bool loadFromFile(const string& path, ResourceManager& resources);

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

    // fondo
    Sprite bgSprite;

    // personaje animado (opcional)
    Sprite characterSprite;
    unique_ptr<SpriteAnimator> characterAnimator;
    Vector2f characterPosition; // posición para dibujar personaje
    bool hasCharacter;

    // diálogo
    unique_ptr<DialogueBox> dialogue;

    bool waitingChoice;
    bool finished;
    string nextScene;
    string basePath;

    void startStep(const SceneStep& s);
    void advanceStep();
    string dirname(const string& path);

    static bool pathLooksLikeAssets(const string& p);
};

#endif
