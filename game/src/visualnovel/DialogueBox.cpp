// src/visualnovel/DialogueBox.cpp
#include "DialogueBox.h"
#include <iostream>
#include <locale>
#include <codecvt>

// helper: convertir UTF-8 std::string -> std::wstring (UTF-16 en Windows)
static std::wstring utf8_to_wstring(const std::string& str) {
    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    } catch (...) {
        // fallback: extensión directa (puede producir mojibake pero evita crash)
        std::wstring out;
        out.assign(str.begin(), str.end());
        return out;
    }
}

DialogueBox::DialogueBox(ResourceManager& res,
                         const string& fontPath,
                         const Vector2f& size,
                         const Vector2f& position,
                         const string& bgTexturePath)
: resources(res),
  font(nullptr),
  usingSpriteBackground(false),
  boxSize(size),
  boxPosition(position),
  charTimer(0.f),
  charInterval(1.0f / 45.0f), // ~45 chars por segundo por defecto
  charIndex(0),
  finishedTyping(true),
  active(false)
{
    // Intentar cargar la fuente (ResourceManager carga bajo demanda o imprime error)
    try {
        font = &resources.getFont(fontPath);
    } catch (exception& e) {
        cout << "DialogueBox: error cargando fuente: " << e.what() << endl;
        font = nullptr;
    }

    // Intentar cargar sprite de fondo si se dio ruta
    if (!bgTexturePath.empty()) {
        try {
            Texture& t = resources.getTexture(bgTexturePath);
            backgroundSprite.setTexture(t);

            // Escalar la textura para cubrir exactamente boxSize (puede deformar si proporciones distintas)
            Vector2u texSize = t.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                float sx = boxSize.x / static_cast<float>(texSize.x);
                float sy = boxSize.y / static_cast<float>(texSize.y);
                backgroundSprite.setScale(sx, sy);
            }
            backgroundSprite.setPosition(boxPosition);
            usingSpriteBackground = true;
        } catch (exception& e) {
            cout << "DialogueBox: no se pudo cargar bg sprite: " << bgTexturePath << " -> " << e.what() << endl;
            usingSpriteBackground = false;
        }
    }

    // Si no se usa sprite (falla o no se dio), usar RectangleShape fallback
    if (!usingSpriteBackground) {
        fallbackBackground.setSize(boxSize);
        fallbackBackground.setPosition(boxPosition);
        fallbackBackground.setFillColor(Color(20, 20, 20, 220));
        fallbackBackground.setOutlineColor(Color(255,255,255,100));
        fallbackBackground.setOutlineThickness(2.f);
    }

    // Inicializar textos (si font está disponible)
    if (font) {
        speakerText.setFont(*font);
        speakerText.setCharacterSize(32);
        speakerText.setPosition(boxPosition.x + 20.f, boxPosition.y + 8.f);
        speakerText.setFillColor(Color(34, 34, 34));

        bodyText.setFont(*font);
        bodyText.setCharacterSize(26);
        bodyText.setPosition(boxPosition.x + 20.f, boxPosition.y + 55.f);
        bodyText.setFillColor(Color(34, 34, 34));

        hintText.setFont(*font);
        hintText.setCharacterSize(18);
        hintText.setPosition(boxPosition.x + boxSize.x - 220.f, boxPosition.y + boxSize.y - 32.f);
        hintText.setString( utf8_to_wstring(std::string("Presiona Space / Click")) );
        hintText.setFillColor(Color(0, 0, 0,180));
    } else {
        // Si no hay fuente, los textos seguirán existiendo pero no mostrarán nada legible.
        cout << "DialogueBox: no se encontró una fuente válida. El texto puede no mostrarse.\n";
    }
}

void DialogueBox::setDialogue(const string& speaker, const string& text) {
    if (font) speakerText.setString( utf8_to_wstring(speaker) );
    fullText = text;
    shownText.clear();
    charTimer = 0.f;
    charIndex = 0;
    finishedTyping = false;
    active = true;

    // limpiar bodyText inmediatamente (para que se vaya llenando)
    if (font) bodyText.setString(utf8_to_wstring(string("")));
}

void DialogueBox::advance() {
    if (!active) return;

    if (!finishedTyping) {
        // mostrar todo inmediatamente
        shownText = fullText;
        charIndex = fullText.size();
        finishedTyping = true;
        if (font) bodyText.setString( utf8_to_wstring(shownText) );
    } else {
        // Si ya terminó al presionar, cerrar caja (la SceneManager/Scene decidirá qué hacer después)
        active = false;
    }
}

void DialogueBox::update(float dt) {
    if (!active || finishedTyping) return;

    charTimer += dt;
    while (charTimer >= charInterval && !finishedTyping) {
        charTimer -= charInterval;
        if (charIndex < fullText.size()) {
            shownText.push_back(fullText[charIndex]);
            charIndex++;
            if (font) bodyText.setString( utf8_to_wstring(shownText) );
        } else {
            finishedTyping = true;
        }
    }
}

void DialogueBox::draw(RenderWindow& window) {
    if (!active) return;

    if (usingSpriteBackground) {
        window.draw(backgroundSprite);
    } else {
        window.draw(fallbackBackground);
    }

    if (font) {
        window.draw(speakerText);
        window.draw(bodyText);
        if (finishedTyping) window.draw(hintText);
    }
}

bool DialogueBox::isWaitingForAdvance() const {
    return active && finishedTyping;
}

bool DialogueBox::isIdle() const {
    return !active;
}

void DialogueBox::handleEvent(const Event& ev) {
    if (!active) return;

    if (ev.type == Event::KeyPressed) {
        if (ev.key.code == Keyboard::Space || ev.key.code == Keyboard::Enter) {
            advance();
        }
    } else if (ev.type == Event::MouseButtonPressed) {
        if (ev.mouseButton.button == Mouse::Left) {
            // opcional: podríamos comprobar si el click está dentro de la caja
            // ahora simplemente lo interpretamos como 'advance'
            advance();
        }
    }
}

void DialogueBox::setCharsPerSecond(float cps) {
    if (cps <= 0.f) return;
    charInterval = 1.0f / cps;
}
