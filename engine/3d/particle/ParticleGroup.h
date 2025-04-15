#pragma once

#include "ParticleEmitter.h"
#include "ParticleManager.h"

class ParticleGroup {

public:

	void Initialize(const std::string& fileName);

	void Update();

	void Draw();

private:

	std::string name_;

	std::vector<ParticleEmitter> emitters_;
};