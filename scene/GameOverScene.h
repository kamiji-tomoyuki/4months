#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "SpriteCommon.h"
#include "ParticleCommon.h"
#include"ViewProjection.h"
#include"DebugCamera.h"

#include "Skydome.h"
#include "Ground.h"
#include "Player.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"

class GameOverScene : public BaseScene
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

	std::unique_ptr<Skydome> skyDome_;

	std::unique_ptr<Ground> ground_;

	std::unique_ptr<Player> player_;

	std::vector<std::unique_ptr<Enemy>> enemies_;

	//タイム
	std::unique_ptr<TimeManager> timeManager_;

	std::unique_ptr<ParticleEmitter> gameOverEmitter_ = nullptr;

	float enemiesDistance_ = 0.0f;

	int enemyMaxColumn_ = 0;
};

