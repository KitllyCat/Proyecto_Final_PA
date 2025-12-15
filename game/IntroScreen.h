#ifndef INTRO_SCREEN_H
#define INTRO_SCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "src/core/ResourceManager.h"

using namespace sf;
using namespace std;

class IntroScreen {
public:
    IntroScreen(ResourceManager& resources, Vector2u windowSize);

    // Actualizar la intro
    void update(float dt);

    // Dibujar la intro
    void draw(RenderWindow& window);

    // Verificar si la intro terminó
    bool isFinished() const;

    // Permitir saltar la intro
    void handleEvent(const Event& ev);

    // Configurar las imágenes de la intro
    void addLogo(const string& imagePath, float displayDuration, const Color& bgColor = Color::Black);

private:
    enum class State {
        FADE_IN,
        DISPLAY,
        FADE_OUT,
        FINISHED
    };

    struct LogoSlide {
        string imagePath;
        float displayDuration;
        Color backgroundColor;
        Sprite sprite;
        bool textureLoaded;
    };

    ResourceManager& resources;
    Vector2u screenSize;

    vector<LogoSlide> slides;
    size_t currentSlideIndex;
    State currentState;

    float timer;
    float fadeDuration;
    Uint8 currentAlpha;

    RectangleShape background;
    bool finished;
    bool skipped;

    void loadSlideTexture(LogoSlide& slide);
    void nextSlide();
    void skip();
};

#endif