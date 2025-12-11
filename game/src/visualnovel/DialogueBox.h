#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "../core/ResourceManager.h"

using namespace std;
using namespace sf;

class DialogueBox {
public:
    // bgTexturePath: ruta a la textura PNG (ideal 300x50 px, pero puede ser otra).
    // size: tamaño en pantalla que deberá cubrir la caja (el sprite se escala a este tamaño).
    DialogueBox(ResourceManager& res,
                const string& fontPath,
                const Vector2f& size,
                const Vector2f& position,
                const string& bgTexturePath = "");

    // Setea un nuevo diálogo: hablante + texto. Reinicia el typewriter.
    void setDialogue(const string& speaker, const string& text);

    // Avanza: si está escribiendo muestra todo; si ya terminó, cierra la caja.
    void advance();

    // Actualiza el efecto typewriter (dt en segundos)
    void update(float dt);

    // Dibuja la caja (solo si activa)
    void draw(RenderWindow& window);

    // ¿Listo para que el jugador avance (ya terminó de escribir y espera input)?
    bool isWaitingForAdvance() const;

    // ¿No hay diálogo en curso?
    bool isIdle() const;

    // Pasar eventos (teclado/ratón) para avanzar
    void handleEvent(const Event& ev);

    // Opcional: ajustar velocidad de escritura (chars por segundo)
    void setCharsPerSecond(float cps);

private:
    ResourceManager& resources;
    Font* font;

    // Background: sprite escalado (ideal) o fallback rectangle
    Sprite backgroundSprite;
    RectangleShape fallbackBackground;
    bool usingSpriteBackground;

    // Layout
    Vector2f boxSize;
    Vector2f boxPosition;

    // Textos
    Text speakerText;
    Text bodyText;
    Text hintText;

    // Typewriter
    string fullText;
    string shownText;
    float charTimer;         // tiempo acumulado para mostrar siguiente carácter
    float charInterval;      // segundos por carácter
    size_t charIndex;
    bool finishedTyping;
    bool active;
};

#endif
