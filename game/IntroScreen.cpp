#include "IntroScreen.h"
#include <iostream>

IntroScreen::IntroScreen(ResourceManager& res, Vector2u windowSize)
: resources(res),
  screenSize(windowSize),
  currentSlideIndex(0),
  currentState(State::FADE_IN),
  timer(0.0f),
  fadeDuration(1.0f),
  currentAlpha(0),
  finished(false),
  skipped(false)
{
    background.setSize(Vector2f(screenSize.x, screenSize.y));
    background.setPosition(0, 0);
}

void IntroScreen::addLogo(const string& imagePath, float displayDuration, const Color& bgColor) {
    LogoSlide slide;
    slide.imagePath = imagePath;
    slide.displayDuration = displayDuration;
    slide.backgroundColor = bgColor;
    slide.textureLoaded = false;
    
    slides.push_back(slide);
}

void IntroScreen::loadSlideTexture(LogoSlide& slide) {
    if (slide.textureLoaded){return;}
    try {
        Texture& tex = resources.getTexture(slide.imagePath);
        slide.sprite.setTexture(tex);
        //Centrar el sprite
        FloatRect bounds = slide.sprite.getLocalBounds();
        slide.sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        slide.sprite.setPosition(screenSize.x / 2.0f, screenSize.y / 2.0f);
        slide.textureLoaded = true;
    }
    catch (const exception& e) {
        cerr << "[IntroScreen] ERROR al cargar: " << slide.imagePath << " - " << e.what() << endl;
        slide.textureLoaded = false;
    }
}

void IntroScreen::update(float dt) {
    if (finished || skipped || slides.empty()) {
        finished = true;
        return;
    }
    if (currentSlideIndex >= slides.size()) {
        finished = true;
        return;
    }
    auto& currentSlide = slides[currentSlideIndex];
    //Cargar textura si no está cargada
    if (!currentSlide.textureLoaded) {
        loadSlideTexture(currentSlide);
    }
    timer += dt;
    switch (currentState) {
        case State::FADE_IN: {
            //Fade in durante fadeDuration en segundos
            float progress = timer / fadeDuration;
            if (progress >= 1.0f) {
                currentAlpha = 255;
                currentState = State::DISPLAY;
                timer = 0.0f;
            } else {
                currentAlpha = static_cast<Uint8>(progress * 255.0f);
            }
            break;
        }
        case State::DISPLAY: {
            //Mostrar durante displayDuration en segundos
            currentAlpha = 255;
            if (timer >= currentSlide.displayDuration) {
                currentState = State::FADE_OUT;
                timer = 0.0f;
            }
            break;
        }
        case State::FADE_OUT: {
            //Fade out durante fadeDuration en segundos
            float progress = timer / fadeDuration;
            if (progress >= 1.0f) {
                currentAlpha = 0;
                nextSlide();
            } else {
                currentAlpha = static_cast<Uint8>((1.0f - progress) * 255.0f);
            }
            break;
        }
        case State::FINISHED: {
            finished = true;
            break;
        }
    }
    //Actualizar el alpha del sprite
    if (currentSlide.textureLoaded) {
        currentSlide.sprite.setColor(Color(255, 255, 255, currentAlpha));
    }
}

void IntroScreen::draw(RenderWindow& window) {
    if (finished || slides.empty()){return;}
    if (currentSlideIndex >= slides.size()){return;}
    auto& currentSlide = slides[currentSlideIndex];
    //Dibujar background con el color del slide actual
    background.setFillColor(currentSlide.backgroundColor);
    window.draw(background);
    //Dibujar sprite si está cargado
    if (currentSlide.textureLoaded) {
        window.draw(currentSlide.sprite);
    }
}

bool IntroScreen::isFinished() const {
    return finished || skipped;
}

void IntroScreen::handleEvent(const Event& ev) {
    //Permitir saltar con cualquier tecla, click o Escape
    if (ev.type == Event::KeyPressed || 
        ev.type == Event::MouseButtonPressed) {
        skip();
    }
}

void IntroScreen::nextSlide() {
    currentSlideIndex++;
    if (currentSlideIndex >= slides.size()) {
        currentState = State::FINISHED;
        finished = true;
    } else {
        currentState = State::FADE_IN;
        timer = 0.0f;
    }
}

void IntroScreen::skip() {
    if (skipped) return;
    skipped = true;
    finished = true;
    cout << "[System] La Introduccion fue omitida!!!" << endl;
}
