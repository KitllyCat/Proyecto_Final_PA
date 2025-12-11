#include "TransitionEffects.h"
#include <algorithm>

TransitionEffects::TransitionEffects()
: state(State::Idle), duration(0.f), timer(0.f), baseColor(Color::Black), callback(nullptr)
{
    mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, 0));
    // tamaño del mask se ajusta en draw() usando window size
}

void TransitionEffects::startFadeOut(float dur, function<void()> onFinished) {
    if (dur <= 0.f) dur = 0.01f;
    state = State::FadingOut;
    duration = dur;
    timer = 0.f;
    callback = onFinished;
    // init alpha 0
    mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, 0));
}

void TransitionEffects::startFadeIn(float dur, function<void()> onFinished) {
    if (dur <= 0.f) dur = 0.01f;
    state = State::FadingIn;
    duration = dur;
    timer = 0.f;
    callback = onFinished;
    // init alpha 255
    mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, 255));
}

void TransitionEffects::update(float dt) {
    if (state == State::Idle) return;
    timer += dt;
    float t = timer / duration;
    if (t > 1.f) t = 1.f;

    if (state == State::FadingOut) {
        // alpha 0 -> 255
        Uint8 a = static_cast<Uint8>(std::round(255.f * t));
        mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, a));
        if (t >= 1.f) {
            // finished fade out
            state = State::Idle;
            callCallback();
        }
    } else if (state == State::FadingIn) {
        // alpha 255 -> 0
        Uint8 a = static_cast<Uint8>(std::round(255.f * (1.f - t)));
        mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, a));
        if (t >= 1.f) {
            // finished fade in
            state = State::Idle;
            callCallback();
        }
    }
}

void TransitionEffects::draw(RenderWindow& window) {
    if (state == State::Idle) {
        // still may want to draw if mask has non-zero alpha? we'll only draw when active
    }
    // Ajustar tamaño para cubrir la ventana
    Vector2u ws = window.getSize();
    mask.setSize(Vector2f(static_cast<float>(ws.x), static_cast<float>(ws.y)));
    mask.setPosition(0.f, 0.f);

    // Dibujar solo si alpha > 0 o está activo
    if (mask.getFillColor().a > 0) {
        window.draw(mask);
    }
}

bool TransitionEffects::isActive() const {
    return state != State::Idle;
}

bool TransitionEffects::isFadingOut() const {
    return state == State::FadingOut;
}

bool TransitionEffects::isFadingIn() const {
    return state == State::FadingIn;
}

void TransitionEffects::setColor(const Color& c) {
    baseColor = c;
    Color cur = mask.getFillColor();
    mask.setFillColor(Color(baseColor.r, baseColor.g, baseColor.b, cur.a));
}

void TransitionEffects::callCallback() {
    if (callback) {
        // move/call and clear to avoid reentrancy issues
        auto cb = callback;
        callback = nullptr;
        cb();
    }
}
