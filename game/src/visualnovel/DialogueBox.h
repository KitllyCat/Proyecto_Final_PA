#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../core/ResourceManager.h"
#include "VoiceBlip.h"

using namespace std;
using namespace sf;

class DialogueBox {
public:
    DialogueBox(ResourceManager& res,
                const string& fontPath,
                const Vector2f& size,
                const Vector2f& position,
                const string& bgTexturePath = "",
                const string& voicePath = "assets/audio/voice_blip.wav");

    // Setea un nuevo diálogo: hablante + texto. Reinicia el typewriter y paginado.
    void setDialogue(const string& speaker, const string& text);

    // Avanza: si está escribiendo muestra todo; si ya terminó, pasa a siguiente página o cierra.
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

    // Ajustar velocidad de escritura (chars por segundo)
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

    // Textos visibles
    Text speakerText;
    Text bodyText;
    Text hintText;

    // Typewriter + paginado
    string fullText;                    // texto original (UTF-8)
    string currentShownText;            // texto de la página actual mostrado por typewriter (UTF-8)
    float charTimer;                    // tiempo acumulado
    float charInterval;                 // segundos por carácter
    size_t charIndexInPage;             // índice dentro de la página actual (bytes)
    bool finishedTyping;                // si terminó la página actual
    bool active;

    // Paginación
    vector<string> pages;               // páginas en UTF-8
    size_t currentPageIndex;

    // Voice blip: reproducir mientras haya typewriting activo
    VoiceBlip voiceBlip;
    string voiceFilePath;

    // Helpers
    void buildPages();                  // genera pages a partir de fullText y boxSize/font
    float measureWidthUtf8(const string& utf8) const; // mide ancho en píxeles de una cadena UTF-8
    static wstring utf8_to_wstring(const string& str);
};

#endif
