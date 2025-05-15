#include "TutorialEvent.h"

#include "Input.h"

void TutorialEvent::Initialize(Player* player) {
	player_ = player;
	// --- UIの初期化 ---

	for (int i = 0; i < EventType::END; i++) {
		std::unique_ptr<TutorialUI> newUI = std::make_unique<TutorialUI>();
		tutorialUI_.push_back(std::move(newUI));
	}

	// --- UIの初期化 ---

	tutorialUI_[EventType::MOVE]->Initialize(
		Vector2(100.0f, 100.0f),
		"Move.png",
		{
			TutorialUI::UIType::kLStick
		}
	);

	tutorialUI_[EventType::ATTACKDIRECTION]->Initialize(
		Vector2(1180.0f, 100.0f),
		"AttackDirection.png",
		{
			TutorialUI::UIType::kRStick
		}
	);

	tutorialUI_[EventType::TOPDEFENSE]->Initialize(
		Vector2(100.0f, 100.0f),
		"TopDefense.png",
		{
			TutorialUI::UIType::kRStickUp,
			TutorialUI::UIType::kLButton,
		}
		);

	tutorialUI_[EventType::BOTTOMDEFENSE]->Initialize(
		Vector2(100.0f, 270.0f),
		"BottomDefense.png",
		{
			TutorialUI::UIType::kRStickDown,
			TutorialUI::UIType::kLButton,
		}
		);

	tutorialUI_[EventType::LEFTDEFENSE]->Initialize(
		Vector2(100.0f, 440.0f),
		"LeftDefense.png",
		{
			TutorialUI::UIType::kRStickLeft,
			TutorialUI::UIType::kLButton,
		}
		);

	tutorialUI_[EventType::RIGHTDEFENSE]->Initialize(
		Vector2(100.0f, 610.0f),
		"RightDefense.png",
		{
			TutorialUI::UIType::kRStickRight,
			TutorialUI::UIType::kLButton,
		}
		);

	tutorialUI_[EventType::DOWNSWING]->Initialize(
		Vector2(1180.0f, 100.0f),
		"DownSwing.png",
		{
			TutorialUI::UIType::kRStickUp,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[EventType::THRUST]->Initialize(
		Vector2(1180.0f, 270.0f),
		"Thrust.png",
		{
			TutorialUI::UIType::kRStickDown,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[EventType::LEFTSLASH]->Initialize(
		Vector2(1180.0f, 440.0f),
		"LeftSlash.png",
		{
			TutorialUI::UIType::kRStickLeft,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[EventType::RIGHTSLASH]->Initialize(
		Vector2(1180.0f, 610.0f),
		"RightSlash.png",
		{
			TutorialUI::UIType::kRStickRight,
			TutorialUI::UIType::kRButton,
		}
		);

	nextUI_ = std::make_unique<Sprite>();
	nextUI_->Initialize("Next.png", { 640.0f,600.0f }, { 0.2f,0.2f,0.2f,1 }, { 0.5f,0.5f });

	successUI_ = std::make_unique<Sprite>();
	successUI_->Initialize("Success.png", { 640.0f,100.0f }, { 1,1,1,1 }, { 0.5f,0.5f });
	successUI_->SetRotation(-0.1f);

	eventCount_ = 1;

	canNext_ = false;

	alphaTimer_ = 0.0f;

	alphaSpeed_ = 0.02f;
}

void TutorialEvent::Update() {

	XINPUT_STATE joyState;

	for (auto& ui : tutorialUI_) {
		ui->SetIsActive(false);
	}

	eventList_.clear();

	if (canNext_) {

		// UI点滅
		alphaTimer_ += alphaSpeed_;

		if (alphaTimer_ >= 1.0f || alphaTimer_ < 0.0f) {
			alphaSpeed_ *= -1.0f;
		}

		nextUI_->SetAlpha(alphaTimer_);
	} else {

		nextUI_->SetAlpha(0.0f);

		successUI_->SetAlpha(0.0f);
	}

	switch (eventCount_) {

	case 1:

		AddEvent(EventType::MOVE);
		AddEvent(EventType::ATTACKDIRECTION);

		break;

	case 2:

		AddEvent(EventType::TOPDEFENSE);
		AddEvent(EventType::BOTTOMDEFENSE);
		AddEvent(EventType::LEFTDEFENSE);
		AddEvent(EventType::RIGHTDEFENSE);
		AddEvent(EventType::DOWNSWING);
		AddEvent(EventType::THRUST);
		AddEvent(EventType::LEFTSLASH);
		AddEvent(EventType::RIGHTSLASH);

		break;

	default:
		break;
	}

	UpdateEvent();

	if (canNext_) {

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {

			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

				eventCount_++;

				canNext_ = false;
			}
		}
	}

	for (auto& ui : tutorialUI_) {
		ui->Update();
	}
}

void TutorialEvent::Draw() {

	for (auto& ui : tutorialUI_) {
		ui->Draw();
	}

	if (canNext_) {

		nextUI_->Draw();

		successUI_->Draw();
	}

}

void TutorialEvent::AddEvent(EventType event) {

	eventList_.push_back(event);
}

void TutorialEvent::UpdateEvent() {

	for (auto event : eventList_) {

		XINPUT_STATE joyState;

		tutorialUI_[event]->SetIsActive(true);

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {

			switch (event) {
			case TutorialEvent::MOVE:

				if (Input::GetInstance()->GetJoyStickDirection(0, false) != Input::JoyStickDirection::None) {
					tutorialUI_[EventType::MOVE]->SetIsSuccess(true);
				}
				break;
			case TutorialEvent::ATTACKDIRECTION:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) != Input::JoyStickDirection::None) {
					tutorialUI_[EventType::ATTACKDIRECTION]->SetIsSuccess(true);
				}
				break;
			case TutorialEvent::TOPDEFENSE:

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

					if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Up) {
						tutorialUI_[EventType::TOPDEFENSE]->SetIsSuccess(true);
					}
				}
				break;
			case TutorialEvent::BOTTOMDEFENSE:

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

					if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Down) {
						tutorialUI_[EventType::BOTTOMDEFENSE]->SetIsSuccess(true);
					}
				}
				break;
			case TutorialEvent::LEFTDEFENSE:

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

					if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Left) {
						tutorialUI_[EventType::LEFTDEFENSE]->SetIsSuccess(true);
					}
				}
				break;
			case TutorialEvent::RIGHTDEFENSE:

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

					if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Right) {
						tutorialUI_[EventType::RIGHTDEFENSE]->SetIsSuccess(true);
					}
				}
				break;
			case TutorialEvent::DOWNSWING:

				if (player_->GetAttackType() == Player::AttackType::kDownSwing) {
					tutorialUI_[EventType::DOWNSWING]->SetIsSuccess(true);
				}
				break;
			case TutorialEvent::THRUST:

				if (player_->GetAttackType() == Player::AttackType::kThrust) {
					tutorialUI_[EventType::THRUST]->SetIsSuccess(true);
				}
				break;
			case TutorialEvent::LEFTSLASH:

				if (player_->GetAttackType() == Player::AttackType::kRightSlash) {
					tutorialUI_[EventType::LEFTSLASH]->SetIsSuccess(true);
				}
				break;
			case TutorialEvent::RIGHTSLASH:

				if (player_->GetAttackType() == Player::AttackType::kLeftSlash) {
					tutorialUI_[EventType::RIGHTSLASH]->SetIsSuccess(true);
				}
				break;
			}
		}
	}

	canNext_ = true;

	for (auto event : eventList_) {

		if (!tutorialUI_[event]->GetIsSuccess()) {

			canNext_ = false;

			alphaTimer_ = 0.0f;

			alphaSpeed_ = 0.02f;

			return;
		}
	}

}
