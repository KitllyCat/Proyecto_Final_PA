#include "SaveManager.h"
#include <iostream>

SaveManager& SaveManager::getInstance() {
    static SaveManager instance;
    return instance;
}

void SaveManager::loadData() {
    if (dataLoaded) return;
    
    ifstream file(savePath);
    if (file.is_open()) {
        try {
            file >> saveData;
            dataLoaded = true;
        } catch (const exception& e) {
            cerr << "[SaveManager] Error al cargar datos: " << e.what() << endl;
            saveData = json::object();
        }
        file.close();
    } else {
        saveData = json::object();
    }
    
    // Asegurar que existe la estructura de flags
    if (!saveData.contains("flags")) {
        saveData["flags"] = json::object();
    }
    
    dataLoaded = true;
}

void SaveManager::saveData_internal() {
    ofstream file(savePath);
    if (!file.is_open()) {
        cerr << "[SaveManager] No se pudo crear autosave.json\n";
        return;
    }

    file << saveData.dump(4);
    file.close();
}

void SaveManager::save(const string& sceneId, int stepIndex) {
    loadData();
    
    saveData["scene"] = sceneId;
    saveData["step"] = stepIndex;
    
    saveData_internal();
    
    cout << "[AUTOSAVE] Escena: " << sceneId << " Step: " << stepIndex << endl;
}

bool SaveManager::load(string& sceneId, int& stepIndex) {
    loadData();
    
    if (!saveData.contains("scene")) {
        return false;
    }

    sceneId = saveData.value("scene", "");
    stepIndex = saveData.value("step", 0);

    return !sceneId.empty();
}

// SISTEMA DE FLAGS

void SaveManager::setFlag(const string& flagName, bool value) {
    loadData();
    saveData["flags"][flagName] = value;
    saveData_internal();
    
    cout << "[SaveManager] Flag guardado: " << flagName << " = " << value << endl;
}

void SaveManager::setFlag(const string& flagName, int value) {
    loadData();
    saveData["flags"][flagName] = value;
    saveData_internal();
    
    cout << "[SaveManager] Flag guardado: " << flagName << " = " << value << endl;
}

void SaveManager::setFlag(const string& flagName, const string& value) {
    loadData();
    saveData["flags"][flagName] = value;
    saveData_internal();
    
    cout << "[SaveManager] Flag guardado: " << flagName << " = " << value << endl;
}

bool SaveManager::hasFlag(const string& flagName) const {
    const_cast<SaveManager*>(this)->loadData();
    
    if (!saveData.contains("flags")) return false;
    return saveData["flags"].contains(flagName);
}

bool SaveManager::getFlagBool(const string& flagName, bool defaultValue) const {
    const_cast<SaveManager*>(this)->loadData();
    
    if (!saveData.contains("flags") || !saveData["flags"].contains(flagName)) {
        return defaultValue;
    }
    
    try {
        return saveData["flags"][flagName].get<bool>();
    } catch (...) {
        return defaultValue;
    }
}

int SaveManager::getFlagInt(const string& flagName, int defaultValue) const {
    const_cast<SaveManager*>(this)->loadData();
    
    if (!saveData.contains("flags") || !saveData["flags"].contains(flagName)) {
        return defaultValue;
    }
    
    try {
        return saveData["flags"][flagName].get<int>();
    } catch (...) {
        return defaultValue;
    }
}

string SaveManager::getFlagString(const string& flagName, const string& defaultValue) const {
    const_cast<SaveManager*>(this)->loadData();
    
    if (!saveData.contains("flags") || !saveData["flags"].contains(flagName)) {
        return defaultValue;
    }
    
    try {
        return saveData["flags"][flagName].get<string>();
    } catch (...) {
        return defaultValue;
    }
}

map<string, json> SaveManager::getAllFlags() const {
    const_cast<SaveManager*>(this)->loadData();
    
    map<string, json> result;
    
    if (saveData.contains("flags")) {
        for (auto& [key, value] : saveData["flags"].items()) {
            result[key] = value;
        }
    }
    
    return result;
}

int SaveManager::countFlagsWithPrefix(const string& prefix) const {
    const_cast<SaveManager*>(this)->loadData();
    
    int count = 0;
    
    if (saveData.contains("flags")) {
        for (auto& [key, value] : saveData["flags"].items()) {
            if (key.find(prefix) == 0) {  // Empieza con prefix
                // Si es booleano, solo contar los true
                if (value.is_boolean() && value.get<bool>()) {
                    count++;
                } else if (!value.is_boolean()) {
                    count++;
                }
            }
        }
    }
    
    return count;
}

bool SaveManager::hasAllFlags(const vector<string>& flags) const {
    for (const auto& flag : flags) {
        if (!hasFlag(flag) || !getFlagBool(flag, false)) {
            return false;
        }
    }
    return true;
}

bool SaveManager::hasAnyFlag(const vector<string>& flags) const {
    for (const auto& flag : flags) {
        if (hasFlag(flag) && getFlagBool(flag, false)) {
            return true;
        }
    }
    return false;
}

// UTILIDADES

bool SaveManager::hasSave() const {
    ifstream file(savePath);
    return file.good();
}

bool SaveManager::exists() const {
    ifstream file(savePath);
    return file.good();
}

void SaveManager::clear() {
    remove(savePath.c_str());
    saveData = json::object();
    dataLoaded = false;
    
    cout << "[SaveManager] Guardado limpiado completamente" << endl;
}

void SaveManager::clearFlags() {
    loadData();
    saveData["flags"] = json::object();
    saveData_internal();
    
    cout << "[SaveManager] Flags limpiados (progreso mantenido)" << endl;
}
