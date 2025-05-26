#include "TutorialUI.h"

#include "TextureManager.h"

#include "Input.h"
#include "Easing.h"

void TutorialUI::Initialize(Vector2 pos, std::string textSpriteFileName, std::initializer_list<UIType> uiTypes) {

	uiTypes_ = uiTypes;

	/// === バッググラウンドスプライトの生成 === ///

	//生成
	textBackGround_ = std::make_unique<Sprite>();

	//初期化
	textBackGround_->Initialize("TextBackGround.png", pos);

	textBackGround_->SetColor(Vector3(0.0f, 0.0f, 0.0f));

	textBackGround_->SetAlpha(0.0f);

	//アンカーポイントを設定
	textBackGround_->SetAnchorPoint({ 0.5f, 0.5f });

	float bgWidth = textBackGround_->GetSize().x / 2.0f;

	float bgHeight = textBackGround_->GetSize().y / 2.0f;

	Vector2 bgPos = textBackGround_->GetPosition();

	/// === テキストスプライトの生成 === ///

	//生成
	textSprite_ = std::make_unique<Sprite>();

	//初期化
	textSprite_->Initialize(textSpriteFileName, Vector2(bgPos.x, bgPos.y - 25.0f));

	textSprite_->SetAlpha(0.0f);

	//アンカーポイントを設定
	textSprite_->SetAnchorPoint({ 0.5f, 0.5f });

	/// === アイコンスプライトの生成 === ///

	//アイコンの数
	int uiCount = static_cast<int>(uiTypes.size());

	//アイコンの配置番号
	int layoutNumber = -uiCount + 1;

	for (UIType uiType : uiTypes) {

		std::unique_ptr<Sprite> newUISprite = nullptr;

		newUISprite = std::make_unique<Sprite>();

		switch (uiType) {
		case TutorialUI::UIType::kRStickUp:

			newUISprite->Initialize("xbox_stick_r_up.png", pos);
			break;
		case TutorialUI::UIType::kRStickDown:

			newUISprite->Initialize("xbox_stick_r_down.png", pos);
			break;
		case TutorialUI::UIType::kRStickLeft:

			newUISprite->Initialize("xbox_stick_r_left.png", pos);
			break;
		case TutorialUI::UIType::kRStickRight:

			newUISprite->Initialize("xbox_stick_r_right.png", pos);
			break;
		case TutorialUI::UIType::kRStick:

			newUISprite->Initialize("xbox_stick_r.png", pos);
			break;
		case TutorialUI::UIType::kLStick:

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

			newUISprite->Initialize("xbox_lb.png", pos);
			break;
		case TutorialUI::UIType::kRTrigger:
			break;
		case TutorialUI::UIType::kLTrigger:
			break;
		case TutorialUI::UIType::kCount:

			TextureManager::GetInstance()->LoadTexture("Zero.png");
			TextureManager::GetInstance()->LoadTexture("One.png");
			TextureManager::GetInstance()->LoadTexture("Two.png");
			TextureManager::GetInstance()->LoadTexture("Three.png");
			TextureManager::GetInstance()->LoadTexture("Four.png");
			TextureManager::GetInstance()->LoadTexture("Five.png");

			newUISprite->Initialize("Zero.png", pos);

			break;
		case TutorialUI::UIType::kMaxCount:

			newUISprite->Initialize("Five.png", pos);
			break;
		case TutorialUI::UIType::kSlash:

			newUISprite->Initialize("Slash.png", pos);
			break;
		default:

			newUISprite->Initialize("xbox_guide.png", pos);
			break;
		}

		float uiWidth = newUISprite->GetSize().x / 2.0f;

		float uiHeight = newUISprite->GetSize().y / 2.0f;

		if (uiType == UIType::kCount || uiType == UIType::kMaxCount) {

			uiWidth *= 0.5f;
		}

		newUISprite->SetAnchorPoint({ 0.5f, 0.5f });

		newUISprite->SetPosition(
			Vector2(
				bgPos.x + static_cast<float>(layoutNumber) * uiWidth,
				bgPos.y + uiHeight
			)
		);

		newUISprite->SetColor(Vector3(0.2f, 0.2f, 0.2f));

		newUISprite->SetAlpha(0.0f);

		controllerUI_.push_back(std::move(newUISprite));

		layoutNumber += 2;
	}

	alphaTime_ = 0.0f;

	alphaTimeMax_ = 0.5f;

	isSuccess = false;

	isActive = false;
}

void TutorialUI::Update() {

	for (int i = 0; i < uiTypes_.size(); i++) {

		XINPUT_STATE joyState;

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {

			switch (uiTypes_[i]) {
			case TutorialUI::UIType::kRStickUp:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Up) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}

				break;
			case TutorialUI::UIType::kRStickDown:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Down) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}
				break;
			case TutorialUI::UIType::kRStickLeft:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Left) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}

				break;
			case TutorialUI::UIType::kRStickRight:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) == Input::JoyStickDirection::Right) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}

				break;
			case TutorialUI::UIType::kRStick:

				if (Input::GetInstance()->GetJoyStickDirection(0, true) != Input::JoyStickDirection::None) {
					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {
					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}
				break;
			case TutorialUI::UIType::kLStick:

				if (Input::GetInstance()->GetJoyStickDirection(0, false) != Input::JoyStickDirection::None) {
					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {
					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}
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

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}
				break;
			case TutorialUI::UIType::kLButton:

				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {

					controllerUI_[i]->SetColor(Vector3(1.0f, 1.0f, 1.0f));
				} else {

					controllerUI_[i]->SetColor(Vector3(0.2f, 0.2f, 0.2f));
				}
				break;
			case TutorialUI::UIType::kRTrigger:
				break;
			case TutorialUI::UIType::kLTrigger:
				break;
			case TutorialUI::UIType::kCount:

				switch (count_) {
				case 0:

					controllerUI_[i]->SetTexturePath("Zero.png");

					break;
				case 1:

					controllerUI_[i]->SetTexturePath("One.png");

					break;
				case 2:

					controllerUI_[i]->SetTexturePath("Two.png");

					break;
				case 3:

					controllerUI_[i]->SetTexturePath("Three.png");

					break;
				case 4:

					controllerUI_[i]->SetTexturePath("Four.png");

					break;
				case 5:

					controllerUI_[i]->SetTexturePath("Five.png");

					break;
				}

				break;
			default:

				break;
			}
		}
	}

	if (isActive) {

		alphaTime_ += 1.0f / 60.0f;
	} else {

		alphaTime_ -= 1.0f / 60.0f;
	}

	if (alphaTime_ >= alphaTimeMax_) {

		alphaTime_ = alphaTimeMax_;
	} else if (alphaTime_ <= 0.0f) {

		alphaTime_ = 0.0f;
	} else {

		if (isSuccess) {
			isSuccess = false;
		}
	}

	float t = alphaTime_ / alphaTimeMax_;

	textBackGround_->SetAlpha(Lerp(0.0f, 1.0f, t));

	textSprite_->SetAlpha(Lerp(0.0f, 1.0f, t));

	if (isSuccess) {

		textBackGround_->SetColor(Vector3(0.0f, 1.0f, 0.0f));
	}

	for (auto& ui : controllerUI_) {
		ui->SetAlpha(Lerp(0.0f, 1.0f, t));
	}
}

void TutorialUI::Draw() {

	textBackGround_->Draw();

	textSprite_->Draw();

	for (auto& ui : controllerUI_) {
		ui->Draw();
	}
}
