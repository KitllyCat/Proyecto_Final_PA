#pragma once
#include <string>
#include <fstream>
#include <map>
#include "../json.hpp"
using namespace std;
using json = nlohmann::json;

class SaveManager {
public:
    static SaveManager& getInstance();
    //Guardar progreso
    void save(const string& sceneId, int stepIndex);
    //Cargar progreso
    bool load(string& sceneId, int& stepIndex);
    //Sistema de flags
    void setFlag(const string& flagName, bool value = true);
    void setFlag(const string& flagName, int value);
    void setFlag(const string& flagName, const string& value);
    bool hasFlag(const string& flagName) const;
    bool getFlagBool(const string& flagName, bool defaultValue = false) const;
    int getFlagInt(const string& flagName, int defaultValue = 0) const;
    string getFlagString(const string& flagName, const string& defaultValue = "") const;
    //Obtener todos los flags
    map<string, json> getAllFlags() const;
    //Contar flags de un tipo
    int countFlagsWithPrefix(const string& prefix) const;
    //Verificar multiples flags
    bool hasAllFlags(const vector<string>& flags) const;
    bool hasAnyFlag(const vector<string>& flags) const;
    //Utilidades
    bool hasSave() const;
    bool exists() const;
    void clear();
    void clearFlags();
private:
    SaveManager() = default;
    string savePath = "data/autosave.json";
    //Cache de datos
    json saveData;
    bool dataLoaded = false;
    void loadData();
    void saveData_internal();
};
