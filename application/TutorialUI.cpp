#include "TutorialUI.h"

#include "Player.h"

void TutorialUI::Initialize(Player* player, Vector2 pos, std::string textSpriteFileName, std::initializer_list<UIType> uiTypes) {

	//プレイヤーのポインタを取得
	player_ = player;

	/// === バッググラウンドスプライトの生成 === ///

	//生成
	textBackGround_ = std::make_unique<Sprite>();

	//初期化
	textBackGround_->Initialize("white1x1.png", pos);

	//アンカーポイントを設定
	textBackGround_->SetAnchorPoint({ 0.5f, 0.5f });

	//サイズを設定
	textBackGround_->SetSize(Vector2(160.0f, 120.0f));

	//色を設定
	textBackGround_->SetColor(Vector3(0.0f, 0.0f, 0.0f));

	//アルファ値を設定
	textBackGround_->SetAlpha(0.5f);

	float bgWidth = textBackGround_->GetSize().x / 2.0f;

	float bgHeight = textBackGround_->GetSize().y / 2.0f;

	Vector2 bgPos = textBackGround_->GetPosition();

	/// === テキストスプライトの生成 === ///

	//生成
	textSprite_ = std::make_unique<Sprite>();

	//初期化
	textSprite_->Initialize(textSpriteFileName, Vector2(bgPos.x,bgPos.y - 25.0f));

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

			newUISprite->Initialize("xbox_guide.png", pos);
			break;
		}

		float uiWidth = newUISprite->GetSize().x / 2.0f;

		float uiHeight = newUISprite->GetSize().y / 2.0f;

		newUISprite->SetAnchorPoint({ 0.5f, 0.5f });

		newUISprite->SetPosition(
			Vector2(
				bgPos.x + static_cast<float>(layoutNumber) * uiWidth,
				bgPos.y + uiHeight
			)
		);

		controllerUI_.push_back(std::move(newUISprite));

		layoutNumber += 2;
	}
}

void TutorialUI::Update() {

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
