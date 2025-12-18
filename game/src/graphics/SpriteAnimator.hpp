#ifndef SPRITE_ANIMATOR_HPP
#define SPRITE_ANIMATOR_HPP

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class SpriteAnimator {
private:
    const Texture* texA;
    const Texture* texB;
    Sprite* sprite;
    float frameDuration;
    float timer;
    bool usingA;
    bool playing;

public:
    SpriteAnimator();

    void attachSprite(Sprite* s);
    void loadFrames(const Texture* a, const Texture* b);
    void setFPS(int fps);

    void play();
    void stop();
    void update(float dt);
};

#endif