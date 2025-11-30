/*
 * SpriteAnimator.h
 * Sistema de animación de 2 frames para personajes
 * Alterna entre dos imágenes para crear efecto de "vida"
 * 
 * Uso:
 *   SpriteAnimator character;
 *   character.loadFrames("frame1.png", "frame2.png");
 *   character.setScale(6, 6);
 *   character.update(deltaTime);
 *   window.draw(character);
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

class SpriteAnimator : public Drawable {
public:
    SpriteAnimator();
    ~SpriteAnimator();
    
    // === CARGA DE FRAMES ===
    bool loadFrames(const string& frame1Path, const string& frame2Path);
    bool loadFramesFromManager(const string& frame1Id, const string& frame2Id);
    
    // === CONFIGURACIÓN ===
    void setFrameTime(float seconds);        // Tiempo entre frames (default: 0.5s)
    void setScale(float scaleX, float scaleY);
    void setScale(float uniformScale);       // Escala uniforme
    void setPosition(float x, float y);
    void setPosition(const Vector2f& pos);
    void setOrigin(float x, float y);        // Punto de origen (para centrar)
    void setColor(const Color& color);       // Tinte de color
    
    // === CONTROL DE ANIMACIÓN ===
    void play();                             // Reproducir animación
    void pause();                            // Pausar animación
    void stop();                             // Detener y resetear
    void reset();                            // Volver al frame 1
    void setFlipHorizontal(bool flip);       // Voltear horizontalmente
    
    // === ACTUALIZACIÓN ===
    void update(float deltaTime);
    
    // === GETTERS ===
    Vector2f getPosition() const;
    FloatRect getBounds() const;             // Bounding box para colisiones
    bool isPlaying() const;
    int getCurrentFrame() const;             // 0 o 1
    
private:
    // Implementación de sf::Drawable
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    
    // === DATOS INTERNOS ===
    Texture texture1;
    Texture texture2;
    Sprite sprite;
    
    bool frame1Active;        // true = frame1, false = frame2
    float frameTime;          // Tiempo entre cambios de frame
    float elapsedTime;        // Tiempo acumulado
    
    bool playing;             // Estado de reproducción
    bool framesLoaded;        // Si los frames están cargados
    bool flipHorizontal;      // Flip horizontal
    
    Vector2f position;
    Vector2f scale;
    Color color;
};