#include "TitleScene.h"
#include "ImGuiManager.h"
#include"SceneManager.h"
#include"SrvManager.h"

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG
#include <LightGroup.h>
#include"line/DrawLine3D.h"
#include <Easing.h>

void TitleScene::Initialize()
{
	audio_ = Audio::GetInstance();
	objCommon_ = Object3dCommon::GetInstance();
	spCommon_ = SpriteCommon::GetInstance();
	ptCommon_ = ParticleCommon::GetInstance();
	input_ = Input::GetInstance();
	vp_.Initialize();
	vp_.translation_ = { 0.0f,0.0f,-30.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(&vp_);

	/*wtTitle_.Initialize();
	wtTitle_.translation_ = { 0.0f,3.0f,0.0f };
	wtTitle_.rotation_ = { 0.0f,0.0f,0.0f };
	title_ = std::make_unique<Object3d>();
	title_->Initialize("TitleScene/Title.obj");*/

	//天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init();
	skydome_->SetViewProjection(&vp_);

	/*UI_ = std::make_unique<Sprite>();
	UI_->Initialize("titleUI.png", { 640,360 }, { 1,1,1,1 }, { 0.5f,0.5f });

	UIPad_ = std::make_unique<Sprite>();
	UIPad_->Initialize("TitlePad.png", { 1230,30 }, { 1,1,1,1 }, { 1.0f,0.0f });
	UIPad_->SetSize(UIPad_->GetSize() * 0.5f);*/

	// BGM
	audio_->StopWave(0);
	audio_->StopWave(1);
	audio_->StopWave(2);
	audio_->StopWave(3);
	audio_->StopWave(4);
	audio_->LoadWave("BGM/title.wav");
	audio_->LoadWave("BGM/battle.wav");
	audio_->LoadWave("BGM/gameClear.wav");
	audio_->LoadWave("BGM/gameOver.wav");
	audio_->LoadWave("BGM/tutorial.wav");

	// SE
	audio_->LoadWave("SE/scene.wav");		// 5
	audio_->LoadWave("SE/punch.wav");		// 6
	audio_->LoadWave("SE/battleStart.wav");	// 7
	audio_->LoadWave("SE/battleEnd.wav");	// 8
	audio_->LoadWave("SE/hit.wav");			// 9	ダメージをもらった時
	audio_->LoadWave("SE/hitBigDamage.wav");// 10	大ダメージを与えた/もらった時
	audio_->LoadWave("SE/reflection.wav");	// 11	壁に反射したとき

	audio_->PlayWave(0, 0.1f, true);

	for (int i = 0; i < 1; ++i) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		emitters_.push_back(std::move(emitter_));
	}
	emitters_[0]->Initialize("star", "GameScene/star.obj");
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
#ifdef _DEBUG
	// デバッグ
	Debug();
#endif // _DEBUG

	// カメラ更新
	CameraUpdate();

	// シーン切り替え　
	ChangeScene();

	//wtTitle_.scale_ = { 1.3f,1.3f,1.3f };
	//// 3.0fを中心に上下に揺らす
	//wtTitle_.translation_.y = EaseInOutQuint(2.5f, 3.5f, timer_, 1.0f);
	// 
	//wtTitle_.UpdateMatrix();

	skydome_->SetScale({ 1000.0f,1000.0f,1000.0f });// 天球のScale
	skydome_->Update();

	//// UI点滅
	//timer_ += speed_;
	//if (timer_ >= 1.0f || timer_ < 0.0f) {
	//	speed_ *= -1.0f;
	//}
	//UI_->SetAlpha(timer_);
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->Update(vp_);
	}
}

void TitleScene::Draw()
{
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
	if (Input::GetInstance()->IsAnyJoystickConnected()) {

	}
	/*title_->Draw(wtTitle_, vp_);*/
	skydome_->Draw(vp_);
	//--------------------------

	/// Particleの描画準備
	ptCommon_->DrawCommonSetting();
	//------Particleの描画開始-------

	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->Draw();
		//emitter_->DrawEmitter();
	}

	//-----------------------------

	/// Spriteの描画準備
	spCommon_->DrawCommonSetting();
	//-----Spriteの描画開始-----
	/*UI_->Draw();
	UIPad_->Draw();*/
	//------------------------

	//-----線描画-----
	DrawLine3D::GetInstance()->Draw(vp_);
	//---------------

	/// ----------------------------------

	/// -------描画処理終了-------
}

void TitleScene::DrawForOffScreen()
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


void TitleScene::Debug()
{
	ImGui::Begin("TitleScene:Debug");
	debugCamera_->imgui();
	LightGroup::GetInstance()->imgui();

	int emitterId = 0;
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		ImGui::PushID(emitterId);
		emitter_->imgui();
		ImGui::PopID();
		++emitterId;
	}
	
	ImGui::End();
}

void TitleScene::CameraUpdate()
{
	if (debugCamera_->GetActive()) {
		debugCamera_->Update();
	}
	else {
		vp_.UpdateMatrix();
	}
}

void TitleScene::ChangeScene()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A || input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->NextSceneReservation("GAME");
		if (isChangeScene) {
			audio_->PlayWave(5, 1.0f, false);
			isChangeScene = false;
		}
	}
}
