#include "JsonLoader.h"
#include <fstream>
#include <iostream>

bool JsonLoader::LoadFromFile(const std::string& filePath) {
    const std::string fullPath = "resources/jsons/" + filePath;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    try {
        file >> jsonData_;
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool JsonLoader::SaveToFile(const std::string& filePath) const {
    const std::string fullPath = "resources/jsons/" + filePath;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    try {
        file << jsonData_.dump(4);
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

std::optional<nlohmann::json> JsonLoader::GetValue(const std::string& key) const {
    if (jsonData_.contains(key)) {
        return jsonData_.at(key);
    }
    return std::nullopt;
}

void JsonLoader::SetValue(const std::string& key, const nlohmann::json& value) {
    jsonData_[key] = value;
}

void JsonLoader::RemoveValue(const std::string& key) {
    jsonData_.erase(key);
}

void JsonLoader::Clear() {
    jsonData_.clear();
}

bool JsonLoader::GetName(const std::string& filePath, const std::string& targetName) const {
    const std::string fullPath = "resources/jsons/" + filePath;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    }
    catch (const std::exception&) {
        return false;
    }
    if (!jsonData.contains("objects") || !jsonData["objects"].is_array()) {
        return false;
    }
    for (const auto& obj : jsonData["objects"]) {
        if (obj.contains("name") && obj["name"] == targetName) {
            return true;
        }
    }
    return false;
}

Vector3 JsonLoader::GetWorldTransform(const std::string& filePath, const std::string& targetName) const {
    LoadTransformData(filePath);
    const auto& fileMap = transformCache_[filePath];

    auto it = fileMap.find(targetName);
    if (it != fileMap.end() && !it->second.empty()) {
        return it->second[0];
    }

    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 JsonLoader::GetWorldTransformRandom(const std::string& filePath, const std::string& targetName) const {
    LoadTransformData(filePath);
    const auto& fileMap = transformCache_[filePath];

    std::vector<Vector3> matchedPositions;
    for (const auto& [name, positions] : fileMap) {
        if (name.find(targetName) != std::string::npos) {
            matchedPositions.insert(matchedPositions.end(), positions.begin(), positions.end());
        }
    }

    if (matchedPositions.empty()) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, matchedPositions.size() - 1);
    return matchedPositions[dist(gen)];
}

void JsonLoader::LoadTransformData(const std::string& filePath) const
{
    const std::string fullPath = "resources/jsons/" + filePath;
    if (transformCache_.count(filePath)) return;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        transformCache_[filePath] = {};
        return;
    }

    nlohmann::json jsonData;
    try {
        file >> jsonData;
    }
    catch (const std::exception&) {
        transformCache_[filePath] = {};
        return;
    }

    if (!jsonData.contains("objects") || !jsonData["objects"].is_array()) {
        transformCache_[filePath] = {};
        return;
    }

    std::map<std::string, std::vector<Vector3>> nameToPositions;
    for (const auto& obj : jsonData["objects"]) {
        if (obj.contains("name") && obj.contains("transform") && obj["transform"].contains("translation")) {
            std::string name = obj["name"];
            std::vector<float> pos = obj["transform"]["translation"];
            if (pos.size() == 3) {
                nameToPositions[name].emplace_back(pos[0], pos[1], pos[2]);
            }
        }
    }

    transformCache_[filePath] = std::move(nameToPositions);
}
