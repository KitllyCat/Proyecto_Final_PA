#include "TransitionManager.h"
#include <iostream>

TransitionManager::TransitionManager()
: currentType(Type::NONE),
  duration(1.0f),
  timer(0.0f),
  active(false),
  completed(false)
{
    // Configurar el rectángulo negro
    fadeRect.setFillColor(Color(0, 0, 0, 0));  // Negro transparente inicial
    fadeRect.setSize(Vector2f(1920, 1080));    // Tamaño por defecto
    fadeRect.setPosition(0, 0);
}

void TransitionManager::setScreenSize(Vector2u size) {
    fadeRect.setSize(Vector2f(size.x, size.y));
}

void TransitionManager::start(Type type, float dur) {
    if (type == Type::NONE) {
        active = false;
        completed = true;
        return;
    }

    currentType = type;
    duration = dur;
    timer = 0.0f;
    active = true;
    completed = false;

    cout << "[Transition] Iniciando... "<< endl;
}

void TransitionManager::update(float dt) {
    if (!active || completed) return;

    timer += dt;

    if (timer >= duration) {
        timer = duration;
        completed = true;
        active = false;
        
        cout << "[Transition] Completada" << endl;
    }

    // Actualizar el alpha del rectángulo
    Uint8 alpha = calculateAlpha();
    fadeRect.setFillColor(Color(0, 0, 0, alpha));
}

void TransitionManager::draw(RenderWindow& window) {
    if (!active && !completed) return;
    
    // Si está activa o recién completada, dibujar
    if (active || (completed && currentType == Type::FADE_TO_BLACK)) {
        window.draw(fadeRect);
    }
}

bool TransitionManager::isComplete() const {
    return completed;
}

bool TransitionManager::isActive() const {
    return active;
}

void TransitionManager::complete() {
    if (!active) return;
    
    timer = duration;
    completed = true;
    active = false;
    
    // Configurar alpha final
    Uint8 finalAlpha = (currentType == Type::FADE_TO_BLACK) ? 255 : 0;
    fadeRect.setFillColor(Color(0, 0, 0, finalAlpha));
}

void TransitionManager::reset() {
    currentType = Type::NONE;
    active = false;
    completed = false;
    timer = 0.0f;
    fadeRect.setFillColor(Color(0, 0, 0, 0));
    cout << "[Transition] Reset a transparente" << endl;
}

Uint8 TransitionManager::calculateAlpha() const {
    if (duration <= 0.0f) return 0;

    float progress = timer / duration;  // 0.0 a 1.0

    if (currentType == Type::FADE_TO_BLACK) {
        // De transparente (0) a opaco (255)
        return static_cast<Uint8>(progress * 255.0f);
    } 
    else if (currentType == Type::FADE_FROM_BLACK) {
        // De opaco (255) a transparente (0)
        return static_cast<Uint8>((1.0f - progress) * 255.0f);
    }

    return 0;
}