#include "TutorialEvent.h"

void TutorialEvent::Initialize(Player* player) {
	player_ = player;
	// --- UIの初期化 ---

	for (int i = 0; i < 4; i++) {
		std::unique_ptr<TutorialUI> newUI = std::make_unique<TutorialUI>();
		tutorialUI_.push_back(std::move(newUI));
	}

	// --- UIの初期化 ---

	tutorialUI_[0]->Initialize(
		Vector2(100.0f, 100.0f),
		"DownSwing.png",
		{
			TutorialUI::UIType::kRStickUp,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[1]->Initialize(
		Vector2(100.0f, 270.0f),
		"Thrust.png",
		{
			TutorialUI::UIType::kRStickDown,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[2]->Initialize(
		Vector2(100.0f, 440.0f),
		"RightSlash.png",
		{
			TutorialUI::UIType::kRStickLeft,
			TutorialUI::UIType::kRButton,
		}
		);

	tutorialUI_[3]->Initialize(
		Vector2(100.0f, 610.0f),
		"LeftSlash.png",
		{
			TutorialUI::UIType::kRStickRight,
			TutorialUI::UIType::kRButton,
		}
		);
}

void TutorialEvent::Update() {

	if (player_->GetAttackType() == Player::AttackType::kDownSwing) {
		tutorialUI_[0]->SetIsSuccess(true);
	}

	if (player_->GetAttackType() == Player::AttackType::kThrust) {
		tutorialUI_[1]->SetIsSuccess(true);
	}

	if (player_->GetAttackType() == Player::AttackType::kRightSlash) {
		tutorialUI_[2]->SetIsSuccess(true);
	}

	if (player_->GetAttackType() == Player::AttackType::kLeftSlash) {
		tutorialUI_[3]->SetIsSuccess(true);
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
