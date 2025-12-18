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
	//Constructor importante
    DialogueBox(ResourceManager& res,
                const string& fontPath,
                const Vector2f& size,
                const Vector2f& position,
                const string& bgTexturePath = "",
                const string& voicePath = "assets/audio/voice_blip.wav");
    //Set a un nuevo dialogo
    void setDialogue(const string& speaker, const string& text);
    //Avanza si esta escribiendo
    void advance();
    //Actualiza el efecto typewriter
    void update(float dt);
    //Dibuja la caja
    void draw(RenderWindow& window);
    //Listo para que el jugador avance?
    bool isWaitingForAdvance() const;
    //No hay dialogo en curso?
    bool isIdle() const;
    //Pasar eventos para avanzar
    void handleEvent(const Event& ev);
    //Ajustar velocidad de escritura
    void setCharsPerSecond(float cps);
private:
    ResourceManager& resources;
    Font* font;
    //Background
    Sprite backgroundSprite;
    RectangleShape fallbackBackground;
    bool usingSpriteBackground;
    //Layout
    Vector2f boxSize;
    Vector2f boxPosition;
    //Textos visibles
    Text speakerText;
    Text bodyText;
    Text hintText;
    //Typewriter + paginado
    string fullText;
    string currentShownText;
    float charTimer;
    float charInterval;
    size_t charIndexInPage;
    bool finishedTyping;
    bool active;
    //Paginacion
    vector<string> pages;
    size_t currentPageIndex;
    //Voice blip mientras haya typewriting activo
    VoiceBlip voiceBlip;
    string voiceFilePath;
    //Helpers
    void buildPages();
    float measureWidthUtf8(const string& utf8) const;
    static wstring utf8_to_wstring(const string& str);
};

#endif