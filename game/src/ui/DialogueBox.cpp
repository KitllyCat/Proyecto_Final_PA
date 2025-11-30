/*
 * DialogueBox.cpp
 * Implementación del sistema de diálogo
 */

#include "DialogueBox.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

// ========================================
// CONSTRUCTOR / DESTRUCTOR
// ========================================

DialogueBox::DialogueBox()
    : currentFont(nullptr),
      textSpeed(50.0f),
      charTimer(0.0f),
      currentCharIndex(0),
      textComplete(false),
      autoAdvance(false),
      autoAdvanceDelay(3.0f),
      autoAdvanceTimer(0.0f),
      indicatorBlinkTimer(0.0f),
      indicatorVisible(true),
      visible(true),
      spriteLoaded(false),
      nameAreaHeight(48.0f),
      textPadding(30.0f),
      borderThickness(6.0f)
{
    // Colores de texto (paleta PaperPixels)
    textColor = Color(76, 106, 51);             // Verde profundo
    nameColor = Color(243, 230, 211);           // Beige claro
    
    // Configurar texto del indicador
    continueIndicator.setString("[>>]");
    continueIndicator.setCharacterSize(24);
    continueIndicator.setFillColor(textColor);
}

DialogueBox::~DialogueBox() {
}

// ========================================
// CONFIGURACIÓN
// ========================================

void DialogueBox::setPosition(float x, float y) {
    position = Vector2f(x, y);
    
    // Actualizar posición del sprite
    boxSprite.setPosition(position);
}

void DialogueBox::setSize(float width, float height) {
    size = Vector2f(width, height);
    
    // Escalar sprite si está cargado
    if (spriteLoaded && boxTexture.getSize().x > 0) {
        float scaleX = width / boxTexture.getSize().x;
        float scaleY = height / boxTexture.getSize().y;
        boxSprite.setScale(scaleX, scaleY);
    }
    
    maxTextWidth = width - textPadding * 2 - borderThickness * 2;
    
    // Posicionar indicador (esquina inferior derecha)
    continueIndicator.setPosition(
        position.x + width - 100,
        position.y + height - 50
    );
}

void DialogueBox::loadFont(const string& fontPath) {
    if (defaultFont.loadFromFile(fontPath)) {
        currentFont = &defaultFont;
        characterNameText.setFont(defaultFont);
        dialogueText.setFont(defaultFont);
        continueIndicator.setFont(defaultFont);
        cout << "[DialogueBox] Fuente cargada: " << fontPath << endl;
    } else {
        cerr << "[DialogueBox ERROR] No se pudo cargar fuente: " << fontPath << endl;
    }
}

void DialogueBox::setFont(Font* font) {
    if (font) {
        currentFont = font;
        characterNameText.setFont(*font);
        dialogueText.setFont(*font);
        continueIndicator.setFont(*font);
    }
}

// ========================================
// CARGA DE SPRITE
// ========================================

bool DialogueBox::loadBoxSprite(const string& spritePath) {
    if (!boxTexture.loadFromFile(spritePath)) {
        cerr << "[DialogueBox ERROR] No se pudo cargar sprite: " << spritePath << endl;
        return false;
    }
    
    boxSprite.setTexture(boxTexture);
    boxSprite.setPosition(position);
    
    // Escalar al tamaño configurado
    if (size.x > 0 && size.y > 0) {
        float scaleX = size.x / boxTexture.getSize().x;
        float scaleY = size.y / boxTexture.getSize().y;
        boxSprite.setScale(scaleX, scaleY);
    }
    
    spriteLoaded = true;
    cout << "[DialogueBox] Sprite cargado: " << spritePath << endl;
    return true;
}

// ========================================
// CONTENIDO
// ========================================

void DialogueBox::setCharacterName(const string& name) {
    characterName = name;
    characterNameText.setString(name);
    characterNameText.setCharacterSize(28);
    characterNameText.setFillColor(nameColor);
    characterNameText.setStyle(Text::Bold);
    
    // Centrar nombre en el área
    FloatRect bounds = characterNameText.getLocalBounds();
    characterNameText.setPosition(
        position.x + textPadding,
        position.y + borderThickness + (nameAreaHeight - borderThickness * 2 - bounds.height) / 2 - bounds.top
    );
}

void DialogueBox::setText(const string& text) {
    fullText = text;
    displayedText = "";
    currentCharIndex = 0;
    textComplete = false;
    autoAdvanceTimer = 0.0f;
    
    // Configurar texto
    dialogueText.setCharacterSize(26);
    dialogueText.setFillColor(textColor);
    dialogueText.setLineSpacing(1.3f);
    
    // Posicionar texto
    dialogueText.setPosition(
        position.x + textPadding + borderThickness,
        position.y + nameAreaHeight + textPadding
    );
    
    wrapText(fullText);
}

void DialogueBox::clearText() {
    fullText = "";
    displayedText = "";
    currentCharIndex = 0;
    textComplete = false;
    dialogueText.setString("");
}

// ========================================
// CONTROL DE TEXTO
// ========================================

void DialogueBox::setTextSpeed(float charsPerSecond) {
    textSpeed = charsPerSecond;
}

void DialogueBox::setAutoAdvance(bool enable) {
    autoAdvance = enable;
}

void DialogueBox::setAutoAdvanceDelay(float seconds) {
    autoAdvanceDelay = seconds;
}

void DialogueBox::showInstantly() {
    displayedText = fullText;
    currentCharIndex = fullText.length();
    textComplete = true;
    updateTextDisplay();
}

void DialogueBox::skipToEnd() {
    showInstantly();
}

void DialogueBox::advance() {
    if (textComplete) {
        // Lógica para avanzar al siguiente diálogo
        // (será manejado por DialogueManager)
        cout << "[DialogueBox] Listo para avanzar" << endl;
    } else {
        showInstantly();
    }
}

void DialogueBox::reset() {
    clearText();
    autoAdvanceTimer = 0.0f;
    indicatorBlinkTimer = 0.0f;
    indicatorVisible = true;
}

bool DialogueBox::isTextComplete() const {
    return textComplete;
}

bool DialogueBox::isWaitingForInput() const {
    return textComplete && !autoAdvance;
}

// ========================================
// ACTUALIZACIÓN
// ========================================

void DialogueBox::update(float deltaTime) {
    if (!visible) return;
    
    // Efecto typewriter
    if (!textComplete && currentCharIndex < fullText.length()) {
        charTimer += deltaTime;
        
        float timePerChar = 1.0f / textSpeed;
        
        while (charTimer >= timePerChar && currentCharIndex < fullText.length()) {
            displayedText += fullText[currentCharIndex];
            currentCharIndex++;
            charTimer -= timePerChar;
            
            updateTextDisplay();
        }
        
        if (currentCharIndex >= fullText.length()) {
            textComplete = true;
        }
    }
    
    // Auto-avance
    if (textComplete && autoAdvance) {
        autoAdvanceTimer += deltaTime;
        if (autoAdvanceTimer >= autoAdvanceDelay) {
            advance();
        }
    }
    
    // Parpadeo del indicador
    if (textComplete) {
        indicatorBlinkTimer += deltaTime;
        if (indicatorBlinkTimer >= 0.5f) {
            indicatorVisible = !indicatorVisible;
            indicatorBlinkTimer = 0.0f;
        }
    }
}

// ========================================
// VISIBILIDAD
// ========================================

void DialogueBox::show() {
    visible = true;
}

void DialogueBox::hide() {
    visible = false;
}

bool DialogueBox::isVisible() const {
    return visible;
}

// ========================================
// COLORES
// ========================================

void DialogueBox::setTextColor(const Color& color) {
    textColor = color;
    dialogueText.setFillColor(textColor);
    continueIndicator.setFillColor(textColor);
}

void DialogueBox::setNameColor(const Color& color) {
    nameColor = color;
    characterNameText.setFillColor(nameColor);
}

// ========================================
// MÉTODOS INTERNOS
// ========================================

void DialogueBox::updateTextDisplay() {
    dialogueText.setString(displayedText);
}

void DialogueBox::wrapText(const string& text) {
    // Word wrapping básico
    // Por ahora, el texto se ajusta automáticamente con el tamaño de fuente
    // En una versión futura, se puede implementar wrapping manual
}

// ========================================
// RENDERIZADO
// ========================================

void DialogueBox::draw(RenderTarget& target, RenderStates states) const {
    if (!visible || !spriteLoaded) return;
    
    // Dibujar sprite de la caja
    target.draw(boxSprite, states);
    
    // Dibujar texto encima
    if (currentFont) {
        target.draw(characterNameText, states);
        target.draw(dialogueText, states);
        
        // Dibujar indicador solo si el texto está completo
        if (textComplete && indicatorVisible) {
            target.draw(continueIndicator, states);
        }
    }
}
