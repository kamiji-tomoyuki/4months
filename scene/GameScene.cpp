#include "GameScene.h"
#include <LightGroup.h>
#include"SceneManager.h"
#include <line/DrawLine3D.h>
#include <fstream>
#include "Boss.h"
#include <CollisionTypeIdDef.h>

void GameScene::Finalize()
{

}

void GameScene::Initialize()
{

	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	input_->SetJoystickDeadZone(0, 3000, 3000);

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init("WildsSkyDome.obj");
	skydome_->SetViewProjection(&vp_);

	//地面
	ground_ = std::make_unique<Ground>();
	ground_->Init();

	timeManager_ = std::make_unique<TimeManager>();
	timeManager_->Initialize();
	timeManager_->SetTimer("start", 2.0f / 60.0f);
	//プレイヤー
	Player::SetPlayerID(0);
	for (uint32_t i = 0; i < 1; ++i) {
		std::unique_ptr<Player> player = std::make_unique<Player>();
		player->SetTimeManager(timeManager_.get());
		player->Init();
		player->SetViewProjection(&vp_);
		players_.push_back(std::move(player));
	}
	players_[0]->SetPosition({ 0.0f,1.750f,-50.0f });

	//敵
	Enemy::SetEnemyID(0);
	for (size_t i = 0; i < 1; i++) {
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Boss>();
		newEnemy->SetPlayer(players_[0].get());
		newEnemy->SetTimeManager(timeManager_.get());
		newEnemy->Init();
		newEnemy->SetTranslation({0.0f,0.0f,100.0f});
		enemies_.push_back(std::move(newEnemy));
	}
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();
	newEnemy->SetPlayer(players_[0].get());
	newEnemy->SetTimeManager(timeManager_.get());
	newEnemy->Init();
	newEnemy->SetTranslation({0,0,0});
	enemies_.push_back(std::move(newEnemy));
	LoadEnemyPopData();

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

	// HPバーのスプライトを作成
	hpBar_ = std::make_unique<Sprite>();
	hpBar_->Initialize("hp.png", Vector2(1180.0f, 200.0f)); // 右端に配置
	hpBar_->SetSize(Vector2(70.0f, 500.0f)); // 横幅を少し太く
	hpBar_->SetAnchorPoint({ 0.5f,0.0f });

	// 敵の HP バーのスプライトを作成
	enemyHpBar_ = std::make_unique<Sprite>();
	enemyHpBar_->Initialize("enemyHpBar.png", Vector2(50.0f, 200.0f)); // 左端に配置
	enemyHpBar_->SetSize(Vector2(70.0f, 500.0f)); // 横幅を少し太く
	enemyHpBar_->SetAnchorPoint({ 0.0f,0.0f });

	particleManager_ = ParticleManager::GetInstance();

	starEmitter_ = std::make_unique<ParticleEmitter>();

	starEmitter_->Initialize("Star.json");

	starEmitter_->Start();

	audio_->StopWave(0);
	audio_->StopWave(1);
	audio_->StopWave(2);
	audio_->StopWave(3);
	audio_->StopWave(4);
	audio_->PlayWave(1, 0.1f, true);

	audio_->PlayWave(7, 1.0f, false);
}

void GameScene::Update()
{
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG
	//前 敵処理
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		if (!enemy->GetIsAlive()) {
			lockOn_->ResetTarget();
			if (enemy->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
				isClear = true;
			}
		}
	}
	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		if (!enemy->GetIsAlive()) {
			return true;
		}
		return false;
		});
	// タイマー更新
	timeManager_->Update();
	// プレイヤー更新
	for (std::unique_ptr<Player>& player : players_) {
		player->Update(); 
		player->UpdateParticle(vp_);
	}
	//今 敵処理
	UpdateEnemyPopCommands();
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
	}
	skydome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale
	skydome_->Update();

	ground_->Update();

	// HPバーのサイズと位置を更新
	float hpRatio = static_cast<float>(players_[0]->GetHP()) / kMaxHp;
	float newHeight = 500.0f * hpRatio; // HPに応じた高さ
	hpBar_->SetSize(Vector2(100.0f, newHeight)); // 横幅を70pxに変更
	hpBar_->SetPosition(Vector2(1180.0f, 100 + (500.0f - newHeight))); // 右側に配置

	// 敵の HPバーのサイズと位置を更新
	//float enemyHpRatio = static_cast<float>(players_[1]->GetHP()) / kMaxHp;
	//float enemyNewHeight = 500.0f * enemyHpRatio;
	//enemyHpBar_->SetSize(Vector2(100.0f, enemyNewHeight)); // 横幅を70pxに変更
	//enemyHpBar_->SetPosition(Vector2(50.0f, 100 + (500.0f - enemyNewHeight))); // 左側に配置

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

	starEmitter_->SetPosition(players_[0]->GetCenterPosition());

	starEmitter_->Update();

	particleManager_->Update(vp_);
}

void GameScene::Draw()
{
	/// -------描画処理開始-------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	//ロックオンマーク
	lockOn_->Draw();
	// HPバーの描画
	hpBar_->Draw();
	//enemyHpBar_->Draw(); // 敵の HPバーも描画

	//------------------------

	objCommon_->skinningDrawCommonSetting();
	//-----アニメーションの描画開始-----
	for (std::unique_ptr<Player>& player : players_) {
		player->DrawAnimation(vp_);
	}
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->DrawAnimation(vp_);
	}
	//------------------------------


	objCommon_->DrawCommonSetting();
	//-----3DObjectの描画開始-----

	for (std::unique_ptr<Player>& player : players_) {
		player->Draw(vp_);
	}
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(vp_);
	}
	skydome_->Draw(vp_);
	ground_->Draw(vp_);
	//--------------------------
	                 

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	particleManager_->Draw();

	for (std::unique_ptr<Player>& player : players_) {
		player->DrawParticle(vp_);
	}
	
	//-----------------------------

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void GameScene::DrawForOffScreen()
{
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

void GameScene::Debug()
{
	ImGui::Begin("GameScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	for (std::unique_ptr<Player>& player : players_) {
		player->ImGui();
	}

	ImGui::End();
}

void GameScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
		vp_.matView_ = followCamera_->GetViewProjection().matView_;
		vp_.matProjection_ = followCamera_->GetViewProjection().matProjection_;
		vp_.TransferMatrix();
		//vp_.UpdateMatrix();
		lockOn_->Update(enemies_, vp_);
	}
}

void GameScene::ChangeScene()
{
	if (isClear) {
		sceneManager_->NextSceneReservation("CLEAR");
		if (isPlay) {
			audio_->PlayWave(8, 1.0f, false);
			isPlay = false;
		}
		isClear = false;
	}
	for (std::unique_ptr<Player>& player : players_) {
		if (player->IsGameOver())
		{
			sceneManager_->NextSceneReservation("GAMEOVER");
			if (isPlay) {
				audio_->PlayWave(8, 1.0f, false);
				isPlay = false;
			}
		}
	}
}


void GameScene::LoadEnemyPopData() {
    std::ifstream file;
    file.open("./resources/enemyPop.csv");
    assert(file.is_open());
    //
    enemyPopCommands << file.rdbuf();
    //
    file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	//待機処理
	if (timeManager_->GetTimer("enemyPop").isStart) {
		return;
	}
	//
	std::string line;
	while (getline(enemyPopCommands, line)) {
		std::stringstream line_stream(line);

		std::string word;
		//
		getline(line_stream, word, ',');
		if (word.find("//") == 0) {
			continue;
		}
		//POP
		if (word.find("POP") == 0) {
			//x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			//y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			//z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());
			//
			AddEnemy(Vector3(x, y, z));
		}
		//WAIT
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');
			//
			int32_t waitTime = atoi(word.c_str());
			//待機開始
			timeManager_->SetTimer("enemyPop",(float)waitTime);
			//
			break;
		}
	}

}

void GameScene::AddEnemy(const Vector3& position) {
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemy->GetSerialNumber() == enemy->GetNextSerialNumber() -1) {
			enemy->SetTranslation(position);
			enemy->Update();
		}
	}
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();
	newEnemy->SetPlayer(players_[0].get());
	newEnemy->SetTimeManager(timeManager_.get());
	newEnemy->Init();
	newEnemy->SetTranslation({0,0,0});
	enemies_.push_back(std::move(newEnemy));
}