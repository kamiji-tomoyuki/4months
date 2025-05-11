#include "TutorialUI.h"

#include "Player.h"

void TutorialUI::Initialize(Player* player, Vector2 pos, std::string textSpriteFileName, std::initializer_list<UIType> uiTypes) {

	player_ = player;

	textBackGround_ = std::make_unique<Sprite>();

	textBackGround_->Initialize("white1x1.png", pos);

	textBackGround_->SetSize(Vector2(160.0f, 120.0f));

	textBackGround_->SetColor(Vector3(0.0f, 0.0f, 0.0f));

	textBackGround_->SetAlpha(0.2f);

	textSprite_ = std::make_unique<Sprite>();

	textSprite_->Initialize(textSpriteFileName, pos);

	int uiCount = static_cast<int>(uiTypes.size());

	int layoutNumber = -uiCount + 1;

	float bgWidth = textBackGround_->GetSize().x / 2.0f;

	float bgHeight = textBackGround_->GetSize().y / 2.0f;

	for (UIType uiType : uiTypes) {

		std::unique_ptr<Sprite> newUISprite = nullptr;

		newUISprite = std::make_unique<Sprite>();

		switch (uiType) {
		case TutorialUI::UIType::kRightStick:

			newUISprite->Initialize("xbox_stick_r.png", pos);

			break;
		case TutorialUI::UIType::kLeftStick:

			newUISprite->Initialize("xbox_stick_l.png", pos);

			break;
		case TutorialUI::UIType::kAButton:
			break;
		case TutorialUI::UIType::kButton:
			break;
		case TutorialUI::UIType::kXButton:
			break;
		case TutorialUI::UIType::kYButton:
			break;
		case TutorialUI::UIType::kRButton:

			newUISprite->Initialize("xbox_rb.png", pos);

			break;
		case TutorialUI::UIType::kLButton:
			break;
		case TutorialUI::UIType::kRTrigger:
			break;
		case TutorialUI::UIType::kLTrigger:
			break;
		default:
			break;
		}

		float uiWidth = newUISprite->GetSize().x / 2.0f;

		float uiHeight = newUISprite->GetSize().y / 2.0f;

		newUISprite->SetAnchorPoint({ 0.5f, 0.5f });

		newUISprite->SetPosition(
			Vector2(
				pos.x + bgWidth + static_cast<float>(layoutNumber) * uiWidth,
				pos.y + bgHeight + uiHeight
			)
		);

		controllerUI_.push_back(std::move(newUISprite));

		layoutNumber += 2;
	}
}

void TutorialUI::Update() {

	if (player_->GetAttackType() == Player::AttackType::kRightSlash) {

		isSuccess = true;
	}

	if (isSuccess) {

		textBackGround_->SetColor(Vector3(0.0f, 1.0f, 0.0f));
	}
}

void TutorialUI::Draw() {

	textBackGround_->Draw();

	textSprite_->Draw();

	for (auto& ui : controllerUI_) {
		ui->Draw();
	}
}
