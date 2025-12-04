#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <iostream>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace std;
using namespace sf;

class ResourceManager {
private:
    map<string, Texture> textures;
    map<string, Font> fonts;
    map<string, SoundBuffer> sounds;

public:
    ResourceManager() {}

    Texture& getTexture(const string& path);
    Font& getFont(const string& path);
    SoundBuffer& getSound(const string& path);
};

#endif
