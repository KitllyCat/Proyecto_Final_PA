#include "SaveManager.h"
#include <iostream>

SaveManager& SaveManager::getInstance() {
    static SaveManager instance;
    return instance;
}

void SaveManager::save(const string& sceneId, int stepIndex) {
    json j;
    j["scene"] = sceneId;
    j["step"] = stepIndex;

    ofstream file(savePath);
    if (!file.is_open()) {
        cerr << "SaveManager: No se pudo crear autosave.json\n";
        return;
    }

    file << j.dump(4);
    file.close();

    cout << "[AUTOSAVE] Escena: " << sceneId << " Step: " << stepIndex << endl;
}

bool SaveManager::load(string& sceneId, int& stepIndex) {
    if (!hasSave())
        return false;

    ifstream file(savePath);
    if (!file.is_open())
        return false;

    json j;
    file >> j;

    sceneId   = j.value("scene", "");
    stepIndex = j.value("step", 0);

    return !sceneId.empty();
}

bool SaveManager::hasSave() const {
    ifstream file(savePath);
    return file.good();
}

void SaveManager::clear() {
    remove(savePath.c_str());
}

bool SaveManager::exists() const {
    ifstream file(savePath);
    return file.good();
}

