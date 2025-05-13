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

}

void TutorialEvent::Update() {

	XINPUT_STATE joyState;

	for (auto& ui : tutorialUI_) {
		ui->SetIsActive(false);
	}

	switch (eventCount_) {

	case 1:

		tutorialUI_[EventType::MOVE]->SetIsActive(true);

		tutorialUI_[EventType::ATTACKDIRECTION]->SetIsActive(true);

		if (tutorialUI_[EventType::MOVE]->GetIsSuccess() &&
			tutorialUI_[EventType::ATTACKDIRECTION]->GetIsSuccess()) {

			eventCount_++;
		}

		if (Input::GetInstance()->GetJoyStickDirection(0, false) != Input::JoyStickDirection::None) {
			tutorialUI_[EventType::MOVE]->SetIsSuccess(true);
		}

		if (Input::GetInstance()->GetJoyStickDirection(0, true) != Input::JoyStickDirection::None) {
			tutorialUI_[EventType::ATTACKDIRECTION]->SetIsSuccess(true);
		}

		break;

	case 2:

		tutorialUI_[EventType::TOPDEFENSE]->SetIsActive(true);

		tutorialUI_[EventType::BOTTOMDEFENSE]->SetIsActive(true);

		tutorialUI_[EventType::LEFTDEFENSE]->SetIsActive(true);

		tutorialUI_[EventType::RIGHTDEFENSE]->SetIsActive(true);

		tutorialUI_[EventType::DOWNSWING]->SetIsActive(true);

		tutorialUI_[EventType::THRUST]->SetIsActive(true);

		tutorialUI_[EventType::LEFTSLASH]->SetIsActive(true);

		tutorialUI_[EventType::RIGHTSLASH]->SetIsActive(true);

		if (tutorialUI_[EventType::TOPDEFENSE]->GetIsSuccess() &&
			tutorialUI_[EventType::BOTTOMDEFENSE]->GetIsSuccess() &&
			tutorialUI_[EventType::LEFTDEFENSE]->GetIsSuccess() &&
			tutorialUI_[EventType::RIGHTDEFENSE]->GetIsSuccess() &&
			tutorialUI_[EventType::DOWNSWING]->GetIsSuccess() &&
			tutorialUI_[EventType::THRUST]->GetIsSuccess() &&
			tutorialUI_[EventType::LEFTSLASH]->GetIsSuccess() &&
			tutorialUI_[EventType::RIGHTSLASH]->GetIsSuccess()) {

			eventCount_++;
		}

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {

			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Up) {
					tutorialUI_[EventType::TOPDEFENSE]->SetIsSuccess(true);
				}

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Down) {
					tutorialUI_[EventType::BOTTOMDEFENSE]->SetIsSuccess(true);
				}

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Left) {
					tutorialUI_[EventType::LEFTDEFENSE]->SetIsSuccess(true);
				}

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Right) {
					tutorialUI_[EventType::RIGHTDEFENSE]->SetIsSuccess(true);
				}
			}
		}

		if (player_->GetAttackType() == Player::AttackType::kDownSwing) {
			tutorialUI_[EventType::DOWNSWING]->SetIsSuccess(true);
		}

		if (player_->GetAttackType() == Player::AttackType::kThrust) {
			tutorialUI_[EventType::THRUST]->SetIsSuccess(true);
		}

		if (player_->GetAttackType() == Player::AttackType::kRightSlash) {
			tutorialUI_[EventType::RIGHTSLASH]->SetIsSuccess(true);
		}

		if (player_->GetAttackType() == Player::AttackType::kLeftSlash) {
			tutorialUI_[EventType::LEFTSLASH]->SetIsSuccess(true);
		}

		break;

	default:
		break;
	}

	for (auto& ui : tutorialUI_) {
		ui->Update();
	}
}

void TutorialEvent::Draw() {

	for (auto& ui : tutorialUI_) {
		ui->Draw();
	}
}
