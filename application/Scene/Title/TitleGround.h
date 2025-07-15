#pragma once

#include "Ground.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include "memory"
#include "vector"

class TitleGround {

public:

	void Initialize();

	void Update();

	void Draw();

public:

	void SetViewProjection(ViewProjection* vp) { vp_ = vp; }

	WorldTransform& GetGroundWT() { return wtGround_; }

	WorldTransform& GetStageWT() { return wtStage_; }

private:

	ViewProjection* vp_;

	std::unique_ptr<Object3d> ground_;

	std::unique_ptr<Object3d> stage_;

	WorldTransform wtGround_;

	WorldTransform wtStage_;
};