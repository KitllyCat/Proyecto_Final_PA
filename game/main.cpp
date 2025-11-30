#include <SFML/Graphics.hpp>
#include <iostream>
#include <direct.h>
#include "src/core/ResourceManager.h"
#include "src/graphics/SpriteAnimator.h"
#include "src/ui/DialogueBox.h"
#include "src/visualnovel/SceneManager.h"  // ← NUEVO

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
    
    // === INICIALIZAR DIALOGUEBOX ===
    DialogueBox dialogueBox;
    Font dialogueFont;
    
    // Cargar fuente
    if (!dialogueFont.loadFromFile("assets/fonts/arial.ttf")) {
        cerr << "[ERROR] No se pudo cargar fuente para DialogueBox" << endl;
        return -1;
    }
    dialogueBox.setFont(&dialogueFont);
    
    // Configurar DialogueBox
    dialogueBox.setPosition(60, 750);
    dialogueBox.setSize(1800, 300);
    
    // Cargar sprite de DialogueBox (OBLIGATORIO)
    if (!dialogueBox.loadBoxSprite("assets/images/ui/dialogue_box.png")) {
        cerr << "[ERROR FATAL] No se pudo cargar dialogue_box.png" << endl;
        cerr << "[ERROR] Crea el sprite (300x50px) en: assets/images/ui/" << endl;
        return -1;
    }
    
    cout << "[OK] DialogueBox inicializado" << endl;
    
    // === INICIALIZAR SCENEMANAGER ===
    SceneManager sceneManager;
    sceneManager.setDialogueBox(&dialogueBox);
    sceneManager.setTextSpeed(50);
    
    // Cargar escena inicial
    if (!sceneManager.loadSceneById("prologue")) {
        cerr << "[ERROR FATAL] No se pudo cargar escena inicial" << endl;
        cerr << "[ERROR] Crea 'prologue.json' en: data/storydata/scenes/" << endl;
        return -1;
    }
    
    cout << "[OK] SceneManager inicializado" << endl;
    
	Font instructionFont;
    Text instructionsText;
    
    if (instructionFont.loadFromFile("assets/fonts/arial.ttf")) {
        instructionsText.setFont(instructionFont);
        instructionsText.setString(
            "Remoria - Visual Novel Engine\n\n"
            "ENTER/SPACE: Avanzar dialogo\n"
            "S: Saltar texto\n"
            "1-9: Seleccionar opcion (cuando aparezcan)\n"
            "ESC: Salir"
        );
        instructionsText.setCharacterSize(20);
        instructionsText.setFillColor(Color(76, 106, 51));
        instructionsText.setPosition(50, 50);
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
            
            // Controles de teclado
            if (event.type == Event::KeyPressed) {
                // ESC para salir
                if (event.key.code == Keyboard::Escape) {
                    running = false;
                }
            }
            
            // Pasar eventos al SceneManager
            sceneManager.handleInput(event);
        }
        
        // Actualizar SceneManager (actualiza escena + DialogueBox)
        sceneManager.update(deltaTime);
        
        // Renderizado
        window.clear(bgColor);
        
        // Dibujar escena (background + personajes)
        window.draw(sceneManager);
        
        // Dibujar DialogueBox
        window.draw(dialogueBox);
        
        // Dibujar instrucciones
        if (instructionFont.getInfo().family != "") {
            window.draw(instructionsText);
        }
        
        window.display();
    }
    
    cout << "=== Remoria - Cerrando ===" << endl;
    
    // Limpieza (ResourceManager se limpia automáticamente)
    window.close();
    
    return 0;
}
