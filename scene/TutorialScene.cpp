#include "TutorialScene.h"

#include "LightGroup.h"
#include "SceneManager.h"

#include "line/DrawLine3D.h"

void TutorialScene::Initialize() {

	/// === シングルトンインスタンスの取得 === ///

	audio_ = Audio::GetInstance();

	objCommon_ = Object3dCommon::GetInstance();

	spCommon_ = SpriteCommon::GetInstance();

	ptCommon_ = ParticleCommon::GetInstance();

	input_ = Input::GetInstance();

	input_->SetJoystickDeadZone(0, 3000, 3000);

	/// === 天球の初期化 === ///

	skydome_ = std::make_unique<Skydome>();

	skydome_->Init("WildsSkyDome.obj");

	skydome_->SetViewProjection(&vp_);

	skydome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale

	/// === 地面の初期化 === ///

	ground_ = std::make_unique<Ground>();

	ground_->Init();

	/// === タイムマネージャーの初期化 === ///

	timeManager_ = std::make_unique<TimeManager>();

	timeManager_->Initialize();

	timeManager_->SetTimer("start", 2.0f / 60.0f);

	/// === プレイヤーの初期化 === ///

	Player::SetPlayerID(0);

	player_ = std::make_unique<Player>();

	player_->SetTimeManager(timeManager_.get());

	player_->Init();

	player_->SetViewProjection(&vp_);

	player_->SetPosition({ 0.0f,1.750f,-50.0f });

	/// === エネミーの初期化 === ///

	Enemy::SetEnemyID(0);

	for (size_t i = 0; i < 1; i++) {

		std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();

		newEnemy->SetPlayer(player_.get());

		newEnemy->SetTimeManager(timeManager_.get());

		newEnemy->Init();

		newEnemy->SetTranslation({ 0.0f,1.750f,0.0f });

		enemies_.push_back(std::move(newEnemy));
	}

	/// === カメラの初期化 === ///

	//デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>();

	debugCamera_->Initialize(&vp_);

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();

	followCamera_->Initialize();

	followCamera_->SetTarget(&player_->GetWorldTransform());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	player_->SetFollowCamera(followCamera_.get());

	/// === ロックオンの初期化 === ///

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();

	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());

	player_->SetLockOn(lockOn_.get());

	/// === HPBarの初期化 === ///

	hpBar_ = std::make_unique<Sprite>();

	hpBar_->Initialize("hp.png", Vector2(400.0f, 700.0f)); // 下に配置

	hpBar_->SetSize(Vector2(70.0f, 500.0f)); // 横幅を少し太く

	hpBar_->SetRotation(-1.57f);

	hpBar_->SetAnchorPoint({ 0.0f,0.0f });

	/// === エミッターの初期化 === ///
  
	particleManager_ = ParticleManager::GetInstance();

	starEmitter_ = std::make_unique<ParticleEmitter>();

	starEmitter_->Initialize("Star.json");

	starEmitter_->Start();

	tutorialEvent_ = std::make_unique<TutorialEvent>();

	tutorialEvent_->Initialize();

	tutorialEvent_->SetPlayer(player_.get());

	/// === オーディオの設定 === ///

	//audio_->StopWave(0);

	//audio_->StopWave(1);

	//audio_->StopWave(2);

	//audio_->StopWave(3);

	//audio_->StopWave(4);

	//audio_->PlayWave(1, 0.1f, true);

	//audio_->PlayWave(7, 1.0f, false);
}

void TutorialScene::Finalize() {
}

void TutorialScene::Update() {
#ifdef _DEBUG

	// デバッグ
	Debug();
#endif // _DEBUG

	for (auto& enemy : enemies_) {
		if (!enemy->GetIsAlive()) {
			tutorialEvent_->SetEnemy(nullptr);
		}
	}

	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		if (!enemy->GetIsAlive()) {
			return true;
		}
		return false;
		});

	//タイムマネージャーの更新
	timeManager_->Update();

	ReSpawnEnemy();

	//プレイヤーの更新
	player_->Update();
	//プレイヤーのパーティクル更新
	player_->UpdateParticle(vp_);

	bool skipStart = false;

	for (auto& enemy : enemies_) {

		if (enemies_.size() == 1) {
			break;
		}

		if (!skipStart) {

			//エネミーの更新
			enemy->Update();

			tutorialEvent_->SetEnemy(enemy.get());

			//エネミーのパーティクル更新
			enemy->UpdateParticle(vp_);

			skipStart = true;
		}
	}

	//天球の更新
	skydome_->Update();

	//地面の更新
	ground_->Update();

	//HPの比率を計算
	float hpRatio = static_cast<float>(player_->GetHP()) / 10000;
	//HPに応じた高さ
	float newHeight = 500.0f * hpRatio;
	//横幅を70pxに変更
	hpBar_->SetSize(Vector2(70.0f, newHeight));

	//カメラの更新
	CameraUpdate();

	//シーンの切り替え
	ChangeScene();

	//エミッターの位置をプレイヤーの中心に設定
	starEmitter_->SetPosition(player_->GetCenterPosition());

	//エミッターの更新
	starEmitter_->Update();

	//パーティクルマネージャーの更新
	particleManager_->Update(vp_);

	tutorialEvent_->Update();
}

void TutorialScene::Draw() {
	/// -------描画処理開始-------

	//-----Spriteの描画開始-----

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();

	//ロックオンマーク
	lockOn_->Draw();

	// HPバーの描画
	hpBar_->Draw();

	//------------------------

	//-----アニメーションの描画開始-----

	/// アニメーションの描画準備
	objCommon_->skinningDrawCommonSetting();

	//プレイヤーのアニメーション描画
	player_->DrawAnimation(vp_);

	//敵のアニメーション描画
	for (auto& enemy : enemies_) {
		enemy->DrawAnimation(vp_);
	}

	//------------------------------

	//-----3DObjectの描画開始-----

	/// 3DObjectの描画準備
	objCommon_->DrawCommonSetting();

	//プレイヤーの描画
	player_->Draw(vp_);

	//敵の描画
	for (auto& enemy : enemies_) {
		enemy->Draw(vp_);
	}

	//天球の描画
	skydome_->Draw(vp_);

	//地面の描画
	ground_->Draw(vp_);

	//--------------------------

	//------Particleの描画開始-------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();

	//パーティクルの描画
	particleManager_->Draw();

	//プレイヤーのパーティクル描画
	player_->DrawParticle(vp_);

	//敵のパーティクル描画
	for (auto& enemy : enemies_) {
		enemy->DrawParticle(vp_);
	}

	//-----------------------------

	//-----UIの描画開始-----

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();

	tutorialEvent_->Draw();

	//-----------------------------

	//-----線描画-----

	DrawLine3D::GetInstance()->Draw(vp_);

	//---------------

	/// -------描画処理終了-------
}

void TutorialScene::DrawForOffScreen() {
	/// -------描画処理開始-------

	//-----Spriteの描画開始-----

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();

	//------------------------

	//-----アニメーションの描画開始-----

	objCommon_->skinningDrawCommonSetting();

	//------------------------------


	//-----3DObjectの描画開始-----

	objCommon_->DrawCommonSetting();

	//--------------------------

	//------Particleの描画開始-------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();

	//-----------------------------

	/// -------描画処理終了-------
}

void TutorialScene::Debug() {

	ImGui::Begin("TutorialScene:Debug");

	//デバッグカメラのImGui
	debugCamera_->imgui();

	//ライトのImGui
	LightGroup::GetInstance()->imgui();

	//プレイヤーのImGui
	player_->ImGui();

	ImGui::End();
}

void TutorialScene::CameraUpdate() {

	if (debugCamera_->GetActive()) {

		//デバッグカメラの更新
		debugCamera_->Update();
	} else {

		//追従カメラの更新
		followCamera_->Update();

		//ビュー行列の更新
		vp_.matView_ = followCamera_->GetViewProjection().matView_;

		//プロジェクション行列の更新
		vp_.matProjection_ = followCamera_->GetViewProjection().matProjection_;

		//ビュープロジェクションの更新
		vp_.TransferMatrix();

		//vp_.UpdateMatrix();

		//ロックオンの更新
		lockOn_->Update(enemies_, vp_);
	}
}

void TutorialScene::ChangeScene() {

#pragma region プレイ会用機能

	// タイトルシーンへ戻す
	if (tutorialEvent_->GetIsSceneChange()) {

		sceneManager_->NextSceneReservation("TITLE");

		if (isPlay) {

			isPlay = false;
		}
	}
}

void TutorialScene::ReSpawnEnemy() {

	if (enemies_.size() <= 1) {

		spawnTimer_ += 1.0f / 60.0f;

		if (spawnTimer_ >= 3.0f) {

			std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();

			newEnemy->SetPlayer(player_.get());

			newEnemy->SetTimeManager(timeManager_.get());

			newEnemy->Init();

			newEnemy->SetTranslation({ 0.0f,1.750f,0.0f });

			enemies_.push_back(std::move(newEnemy));

			spawnTimer_ = 0.0f;
		}
	}
}