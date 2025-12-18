#include "CreditsScreen.h"
#include <cmath>

CreditsScreen::CreditsScreen(ResourceManager& res, Vector2u winSize) {
    background.setSize(Vector2f(winSize));
    background.setFillColor(Color(34, 3, 12));
	//Carga titulo
    font = &res.getFont("assets/fonts/default.ttf");
    titleFont = &res.getFont("assets/fonts/title.ttf");
	title.setFont(*titleFont);
	title.setString("CREDITOS");
	title.setCharacterSize(120);
	title.setFillColor(Color(252, 244, 228));
	FloatRect bounds = title.getLocalBounds();
	title.setOrigin(bounds.left + bounds.width / 2.f,bounds.top  + bounds.height / 2.f);
	title.setPosition(winSize.x / 2.f, 120.f);
	//String mostrado
    vector<string> content = {
        "Visualnovel desarrollado por:",
        "KitllyCat",
        "Matoricore",
        "",
        "Musica y Audio:",
        "Pufino - Setuniman",
        "",
        "Herramientas Utilizadas:",
        "Dev-C++ 6.3 y SFML 2.5.1",
        "nlohmann/json",
        "",
        "Agradecimientos Especiales:",
        "Al encargado de sprites y graficos, al creador de la historia y a todos los que apoyaron este proyecto...",
        "Gracias por jugar Remoria~... <3"
    };
    float y = 300.f;
    for (auto& s : content) {
        Text t(s, *font, 32);
        t.setFillColor(Color(200, 200, 220));
        t.setPosition(winSize.x / 2.f - t.getLocalBounds().width / 2, y);
        y += 48;
        lines.push_back(t);
    }
    hint.setFont(*font);
    hint.setString("Presiona cualquier tecla o click para volver");
    hint.setCharacterSize(24);
    hint.setFillColor(Color(150, 150, 150));
    hint.setPosition(winSize.x - 500, winSize.y - 60);
}

void CreditsScreen::handleEvent(const Event& ev) {
    if (ev.type == Event::KeyPressed || ev.type == Event::MouseButtonPressed)
        back = true;
}

void CreditsScreen::update(float dt) {
    static float t = 0;
    t += dt;
    Uint8 a = 150 + 100 * sin(t * 2);
    hint.setFillColor(Color(150, 150, 150, a));
}

void CreditsScreen::draw(RenderWindow& window) {
    window.draw(background);
    window.draw(title);
    for (auto& l : lines) window.draw(l);
    window.draw(hint);
}

bool CreditsScreen::backRequested() const { return back; }
void CreditsScreen::reset() { back = false; }
