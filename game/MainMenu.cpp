#include "MainMenu.h"
#include "src/save/SaveManager.h"

using namespace sf;

MainMenu::MainMenu(ResourceManager& res, Vector2u windowSize)
: resources(res),
  bgFrame1(res.getTexture("assets/images/menu_bg_1.png")),
  bgFrame2(res.getTexture("assets/images/menu_bg_2.png")),
  titleFrame1(res.getTexture("assets/images/ui/title_01.png")),
  titleFrame2(res.getTexture("assets/images/ui/title_02.png"))
{
    // Fondo
    bgSprite.setTexture(bgFrame1);

    filter.setTexture(
        resources.getTexture("assets/images/vignette_filter.png")
    );

    // Fuente
    font = &resources.getFont("assets/fonts/title.ttf");

    // Título
    titleSprite.setTexture(titleFrame1);
    titleSprite.setPosition(120.f, 360.f);

    // Botones principales
    setupButton(btnNew, "Nuevo Juego",
        { windowSize.x * 0.68f, windowSize.y * 0.35f });

    setupButton(btnContinue, "Continuar",
        { windowSize.x * 0.68f, windowSize.y * 0.50f });

    btnContinue.enabled = SaveManager::getInstance().exists();

    // Botón Créditos
    btnCredits.normal   = &resources.getTexture("assets/images/ui/button_smallx.png");
    btnCredits.hover    = &resources.getTexture("assets/images/ui/button_smallx_hover.png");
    btnCredits.disabled = btnCredits.normal;

    btnCredits.sprite.setTexture(*btnCredits.normal);

    Vector2f creditsPos(
        windowSize.x - btnCredits.sprite.getGlobalBounds().width - 40.f,
        windowSize.y - btnCredits.sprite.getGlobalBounds().height - 40.f
    );

    btnCredits.sprite.setPosition(creditsPos);

    btnCredits.text.setFont(*font);
    btnCredits.text.setString("Creditos");
    btnCredits.text.setCharacterSize(28);
    btnCredits.text.setFillColor(Color(61, 13, 30));

    FloatRect t = btnCredits.text.getLocalBounds();
    btnCredits.text.setOrigin(t.width / 2, t.height / 2);
    btnCredits.text.setPosition(
        creditsPos.x + btnCredits.sprite.getGlobalBounds().width / 2,
        creditsPos.y + btnCredits.sprite.getGlobalBounds().height / 2 - 2
    );

    // Audio UI
    clickBuffer = resources.getSound("assets/audio/effects/click.wav");
    clickSound.setBuffer(clickBuffer);
    clickSound.setVolume(50.f);

    hoverBuffer = resources.getSound("assets/audio/effects/hover.wav");
    hoverSound.setBuffer(hoverBuffer);
    hoverSound.setVolume(40.f);
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
        playClickSound();
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
        bgSprite.setTexture(bgToggle ? bgFrame2 : bgFrame1);
    }
	titleTimer += dt;
	if (titleTimer >= titleFrameTime) {
	    titleTimer = 0.f;
	    titleToggle = !titleToggle;
	
	    titleSprite.setTexture(titleToggle ? titleFrame2 : titleFrame1);
	}
    Vector2f mouse = (Vector2f)Mouse::getPosition(window);

    bool wasHovering = false;

	auto updateHover = [&](Button& btn) {
		if (!btn.normal || !btn.hover || !btn.disabled) return;
	    if (!btn.enabled) {
	        btn.sprite.setTexture(*btn.disabled);
	        btn.isHovered = false;
	        return;
	    }
	
	    bool hovering = btn.sprite.getGlobalBounds().contains(mouse);
	
	    // Solo cuando entra al botón
	    if (hovering && !btn.isHovered) {
	        hoverSound.stop();
	        hoverSound.play();
	    }
	
	    btn.sprite.setTexture(hovering ? *btn.hover : *btn.normal);
	    btn.isHovered = hovering;
	};

    updateHover(btnNew);
    updateHover(btnContinue);
    updateHover(btnCredits);
}

void MainMenu::draw(RenderWindow& window) {
	window.draw(bgSprite);
	window.draw(titleSprite);
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

void MainMenu::playClickSound() {
    clickSound.stop();
    clickSound.play();
}

void MainMenu::resetCreditsRequest() {
    credits = false;
}

void MainMenu::clearRequests() {
    newGame = false;
    cont = false;
    credits = false;
}
