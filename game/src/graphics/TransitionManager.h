#ifndef TRANSITION_MANAGER_H
#define TRANSITION_MANAGER_H

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class TransitionManager {
public:
    enum class Type {
        NONE,
        FADE_TO_BLACK,  //Fade out 
        FADE_FROM_BLACK //Fade in
    };
    TransitionManager();
    //Iniciar una transicion
    void start(Type type, float duration = 1.0f);
    //Actualizar la transicion
    void update(float dt);
    //Dibujar la transicion
    void draw(RenderWindow& window);
    //Verificar si termino o esta activa
    bool isComplete() const;
    bool isActive() const;
    //Forzar completar la transicion
    void complete();
    void reset();
    //Configurar tamaño de pantalla
    void setScreenSize(Vector2u size);
    
private:
    Type currentType;
    float duration;
    float timer;
    bool active;
    bool completed;
	//Rectangulo negro para el fade
    RectangleShape fadeRect;
    Uint8 calculateAlpha() const;
};

#endif