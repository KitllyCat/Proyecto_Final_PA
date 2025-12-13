// src/visualnovel/Scene.cpp
#include "Scene.h"
#include "../save/SaveManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

Scene::Scene()
: resources(nullptr),
  currentIndex(0),
  characterAnimator(nullptr),
  hasCharacter(false),
  characterPosition(800.f, 400.f),
  waitingChoice(false),
  finished(false)
{
}

string Scene::dirname(const string& path) {
    size_t p = path.find_last_of("/\\");
    if (p == string::npos) return ".";
    return path.substr(0, p);
}

bool Scene::pathLooksLikeAssets(const string& p) {
    if (p.size() < 7) return false;
    string start = p.substr(0, 7);
    for (auto& c : start) c = tolower(c);
    return start == "assets/";
}

bool Scene::loadFromFile(const string& path, ResourceManager& res, int startIndex){
    resources = &res;
    scenePath = path;

    ifstream f(path, ios::binary);
    if (!f.is_open()) {
        cout << "Scene: no se pudo abrir " << path << endl;
        return false;
    }

    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    json j = json::parse(content);
    basePath = dirname(path);

    steps.clear();
    currentIndex = startIndex;
    waitingChoice = false;
    finished = false;
    nextScene.clear();

    if (j.contains("bg")) {
        string bg = j["bg"];
        string full = pathLooksLikeAssets(bg) ? bg : basePath + "/" + bg;
        bgSprite.setTexture(resources->getTexture(full));
    }
    
	hasCharacter = false;
	characterAnimator.reset();
	
	if (j.contains("character")) {
	    auto c = j["character"];
	
	    string f1 = c.value("frame1", "");
	    string f2 = c.value("frame2", "");
	    int fps = c.value("fps", 8);
	
	    float x = c.value("x", characterPosition.x);
	    float y = c.value("y", characterPosition.y);
	    characterPosition = { x, y };
	
	    if (!f1.empty() && !f2.empty()) {
	        string p1 = pathLooksLikeAssets(f1) ? f1 : basePath + "/" + f1;
	        string p2 = pathLooksLikeAssets(f2) ? f2 : basePath + "/" + f2;
	
	        try {
	            Texture& t1 = resources->getTexture(p1);
	            Texture& t2 = resources->getTexture(p2);
	
	            characterSprite.setTexture(t1);
	            auto s = t1.getSize();
	            characterSprite.setOrigin(s.x / 2.f, s.y / 2.f);
	            characterSprite.setPosition(characterPosition);
	
	            characterAnimator = make_unique<SpriteAnimator>();
	            characterAnimator->attachSprite(&characterSprite);
	            characterAnimator->loadFrames(&t1, &t2);
	            characterAnimator->setFPS(fps);
	            characterAnimator->play();
	
	            hasCharacter = true;
	        }
	        catch (...) {
	            hasCharacter = false;
	        }
	    }
	}
	
    json arr = j.contains("steps") ? j["steps"] : j["sequence"];
    for (auto& item : arr) {
        SceneStep s;
        s.type = item.value("type", "dialogue");

        if (s.type == "dialogue") {
            s.speaker = item.value("speaker", "");
            s.text = item.value("text", "");
        }
        else if (s.type == "change_bg") {
            s.bg_path = item.value("bg", "");
        }
        else if (s.type == "choice") {
            for (auto& c : item["choices"]) {
                SceneStep::Choice ch;
                ch.text = c.value("text", "");
                ch.goto_scene = c.value("goto", "");
                s.choices.push_back(ch);
            }
        }
        // checkpoint no necesita datos
        steps.push_back(s);
    }

    dialogue = make_unique<DialogueBox>(
        *resources,
        "assets/fonts/default.ttf",
        Vector2f(1700.f, 260.f),
        Vector2f(110.f, 780.f),
        "assets/images/ui/dialogue_box.png"
    );
	currentIndex = startIndex;
    if (currentIndex < steps.size())
        startStep(steps[currentIndex]);
    else
        finished = true;

    return true;
}

void Scene::startStep(const SceneStep& s) {
    if (s.type == "checkpoint") {
        string id = scenePath.substr(scenePath.find_last_of("/\\") + 1);
        id = id.substr(0, id.find(".json"));

        SaveManager::getInstance().save(id, currentIndex);
        advanceStep();
        return;
    }

    if (s.type == "dialogue") {
        dialogue->setDialogue(s.speaker, s.text);
    }
    else if (s.type == "change_bg") {
        string full = pathLooksLikeAssets(s.bg_path)
                    ? s.bg_path
                    : basePath + "/" + s.bg_path;
        bgSprite.setTexture(resources->getTexture(full));
        advanceStep();
    }
    else if (s.type == "choice") {
        waitingChoice = true;
        string text;
        for (size_t i = 0; i < s.choices.size(); ++i)
            text += to_string(i + 1) + ". " + s.choices[i].text + "\n";
        dialogue->setDialogue("Elige", text);
    }
}

void Scene::advanceStep() {
    if (++currentIndex < steps.size())
        startStep(steps[currentIndex]);
    else
        finished = true;
}

void Scene::update(float dt) {
    if (dialogue) dialogue->update(dt);
    if (hasCharacter && characterAnimator)
        characterAnimator->update(dt);
}

void Scene::handleEvent(const Event& ev) {
    if (dialogue) dialogue->handleEvent(ev);

    if (waitingChoice && dialogue->isWaitingForAdvance()) {
        if (ev.type == Event::KeyPressed) {
            int n = ev.key.code - Keyboard::Num1;
            if (n >= 0 && n < (int)steps[currentIndex].choices.size()) {
                nextScene = steps[currentIndex].choices[n].goto_scene;
                waitingChoice = false;
                finished = true;
            }
        }
    }
    else if (dialogue && dialogue->isIdle() && !finished) {
        advanceStep();
    }
}

void Scene::draw(RenderWindow& window) {
    window.draw(bgSprite);
    if (hasCharacter)
        window.draw(characterSprite);
    if (dialogue)
        dialogue->draw(window);
}

bool Scene::isFinished() const { return finished; }
string Scene::getNextScene() const { return nextScene; }
