#include "StageManager.h"

void StageManager::Initialize(ViewProjection* viewProjection) {

	vp_ = viewProjection; // ビュープロジェクションをセット

	//天球
	skyDome_ = std::make_unique<Skydome>();
	skyDome_->Init("WildsSkyDome.obj");
	skyDome_->SetViewProjection(vp_);
	skyDome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init();

	// コロシアム
	coliseum_ = std::make_unique<Coliseum>();
	coliseum_->Init("sphere.obj");
	coliseum_->SetViewProjection(vp_);
	coliseum_->SetScale({ 320.0f,320.0f,320.0f });// コロシアムのScale
	coliseum_->SetRadius(275.0f);

	// ステージ
	stage_ = std::make_unique<Object3d>();
	stage_->Initialize("stage/stage.obj");
	stage_->SetSize(Vector3{ size_,size_,size_ });

	wtStage_.Initialize();

}

void StageManager::Update() {

	skyDome_->Update();

	ground_->Update();

	coliseum_->Update();

	stage_->Update(wtStage_, *vp_);

	wtStage_.UpdateMatrix();
}

void StageManager::Draw() {
	skyDome_->Draw(*vp_);
	ground_->Draw(*vp_);
	stage_->Draw(wtStage_, *vp_);
}
