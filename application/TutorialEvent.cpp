#include "TutorialEvent.h"

#include "Input.h"
#include "Easing.h"

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

	for (size_t i = 0; i < 2; i++) {

		std::unique_ptr<Sprite> newSprite;

		newSprite = std::make_unique<Sprite>();

		tutorialText_.push_back(std::move(newSprite));
	}

	nextUI_ = std::make_unique<Sprite>();
	nextUI_->Initialize("Next.png", { 640.0f,600.0f }, { 0.2f,0.2f,0.2f,1 }, { 0.5f,0.5f });
	nextUI_->SetAlpha(0.0f);

	successUI_ = std::make_unique<Sprite>();
	successUI_->Initialize("Success.png", { 640.0f,100.0f }, { 1,1,1,1 }, { 0.5f,0.5f });
	successUI_->SetRotation(-0.1f);
	successUI_->SetAlpha(0.0f);

	tutorialText_[0]->Initialize("TutorialText01.png", { 640.0f,100.0f }, { 1,1,1,1 }, { 0.5f,0.5f });
	tutorialText_[0]->SetAlpha(0.0f);

	tutorialText_[1]->Initialize("TutorialText02.png", { 640.0f,100.0f }, { 1,1,1,1 }, { 0.5f,0.5f });
	tutorialText_[1]->SetAlpha(0.0f);

	eventCount_ = 1;

	canNext_ = false;

	alphaTimer_ = 0.0f;

	alphaSpeed_ = 0.02f;

	successTimer_ = 0.0f;

	successUISize_ = successUI_->GetSize();

	isSceneChange_ = false;
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

		successTimer_ += 1.0f / 60.0f;

		if (alphaTimer_ >= 1.0f || alphaTimer_ < 0.0f) {
			alphaSpeed_ *= -1.0f;
		}

		if (successTimer_ >= 1.0f) {

			successTimer_ = 1.0f;
		}

		nextUI_->SetAlpha(alphaTimer_);

		successUI_->SetAlpha(1.0f);

		successUI_->SetSize(EaseOutElastic(Vector2(successUISize_.x * 1.5f, successUISize_.y * 1.5f), successUISize_, successTimer_, 1.0f));
	} else {

		successTimer_ = 0.0f;

		nextUI_->SetAlpha(nextUI_->GetColor().w - 1.0f / 60.0f);

		successUI_->SetAlpha(successUI_->GetColor().w - 1.0f / 60.0f);

		if (nextUI_->GetColor().w <= 0.0f) {

			nextUI_->SetAlpha(0.0f);
		}

		if (successUI_->GetColor().w <= 0.0f) {

			successUI_->SetAlpha(0.0f);
		}

		successUI_->SetSize(successUISize_);
	}

	for (auto& text : tutorialText_) {

		text->SetAlpha(text->GetColor().w - 1.0f / 60.0f);

		if (text->GetColor().w <= 0.0f) {

			text->SetAlpha(0.0f);
		} else if (text->GetColor().w >= 1.0f) {

			text->SetAlpha(1.0f);
		}
	}

	switch (eventCount_) {

	case 1:

		tutorialText_[0]->SetAlpha(tutorialText_[0]->GetColor().w + 2.0f / 60.0f);

		AddEvent(EventType::MOVE);
		AddEvent(EventType::ATTACKDIRECTION);

		break;

	case 2:

		tutorialText_[1]->SetAlpha(tutorialText_[1]->GetColor().w + 2.0f / 60.0f);

		AddEvent(EventType::TOPDEFENSE);
		AddEvent(EventType::BOTTOMDEFENSE);
		AddEvent(EventType::LEFTDEFENSE);
		AddEvent(EventType::RIGHTDEFENSE);
		AddEvent(EventType::DOWNSWING);
		AddEvent(EventType::THRUST);
		AddEvent(EventType::LEFTSLASH);
		AddEvent(EventType::RIGHTSLASH);

		break;

	case 3:

		isSceneChange_ = true;

		//successUI_->SetAlpha(tutorialText_[1]->GetColor().w);

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

	for (auto& text : tutorialText_) {
		text->Draw();
	}

	nextUI_->Draw();

	successUI_->Draw();
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

	for (auto event : eventList_) {

		if (tutorialUI_[event]->GetSprite()->GetColor().w < 1.0f) {

			canNext_ = false;

			alphaTimer_ = 0.0f;

			alphaSpeed_ = 0.02f;

			return;
		}

		if (!tutorialUI_[event]->GetIsSuccess()) {

			canNext_ = false;

			alphaTimer_ = 0.0f;

			alphaSpeed_ = 0.02f;

			return;
		}

		if (eventList_.size() == 0) {

			canNext_ = false;

			alphaTimer_ = 0.0f;

			alphaSpeed_ = 0.02f;

			return;
		}

		canNext_ = true;

	}

}
