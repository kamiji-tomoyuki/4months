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
#include "TimeManager.h"
#include "LockOn.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Enemy.h"
#include "Soldier.h"
#include "TutorialEvent.h"

class TutorialScene : public BaseScene 
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

private:


	void Debug();

	void CameraUpdate();

	void ChangeScene();

	void ReSpawnEnemy();

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

	std::unique_ptr<Player> player_;
	//Enemy
	std::list<std::unique_ptr<Enemy>> enemies_;

	//ロックオン
	std::unique_ptr<LockOn> lockOn_;

	//カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//タイム
	std::unique_ptr<TimeManager> timeManager_;

	std::unique_ptr<TutorialEvent> tutorialEvent_;

	// HPバー
	std::unique_ptr<Sprite> hpBar_;

	// HPの最大値
	const int kMaxHp = 10000;

	//パーティクルエミッタ
	std::unique_ptr<ParticleEmitter> starEmitter_;

	// SE 多重再生防止
	bool isPlay = true;

	float spawnTimer_;

};