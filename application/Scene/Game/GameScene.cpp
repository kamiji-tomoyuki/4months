#include "GameScene.h"
#include <LightGroup.h>
#include"SceneManager.h"
#include <line/DrawLine3D.h>
#include "Boss.h"
#include <CollisionTypeIdDef.h>

void GameScene::Finalize() {

}

void GameScene::Initialize() {

	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	input_->SetJoystickDeadZone(0, 3000, 3000);

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	timeManager_ = std::make_unique<TimeManager>();
	timeManager_->Initialize();
	timeManager_->SetTimer("start", 2.0f / 60.0f);

	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init("WildsSkyDome.obj");
	skydome_->SetViewProjection(&vp_);

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init();

	//プレイヤー
	Player::SetPlayerID(0);
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize();
	enemyManager_->SetGameScene(this);
	LoadLevelData();

	// コロシアム
	coliseum_ = std::make_unique<Coliseum>();
	coliseum_->Init("sphere.obj");
	coliseum_->SetViewProjection(&vp_);

	//カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&players_[0]->GetWorldTransform());
	players_[0]->SetViewProjection(&followCamera_->GetViewProjection());
	for (std::unique_ptr<Player>& player : players_) {
		player->SetFollowCamera(followCamera_.get());
	}
	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	followCamera_->SetLockOn(lockOn_.get());
	players_[0]->SetLockOn(lockOn_.get());

	//ポーズ
	pause_ = std::make_unique<Pause>();
	pause_->Initialize();

	// UIマネージャ
	uiManager_ = std::make_unique<UIManager>();
	int playerMaxHp = 0, bossMaxHp = 0;
	for(std::unique_ptr<Player>& player : players_) {
		playerMaxHp = player->GetHP();
	}
	for(const std::unique_ptr<Enemy>& enemy : enemyManager_->GetEnemies()) {
		if (enemy->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
			bossMaxHp = enemy->GetHP();
		}
	}
	uiManager_->Initialize(playerMaxHp, bossMaxHp); // プレイヤーとボスの最大HPを設定

	particleManager_ = ParticleManager::GetInstance();

	starEmitter_ = std::make_unique<ParticleEmitter>();

	starEmitter_->Initialize("Star.json");

	starEmitter_->Start();

	// ステージ
	stage_ = std::make_unique<Object3d>();
	stage_->Initialize("stage/stage.obj");

	wtStage_.Initialize();

	stage_->SetSize(Vector3{ size_,size_,size_ });

	audio_->StopWave("BGM/title.wav");
	audio_->StopWave("BGM/battle.wav");
	audio_->StopWave("BGM/gameClear.wav");
	audio_->StopWave("BGM/gameOver.wav");
	audio_->StopWave("BGM/tutorial.wav");
	audio_->PlayWave("BGM/battle.wav", 0.1f, true);

	audio_->PlayWave("SE/battleStart.wav", 1.0f, false);
}

void GameScene::Update() {
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG
	ClearUpdate();
	// タイマー更新
	timeManager_->Update();
  
	ground_->Update();
	coliseum_->SetScale({ 320.0f,320.0f,320.0f });// コロシアムのScale
	coliseum_->SetRadius(275.0f);
	coliseum_->Update();

	pause_->Update();

	if (!pause_->GetIsPause()) {

		// プレイヤー更新
		for (std::unique_ptr<Player>& player : players_) {
			player->Update();
			player->UpdateParticle(vp_);
			uiManager_->SetPlayerHP(player->GetHP()); // プレイヤーのHPをUIマネージャに設定
		}
		enemyManager_->Update();

		uiManager_->Update();

		skydome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale
		skydome_->Update();

		ground_->Update();
		coliseum_->SetScale({ 320.0f,320.0f,320.0f });// コロシアムのScale
		coliseum_->SetRadius(275.0f);
		coliseum_->Update();

		// カメラ更新
		CameraUpdate();

		starEmitter_->SetPosition(players_[0]->GetCenterPosition());

		starEmitter_->Update();

		particleManager_->Update(vp_);

	}

	// シーン切り替え
	ChangeScene();

	stage_->Update(wtStage_, vp_);
	wtStage_.UpdateMatrix();
}

void GameScene::Draw() {
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	
	//------------------------

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----
	for (std::unique_ptr<Player>& player : players_) {
		player->DrawAnimation(vp_);
	}
	enemyManager_->DrawAnimation(vp_);
	//------------------------------


	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	for (std::unique_ptr<Player>& player : players_) {
		player->Draw(vp_);
	}
	enemyManager_->Draw(vp_);
	skydome_->Draw(vp_);
	//coliseum_->Draw(vp_);
	ground_->Draw(vp_);

	stage_->Draw(wtStage_, vp_);
	//--------------------------


	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	particleManager_->Draw();

	for (std::unique_ptr<Player>& player : players_) {
		player->DrawParticle(vp_);
	}

	//-----------------------------

	//-----UIの描画開始-----

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//ロックオンマーク
	lockOn_->Draw();
	// UIの描画
	uiManager_->Draw();
	
	pause_->Draw();

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void GameScene::DrawForOffScreen() {
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

void GameScene::Debug() {
	ImGui::Begin("GameScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	for (std::unique_ptr<Player>& player : players_) {
		player->ImGui();
	}

	ImGui::End();
}

void GameScene::CameraUpdate() {
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
		vp_.matView_ = followCamera_->GetViewProjection().matView_;
		vp_.matProjection_ = followCamera_->GetViewProjection().matProjection_;
		vp_.TransferMatrix();
		//vp_.UpdateMatrix();

		lockOn_->Update(enemyManager_->GetEnemies(), vp_);
	}
}

void GameScene::ChangeScene() {
	if (isClear) {
		sceneManager_->NextSceneReservation("CLEAR");
		if (isPlay) {
			audio_->PlayWave("SE/battleEnd.wav", 1.0f, false);
			isPlay = false;
		}
		isClear = false;
	}
	for (std::unique_ptr<Player>& player : players_) {
		if (player->IsGameOver()) {
			sceneManager_->NextSceneReservation("GAMEOVER");
			if (isPlay) {
				audio_->PlayWave("SE/battleEnd.wav", 1.0f, false);
				isPlay = false;
			}
		}
	}
}

void GameScene::ClearUpdate(){
	enemyManager_->DeathUpdate();
	isClear = enemyManager_->GetEnemies().size() <= 1 ? true : false;
}

void GameScene::LoadLevelData(){
	json_ = std::make_unique<JsonLoader>();
	std::string filePath = "scene/stage" + std::to_string(stageNum_) + ".json";
	json_->LoadTransformData(filePath);
	auto data = json_->GetTransformData();
	for (const auto& [filename, names] : data) {
		if (filename != filePath) {
			continue;
		}
		for(const auto& [name, positions] : names) {
			//プレイヤー
			if (name == "aplayer") {
				std::unique_ptr<Player> player = std::make_unique<Player>();
				player->SetTimeManager(timeManager_.get());
				player->Init();
				player->SetViewProjection(&vp_);
				int count = 0;
				for (const Vector3& pos : positions) {
					if (count == 0) {
						player->SetPosition(pos);
					}
					count++;
				}
				players_.push_back(std::move(player));
			}
			//敵
			if (name == "boss") {
				int count = 0;
				for (const Vector3& pos : positions) {
					if (count == 0) {
						enemyManager_->AddBoss(pos);
					}
					count++;
				}
			}
			if (name.rfind("soldier", 0) == 0) {
				int count = 0;
				for (const Vector3& pos : positions) {
					if (count == 0) {
						enemyManager_->AddEnemy(pos);
					}
					count++;
				}
			}
		}
	}
}