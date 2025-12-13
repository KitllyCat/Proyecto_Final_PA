#ifndef VOICEBLIP_H
#define VOICEBLIP_H

#include <SFML/Audio.hpp>
#include <string>
#include <memory>

using namespace std;
using namespace sf;

class VoiceBlip {
public:
    VoiceBlip();
    ~VoiceBlip();

    // Carga un archivo .wav/.ogg. Devuelve true si se cargó correctamente.
    bool loadFromFile(const string& path);

    // Reproduce el blip en bucle (setLoop(true)). No relanza si ya está sonando.
    void playLoop();

    // Detiene la reproducción (y resetea a offset 0).
    void stop();

    // Ajusta el volumen (0..100)
    void setVolume(float vol);

    // ¿Está sonando actualmente?
    bool isPlaying() const;

private:
    // Usamos unique_ptr para gestionar el buffer en heap (evita copias)
    unique_ptr<SoundBuffer> buffer;
    Sound sound;
};

#endif
