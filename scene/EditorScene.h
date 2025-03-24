#pragma once
#include"BaseScene.h"
#include "Audio.h"
#include "Input.h"
#include"Object3dCommon.h"
#include "SpriteCommon.h"
#include"ParticleCommon.h"
#include"DebugCamera.h"
#include"Object3d.h"
#include"WorldTransform.h"
#include"ParticleEmitter.h"
#include "ParticleEditor.h"

class EditorScene : public BaseScene {

public:

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

	bool loop = true;

	std::unique_ptr<ParticleEditor> editor_;

	ParticleManager* particleManager_;

	bool isChangeScene = true;
};