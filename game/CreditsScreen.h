#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "src/core/ResourceManager.h"
using namespace sf;
using namespace std;

class CreditsScreen {
public:
    CreditsScreen(ResourceManager& res, Vector2u winSize);
    void handleEvent(const Event& ev);
    void update(float dt);
    void draw(RenderWindow& window);
    bool backRequested() const;
    void reset();

private:
    RectangleShape background;
    Font* font;
    Font* titleFont;
    Text title;
    vector<Text> lines;
    Text hint;
    bool back = false;
};
