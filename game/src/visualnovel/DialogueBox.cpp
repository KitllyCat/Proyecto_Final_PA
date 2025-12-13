#include "DialogueBox.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <sstream>
#include <cmath>

// Conversión UTF-8 -> std::wstring (UTF-16 en Windows)
wstring DialogueBox::utf8_to_wstring(const string& str) {
    try {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    } catch (...) {
        wstring out;
        out.assign(str.begin(), str.end());
        return out;
    }
}

DialogueBox::DialogueBox(ResourceManager& res,
                         const string& fontPath,
                         const Vector2f& size,
                         const Vector2f& position,
                         const string& bgTexturePath,
                         const string& voicePath)
: resources(res),
  font(nullptr),
  usingSpriteBackground(false),
  boxSize(size),
  boxPosition(position),
  charTimer(0.f),
  charInterval(1.0f / 45.0f),
  charIndexInPage(0),
  finishedTyping(true),
  active(false),
  currentPageIndex(0),
  voiceFilePath(voicePath)
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

    // fallback rectangle
    if (!usingSpriteBackground) {
        fallbackBackground.setSize(boxSize);
        fallbackBackground.setPosition(boxPosition);
        fallbackBackground.setFillColor(Color(20, 20, 20, 220));
        fallbackBackground.setOutlineColor(Color(255,255,255,100));
        fallbackBackground.setOutlineThickness(2.f);
    }

    // Inicializar sf::Text
    if (font) {
        speakerText.setFont(*font);
        speakerText.setCharacterSize(32);
        speakerText.setPosition(boxPosition.x + 20.f, boxPosition.y + 8.f);
        speakerText.setFillColor(Color(34, 34, 34));

        bodyText.setFont(*font);
        bodyText.setCharacterSize(26);
        bodyText.setPosition(boxPosition.x + 20.f, boxPosition.y + 55.f);
        bodyText.setFillColor(Color(34, 34, 34));
		bodyText.setLineSpacing(1.0f);

        hintText.setFont(*font);
        hintText.setCharacterSize(18);
        hintText.setPosition(boxPosition.x + boxSize.x - 220.f, boxPosition.y + boxSize.y - 32.f);
        hintText.setString( utf8_to_wstring(std::string("Presiona Space / Click")) );
        hintText.setFillColor(Color(0, 0, 0,180));
    } else {
        cout << "DialogueBox: no se encontró una fuente válida. El texto puede no mostrarse.\n";
    }

    // Intentar cargar voice blip (no fatal)
    if (!voiceFilePath.empty()) {
        if (!voiceBlip.loadFromFile(voiceFilePath)) {
            // si falla, simplemente no se reproducirá sonido
            // cout << "DialogueBox: no se pudo cargar voice blip: " << voiceFilePath << endl;
        }
    }
}

// --------------------------------------------------
// Medir anchura en píxeles de una cadena UTF-8
// --------------------------------------------------
float DialogueBox::measureWidthUtf8(const string& utf8) const {
    if (!font) return 0.f;
    wstring w = utf8_to_wstring(utf8);
    Text tmp;
    tmp.setFont(*font);
    tmp.setCharacterSize(bodyText.getCharacterSize());
    tmp.setString(w);
    FloatRect r = tmp.getLocalBounds();
    return r.width;
}

// --------------------------------------------------
// Construir pages: envolver por palabras y paginar
// --------------------------------------------------
void DialogueBox::buildPages() {
    pages.clear();
    currentPageIndex = 0;

    if (fullText.empty()) return;

    float maxWidth = boxSize.x - 24.f; // padding left+right (12 + 12)
    float availableHeight = (boxPosition.y + boxSize.y) - bodyText.getPosition().y - 12.f;
    float lineHeight = static_cast<float>(bodyText.getCharacterSize()) * 1.2f; // factor de interlineado
    int maxLinesPerPage = std::max(1, static_cast<int>(std::floor(availableHeight / lineHeight)));

    // tokenize words (manteniendo espacios y saltos de linea)
    vector<string> words;
    {
        string token;
        for (size_t i = 0; i < fullText.size();) {
            char c = fullText[i];
            if (c == '\n') {
                if (!token.empty()) { words.push_back(token); token.clear(); }
                words.push_back("\n");
                ++i;
            } else if (c == ' ') {
                if (!token.empty()) { words.push_back(token); token.clear(); }
                words.push_back(" ");
                ++i;
            } else {
                unsigned char uc = static_cast<unsigned char>(c);
                size_t charBytes = 1;
                if ((uc & 0x80) == 0x00) charBytes = 1;
                else if ((uc & 0xE0) == 0xC0) charBytes = 2;
                else if ((uc & 0xF0) == 0xE0) charBytes = 3;
                else if ((uc & 0xF8) == 0xF0) charBytes = 4;
                for (size_t b = 0; b < charBytes && i < fullText.size(); ++b, ++i) token.push_back(fullText[i]);
            }
        }
        if (!token.empty()) words.push_back(token);
    }

    // Build lines by adding words until width exceeded
    vector<string> lines;
    string curLine;
    for (size_t i = 0; i < words.size(); ++i) {
        string w = words[i];
        if (w == "\n") {
            lines.push_back(curLine);
            curLine.clear();
            continue;
        } else if (w == " ") {
            if (!curLine.empty()) curLine.push_back(' ');
            continue;
        } else {
            string trial = curLine;
            if (!trial.empty()) trial.push_back(' ');
            trial += w;
            float wpx = measureWidthUtf8(trial);
            if (wpx <= maxWidth) {
                if (!curLine.empty()) curLine.push_back(' ');
                curLine += w;
            } else {
                if (curLine.empty()) {
                    // break word
                    string piece;
                    for (size_t idx = 0; idx < w.size();) {
                        unsigned char uc = static_cast<unsigned char>(w[idx]);
                        size_t charBytes = 1;
                        if ((uc & 0x80) == 0x00) charBytes = 1;
                        else if ((uc & 0xE0) == 0xC0) charBytes = 2;
                        else if ((uc & 0xF0) == 0xE0) charBytes = 3;
                        else if ((uc & 0xF8) == 0xF0) charBytes = 4;
                        string nextPiece = piece + w.substr(idx, charBytes);
                        if (measureWidthUtf8(nextPiece) <= maxWidth) {
                            piece = nextPiece;
                            idx += charBytes;
                        } else {
                            break;
                        }
                    }
                    if (!piece.empty()) {
                        lines.push_back(piece);
                        string rem = w.substr(piece.size());
                        if (!rem.empty()) {
                            words.insert(words.begin() + i + 1, rem);
                        }
                        curLine.clear();
                    } else {
                        lines.push_back(w);
                        curLine.clear();
                    }
                } else {
                    lines.push_back(curLine);
                    curLine = w;
                }
            }
        }
    }
    if (!curLine.empty()) lines.push_back(curLine);

    // Group lines into pages
    string pageAccum;
    int lineCount = 0;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lineCount >= maxLinesPerPage) {
            pages.push_back(pageAccum);
            pageAccum.clear();
            lineCount = 0;
        }
        if (!pageAccum.empty()) pageAccum += "\n";
        pageAccum += lines[i];
        lineCount++;
    }
    if (!pageAccum.empty()) pages.push_back(pageAccum);

    if (pages.empty()) pages.push_back(string(""));

    // prepare first page state
    currentPageIndex = 0;
    currentShownText.clear();
    charIndexInPage = 0;
    finishedTyping = false;
}

// --------------------------------------------------
// setDialogue: asigna fullText y construye páginas
// --------------------------------------------------
void DialogueBox::setDialogue(const string& speaker, const string& text) {
    if (font) speakerText.setString( utf8_to_wstring(speaker) );
    fullText = text;
    buildPages();

    currentShownText.clear();
    charTimer = 0.f;
    charIndexInPage = 0;
    finishedTyping = false;
    active = true;

    // mostrar vacío y empezar typewriter
    if (font) bodyText.setString( utf8_to_wstring(string("")) );

    // iniciar sonido de blip (si cargado)
    voiceBlip.playLoop();
}

// --------------------------------------------------
// advance: mostrar todo o cambiar page o cerrar
// --------------------------------------------------
void DialogueBox::advance() {
    if (!active) return;

    if (!finishedTyping) {
        // mostrar página completa
        currentShownText = pages[currentPageIndex];
        charIndexInPage = currentShownText.size();
        finishedTyping = true;
        if (font) bodyText.setString( utf8_to_wstring(currentShownText) );
        // detener blip
        voiceBlip.stop();
    } else {
        // página ya completa
        if (currentPageIndex + 1 < pages.size()) {
            // ir a siguiente página y reiniciar typewriter
            currentPageIndex++;
            currentShownText.clear();
            charIndexInPage = 0;
            finishedTyping = false;
            charTimer = 0.f;
            if (font) bodyText.setString( utf8_to_wstring(string("")) );
            // iniciar blip de nuevo para la nueva página
            voiceBlip.playLoop();
        } else {
            // última página -> cerrar caja
            active = false;
            // asegurar detener blip
            voiceBlip.stop();
        }
    }
}

// --------------------------------------------------
// update: typewriter por página (byte-wise, funciona con UTF-8)
// --------------------------------------------------
void DialogueBox::update(float dt) {
    if (!active) return;
    if (finishedTyping) return;

    if (currentPageIndex >= pages.size()) {
        finishedTyping = true;
        // asegurar detener blip
        voiceBlip.stop();
        return;
    }

    const string& page = pages[currentPageIndex];
    charTimer += dt;
    while (charTimer >= charInterval && !finishedTyping) {
        charTimer -= charInterval;
        if (charIndexInPage < page.size()) {
            currentShownText.push_back(page[charIndexInPage]);
            charIndexInPage++;
            if (font) bodyText.setString( utf8_to_wstring(currentShownText) );
        } else {
            finishedTyping = true;
            // detener blip en cuanto la página se completó
            voiceBlip.stop();
        }
    }
}

// --------------------------------------------------
// draw: dibujar background y textos
// --------------------------------------------------
void DialogueBox::draw(RenderWindow& window) {
    if (!active) return;

    if (usingSpriteBackground) window.draw(backgroundSprite);
    else window.draw(fallbackBackground);

    if (font) {
        window.draw(speakerText);
        window.draw(bodyText);
        if (finishedTyping) window.draw(hintText);
    }
}

// --------------------------------------------------
// handleEvent y auxiliares
// --------------------------------------------------
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
            advance();
        }
    }
}

void DialogueBox::setCharsPerSecond(float cps) {
    if (cps <= 0.f) return;
    charInterval = 1.0f / cps;
}
