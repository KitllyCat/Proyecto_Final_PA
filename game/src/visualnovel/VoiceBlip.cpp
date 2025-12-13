#include "VoiceBlip.h"
#include <iostream>

VoiceBlip::VoiceBlip()
: buffer(nullptr)
{
    sound.setLoop(true); // por defecto configuramos loop; playLoop() controlará start/stop
    sound.setVolume(60.f);
}

VoiceBlip::~VoiceBlip() {
    // Asegurar detener sonido antes de destruir buffer
    try {
        if (sound.getStatus() == Sound::Playing) sound.stop();
    } catch (...) {}
}

bool VoiceBlip::loadFromFile(const string& path) {
    buffer = make_unique<SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        std::cerr << "VoiceBlip: no se pudo cargar audio: " << path << "\n";
        buffer.reset();
        return false;
    }
    sound.setBuffer(*buffer);
    return true;
}

void VoiceBlip::playLoop() {
    if (!buffer) return; // nothing loaded
    if (sound.getStatus() != Sound::Playing) {
        sound.setLoop(true);
        sound.play();
    }
}

void VoiceBlip::stop() {
    if (sound.getStatus() == Sound::Playing) {
        sound.stop();
        // opcion: setOffset(0) no disponible en SFML 2.5? hay setPlayingOffset
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
