#include "TitleGround.h"

void TitleGround::Initialize() {

	ground_ = std::make_unique<Object3d>();
	ground_->Initialize("GameScene/ball.obj");

	wtGround_.Initialize();
	wtGround_.scale_ = { 325.0f,0.01f,325.0f };

	stage_ = std::make_unique<Object3d>();
	stage_->Initialize("stage/stage.obj");
	stage_->SetSize({ 15.0f,15.0f,15.0f });

	wtStage_.Initialize();
}

void TitleGround::Update() {

	ground_->Update(wtGround_, *vp_);

	wtGround_.UpdateMatrix();

	stage_->Update(wtStage_, *vp_);

	wtStage_.UpdateMatrix();
}

void TitleGround::Draw() {

	ground_->Draw(wtGround_, *vp_);

	stage_->Draw(wtStage_, *vp_);
}