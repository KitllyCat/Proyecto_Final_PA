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
    void update(float dt, const RenderWindow& window);
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
	    Texture* normal = nullptr;
	    Texture* hover = nullptr;
	    Texture* disabled = nullptr;
	    bool enabled = true;
	};

    ResourceManager& resources;

    Sprite background;
    Sprite filter;
    Font* font;
	Text title;
	Font* titleFont;
	Text titleShadow;
    Button btnNew;
    Button btnContinue;
    Button btnCredits;
	Music menuMusic;
	
	Sprite bgSprite;
	Texture* bgFrame1;
	Texture* bgFrame2;
	
	float bgTimer = 0.f;
	float bgFrameTime = 0.8f;
	bool bgToggle = false;
	
    bool newGame = false;
    bool cont = false;
    bool credits = false;

    void setupButton(Button& btn, const string& label, Vector2f pos);
    void updateButton(Button& btn, const RenderWindow& window);
    
};
