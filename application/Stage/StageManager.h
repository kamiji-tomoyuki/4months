#pragma once

#include "Skydome.h"
#include "Ground.h"
#include "Coliseum.h"

#include "memory"

class StageManager {

public:

	void Initialize(ViewProjection* viewProjection);

	void Update();

	void Draw();

private:

	ViewProjection* vp_ = nullptr; // ビュープロジェクション

	std::unique_ptr<Skydome> skyDome_ = nullptr;// 天球
	std::unique_ptr<Ground> ground_ = nullptr;//地面
	std::unique_ptr<Coliseum> coliseum_ = nullptr;//コロシアム

	// ステージ
	std::unique_ptr<Object3d> stage_;
	WorldTransform wtStage_;
	float size_ = 15.0f;
};