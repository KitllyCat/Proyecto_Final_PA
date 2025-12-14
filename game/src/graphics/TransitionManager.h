#ifndef TRANSITION_MANAGER_H
#define TRANSITION_MANAGER_H

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class TransitionManager {
public:
    enum class Type {
        NONE,
        FADE_TO_BLACK,  // Fade out (aparecer negro)
        FADE_FROM_BLACK // Fade in (desaparecer negro)
    };

    TransitionManager();

    // Iniciar una transición
    void start(Type type, float duration = 1.0f);

    // Actualizar la transición
    void update(float dt);

    // Dibujar la transición (llamar AL FINAL, después de todo lo demás)
    void draw(RenderWindow& window);

    // Verificar si la transición terminó
    bool isComplete() const;

    // Verificar si hay una transición activa
    bool isActive() const;

    // Forzar completar la transición
    void complete();
    void reset();

    // Configurar tamaño de pantalla
    void setScreenSize(Vector2u size);

private:
    Type currentType;
    float duration;        // Duración total de la transición
    float timer;           // Tiempo transcurrido
    bool active;
    bool completed;

    RectangleShape fadeRect;  // Rectángulo negro para el fade

    // Calcular el alpha actual basado en el progreso
    Uint8 calculateAlpha() const;
};

#endif
