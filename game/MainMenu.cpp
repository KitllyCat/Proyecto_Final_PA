#include "MainMenu.h"
#include "src/save/SaveManager.h"

using namespace sf;

MainMenu::MainMenu(ResourceManager& res, Vector2u windowSize)
: resources(res) {

    // Fondo
    background.setTexture(resources.getTexture("assets/images/menu_bg.png"));

    // Fuente
    font = &resources.getFont("assets/fonts/default.ttf");
	titleFont = &resources.getFont("assets/fonts/title.ttf");

	title.setFont(*titleFont);
	title.setString("Remoria...");
	title.setCharacterSize(240);
	title.setFillColor(Color::Black);
	
	// Posición izquierda, como en tu boceto
	title.setPosition(120.f, 380.f);
	
    // Botones
    setupButton(btnNew, "Nuevo Juego", { windowSize.x * 0.68f, windowSize.y * 0.35f });
    setupButton(btnContinue, "Continuar", { windowSize.x * 0.68f, windowSize.y * 0.50f });
    setupButton(btnCredits, "Creditos", { windowSize.x * 0.78f, windowSize.y * 0.75f });

    // Desactivar continuar si no hay save
    btnContinue.enabled = SaveManager::getInstance().exists();
}

void MainMenu::setupButton(Button& btn, const std::string& label, Vector2f pos) {
    btn.sprite.setTexture(resources.getTexture("assets/images/ui/button.png"));
    btn.sprite.setPosition(pos);

    btn.text.setFont(*font);
    btn.text.setString(label);
    btn.text.setCharacterSize(36);
    btn.text.setFillColor(Color::Black);

    FloatRect bounds = btn.text.getLocalBounds();
    btn.text.setOrigin(bounds.width / 2, bounds.height / 2);
    btn.text.setPosition(
        pos.x + btn.sprite.getGlobalBounds().width / 2,
        pos.y + btn.sprite.getGlobalBounds().height / 2 - 5
    );
}

void MainMenu::handleEvent(const Event& ev, const RenderWindow& window) {
    if (ev.type != Event::MouseButtonPressed ||
        ev.mouseButton.button != Mouse::Left)
        return;

    auto checkClick = [&](Button& btn, bool& flag) {
        if (!btn.enabled) return;
        if (btn.sprite.getGlobalBounds()
            .contains((Vector2f)Mouse::getPosition(window))) {
            flag = true;
        }
    };

    checkClick(btnNew, newGame);
    checkClick(btnContinue, cont);
    checkClick(btnCredits, credits);
}

void MainMenu::update(float) {
    // Hover
    auto updateHover = [&](Button& btn) {
        if (!btn.enabled) {
            btn.sprite.setTexture(resources.getTexture("assets/images/ui/button_disabled.png"));
            return;
        }

        if (btn.sprite.getGlobalBounds()
            .contains((Vector2f)Mouse::getPosition())) {
            btn.sprite.setTexture(resources.getTexture("assets/images/ui/button.png"));
        } else {
            btn.sprite.setTexture(resources.getTexture("assets/images/ui/button.png"));
        }
    };

    updateHover(btnNew);
    updateHover(btnContinue);
    updateHover(btnCredits);
}

void MainMenu::draw(RenderWindow& window) {
    window.draw(background);
	window.draw(title);
    auto drawBtn = [&](Button& btn) {
        window.draw(btn.sprite);
        window.draw(btn.text);
    };
    drawBtn(btnNew);
    drawBtn(btnContinue);
    drawBtn(btnCredits);
}

bool MainMenu::startNewGameRequested() const { return newGame; }
bool MainMenu::continueRequested() const { return cont; }
bool MainMenu::creditsRequested() const { return credits; }

void MainMenu::playMusic() {
    if (menuMusic.getStatus() == Music::Playing)
        return;

    if (!menuMusic.openFromFile("assets/audio/menu_theme.ogg")) {
        cout << "ERROR: No se pudo cargar musica del menu\n";
        return;
    }

    menuMusic.setLoop(true);
    menuMusic.setVolume(60.f);
    menuMusic.play();
}

void MainMenu::stopMusic() {
    if (menuMusic.getStatus() == sf::Music::Playing)
        menuMusic.stop();
}
