#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "src/core/ResourceManager.h"
using namespace sf;
using namespace std;

class MainMenu {
public:
    MainMenu(ResourceManager& resources, Vector2u windowSize);

    void handleEvent(const Event& ev, const RenderWindow& window);
    void update(float dt, const RenderWindow& window);
    void draw(RenderWindow& window);
	void resetCreditsRequest();
    bool startNewGameRequested() const;
    bool continueRequested() const;
    bool creditsRequested() const;
    
    void playMusic();
    void stopMusic();

private:
    struct Button {
        Sprite sprite;
        Text text;
	    const Texture* normal;
	    const Texture* hover;
	    const Texture* disabled;
        bool enabled = true;
        bool isHovered = false;
    };

    ResourceManager& resources;
    //Fondo
    Sprite bgSprite;
    Sprite filter;
    const Texture& bgFrame1;
	const Texture& bgFrame2;
    float bgTimer = 0.f;
    float bgFrameTime = 0.8f;
    bool bgToggle = false;
    //Fuente
    Font* font = nullptr;
    //Titulo animado
    Sprite titleSprite;
	const Texture& titleFrame1;
	const Texture& titleFrame2;
    float titleTimer = 0.f;
    float titleFrameTime = 0.9f;
    bool titleToggle = false;
    //Botones
    Button btnNew;
    Button btnContinue;
    Button btnCredits;
    void setupButton(Button& btn, const string& label, Vector2f pos);
	//Musica
    Music menuMusic;
	SoundBuffer clickBuffer;
    Sound clickSound;
    void playClickSound();
	SoundBuffer hoverBuffer;
	Sound hoverSound;
	//Estados
    bool newGame = false;
    bool cont = false;
    bool credits = false;
	void clearRequests();
};
