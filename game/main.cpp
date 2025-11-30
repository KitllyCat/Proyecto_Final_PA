/*
 * Remoria - Visual Novel Engine
 * Main Entry Point
 * 
 * Resolución nativa: 1920x1080
 * Resolución de sprites: 320x180 (escalado x6)
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <direct.h>
#include "src/core/ResourceManager.h"
#include "src/graphics/SpriteAnimator.h"  // ← NUEVO

using namespace std;
using namespace sf;

// Configuración de resolución
const int NATIVE_WIDTH = 1920;
const int NATIVE_HEIGHT = 1080;
const int SCALE_FACTOR = 6;

// Tamaños de sprites
const int CHARACTER_SPRITE_W = 80;
const int CHARACTER_SPRITE_H = 112;
const int DIALOGUE_BOX_W = 300;
const int DIALOGUE_BOX_H = 50;

const string WINDOW_TITLE = "Remoria - A Memory's Journey";

int main() {
    // Verificar Working Directory
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    cout << "[DEBUG] Working Directory: " << cwd << endl;
    
    // Crear ventana en resolución nativa 1920x1080
    RenderWindow window(
        VideoMode(NATIVE_WIDTH, NATIVE_HEIGHT), 
        WINDOW_TITLE,
        Style::Titlebar | Style::Close
    );
    window.setFramerateLimit(60);
    
    cout << "=== Remoria - Inicializando ===" << endl;
    cout << "[INFO] Resolucion nativa: " << NATIVE_WIDTH << "x" << NATIVE_HEIGHT << endl;
    cout << "[INFO] Sprites de personaje: " << CHARACTER_SPRITE_W << "x" << CHARACTER_SPRITE_H 
         << " (x" << SCALE_FACTOR << " = " << CHARACTER_SPRITE_W * SCALE_FACTOR 
         << "x" << CHARACTER_SPRITE_H * SCALE_FACTOR << "px)" << endl;
    
    // Inicializar ResourceManager
    ResourceManager& resources = ResourceManager::getInstance();
    
    // Cargar configuración inicial (desde /game/)
    if (!resources.loadConfig("data/game_config.json")) {
        cerr << "[ERROR] No se pudo cargar game_config.json" << endl;
        cerr << "[DEBUG] Asegurate de ejecutar desde /game/" << endl;
        return -1;
    }
    
    cout << "[OK] Configuracion cargada" << endl;
    
    // === PRUEBA DE SPRITEANIMATOR ===
    SpriteAnimator testCharacter;
    bool characterLoaded = false;
    
    // Intenta cargar sprites de prueba (coloca tus 2 frames PNG aquí)
    if (testCharacter.loadFrames(
        "assets/images/characters/protagonist/test_01.png",
        "assets/images/characters/protagonist/test_02.png"
    )) {
        testCharacter.setScale(SCALE_FACTOR, SCALE_FACTOR);
        
        // Centrar horizontalmente, posicionar verticalmente
        float posX = (NATIVE_WIDTH - CHARACTER_SPRITE_W * SCALE_FACTOR) / 2.0f;
        float posY = 200.0f;  // Arriba del área de diálogo
        
        testCharacter.setPosition(posX, posY);
        testCharacter.setFrameTime(0.5f);  // Alterna cada 0.5 segundos
        testCharacter.play();
        
        characterLoaded = true;
        cout << "[OK] Sprite de prueba cargado y posicionado" << endl;
    } else {
        cout << "[INFO] No se encontraron sprites de prueba" << endl;
        cout << "[INFO] Coloca 'test_01.png' y 'test_02.png' en:" << endl;
        cout << "       assets/images/characters/protagonist/" << endl;
    }
    
    // Texto temporal para prueba
    Font testFont;
    Text testText;
    Text instructionsText;
    
    if (testFont.loadFromFile("assets/fonts/arial.ttf")) {
        testText.setFont(testFont);
        testText.setString("Remoria - SpriteAnimator Test");
        testText.setCharacterSize(32);
        testText.setFillColor(Color(76, 106, 51)); // Verde de paleta
        testText.setPosition(50, 50);
        
        instructionsText.setFont(testFont);
        instructionsText.setString(
            "ESC: Salir\n"
            "SPACE: Pausar/Reanudar animacion\n"
            "R: Reiniciar animacion"
        );
        instructionsText.setCharacterSize(20);
        instructionsText.setFillColor(Color(76, 106, 51));
        instructionsText.setPosition(50, 100);
    }
    
    // Fondo con color de paleta PaperPixels
    Color bgColor(243, 230, 211); // Beige claro (#F3E6D3)
    
    // Ejemplo de sprite escalado (cuando tengas uno)
    // Texture spriteTexture;
    // spriteTexture.loadFromFile("assets/images/test_sprite.png");
    // Sprite sprite(spriteTexture);
    // sprite.setScale(SCALE_FACTOR, SCALE_FACTOR);  // Escala x6
    
    // Loop principal
    Clock clock;
    bool running = true;
    
    cout << "[OK] Entrando al loop principal..." << endl;
    
    while (running && window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Manejo de eventos
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                running = false;
            }
            
            // ESC para salir
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    running = false;
                }
                
                // SPACE para pausar/reanudar
                if (event.key.code == Keyboard::Space && characterLoaded) {
                    if (testCharacter.isPlaying()) {
                        testCharacter.pause();
                        cout << "[DEBUG] Animacion pausada" << endl;
                    } else {
                        testCharacter.play();
                        cout << "[DEBUG] Animacion reanudada" << endl;
                    }
                }
                
                // R para reiniciar
                if (event.key.code == Keyboard::R && characterLoaded) {
                    testCharacter.reset();
                    testCharacter.play();
                    cout << "[DEBUG] Animacion reiniciada" << endl;
                }
            }
        }
        
        // Actualizar animaciones
        if (characterLoaded) {
            testCharacter.update(deltaTime);
        }
        
        // Renderizado
        window.clear(bgColor);
        
        // Dibujar sprite animado
        if (characterLoaded) {
            window.draw(testCharacter);
        }
        
        // Dibujar UI
        if (testFont.getInfo().family != "") {
            window.draw(testText);
            window.draw(instructionsText);
        }
        
        window.display();
    }
    
    cout << "=== Remoria - Cerrando ===" << endl;
    
    // Limpieza (ResourceManager se limpia automáticamente)
    window.close();
    
    return 0;
}