#include "ResourceManager.h"

Texture& ResourceManager::getTexture(const string& path) {
    if (!textures.count(path)) {
        textures[path] = Texture();
        if (!textures[path].loadFromFile(path)) {
            cout << "ERROR: No se pudo cargar textura: " << path << endl;
        }
    }
    return textures[path];
}

Font& ResourceManager::getFont(const string& path) {
    if (!fonts.count(path)) {
        fonts[path] = Font();
        if (!fonts[path].loadFromFile(path)) {
            cout << "ERROR: No se pudo cargar fuente: " << path << endl;
        }
    }
    return fonts[path];
}

SoundBuffer& ResourceManager::getSound(const string& path) {
    if (!sounds.count(path)) {
        sounds[path] = SoundBuffer();
        if (!sounds[path].loadFromFile(path)) {
            cout << "ERROR: No se pudo cargar sonido: " << path << endl;
        }
    }
    return sounds[path];
}
