#include "EditorScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>
#include"line/DrawLine3D.h"
#include <Easing.h>

void EditorScene::Initialize() {
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	vp_.Initialize();
	vp_.translation_ = { 0.0f,0.0f,-30.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	audio_->StopWave(0);
	audio_->StopWave(1);
	audio_->StopWave(2);
	audio_->StopWave(3);
	audio_->StopWave(4);

	editor_ = std::make_unique<ParticleEditor>();

	editor_->Initialize(&vp_);

	editor_->CreateEmitter();

	particleManager_ = ParticleManager::GetInstance();
}

void EditorScene::Finalize() {
}

void EditorScene::Update() {
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG

	// カメラ更新
	CameraUpdate();

	// シーン切り替え　
	ChangeScene();

	editor_->Update();

	particleManager_->Update(vp_);
}

void EditorScene::Draw() {

	/// -------描画処理開始-------

	//emitter_->DrawEmitter();

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

	editor_->Draw();

	particleManager_->Draw();

	//-----------------------------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();

	//-----Spriteの描画開始-----

	//------------------------

	//-----線描画-----

	DrawLine3D::GetInstance()->Draw(vp_);

	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void EditorScene::DrawForOffScreen() {
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

void EditorScene::Debug() {

	editor_->ImGui();

}

void EditorScene::CameraUpdate() {
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	} else {
		vp_.UpdateMatrix();
	}
}

void EditorScene::ChangeScene() {

}
