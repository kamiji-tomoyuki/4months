#include "ClearScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>
#include"line/DrawLine3D.h"

#include "Soldier.h"

/// 終了
void ClearScene::Finalize() {

}

/// 初期化
void ClearScene::Initialize() {
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	particleManager_ = ParticleManager::GetInstance();

	enemiesDistance_ = 10.0f;

	enemyMaxColumn_ = 6;

	vp_.Initialize();
	vp_.translation_ = { 0.0f,10.0f,-30.0f };
	vp_.rotation_ = { 0.1f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	//天球
	skyDome_ = std::make_unique<Skydome>();
	skyDome_->Init("WildsSkyDome.obj");
	skyDome_->SetViewProjection(&vp_);
	skyDome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init();

	//タイム
	timeManager_ = std::make_unique<TimeManager>();
	timeManager_->Initialize();
	timeManager_->SetTimer("start", 2.0f / 60.0f);

	//プレイヤー
	Player::SetPlayerID(0);
	player_ = std::make_unique<Player>();
	player_->SetTimeManager(timeManager_.get());
	player_->Init();
	player_->SetViewProjection(&vp_);
	player_->SetPosition({ 0.0f,0.0f,0.0f });
	player_->SetRotation({ 0.0f,3.14f,0.0f });

	player_->Update();

	//エネミー
	Enemy::SetEnemyID(0);

	for (int i = -enemyMaxColumn_ + 1; i <= enemyMaxColumn_ + 1; i += 2) {
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();
		newEnemy->SetPlayer(player_.get());
		newEnemy->SetTimeManager(timeManager_.get());
		newEnemy->Init();
		newEnemy->SetTranslation({ i * enemiesDistance_,0.0f,50.0f});
		enemies_.push_back(std::move(newEnemy));
	}

	for (auto& enemy : enemies_) {
		enemy->Update();

		enemy->SetRotation({ enemy->GetCenterRotation().x - 1.0f,enemy->GetCenterRotation().y,enemy->GetCenterRotation().z });

		enemy->UpdateTransform();
	}

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("Clear.json");
	emitter_->Start();
}

/// 更新
void ClearScene::Update() {
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG

	skyDome_->Update();

	ground_->Update();

	timeManager_->Update();

	for (auto& enemy : enemies_) {

		enemy->SetRotation({ enemy->GetCenterRotation().x,enemy->GetCenterRotation().y + 0.01f,enemy->GetCenterRotation().z });

		enemy->UpdateTransform();
	}

	emitter_->Update();

	particleManager_->Update(vp_);

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

}

/// 描画
void ClearScene::Draw() {
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----

	//------------------------------


	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	skyDome_->Draw(vp_);
	ground_->Draw(vp_);
	player_->Draw(vp_);
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(vp_);
	}

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	particleManager_->Draw();

	//-----------------------------

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

/// オフスクリーン上に描画
void ClearScene::DrawForOffScreen() {
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----

	//------------------------

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----

	//------------------------------


	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	//-----------------------------


	/// ----------------------------------

	/// -------描画処理終了-------
}

void ClearScene::Debug() {
	ImGui::Begin("ClearScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::End();
}

void ClearScene::CameraUpdate() {
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		vp_.UpdateMatrix();
	}
}

void ClearScene::ChangeScene() {
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A || input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->NextSceneReservation("TITLE");
	}
}
