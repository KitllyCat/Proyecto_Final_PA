#include "Scene.h"
#include "../save/SaveManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

Scene::Scene() : 
    resources(nullptr), 
    currentIndex(0), 
    characterAnimator(nullptr), 
    hasCharacter(false), 
    characterVisible(true), 
    characterPosition(800.f, 400.f), 
    waitingChoice(false), 
    finished(false), 
    onMusicChange(nullptr), 
    waitingTransition(false)
{
}

void Scene::setScreenSize(Vector2u size){
    transition.setScreenSize(size);
}

void Scene::setMusicChangeCallback(MusicChangeCallback callback){
    onMusicChange = callback;
}

string Scene::dirname(const string& path){
    size_t p = path.find_last_of("/\\");
    if (p == string::npos){
    	return ".";	
	}
    return path.substr(0, p);
}

bool Scene::pathLooksLikeAssets(const string& p){
    if (p.size() < 7){
    	return false;
	}
    string start = p.substr(0, 7);
    for (auto& c : start){
    	c = tolower(c);
	}
    return start == "assets/";
}

bool Scene::loadFromFile(const string& path, ResourceManager& res, int startIndex){
    resources = &res;
    scenePath = path;
    characterVisible = true;
    ifstream f(path, ios::binary);
    if (!f.is_open()){
        cout << "[System] No se pudo abrir " << path << endl;
        return false;
    }
    string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();
    json j = json::parse(content);
    basePath = dirname(path);
    
    steps.clear();
    currentIndex = startIndex;
    waitingChoice = false;
    finished = false;
    nextScene.clear();
    if (j.contains("bg")){
        string bg = j["bg"];
        string full = pathLooksLikeAssets(bg) ? bg : basePath + "/" + bg;
        bgSprite.setTexture(resources->getTexture(full));
    }
    hasCharacter = false;
    characterAnimator.reset();
    if (j.contains("character")){
        auto c = j["character"];
        string f1 = c.value("frame1", "");
        string f2 = c.value("frame2", "");
        int fps = c.value("fps", 8);
        float x = c.value("x", characterPosition.x);
        float y = c.value("y", characterPosition.y);
        characterPosition = { x, y };
        if (!f1.empty() && !f2.empty()){
            string p1 = pathLooksLikeAssets(f1) ? f1 : basePath + "/" + f1;
            string p2 = pathLooksLikeAssets(f2) ? f2 : basePath + "/" + f2;
            try{
                Texture& t1 = resources->getTexture(p1);
                Texture& t2 = resources->getTexture(p2);
                
                characterSprite.setTexture(t1);
                auto s = t1.getSize();
                characterSprite.setOrigin(s.x / 2.f, s.y / 2.f);
                characterSprite.setPosition(characterPosition);
                
                characterAnimator = make_unique<SpriteAnimator>();
                characterAnimator->attachSprite(&characterSprite);
                characterAnimator->loadFrames(&t1, &t2);
                characterAnimator->setFPS(fps);
                characterAnimator->play();
                hasCharacter = true;
            }
            catch (...){
                hasCharacter = false;
            }
        }
    }
    json arr = j.contains("steps") ? j["steps"] : j["sequence"];
    for (auto& item : arr){
        SceneStep s;
        s.type = item.value("type", "dialogue");
        if (s.type == "goto") {
		    if (item.contains("scene")) {
		        s.goto_scene = item["scene"].get<string>();
		    } else if (item.contains("goto")) {
		        s.goto_scene = item["goto"].get<string>();
		    } else {
		        s.goto_scene = "";
		    }
		}
        if (s.type == "dialogue") {
            s.speaker = item.value("speaker", "");
            s.text = item.value("text", "");
            if (item.contains("sfx")){
                s.sfx_path = item.value("sfx", "");
                s.sfx_volume = item.value("sfx_volume", 100.0f);
            }
        } else if (s.type == "change_bg") {
            s.bg_path = item.value("bg", "");
            if (item.contains("music"))
            {
                s.music_path = item.value("music", "");
            }
        } else if (s.type == "goto") {
		    s.goto_scene = item.value("scene", "");
		    if (s.goto_scene.empty()) {
		        cerr << "[System] ERROR: goto sin 'scene'" << endl;
		    }
		} else if (s.type == "choice") {
            if (!item.contains("choices") || !item["choices"].is_array()){
                cerr << "[System] ERROR: 'choice' step debe tener array 'choices'" << endl;
                continue;
            }
            for (auto& c : item["choices"]){
                SceneStep::Choice ch;
                //Texto de la opcion
                ch.text = c.value("text", "");
                //Cambiar a otra escena
                if (c.contains("goto")){
                    ch.goto_scene = c["goto"].get<string>();
                } else if (c.contains("next")){
                    ch.goto_scene = c["next"].get<string>();
                }else{
                    ch.goto_scene = "";
                }
                //Saltar a un step especifico en esta escena
                if (c.contains("goto_step")){
                    try{
                        ch.goto_step = c["goto_step"].get<int>();
                    }catch (...){
                        ch.goto_step = -1;
                    }
                }else{
                    ch.goto_step = -1;
                }
                //Flag a guardar cuando se elige
                if (c.contains("flag")){
                    ch.flag = c["flag"].get<string>();
                }else{
                    ch.flag = "";
                }
                //Flag necesario para que aparezca
                if (c.contains("require_flag")){
                    ch.require_flag = c["require_flag"].get<string>();
                }else{
                    ch.require_flag = "";
                }
                s.choices.push_back(ch);
                if (!ch.goto_scene.empty()){
                    cout << " -> escena: " << ch.goto_scene;
                }else if (ch.goto_step >= 0){
                    cout << " -> step: " << ch.goto_step;
                }
                if (!ch.flag.empty()){
                    cout << " (flag: " << ch.flag << ")";
                }
                if (!ch.require_flag.empty()){
                    cout << " (requiere: " << ch.require_flag << ")";
                }
                cout << endl;
            }
        } else if (s.type == "play_sfx"){
            s.sfx_path = item.value("sound", "");
            if (s.sfx_path.empty()){
                s.sfx_path = item.value("sfx", "");
            }
            s.sfx_volume = item.value("volume", 100.0f);
        } else if (s.type == "transition"){
            s.effect = item.value("effect", "fade");
            s.duration = item.value("duration", 1.0f);
        }
        steps.push_back(s);
    }
    dialogue = make_unique<DialogueBox>(
        *resources,
        "assets/fonts/default.ttf",
        Vector2f(1700.f, 260.f),
        Vector2f(110.f, 780.f),
        "assets/images/ui/dialogue_box.png"
    );
    
    currentIndex = startIndex;
    if (currentIndex < steps.size()){
    	startStep(steps[currentIndex]);
	}else{
		finished = true;
	}
    return true;
}

void Scene::startStep(const SceneStep& s){
    //Limpiar sonidos terminados
    cleanupFinishedSounds();
    if (s.type == "hide_character"){
        characterVisible = false;
        advanceStep();
        return;
    }
    if (s.type == "show_character"){
        characterVisible = true;
        advanceStep();
        return;
    }
    if (s.type == "checkpoint"){
        string id = scenePath.substr(scenePath.find_last_of("/\\") + 1);
        id = id.substr(0, id.find(".json"));
        SaveManager::getInstance().save(id, currentIndex);
        advanceStep();
        return;
    }
    if (s.type == "goto"){
	    if (s.goto_scene.empty()) {
	        cerr << "[Scene] goto sin escena válida" << endl;
	        nextScene = s.goto_scene;
	        finished = true;
	        return;
	    }
	    nextScene = s.goto_scene;
	    finished = true;
	    return;
	}
    
    if (s.type == "dialogue"){
        dialogue->setDialogue(s.speaker, s.text);
        if (!s.sfx_path.empty()){
            playSFX(s.sfx_path, s.sfx_volume);
        }
    }else if (s.type == "change_bg"){
        string full = pathLooksLikeAssets(s.bg_path) ? s.bg_path : basePath + "/" + s.bg_path;
        bgSprite.setTexture(resources->getTexture(full));
        if (!s.music_path.empty() && onMusicChange){
            onMusicChange(s.music_path);
        }
        advanceStep();
    }else if (s.type == "play_sfx"){
        if (!s.sfx_path.empty()){
            playSFX(s.sfx_path, s.sfx_volume);
        }
        advanceStep();
    }else if (s.type == "transition"){
        if (s.effect == "fade" || s.effect == "fade_to_black"){
            transition.start(TransitionManager::Type::FADE_TO_BLACK, s.duration);
            waitingTransition = true;
        }else if (s.effect == "fade_from_black"){
            transition.start(TransitionManager::Type::FADE_FROM_BLACK, s.duration);
            waitingTransition = true;
        }else{
            cout << "[System] Efecto de transición desconocido: " << s.effect << endl;
            advanceStep();
        }
    }else if (s.type == "choice"){
        waitingChoice = true;
        //Filtrar choices segun flags
        vector<SceneStep::Choice> availableChoices;
        for (const auto& choice : s.choices){
            //Si pide flag, verificar si existe
            if (!choice.require_flag.empty()){
                if (!SaveManager::getInstance().hasFlag(choice.require_flag)){
                    cout << "[System] Choice '" << choice.text << "' oculta (falta flag: " << choice.require_flag << ")" << endl;
                    continue;
                }
            }
            availableChoices.push_back(choice);
        }
        //Verificar que haya  una opcion disponible
        if (availableChoices.empty()){
            cerr << "[System] ERROR: Todas las choices están bloqueadas por flags" << endl;
            //Avanzar al siguiente step
            waitingChoice = false;
            advanceStep();
            return;
        }
        //Construir texto con choices disponibles
        string text;
        for (size_t i = 0; i < availableChoices.size(); ++i){
            text += to_string(i + 1) + ". " + availableChoices[i].text + "\n";
        }
        //Guardar temporalmente las choices
        steps[currentIndex].choices = availableChoices;
        dialogue->setDialogue("Elige", text);
    }
}

void Scene::advanceStep(){
    if (finished){
    	return;
	}
    if (++currentIndex < steps.size()){
    	startStep(steps[currentIndex]);
	}else{
		finished = true;
	} 
}

void Scene::update(float dt){
	if (finished){return;}
    //Limpiar sonidos
    static float cleanupTimer = 0.0f;
    cleanupTimer += dt;
    if (cleanupTimer >= 0.5f){
        cleanupFinishedSounds();
        cleanupTimer = 0.0f;
    }
    //Actualizar transicion activa
    if (waitingTransition){
        transition.update(dt);
        if (transition.isComplete()){
            waitingTransition = false;
            transition.reset();
            advanceStep();
        }
        return;
    } 
    if (dialogue){
    	dialogue->update(dt);
	}
    if (hasCharacter && characterAnimator){
    	characterAnimator->update(dt);
	}
}

void Scene::handleEvent(const Event& ev){
    if (finished){return;}
	//No procesar eventos durante transiciones
    if (waitingTransition){
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape){
            cout << "[System] Transición saltada..." << endl;
            transition.complete();
        }
        return;
    }
    if (waitingChoice){
        if (ev.type == Event::KeyPressed){
            int choiceIndex = -1;
            //Soportar teclas numericas y del numpad
            if (ev.key.code >= Keyboard::Num1 && ev.key.code <= Keyboard::Num9){
                choiceIndex = ev.key.code - Keyboard::Num1;
            }else if (ev.key.code >= Keyboard::Numpad1 && ev.key.code <= Keyboard::Numpad9){
                choiceIndex = ev.key.code - Keyboard::Numpad1;
            }
            //Validar que la opcion existe
            if (choiceIndex >= 0 && choiceIndex < (int)steps[currentIndex].choices.size()){
                const auto& chosen = steps[currentIndex].choices[choiceIndex];
                //Guardar flag si esta definido
                if (!chosen.flag.empty()){
                    SaveManager::getInstance().setFlag(chosen.flag, true);
                    cout << "[System] Flag guardado: " << chosen.flag << endl;
                }
                //Decidir si cambiar de escena o hacer branching interno
                if (!chosen.goto_scene.empty()){
                    nextScene = chosen.goto_scene;
                    waitingChoice = false;
                    finished = true;
                }else if (chosen.goto_step >= 0){
                    currentIndex = chosen.goto_step;
                    waitingChoice = false;
                    if (currentIndex < steps.size()){
                        startStep(steps[currentIndex]);
                    }else{
                        cerr << "[System] ERROR: goto_step fuera de rango: " << chosen.goto_step << endl;
                        finished = true;
                    }
                }else{
                    waitingChoice = false;
                    advanceStep();
                }
            }else if (choiceIndex >= (int)steps[currentIndex].choices.size()){
                // Tecla numerica valida pero fuera de rango
                cout << "[System] La opción " << (choiceIndex + 1) << " no existe. Opciones disponibles: 1-" << steps[currentIndex].choices.size() << endl;
            }
        }
        return;
    }
    if (dialogue){
        dialogue->handleEvent(ev);
    }
    if (dialogue && dialogue->isIdle() && !finished && !waitingChoice){
        advanceStep();
    }
}

void Scene::draw(RenderWindow& window){
    if (finished && nextScene.empty()){return;}
    window.draw(bgSprite);
    if (hasCharacter && characterVisible){
    	window.draw(characterSprite);
	}
    if (dialogue){
    	dialogue->draw(window);
	}
    if (waitingTransition){
    	transition.draw(window);
	}   
}

bool Scene::isFinished() const{
    return finished;
}

string Scene::getNextScene() const{
    return nextScene;
}

void Scene::playSFX(const string& path, float volume){
    if (!resources){return;	}
    try{
        string fullPath = pathLooksLikeAssets(path)
            ? path
            : basePath + "/" + path;
        sf::SoundBuffer& buffer = resources->getSound(fullPath);
        auto sound = std::make_unique<sf::Sound>();
        sound->setBuffer(buffer);
        sound->setVolume(volume);
        sound->play();
        activeSounds.push_back(std::move(sound));
    }
    catch (const std::exception& e){
        cerr << "[System] SFX: " << e.what() << endl;
    }
}

void Scene::cleanupFinishedSounds(){
    auto it = activeSounds.begin();
    while (it != activeSounds.end()){
        if ((*it)->getStatus() == sf::Sound::Stopped){
        	it = activeSounds.erase(it);
		}else{
			++it;
		} 
    }
}
