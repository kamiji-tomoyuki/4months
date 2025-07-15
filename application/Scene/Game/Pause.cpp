#include "Pause.h"

void Pause::Initialize() {

	pauseBGSprite_ = std::make_unique<Sprite>();

	pauseBGSprite_->Initialize("white1x1.png", { 0.0f,0.0f, }, { 0.0f,0.0f,0.0f,0.75f }, { 0.0f,0.0f });

	pauseBGSprite_->SetSize({ 1280.0f,720.0f });

	pauseUI_ = std::make_unique<Sprite>();

	pauseUI_->Initialize("Pause.png", { 640.0f,90.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	returnGameUI_ = std::make_unique<Sprite>();

	returnGameUI_->Initialize("Return_Game.png", { 640.0f,310.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	returnTitleUI_ = std::make_unique<Sprite>();

	returnTitleUI_->Initialize("Return_Title.png", { 640.0f,410.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	checkReturnTitle_ = std::make_unique<Sprite>();

	checkReturnTitle_->Initialize("CheckReturnTitle.png", { 640.0f,310.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	yesOrNoUI_ = std::make_unique<Sprite>();

	yesOrNoUI_->Initialize("YesOrNo.png", { 640.0f,410.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f });

	returnScene_ = NONE;

	isPause_ = false;

	isCheckReturnTitle_ = false;

	alphaTimer_ = 0.0f;

	alphaSpeed_ = 0.02f;

}

void Pause::Update() {

	// UI点滅
	alphaTimer_ += alphaSpeed_;

	if (alphaTimer_ >= 1.0f || alphaTimer_ < 0.0f) {
		alphaSpeed_ *= -1.0f;
	}

	pauseUI_->SetAlpha(alphaTimer_);

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {


		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) {

			if (!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {

				isPause_ = !isPause_;

				isCheckReturnTitle_ = false;
			}
		}

		if (isPause_) {

			/// === ポーズ中の場合 === ///

			if (isCheckReturnTitle_) {

				/// === タイトルに戻ろうとしている場合 === ///

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

					if (!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {

						returnScene_ = TITLE;
					}
				}

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {

					if (!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {

						isCheckReturnTitle_ = false;
					}
				}
			} else {

				/// === タイトルに戻ろうとしていない場合 === ///

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {

					if (!(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {

						isCheckReturnTitle_ = true;
					}
				}
			}
		}
	}

	preJoyState_ = joyState;
}

void Pause::Draw() {

	if (isPause_) {

		pauseBGSprite_->Draw();

		if (isCheckReturnTitle_) {

			checkReturnTitle_->Draw();

			yesOrNoUI_->Draw();

			return;
		}

		pauseUI_->Draw();

		returnGameUI_->Draw();

		returnTitleUI_->Draw();
	}

}