#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include "src/core/ResourceManager.h"
using namespace sf;
using namespace std;

class MainMenu {
public:
    MainMenu(ResourceManager& resources, Vector2u windowSize);

    void handleEvent(const Event& ev, const RenderWindow& window);
    void update(float dt);
    void draw(RenderWindow& window);

    bool startNewGameRequested() const;
    bool continueRequested() const;
    bool creditsRequested() const;
    
    void playMusic();
    void stopMusic();

private:
    struct Button {
        Sprite sprite;
        Text text;
        bool enabled = true;
    };

    ResourceManager& resources;

    Sprite background;
    Sprite filter;
    Font* font;
	Text title;
	Font* titleFont;
    Button btnNew;
    Button btnContinue;
    Button btnCredits;
	Music menuMusic;
	
    bool newGame = false;
    bool cont = false;
    bool credits = false;

    void setupButton(Button& btn, const string& label, Vector2f pos);
    void updateButton(Button& btn, const RenderWindow& window);
    
};
