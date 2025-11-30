/*
 * DialogueBox.h
 * Sistema de cuadro de diálogo para Remoria
 * Muestra texto con efecto typewriter, nombre del personaje y controles
 * 
 * Uso:
 *   DialogueBox dialogue;
 *   dialogue.setPosition(60, 750);
 *   dialogue.setSize(1800, 300);
 *   dialogue.setCharacterName("Protagonista");
 *   dialogue.setText("Texto del diálogo...");
 *   dialogue.update(deltaTime);
 *   window.draw(dialogue);
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class DialogueBox : public Drawable {
public:
    DialogueBox();
    ~DialogueBox();
    
    // === CONFIGURACIÓN ===
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void loadFont(const string& fontPath);
    void setFont(Font* font);
    
    // === CARGA DE SPRITE ===
    bool loadBoxSprite(const string& spritePath);     // Cargar sprite de la caja (REQUERIDO)
    
    // === CONTENIDO ===
    void setCharacterName(const string& name);
    void setText(const string& text);
    void clearText();
    
    // === CONTROL DE TEXTO ===
    void setTextSpeed(float charsPerSecond);     // Velocidad typewriter (default: 50)
    void setAutoAdvance(bool enable);            // Avance automático
    void setAutoAdvanceDelay(float seconds);     // Delay para auto-avance
    
    // === ESTADO ===
    void showInstantly();                        // Mostrar todo el texto inmediatamente
    void skipToEnd();                            // Saltar al final del texto
    void advance();                              // Avanzar al siguiente diálogo
    void reset();                                // Reiniciar estado
    
    bool isTextComplete() const;                 // ¿Se mostró todo el texto?
    bool isWaitingForInput() const;              // ¿Esperando input del jugador?
    
    // === ACTUALIZACIÓN ===
    void update(float deltaTime);
    
    // === VISIBILIDAD ===
    void show();
    void hide();
    bool isVisible() const;
    
    // === COLORES DE TEXTO ===
    void setTextColor(const Color& color);
    void setNameColor(const Color& color);
    
private:
    // Implementación de sf::Drawable
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    
    // Métodos internos
    void updateTextDisplay();
    void wrapText(const string& text);           // Word wrapping
    
    // === SPRITE DE LA CAJA ===
    Texture boxTexture;
    Sprite boxSprite;
    bool spriteLoaded;                           // Si el sprite fue cargado correctamente
    
    Vector2f position;
    Vector2f size;
    
    // === TEXTO ===
    Font defaultFont;
    Font* currentFont;
    Text characterNameText;
    Text dialogueText;
    Text continueIndicator;                      // Indicador [▼]
    
    string fullText;                             // Texto completo
    string displayedText;                        // Texto actualmente visible
    string characterName;
    
    // === ESTADO TYPEWRITER ===
    float textSpeed;                             // Caracteres por segundo
    float charTimer;                             // Timer para próximo carácter
    size_t currentCharIndex;                     // Índice del carácter actual
    bool textComplete;                           // ¿Texto completamente mostrado?
    
    // === AUTO-AVANCE ===
    bool autoAdvance;
    float autoAdvanceDelay;
    float autoAdvanceTimer;
    
    // === INDICADOR "CONTINUAR" ===
    float indicatorBlinkTimer;
    bool indicatorVisible;
    
    // === VISIBILIDAD ===
    bool visible;
    
    // === COLORES ===
    Color textColor;
    Color nameColor;
    
    // === CONFIGURACIÓN ===
    float nameAreaHeight;
    float textPadding;
    float borderThickness;
    float maxTextWidth;                          // Para word wrapping
};
