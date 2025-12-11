// src/visualnovel/Scene.cpp
#include "Scene.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

Scene::Scene()
: resources(nullptr), currentIndex(0),
  characterAnimator(nullptr), hasCharacter(false),
  waitingChoice(false), finished(false), characterPosition(800.f, 400.f) // default pos
{
}

string Scene::dirname(const string& path) {
    size_t p = path.find_last_of("/\\");
    if (p == string::npos) return ".";
    return path.substr(0, p);
}

bool Scene::pathLooksLikeAssets(const string& p) {
    if (p.size() < 7) return false;
    string start = p.substr(0,7);
    for (auto &c : start) c = static_cast<char>(tolower((unsigned char)c));
    return (start == "assets/");
}

bool Scene::loadFromFile(const string& path, ResourceManager& res) {
    resources = &res;

    ifstream f(path, ios::binary);
    if (!f.is_open()) {
        cout << "Scene::loadFromFile -> no se pudo abrir: " << path << endl;
        return false;
    }

    // leer todo el json como string para preservar UTF-8
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    json j;
    try {
        j = json::parse(content);
    } catch (const std::exception& e) {
        cout << "Scene::loadFromFile -> error parseando JSON: " << e.what() << endl;
        return false;
    }

    basePath = dirname(path);

    // Reset state
    steps.clear();
    currentIndex = 0;
    waitingChoice = false;
    finished = false;
    nextScene.clear();
    hasCharacter = false;
    characterAnimator.reset();

    // Background (support assets/ absolute or relative)
    if (j.contains("bg")) {
        string bg = j["bg"].get<string>();
        string bgPath = bg;
        if (!pathLooksLikeAssets(bg)) bgPath = basePath + "/" + bg;
        try {
            Texture& t = resources->getTexture(bgPath);
            bgSprite.setTexture(t);
        } catch (const std::exception& e) {
            cout << "Scene::loadFromFile -> warning bg no cargado: " << bgPath << " (" << e.what() << ")\n";
        }
    }

    // Character block (optional)
    if (j.contains("character")) {
        auto c = j["character"];
        string f1 = c.value("frame1", string(""));
        string f2 = c.value("frame2", string(""));
        int fps = c.value("fps", 8);
        float posx = c.value("x", 0.0f);
        float posy = c.value("y", 0.0f);
        if (posx != 0.0f || posy != 0.0f) characterPosition = Vector2f(posx, posy);

        if (!f1.empty() && !f2.empty()) {
            string p1 = f1;
            string p2 = f2;
            if (!pathLooksLikeAssets(p1)) p1 = basePath + "/" + p1;
            if (!pathLooksLikeAssets(p2)) p2 = basePath + "/" + p2;

            try {
                Texture& t1 = resources->getTexture(p1);
                Texture& t2 = resources->getTexture(p2);
                // set sprite initial texture
                characterSprite.setTexture(t1);
                // center origin (optional)
                Vector2u s = t1.getSize();
                characterSprite.setOrigin(s.x/2.f, s.y/2.f);
                characterSprite.setPosition(characterPosition);

                // create animator
                characterAnimator = make_unique<SpriteAnimator>();
                characterAnimator->attachSprite(&characterSprite);
                characterAnimator->loadFrames(&t1, &t2);
                characterAnimator->setFPS(fps);
                characterAnimator->play();

                hasCharacter = true;
            } catch (const std::exception& e) {
                cout << "Scene::loadFromFile -> no se pudo cargar character frames: " << e.what() << endl;
                hasCharacter = false;
            }
        }
    }

    // Steps: accept "steps" or "sequence"
    json stepsArray = json::array();
    if (j.contains("steps")) stepsArray = j["steps"];
    else if (j.contains("sequence")) stepsArray = j["sequence"];

    for (auto& item : stepsArray) {
        SceneStep s;
        s.type = item.value("type", string("dialogue"));
        if (s.type == "dialogue") {
            s.speaker = item.value("speaker", string(""));
            s.text = item.value("text", string(""));
        } else if (s.type == "change_bg") {
            s.bg_path = item.value("bg", string(""));
        } else if (s.type == "transition") {
            s.effect = item.value("effect", string("fade"));
            s.duration = item.value("duration", 0.5f);
        } else if (s.type == "choice") {
            for (auto& c2 : item["choices"]) {
                SceneStep::Choice ch;
                ch.text = c2.value("text", string(""));
                ch.goto_scene = c2.value("goto", c2.value("next", string("")));
                ch.flag = c2.value("flag", string(""));
                s.choices.push_back(ch);
            }
        } else {
            s.speaker = item.value("speaker", string(""));
            s.text = item.value("text", string(""));
        }
        steps.push_back(std::move(s));
    }

    // Create DialogueBox (same as before) — uses ui sprite in assets
    Vector2f size(1700.f, 260.f);
    Vector2f pos(110.f, 780.f);
    string uiBg = "assets/images/ui/dialogue_box.png";
    dialogue = make_unique<DialogueBox>(*resources, "assets/fonts/default.ttf", size, pos, uiBg);

    // start first step
    if (!steps.empty()) startStep(steps[0]);
    else finished = true;

    return true;
}

void Scene::startStep(const SceneStep& s) {
    if (s.type == "dialogue") {
        dialogue->setDialogue(s.speaker, s.text);
    } else if (s.type == "change_bg") {
        string full = s.bg_path;
        if (!pathLooksLikeAssets(full)) full = basePath + "/" + s.bg_path;
        try {
            Texture& t = resources->getTexture(full);
            bgSprite.setTexture(t);
        } catch (const std::exception& e) {
            cout << "Scene::change_bg -> no se pudo cargar: " << full << " (" << e.what() << ")\n";
        }
        advanceStep();
    } else if (s.type == "transition") {
        advanceStep();
    } else if (s.type == "choice") {
        waitingChoice = true;
        string optText;
        for (size_t i = 0; i < s.choices.size(); ++i) {
            optText += to_string(i+1) + ". " + s.choices[i].text + "\n";
        }
        dialogue->setDialogue("Elige", optText);
    } else {
        advanceStep();
    }
}

void Scene::advanceStep() {
    if (currentIndex + 1 < steps.size()) {
        currentIndex++;
        startStep(steps[currentIndex]);
    } else {
        finished = true;
    }
}

void Scene::update(float dt) {
    if (dialogue) dialogue->update(dt);
    if (hasCharacter && characterAnimator) characterAnimator->update(dt);
}

void Scene::handleEvent(const Event& ev) {
    if (dialogue) dialogue->handleEvent(ev);

    if (waitingChoice && dialogue->isWaitingForAdvance()) {
        if (ev.type == Event::KeyPressed) {
            int choice = -1;
            if (ev.key.code >= Keyboard::Num0 && ev.key.code <= Keyboard::Num9) {
                choice = ev.key.code - Keyboard::Num0;
            } else if (ev.key.code >= Keyboard::Numpad0 && ev.key.code <= Keyboard::Numpad9) {
                choice = ev.key.code - Keyboard::Numpad0;
            }
            if (choice >= 1) {
                SceneStep& s = steps[currentIndex];
                if (choice - 1 < (int)s.choices.size()) {
                    string gotoScene = s.choices[choice-1].goto_scene;
                    if (!gotoScene.empty()) {
                        if (!pathLooksLikeAssets(gotoScene) &&
                            gotoScene.find("data/") == string::npos &&
                            gotoScene.find('/') == string::npos &&
                            gotoScene.find('\\') == string::npos) {
                            nextScene = string("data/scenes/") + gotoScene;
                        } else {
                            nextScene = gotoScene;
                        }
                    }
                    waitingChoice = false;
                    dialogue->advance();
                    finished = true;
                }
            }
        }
    } else {
        if (dialogue && dialogue->isIdle() && !finished) {
            advanceStep();
        }
    }
}

void Scene::draw(RenderWindow& window) {
    // draw bg
    window.draw(bgSprite);

    // draw character if exists
    if (hasCharacter) {
        window.draw(characterSprite);
    }

    // draw dialogue UI on top
    if (dialogue) dialogue->draw(window);
}

bool Scene::isFinished() const { return finished; }
string Scene::getNextScene() const { return nextScene; }

void Scene::reset() {
    currentIndex = 0;
    nextScene.clear();
    waitingChoice = false;
    finished = false;
    if (!steps.empty()) startStep(steps[0]);
}
