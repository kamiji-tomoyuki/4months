#pragma once

#include "ParticleEmitter.h"

#include "vector"

class ParticleEditor {

public:

	void Initialize(ViewProjection* vp);

	void Update();

	void Draw();

	void ImGui();

	void SaveEmitterData(ParticleEmitter* emitter);

	void LoadEmitterData(const std::string& fileName);

	void CreateEmitter(const std::string& name, const std::string& fileName);

private:

	std::vector<std::unique_ptr<ParticleEmitter>> emitters_;

	ViewProjection* viewProjection_;

	float timer_;

	float preTimer_;

	float maxTime_;

	bool isStart_;

	bool isLoop_;
};