/*
 * SpriteAnimator.cpp
 * Implementación del sistema de animación de 2 frames
 */

#include "SpriteAnimator.h"
#include "../core/ResourceManager.h"
#include <iostream>

using namespace std;
using namespace sf;

// ========================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================

SpriteAnimator::SpriteAnimator() 
    : frame1Active(true),
      frameTime(0.5f),
      elapsedTime(0.0f),
      playing(true),
      framesLoaded(false),
      flipHorizontal(false),
      position(0, 0),
      scale(1, 1),
      color(Color::White)
{
}

SpriteAnimator::~SpriteAnimator() {
}

// ========================================
// CARGA DE FRAMES
// ========================================

bool SpriteAnimator::loadFrames(const string& frame1Path, const string& frame2Path) {
    if (!texture1.loadFromFile(frame1Path)) {
        cerr << "[SpriteAnimator ERROR] No se pudo cargar: " << frame1Path << endl;
        return false;
    }
    
    if (!texture2.loadFromFile(frame2Path)) {
        cerr << "[SpriteAnimator ERROR] No se pudo cargar: " << frame2Path << endl;
        return false;
    }
    
    // Configurar sprite con el primer frame
    sprite.setTexture(texture1);
    sprite.setPosition(position);
    sprite.setScale(scale);
    sprite.setColor(color);
    
    framesLoaded = true;
    frame1Active = true;
    
    cout << "[SpriteAnimator] Frames cargados correctamente" << endl;
    return true;
}

bool SpriteAnimator::loadFramesFromManager(const string& frame1Id, const string& frame2Id) {
    ResourceManager& rm = ResourceManager::getInstance();
    
    Texture* tex1 = rm.getTexture(frame1Id);
    Texture* tex2 = rm.getTexture(frame2Id);
    
    if (!tex1 || !tex2) {
        cerr << "[SpriteAnimator ERROR] Texturas no encontradas en ResourceManager" << endl;
        return false;
    }
    
    texture1 = *tex1;
    texture2 = *tex2;
    
    sprite.setTexture(texture1);
    sprite.setPosition(position);
    sprite.setScale(scale);
    sprite.setColor(color);
    
    framesLoaded = true;
    frame1Active = true;
    
    cout << "[SpriteAnimator] Frames cargados desde ResourceManager" << endl;
    return true;
}

// ========================================
// CONFIGURACIÓN
// ========================================

void SpriteAnimator::setFrameTime(float seconds) {
    frameTime = seconds;
}

void SpriteAnimator::setScale(float scaleX, float scaleY) {
    scale = Vector2f(scaleX, scaleY);
    sprite.setScale(scale);
}

void SpriteAnimator::setScale(float uniformScale) {
    setScale(uniformScale, uniformScale);
}

void SpriteAnimator::setPosition(float x, float y) {
    position = Vector2f(x, y);
    sprite.setPosition(position);
}

void SpriteAnimator::setPosition(const Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}

void SpriteAnimator::setOrigin(float x, float y) {
    sprite.setOrigin(x, y);
}

void SpriteAnimator::setColor(const Color& col) {
    color = col;
    sprite.setColor(color);
}

// ========================================
// CONTROL DE ANIMACIÓN
// ========================================

void SpriteAnimator::play() {
    playing = true;
}

void SpriteAnimator::pause() {
    playing = false;
}

void SpriteAnimator::stop() {
    playing = false;
    reset();
}

void SpriteAnimator::reset() {
    frame1Active = true;
    elapsedTime = 0.0f;
    if (framesLoaded) {
        sprite.setTexture(texture1);
    }
}

void SpriteAnimator::setFlipHorizontal(bool flip) {
    flipHorizontal = flip;
    
    if (flipHorizontal) {
        sprite.setScale(-abs(scale.x), scale.y);
        // Ajustar origen para que el flip funcione correctamente
        FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width, 0);
    } else {
        sprite.setScale(abs(scale.x), scale.y);
        sprite.setOrigin(0, 0);
    }
}

// ========================================
// ACTUALIZACIÓN
// ========================================

void SpriteAnimator::update(float deltaTime) {
    if (!playing || !framesLoaded) {
        return;
    }
    
    elapsedTime += deltaTime;
    
    // Cambiar de frame cuando se alcanza el frameTime
    if (elapsedTime >= frameTime) {
        elapsedTime = 0.0f;
        frame1Active = !frame1Active;
        
        // Cambiar textura
        if (frame1Active) {
            sprite.setTexture(texture1);
        } else {
            sprite.setTexture(texture2);
        }
    }
}

// ========================================
// GETTERS
// ========================================

Vector2f SpriteAnimator::getPosition() const {
    return position;
}

FloatRect SpriteAnimator::getBounds() const {
    return sprite.getGlobalBounds();
}

bool SpriteAnimator::isPlaying() const {
    return playing;
}

int SpriteAnimator::getCurrentFrame() const {
    return frame1Active ? 0 : 1;
}

// ========================================
// RENDERIZADO
// ========================================

void SpriteAnimator::draw(RenderTarget& target, RenderStates states) const {
    if (framesLoaded) {
        target.draw(sprite, states);
    }
}