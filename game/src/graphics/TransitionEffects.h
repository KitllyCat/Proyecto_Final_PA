#ifndef TRANSITION_EFFECTS_H
#define TRANSITION_EFFECTS_H

#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;
using namespace sf;

class TransitionEffects {
public:
    TransitionEffects();

    // Inicia un fade out (pantalla se oscurece). duration en segundos.
    // onFinished (opcional) se llama cuando termina el fade out.
    void startFadeOut(float duration, function<void()> onFinished = nullptr);

    // Inicia un fade in (pantalla se aclara). duration en segundos.
    // onFinished (opcional) se llama cuando termina el fade in.
    void startFadeIn(float duration, function<void()> onFinished = nullptr);

    // actualizar (llamar cada frame)
    void update(float dt);

    // dibujar (llamar después de render del mundo pero antes o después de UI según necesites)
    void draw(RenderWindow& window);

    // estados
    bool isActive() const;
    bool isFadingOut() const;
    bool isFadingIn() const;

    // color de la máscara (por defecto negro). Puedes cambiar alfa manualmente si quieres.
    void setColor(const Color& c);

private:
    enum class State { Idle, FadingOut, FadingIn };

    State state;
    float duration;            // duración actual
    float timer;               // tiempo transcurrido
    RectangleShape mask;       // rect que cubre pantalla
    Color baseColor;           // color (sin alpha)
    function<void()> callback; // callback al terminar
    void callCallback();
};

#endif
