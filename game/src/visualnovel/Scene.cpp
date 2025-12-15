// src/visualnovel/Scene.cpp
#include "Scene.h"
#include "../save/SaveManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

Scene::Scene()
: resources(nullptr),
  currentIndex(0),
  characterAnimator(nullptr),
  hasCharacter(false),
  characterPosition(800.f, 400.f),
  waitingChoice(false),
  finished(false),
  onMusicChange(nullptr),
  waitingTransition(false)  // ← NUEVO
{
}

void Scene::setScreenSize(Vector2u size) {
    transition.setScreenSize(size);
}

void Scene::setMusicChangeCallback(MusicChangeCallback callback) {
       onMusicChange = callback;
   }

string Scene::dirname(const string& path) {
    size_t p = path.find_last_of("/\\");
    if (p == string::npos) return ".";
    return path.substr(0, p);
}

bool Scene::pathLooksLikeAssets(const string& p) {
    if (p.size() < 7) return false;
    string start = p.substr(0, 7);
    for (auto& c : start) c = tolower(c);
    return start == "assets/";
}

bool Scene::loadFromFile(const string& path, ResourceManager& res, int startIndex){
    resources = &res;
    scenePath = path;

    ifstream f(path, ios::binary);
    if (!f.is_open()) {
        cout << "Scene: no se pudo abrir " << path << endl;
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

    if (j.contains("bg")) {
        string bg = j["bg"];
        string full = pathLooksLikeAssets(bg) ? bg : basePath + "/" + bg;
        bgSprite.setTexture(resources->getTexture(full));
    }
    
	hasCharacter = false;
	characterAnimator.reset();
	
	if (j.contains("character")) {
	    auto c = j["character"];
	
	    string f1 = c.value("frame1", "");
	    string f2 = c.value("frame2", "");
	    int fps = c.value("fps", 8);
	
	    float x = c.value("x", characterPosition.x);
	    float y = c.value("y", characterPosition.y);
	    characterPosition = { x, y };
	
	    if (!f1.empty() && !f2.empty()) {
	        string p1 = pathLooksLikeAssets(f1) ? f1 : basePath + "/" + f1;
	        string p2 = pathLooksLikeAssets(f2) ? f2 : basePath + "/" + f2;
	
	        try {
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
	        catch (...) {
	            hasCharacter = false;
	        }
	    }
	}
	
    json arr = j.contains("steps") ? j["steps"] : j["sequence"];
    for (auto& item : arr) {
        SceneStep s;
        s.type = item.value("type", "dialogue");

        if (s.type == "dialogue") {
            s.speaker = item.value("speaker", "");
            s.text = item.value("text", "");
	        if (item.contains("sfx")) {
				s.sfx_path = item.value("sfx", "");
				s.sfx_volume = item.value("sfx_volume", 100.0f);
			}
		}
        else if (s.type == "change_bg") {
	       s.bg_path = item.value("bg", "");
	       if (item.contains("music")) {
	           s.music_path = item.value("music", "");
	       }
	   	}else if (s.type == "choice") {
		    if (!item.contains("choices") || !item["choices"].is_array()) {
		        cerr << "[Scene] ERROR: 'choice' step debe tener array 'choices'" << endl;
		        continue;
		    }
		    
		    for (auto& c : item["choices"]) {
		        SceneStep::Choice ch;
		        
		        // Texto de la opción (obligatorio)
		        ch.text = c.value("text", "");
		        
		        // goto_scene: Cambiar a otra escena (opcional)
		        if (c.contains("goto")) {
		            ch.goto_scene = c["goto"].get<string>();
		        } else if (c.contains("next")) {
		            ch.goto_scene = c["next"].get<string>();
		        } else {
		            ch.goto_scene = "";
		        }
		        
		        // goto_step: Saltar a un step específico en esta escena (opcional)
		        if (c.contains("goto_step")) {
		            try {
		                ch.goto_step = c["goto_step"].get<int>();
		            } catch (...) {
		                ch.goto_step = -1;
		            }
		        } else {
		            ch.goto_step = -1;
		        }
		        
		        // flag: Flag a guardar cuando se elige (opcional)
		        if (c.contains("flag")) {
		            ch.flag = c["flag"].get<string>();
		        } else {
		            ch.flag = "";
		        }
		        
		        // require_flag: Flag necesario para que aparezca (opcional)
		        if (c.contains("require_flag")) {
		            ch.require_flag = c["require_flag"].get<string>();
		        } else {
		            ch.require_flag = "";
		        }
		        
		        s.choices.push_back(ch);
		        
		        // Log para debugging
		        cout << "[Scene] Choice: '" << ch.text << "'";
		        if (!ch.goto_scene.empty()) {
		            cout << " -> escena: " << ch.goto_scene;
		        } else if (ch.goto_step >= 0) {
		            cout << " -> step: " << ch.goto_step;
		        }
		        if (!ch.flag.empty()) {
		            cout << " (flag: " << ch.flag << ")";
		        }
		        if (!ch.require_flag.empty()) {
		            cout << " (requiere: " << ch.require_flag << ")";
		        }
		        cout << endl;
		    }
		}else if (s.type == "play_sfx") {
		    s.sfx_path = item.value("sound", "");
		    if (s.sfx_path.empty()) {
		        s.sfx_path = item.value("sfx", "");
		    }
		    s.sfx_volume = item.value("volume", 100.0f);
		}else if (s.type == "transition") {
		    s.effect = item.value("effect", "fade");
		    s.duration = item.value("duration", 1.0f);
		    cout << "[Scene] transition: " << s.effect << " (" << s.duration << "s)" << endl;
		}
        // checkpoint no necesita datos
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
    if (currentIndex < steps.size())
        startStep(steps[currentIndex]);
    else
        finished = true;

    return true;
}

void Scene::startStep(const SceneStep& s) {
    // Limpiar sonidos terminados
    cleanupFinishedSounds();
    
    if (s.type == "checkpoint") {
        string id = scenePath.substr(scenePath.find_last_of("/\\") + 1);
        id = id.substr(0, id.find(".json"));
        SaveManager::getInstance().save(id, currentIndex);
        advanceStep();
        return;
    }

    if (s.type == "dialogue") {
        dialogue->setDialogue(s.speaker, s.text);
        if (!s.sfx_path.empty()) {
            playSFX(s.sfx_path, s.sfx_volume);
        }
    }
    else if (s.type == "change_bg") {
        string full = pathLooksLikeAssets(s.bg_path)
                    ? s.bg_path
                    : basePath + "/" + s.bg_path;
        bgSprite.setTexture(resources->getTexture(full));
        
        if (!s.music_path.empty() && onMusicChange) {
            onMusicChange(s.music_path);
        }
        
        advanceStep();
    }
    else if (s.type == "play_sfx") {
        if (!s.sfx_path.empty()) {
            playSFX(s.sfx_path, s.sfx_volume);
        }
        advanceStep();
    }
    //Manejar transiciones
    else if (s.type == "transition") {
        cout << "[Scene] Iniciando transición: " << s.effect << endl;
        
        if (s.effect == "fade" || s.effect == "fade_to_black") {
            transition.start(TransitionManager::Type::FADE_TO_BLACK, s.duration);
            waitingTransition = true;
        }
        else if (s.effect == "fade_from_black") {
            transition.start(TransitionManager::Type::FADE_FROM_BLACK, s.duration);
            waitingTransition = true;
        }
        else {
            cout << "[Scene] Efecto de transición desconocido: " << s.effect << endl;
            advanceStep();
        }
    }else if (s.type == "choice") {
	    waitingChoice = true;
	    // NUEVO: Filtrar choices según flags requeridos
	    vector<SceneStep::Choice> availableChoices;
	    for (const auto& choice : s.choices) {
	        // Si requiere un flag, verificar si existe
	        if (!choice.require_flag.empty()) {
	            if (!SaveManager::getInstance().hasFlag(choice.require_flag)) {
	                cout << "[Scene] Choice '" << choice.text 
	                     << "' oculta (falta flag: " << choice.require_flag << ")" << endl;
	                continue;  // Saltar esta opción
	            }
	        }
	        availableChoices.push_back(choice);
	    }
	    
	    // Verificar que haya al menos una opción disponible
	    if (availableChoices.empty()) {
	        cerr << "[Scene] ERROR: Todas las choices están bloqueadas por flags" << endl;
	        // Fallback: avanzar al siguiente step
	        waitingChoice = false;
	        advanceStep();
	        return;
	    }
	    
	    // Construir texto con choices disponibles (numeradas desde 1)
	    string text;
	    for (size_t i = 0; i < availableChoices.size(); ++i) {
	        text += to_string(i + 1) + ". " + availableChoices[i].text + "\n";
	    }
	    
	    // Guardar temporalmente las choices disponibles
	    // (sobrescribir el vector original para este step)
	    steps[currentIndex].choices = availableChoices;
	    
	    dialogue->setDialogue("Elige", text);
	}
}

void Scene::advanceStep() {
    if (++currentIndex < steps.size())
        startStep(steps[currentIndex]);
    else
        finished = true;
}

void Scene::update(float dt) {
    // Limpiar sonidos periódicamente
    static float cleanupTimer = 0.0f;
    cleanupTimer += dt;
    if (cleanupTimer >= 0.5f) {
        cleanupFinishedSounds();
        cleanupTimer = 0.0f;
    }
    
    // ✅ NUEVO: Actualizar transición activa
    if (waitingTransition) {
        transition.update(dt);
        
        // Si la transición terminó, avanzar al siguiente step
        if (transition.isComplete()) {
		    cout << "[Scene] Transición completada" << endl;
		    waitingTransition = false;
		    transition.reset();
		    advanceStep();
		}
        
        // Mientras hay transición, no actualizar diálogos ni personajes
        return;
    }
    
    // Actualizar diálogos y animaciones (solo si no hay transición)
    if (dialogue) dialogue->update(dt);
    if (hasCharacter && characterAnimator)
        characterAnimator->update(dt);
}

void Scene::handleEvent(const Event& ev) {
    // ✅ NUEVO: No procesar eventos durante transiciones
    if (waitingTransition) {
        // Opcionalmente, permitir saltar la transición con Escape
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape) {
            cout << "[Scene] Transición saltada por usuario" << endl;
            transition.complete();
        }
        return;
    }
    // Resto del código de handleEvent (sin cambios)
    if (waitingChoice) {
	    if (ev.type == Event::KeyPressed) {
	        int choiceIndex = -1;
	        
	        // Soportar teclas numéricas del teclado principal
	        if (ev.key.code >= Keyboard::Num1 && ev.key.code <= Keyboard::Num9) {
	            choiceIndex = ev.key.code - Keyboard::Num1;
	        }
	        // Soportar teclas del numpad
	        else if (ev.key.code >= Keyboard::Numpad1 && ev.key.code <= Keyboard::Numpad9) {
	            choiceIndex = ev.key.code - Keyboard::Numpad1;
	        }
	        
	        // Validar que la opción existe
	        if (choiceIndex >= 0 && choiceIndex < (int)steps[currentIndex].choices.size()) {
	            const auto& chosen = steps[currentIndex].choices[choiceIndex];
	            
	            cout << "[Scene] ✓ Opción " << (choiceIndex + 1) << " seleccionada: " 
	                 << chosen.text << endl;
	            
	            // NUEVO: Guardar flag si está definido
	            if (!chosen.flag.empty()) {
	                SaveManager::getInstance().setFlag(chosen.flag, true);
	                cout << "[Scene] Flag guardado: " << chosen.flag << endl;
	            }
	            
	            // NUEVO: Decidir si cambiar de escena o hacer branching interno
	            if (!chosen.goto_scene.empty()) {
	                // Cambiar a otra escena
	                cout << "[Scene] → Cambiar a escena: " << chosen.goto_scene << endl;
	                nextScene = chosen.goto_scene;
	                waitingChoice = false;
	                finished = true;
	            } 
	            else if (chosen.goto_step >= 0) {
	                // Saltar a un step específico (branching interno)
	                cout << "[Scene] → Saltar a step: " << chosen.goto_step << endl;
	                currentIndex = chosen.goto_step;
	                waitingChoice = false;
	                
	                if (currentIndex < steps.size()) {
	                    startStep(steps[currentIndex]);
	                } else {
	                    cerr << "[Scene] ERROR: goto_step fuera de rango: " 
	                         << chosen.goto_step << endl;
	                    finished = true;
	                }
	            }
	            else {
	                // No hay goto_scene ni goto_step: continuar al siguiente step
	                cout << "[Scene] → Continuar en esta escena" << endl;
	                waitingChoice = false;
	                advanceStep();
	            }
	            
	        } else if (choiceIndex >= (int)steps[currentIndex].choices.size()) {
	            // Tecla numérica válida pero fuera de rango
	            cout << "[Scene] ✗ Opción " << (choiceIndex + 1) << " no existe. "
	                 << "Opciones disponibles: 1-" << steps[currentIndex].choices.size() << endl;
	        }
	    }
	    
	    return;  // No procesar otros eventos durante choices
	}
    
    if (dialogue) {
        dialogue->handleEvent(ev);
    }
    
    if (dialogue && dialogue->isIdle() && !finished && !waitingChoice) {
        advanceStep();
    }
}

void Scene::draw(RenderWindow& window) {
    // Dibujar fondo
    window.draw(bgSprite);
    
    // Dibujar personaje
    if (hasCharacter)
        window.draw(characterSprite);
    
    // Dibujar diálogo
    if (dialogue)
        dialogue->draw(window);
    
    // NUEVO: Dibujar transición POR ENCIMA DE TODO
    transition.draw(window);
}

bool Scene::isFinished() const { return finished; }
string Scene::getNextScene() const { return nextScene; }

void Scene::playSFX(const string& path, float volume) {
    if (!resources) return;
    
    try {
        string fullPath = pathLooksLikeAssets(path) ? path : basePath + "/" + path;
        SoundBuffer& buffer = resources->getSound(fullPath);
        
        Sound sound;
        sound.setBuffer(buffer);
        sound.setVolume(volume);
        sound.play();
        
        activeSounds.push_back(sound);
    }
    catch (const exception& e) {
        cerr << "[Scene ERROR] SFX: " << e.what() << endl;
    }
}

void Scene::cleanupFinishedSounds() {
    auto it = activeSounds.begin();
    while (it != activeSounds.end()) {
        if (it->getStatus() == Sound::Stopped) {
            it = activeSounds.erase(it);
        } else {
            ++it;
        }
    }
}
