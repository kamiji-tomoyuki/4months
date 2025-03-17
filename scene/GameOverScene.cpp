#include "GameOverScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>
#include"line/DrawLine3D.h"


/// 終了
void GameOverScene::Finalize()
{
}

/// 初期化
void GameOverScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);
}

/// 更新
void GameOverScene::Update()
{
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG

	// カメラ更新
	CameraUpdate();

	// シーン切り替え
	ChangeScene();

}

/// 描画
void GameOverScene::Draw()
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

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

/// オフスクリーン上に描画
void GameOverScene::DrawForOffScreen()
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

void GameOverScene::Debug()
{
	ImGui::Begin("GameOverScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();
	ImGui::End();
}

void GameOverScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		vp_.UpdateMatrix();
	}
}

void GameOverScene::ChangeScene(){
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A || input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->NextSceneReservation("TITLE");
	}
}
