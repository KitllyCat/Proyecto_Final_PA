#include "MainMenu.h"
#include "src/save/SaveManager.h"

using namespace sf;

MainMenu::MainMenu(ResourceManager& res, Vector2u windowSize)
: resources(res) {
    // Fondo
    bgFrame1 = &resources.getTexture("assets/images/menu_bg_1.png");
	bgFrame2 = &resources.getTexture("assets/images/menu_bg_2.png");
	bgSprite.setTexture(*bgFrame1);
    filter.setTexture(resources.getTexture("assets/images/vignette_filter.png"));

    // Fuente
    font = &resources.getFont("assets/fonts/title.ttf");
	titleFont = &resources.getFont("assets/fonts/title.ttf");
	
	title.setFont(*titleFont);
	title.setString("Remoria...");
	title.setCharacterSize(240);
	title.setFillColor(Color(93, 35, 31));
	
	// Posición base
	Vector2f titlePos(120.f, 360.f);
	title.setPosition(titlePos);
	
	titleShadow = title;
	titleShadow.setFillColor(Color(61, 13, 30, 110));
	titleShadow.move(6.f, 6.f);

	// Posición izquierda
	title.setPosition(120.f, 380.f);
	
    // Botones
    setupButton(btnNew, "Nuevo Juego", { windowSize.x * 0.68f, windowSize.y * 0.35f });
    setupButton(btnContinue, "Continuar", { windowSize.x * 0.68f, windowSize.y * 0.50f });
	btnCredits.sprite.setTexture(
    resources.getTexture("assets/images/ui/button_smallx.png")
	);
	
	Vector2f creditsPos(
	    windowSize.x - btnCredits.sprite.getGlobalBounds().width - 40.f,
	    windowSize.y - btnCredits.sprite.getGlobalBounds().height - 40.f
	);
	
	btnCredits.sprite.setPosition(creditsPos);
	
	btnCredits.text.setFont(*font);
	btnCredits.text.setString("Creditos");
	btnCredits.text.setCharacterSize(28);
	btnCredits.text.setFillColor(Color(61, 13, 30));
	
	// Centrar texto dentro del botón
	FloatRect t = btnCredits.text.getLocalBounds();
	btnCredits.text.setOrigin(t.width / 2, t.height / 2);
	btnCredits.text.setPosition(
	    creditsPos.x + btnCredits.sprite.getGlobalBounds().width / 2,
	    creditsPos.y + btnCredits.sprite.getGlobalBounds().height / 2 - 2
	);
	
    // Desactivar continuar si no hay save
    btnContinue.enabled = SaveManager::getInstance().exists();
}

void MainMenu::setupButton(Button& btn, const string& label, Vector2f pos) {
    btn.normal   = &resources.getTexture("assets/images/ui/button.png");
    btn.hover    = &resources.getTexture("assets/images/ui/button_hover.png");
    btn.disabled = &resources.getTexture("assets/images/ui/button_disabled.png");

    btn.sprite.setTexture(*btn.normal);
    btn.sprite.setPosition(pos);

    btn.text.setFont(*font);
    btn.text.setString(label);
    btn.text.setCharacterSize(36);
    btn.text.setFillColor(Color(61, 13, 30));

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

void MainMenu::update(float dt, const RenderWindow& window) {
    bgTimer += dt;
    if (bgTimer >= bgFrameTime) {
        bgTimer = 0.f;
        bgToggle = !bgToggle;
        bgSprite.setTexture(bgToggle ? *bgFrame2 : *bgFrame1);
    }

    Vector2f mouse = (Vector2f)Mouse::getPosition(window);

    auto updateHover = [&](Button& btn) {
	    if (!btn.enabled) {
	        btn.sprite.setTexture(*btn.disabled);
	        return;
	    }
	    if (btn.sprite.getGlobalBounds().contains(mouse)) {
	        btn.sprite.setTexture(*btn.hover);
	    } else {
	        btn.sprite.setTexture(*btn.normal);
	    }
	};

    updateHover(btnNew);
    updateHover(btnContinue);
    updateHover(btnCredits);
}

void MainMenu::draw(RenderWindow& window) {
	window.draw(bgSprite);
	window.draw(titleShadow);
	window.draw(title);
    auto drawBtn = [&](Button& btn) {
        window.draw(btn.sprite);
        window.draw(btn.text);
    };
    drawBtn(btnNew);
    drawBtn(btnContinue);
    drawBtn(btnCredits);
    window.draw(filter);

}

bool MainMenu::startNewGameRequested() const { return newGame; }
bool MainMenu::continueRequested() const { return cont; }
bool MainMenu::creditsRequested() const { return credits; }

void MainMenu::playMusic() {
    if (menuMusic.getStatus() == Music::Playing)
        return;

    if (!menuMusic.openFromFile("assets/audio/title_music.ogg")) {
        cout << "ERROR: No se pudo cargar musica del menu\n";
        return;
    }

    menuMusic.setLoop(true);
    menuMusic.setVolume(45.f);
    menuMusic.play();
}

void MainMenu::stopMusic() {
    if (menuMusic.getStatus() == sf::Music::Playing)
        menuMusic.stop();
}
