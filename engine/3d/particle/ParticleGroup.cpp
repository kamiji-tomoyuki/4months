#include "ParticleGroup.h"
#include "fstream"

void ParticleGroup::Initialize(const std::string& fileName) {

	nlohmann::json jsonData;

	std::string kDirectoryPath = "resources/jsons/particles/group/";

	std::string filePath = kDirectoryPath + fileName;

	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEmitter", 0);
		assert(0);
		return;
	}

	file >> jsonData;

	file.close();


}

void ParticleGroup::Update() {
}

void ParticleGroup::Draw() {
}
