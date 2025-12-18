#include "VoiceBlip.h"
#include <iostream>

VoiceBlip::VoiceBlip()
: buffer(nullptr)
{
    sound.setLoop(true); 
    sound.setVolume(60.f);
}

VoiceBlip::~VoiceBlip() {
    //Asegurar detener sonido antes de destruir buffer
    try {
        if (sound.getStatus() == Sound::Playing){
        	sound.stop();
		}
    } catch (...) {}
}

bool VoiceBlip::loadFromFile(const string& path) {
    buffer = make_unique<SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        cerr << "[System] No se pudo cargar audio: " << path << "\n";
        buffer.reset();
        return false;
    }
    sound.setBuffer(*buffer);
    return true;
}

void VoiceBlip::playLoop() {
    if (!buffer){return;}
    if (sound.getStatus() != Sound::Playing) {
        sound.setLoop(true);
        sound.play();
    }
}

void VoiceBlip::stop() {
    if (sound.getStatus() == Sound::Playing) {
        sound.stop();
        try {
            sound.setPlayingOffset(sf::Time::Zero);
        } catch (...) {}
    }
}

void VoiceBlip::setVolume(float vol) {
    sound.setVolume(vol);
}

bool VoiceBlip::isPlaying() const {
    return sound.getStatus() == Sound::Playing;
}
