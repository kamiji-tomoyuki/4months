#pragma once

#include "ParticleEmitter.h"

#include "vector"

#include "json.hpp"


class ParticleEditor {

public:

	void Initialize(ViewProjection* vp);

	void Update();

	void Draw();

	void ImGui();

	void CreateEmitter();

	void LoadEmitter(const std::string& fileName);

private:

	std::vector<std::unique_ptr<ParticleEmitter>> emitters_;

	std::vector<std::string> emitterFiles_;

	std::string kDirectoryPath_ = "resources/jsons/particles/";

	ViewProjection* viewProjection_;

	float timer_;

	float preTimer_;

	float maxTime_;

	bool isStart_;

	bool isLoop_;
};