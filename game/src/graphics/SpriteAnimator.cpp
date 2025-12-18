#include "SpriteAnimator.hpp"

SpriteAnimator::SpriteAnimator() {
    texA = nullptr;
    texB = nullptr;
    sprite = nullptr;
    frameDuration = 1.0f / 8.0f; //Default 8 fps
    timer = 0.0f;
    usingA = true;
    playing = false;
}

void SpriteAnimator::attachSprite(Sprite* s) {
    sprite = s;
    //Si ya tenemos texA, asignarla al sprite inmediatamente
    if (sprite && texA) {
        sprite->setTexture(*texA);
    }
}

void SpriteAnimator::loadFrames(const Texture* a, const Texture* b) {
    texA = a;
    texB = b;
    if (sprite && texA) {
        sprite->setTexture(*texA);
        usingA = true;
    }
}

void SpriteAnimator::setFPS(int fps) {
	//Evita una paralizacion
    if (fps <= 0){
    	fps = 1;
	}
    frameDuration = 1.0f / static_cast<float>(fps);
}

void SpriteAnimator::play() {
    playing = true;
    //Asegura estado inicial
    timer = 0.0f;
    usingA = true;
    if (sprite && texA){
    	sprite->setTexture(*texA);
	}
}

void SpriteAnimator::stop() {
    playing = false;
}

void SpriteAnimator::update(float dt) {
    if (!playing || !sprite || !texA || !texB){return;}
    timer += dt;
    if (timer >= frameDuration) {
        timer -= frameDuration;
        usingA = !usingA;
        if (usingA){
        	sprite->setTexture(*texA);
		}else{
			sprite->setTexture(*texB);
		}
    }
}
