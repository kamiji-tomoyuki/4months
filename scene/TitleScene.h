#pragma once
#include "Audio.h"
#include"BaseScene.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"DebugCamera.h"
#include"Object3d.h"
#include"WorldTransform.h"
#include"ParticleEmitter.h"
#include <Skydome.h>
#include <Sprite.h>

class TitleScene :public BaseScene
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

	ViewProjection vp_;
	std::unique_ptr<DebugCamera> debugCamera_;

	//WorldTransform wtTitle_;
	//std::unique_ptr<Object3d> title_;
	std::unique_ptr<Sprite> UI_;
	float timer_ = 0.0f;
	float speed_ = 0.02f;

	std::unique_ptr<Sprite> UIPad_;

	std::unique_ptr<Skydome> skydome_ = nullptr;// 天球

	std::unique_ptr<ParticleEmitter> starEmitter_ = nullptr;

	bool loop = true;

	ParticleManager* particleManager_;

	bool isChangeScene = true;
};
