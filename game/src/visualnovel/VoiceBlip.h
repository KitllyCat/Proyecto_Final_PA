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
    //Carga un archivo .wav/.ogg.
    bool loadFromFile(const string& path);
    //Reproduce el blip en bucle
    void playLoop();
    //Detiene la reproduccion
    void stop();
    //Ajusta el volumen
    void setVolume(float vol);
    //Esta sonando actualmente?
    bool isPlaying() const;
private:
    //Usamos unique_ptr para gestionar el buffer en heap
    unique_ptr<SoundBuffer> buffer;
    Sound sound;
};

#endif