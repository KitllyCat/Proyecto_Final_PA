# Proyecto: *Remoria~*  
**Visual Novel creada en C++ con SFML**

Este repositorio contiene el desarrollo de una novela visual centrada en una atmósfera nostálgica y retro. El proyecto busca recrear la sensación de un recuerdo fragmentado, usando gráficos simples, animaciones mínimas y una paleta suave inspirada en ilustraciones sobre papel...

*Remoria* sigue la historia de Kami que intenta reconstruir momentos importantes de su infancia. A través de sus recuerdos, elecciones y pequeñas interacciones, el jugador explora memorias que mezclan ternura, confusión y ciertos matices emocionales más profundos. La experiencia está diseñada para sentirse como mirar un álbum de recuerdos, donde cada imagen parece desvanecerse lentamente con el tiempo.

## Software utilizadas

Este proyecto se desarrolla con:

- **DevC++ 6.3**
  - Compilador del proyecto
- **SFML 2.5.1**
  - Graphics (renderizado 2D)
  - Window
  - System
  - Audio (música y efectos)
- **Aseprite**
  - Sprites semi-pixelart y fondos
- **Audacity**
  - Edicion de musica y audio
- **nlohmann/json**
  - Diálogos y estructura de escenas en formato .json

## Dispocision de carpetas
```bash
/game
├── assets/
│   ├── audio/
│   │   ├── effects/
│   ├── fonts/
│   └── images/
│   │   ├── characters/
│   │   ├── backgrounds/
│   │   ├── ui/
│   │   ├── intro/
│   │   └── icon.png (e .ico)
├── data/
│   ├── game_config.json
│   ├── scenes/
│   │   ├── prologue.json
│   │   └── [...]
│   └── autosave.json
├── src/
│   ├── core/
│   │   ├── ResourceManager.h
│   │   └── ResourceManager.cpp
│   ├── graphics/
│   │   ├── SpriteAnimator.h
│   │   ├── SpriteAnimator.cpp
│   │   ├── TransitionEffects.h
│   │   └── TransitionEffects.cpp
│   ├── icon/
│   │   └── icon.rc
│   ├── save/
│   │   ├── SaveManager.h
│   │   └── SaveManager.cpp
│   ├── visualnovel/
│   │   ├── DialogueBox.h
│   │   ├── DialogueBox.cpp
│   │   ├── Scene.h
│   │   ├── Scene.cpp
│   │   ├── SceneManager.h
│   │   ├── SceneManager.cpp
│   │   ├── VoiceBlip.h
│   │   └── VoiceBlip.cpp
│   └── json.hpp
├── Remoria.exe
├── main.cpp
├── MainMenu.h
├── MainMenu.cpp
├── CreditsScreen.h
├── CreditsScreen.cpp
├── IntroScreen.h
├── IntroScreen.cpp
└── README.md
```

## Builds

El juego en desarrollo tuvo diferentes builds finalizadas con una parte del progreso que avanzaba relativamente. Progresion del juego:
- **pre-alpha   v0.1:**

Fase inicial, primeros archivos creados y base con RecourceManger, DialogueBox, SceneManager y Scene han sido creados. Solo tiene lectura de .json basica con strings limitados y todo silencio.

- **pre-alpha   v0.2:**

Continuacion de la fase inicial, ahora con el SpriteAnimator, se pueden importar imagenes como personajes y animarlos, aun asi el juego se sigue viendo tosco y en desarrollo.

- **alpha       v0.3:**

Se implemento el VoiceBlip y las Transiciones entre escenas base. Se siente con un poco mas de vida, pero faltan implementr mas cosas, tambien ahora se puede cargar musica desde .json

- **pre-build   v0.4:**

Aqui se creo el MainMenu, un diseño rudimentario y tosco con escalibilidad de mejorarse mas adelante. Funciones en los demas archivos pulidas y un sistema de flags añadido.

- **pre-alpha   v0.5:**

Se añadio el IntroScreen y CreditsScreen. Estas nuevas funcionalidades daban vida al juego, que a su vez se mejoro relativamente el MainMenu visualmente. Se encontro un bug critico.

- **build       v0.6:**

Bug critico con los creditos y un bucle infinito de loop solucionado. Se añadio un icono al juego, ademas de mejoro el sistema de cargado de texturas al no usar punteros inexistentes. El cargado de .json ya esta disponible para el desarrollo de escenas y desde este punto se puede comenzar a trabajar en el juego en sí, pero siempre revisando el codigo.

- **build        v0.6+:**

Se mejoro ligeramente el codigo, se añadio un goto de scenas sin la necesidad de choise, y un sistema para oculpar al personaje si es que se desea, una leve mejora a la version anterior y quizas la que se empiece a trabajar en el juego.

- **Beta         v0.6.9+:**

Implementado el sistema de ecalar a ventana en caso que la resolución sea menor, pero afecta a los botones del juego y a los efectos. Y es jugable a modo de historia, Capitulo 1 y 2 añadidos.

- **Beta         v.0.7:**
En desarrollo...

***(Siendo esta la ultima actualizacion disponible...)***

## Compilacion del juego

Puedes descargar el game.rar, y luego descomprimirlo para poder jugarlo. Se encontrara el ejecutable .exe listo para su uso.

En caso que se desee compilar desde 0 el juego, el prodecimiento se explica en el siguiente docs:

[![Google Docs](https://img.shields.io/badge/Google%20Docs-4285F4?style=flat&logo=google&logoColor=white)](https://docs.google.com/document/d/1U7nhNjDhPBOscAGJqNWPGV12pQJSWbbs4f7op4hos8U/view?usp=sharing)

## Nota

Este README será actualizado progresivamente conforme el proyecto avance...
¡¡¡Gracias por visitar nuestro repositorio nwn!!!
- 
## Autores

- **Cesar Jimmy Chambe Mamani**     [![GitHub](https://img.shields.io/badge/GitHub-000?style=flat&logo=github&logoColor=white)](https://github.com/KitllyCat)
- **Luis Fernando Chura Coaquira**     [![GitHub](https://img.shields.io/badge/GitHub-000?style=flat&logo=github&logoColor=white)](https://github.com/AlguienXD888)
