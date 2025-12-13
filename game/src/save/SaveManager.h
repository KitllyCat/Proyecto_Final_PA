#pragma once
#include <string>
#include <fstream>
#include "../json.hpp"

using namespace std;
using json = nlohmann::json;

class SaveManager {
public:
    static SaveManager& getInstance();

    void save(const string& sceneId, int stepIndex);
    bool load(string& sceneId, int& stepIndex);

    bool hasSave() const;
    void clear();

private:
    SaveManager() = default;
    string savePath = "data/autosave.json";
};
