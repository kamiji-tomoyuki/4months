#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "ParticleCommon.h"
#include"ViewProjection.h"
#include"DebugCamera.h"
#include "Player.h"
#include "FollowCamera.h"
#include "PlayerSword.h"
#include <array>
#include <vector>

#include "Skydome.h"
#include "Ground.h"
#include "Coliseum.h"
#include "TimeManager.h"
#include "UIManager.h"
#include "LockOn.h"
#include "Pause.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "EnemyManager.h"
#include <json/JsonLoader.h>


class GameScene : public BaseScene
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// オフスクリーン上に描画
	/// </summary>
	void DrawForOffScreen()override;

	ViewProjection* GetViewProjection()override { return &vp_; }
	void SetStageNum(int stageNum) { stageNum_ = stageNum; }
private:
	void Debug();

	void CameraUpdate();

	void ChangeScene();
	void ClearUpdate();
	//enemyPop
	void LoadLevelData();
private:

	Audio* audio_;
	Input* input_;
	Object3dCommon* objCommon_;
	SpriteCommon* spCommon_;
	ParticleCommon* ptCommon_;

	ParticleManager* particleManager_;

	// ビュープロジェクション
	ViewProjection vp_;
	std::unique_ptr<DebugCamera> debugCamera_;

	std::unique_ptr<Skydome> skydome_ = nullptr;// 天球
	std::unique_ptr<Ground> ground_ = nullptr;//地面
	std::unique_ptr<Coliseum> coliseum_ = nullptr;//コロシアム

	std::vector<std::unique_ptr<Player>> players_;
	std::unique_ptr<EnemyManager> enemyManager_; // 敵管理クラス
	//ロックオン
	std::unique_ptr<LockOn> lockOn_;
	//ポーズ
	std::unique_ptr<Pause> pause_;
	//カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//タイム
	std::unique_ptr<TimeManager> timeManager_;
	// UI マネージャ
	std::unique_ptr<UIManager> uiManager_;
	//パーティクルエミッタ
	std::unique_ptr<ParticleEmitter> starEmitter_;
	// SE 多重再生防止
	bool isPlay = true;
	//クリア
	bool isClear = false;

	// ステージ
	std::unique_ptr<Object3d> stage_;
	float size_ = 15.0f;
	WorldTransform wtStage_;

	int stageNum_ = 1; // ステージ番号
	std::unique_ptr<JsonLoader> json_;
public:
	Player* GetPlayer() { return players_[0].get(); }
	LockOn* GetLockOn() { return lockOn_.get(); }
	TimeManager* GetTimeManager() { return timeManager_.get(); }
	UIManager* GetUIManager() { return uiManager_.get(); }
	void SetClear(bool clear) { isClear = clear; }
};
