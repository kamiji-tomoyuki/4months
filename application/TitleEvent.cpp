#include "TitleEvent.h"

#include "Easing.h"

#include "Input.h"

void TitleEvent::Initialize() {

	state_ = TITLE;

	stageSelectState_ = TUTORIAL;

	isEventEnd_ = true;

	isSceneChange_ = false;

	maxTime_ = 1.0f;

	timer_ = 0.0f;

	titleTranslation_ = { 0.0f, 1.0f, 0.0f };
	titleRotation_ = { -0.1f, 0.0f, 0.0f };

	stageSelectTranslation_ = { 0.0f, 6000.0f, -7000.0f };
	stageSelectRotation_ = { 0.7f, 0.0f, 0.0f };

	gameStartTranslation_ = { 0.0f, 1.0f, 0.0f };
	gameStartRotation_ = { 0.7f, 0.0f, 0.0f };

	stageDistance_ = 3000.0f;

	stageSelectUI_ = std::make_unique<Sprite>();
	stageSelectUI_->Initialize("StageSelect.png", { 640.0f,600.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });

	startUI_ = std::make_unique<Sprite>();
	startUI_->Initialize("Return_Start.png", { 640.0f,650.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });

	stage1UI_ = std::make_unique<Sprite>();
	stage1UI_->Initialize("Stage1.png", { 640.0f,60.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });

	padUI_ = std::make_unique<Sprite>();
	padUI_->Initialize("TitlePad.png", { 640.0f,60.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });
	padUI_->SetSize(padUI_->GetSize() * 0.5f);

	selectUI_ = std::make_unique<Sprite>();
	selectUI_->Initialize("Select.png", { 640.0f,550.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });

	tutorialStageUI_ = std::make_unique<Sprite>();
	tutorialStageUI_->Initialize("TutorialStage.png", { 640.0f,60.0f }, { 0.25f,0.25f,0.25f,1.0f }, { 0.5f,0.5f });

}

void TitleEvent::Update() {

	XINPUT_STATE joyState;

	size_t groundCount = 0;

	isEventEnd_ = true;

	if (timer_ <= maxTime_) {
		timer_ += 1.0f / 60.0f;

		isEventEnd_ = false;
	}

	//// UI点滅
	alphaTimer_ += alphaSpeed_;

	if (alphaTimer_ >= 1.0f || alphaTimer_ < 0.0f) {
		alphaSpeed_ *= -1.0f;
	}

	if (isEventEnd_) {

		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {

			state_ = static_cast<State>(static_cast<int>(state_) - 1);

			timer_ = 0.0f;
		}

		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

			state_ = static_cast<State>(static_cast<int>(state_) + 1);

			timer_ = 0.0f;
		}
	}

	if (state_ <= 0) {
		state_ = static_cast<State>(0);
	}

	if (state_ >= static_cast<State>(State::STATEEND)) {
		state_ = static_cast<State>(State::STATEEND - 1);
	}

	switch (state_) {
	case TITLE:

		vp_->translation_ = EaseOutQuad(vp_->translation_, titleTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseOutQuad(vp_->rotation_, titleRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(EaseOutQuad(ground->GetCenterRotation(), { 0.0f,0.0f,0.0f }, timer_, maxTime_));

			ground->SetWorldPosition(
				EaseInCubic(
					ground->GetCenterPosition(),
					Vector3(stageDistance_ * static_cast<float>(static_cast<int>(groundCount) - static_cast<int>(stageSelectState_)), 0.0f, 0.0f),
					timer_,
					maxTime_
				)
			);

			groundCount++;
		}

		stageSelectUI_->SetAlpha(alphaTimer_);

		break;
	case STAGESELECT:

		vp_->translation_ = EaseInCubic(vp_->translation_, stageSelectTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseInCubic(vp_->rotation_, stageSelectRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(ground->GetCenterRotation() + Vector3(0.0f, 0.01f, 0.0f));

			ground->SetWorldPosition(
				EaseInCubic(
					ground->GetCenterPosition(),
					Vector3(stageDistance_ * static_cast<float>(static_cast<int>(groundCount) - static_cast<int>(stageSelectState_)), 0.0f, 0.0f),
					timer_,
					maxTime_
				)
			);

			groundCount++;
		}

		if (isEventEnd_) {

			if (Input::GetInstance()->GetJoyStickDirection(0, false) == Input::JoyStickDirection::Left) {

				stageSelectState_ = static_cast<StageSelect>(static_cast<int>(stageSelectState_) - 1);

				timer_ = 0.0f;
			}

			if (Input::GetInstance()->GetJoyStickDirection(0, false) == Input::JoyStickDirection::Right) {

				stageSelectState_ = static_cast<StageSelect>(static_cast<int>(stageSelectState_) + 1);

				timer_ = 0.0f;
			}
		}

		if (stageSelectState_ <= 0) {
			stageSelectState_ = static_cast<StageSelect>(0);
		}

		if (stageSelectState_ >= static_cast<StageSelect>(StageSelect::STAGEEND)) {
			stageSelectState_ = static_cast<StageSelect>(StageSelect::STAGEEND - 1);
		}

		startUI_->SetAlpha(alphaTimer_);
		selectUI_->SetAlpha(alphaTimer_);

		break;
	case GAMESTART:

		vp_->translation_ = EaseInOutQuad(vp_->translation_, gameStartTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseInOutQuad(vp_->rotation_, gameStartRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(ground->GetCenterRotation() + Vector3(0.0f, 0.01f, 0.0f));
		}

		isSceneChange_ = true;

		break;
	}

	padUI_->SetAlpha(timer_);
	tutorialStageUI_->SetAlpha(timer_);
	stage1UI_->SetAlpha(timer_);
}

void TitleEvent::Draw() {

	switch (state_) {
	case TitleEvent::TITLE:

		padUI_->Draw();

		stageSelectUI_->Draw();

		break;
	case TitleEvent::STAGESELECT:

		switch (stageSelectState_) {
		case TitleEvent::TUTORIAL:

			tutorialStageUI_->Draw();
			break;
		case TitleEvent::STAGE1:

			stage1UI_->Draw();
			break;
		}

		startUI_->Draw();

		selectUI_->Draw();

		break;
	case TitleEvent::GAMESTART:
		break;
	case TitleEvent::STATEEND:
		break;
	default:
		break;
	}
}

void TitleEvent::ImGui() {

#ifdef _DEBUG

	if (ImGui::BeginTabBar("TitleEvent")) {

		if (ImGui::BeginTabItem("TitleEvent")) {

			if (ImGui::Button("State Title")) {
				state_ = TITLE;
				timer_ = 0.0f;
			}
			if (ImGui::Button("State StageSelect")) {
				state_ = STAGESELECT;
				timer_ = 0.0f;
			}
			if (ImGui::Button("State GameStart")) {
				state_ = GAMESTART;
				timer_ = 0.0f;
			}

			ImGui::Text("State : %s", GetStateString());

			ImGui::Text("StageSelect : %s", GetStageSelectString());

			ImGui::Text("timer_ : %f", timer_);

			ImGui::Text("maxTime_ : %f", maxTime_);

			ImGui::Text("VP translation : %f,%f,%f", vp_->translation_.x, vp_->translation_.y, vp_->translation_.z);

			ImGui::Text("VP rotation : %f,%f,%f", vp_->rotation_.x, vp_->rotation_.y, vp_->rotation_.z);

			ImGui::DragFloat3("Title Translation", &titleTranslation_.x, 0.1f);

			ImGui::DragFloat3("Title Rotation", &titleRotation_.x, 0.1f);

			ImGui::DragFloat3("StageSelect Translation", &stageSelectTranslation_.x, 0.1f);

			ImGui::DragFloat3("StageSelect Rotation", &stageSelectRotation_.x, 0.1f);

			ImGui::DragFloat3("GameStart Translation", &gameStartTranslation_.x, 0.1f);

			ImGui::DragFloat3("GameStart Rotation", &gameStartRotation_.x, 0.1f);

			ImGui::Text("TutorialGroundTranslation : %f,%f,%f", grounds_[0]->GetWorldPosition().x, grounds_[0]->GetWorldPosition().y, grounds_[0]->GetWorldPosition().z);

			ImGui::Text("Stage1GroundTranslation : %f,%f,%f", grounds_[1]->GetWorldPosition().x, grounds_[1]->GetWorldPosition().y, grounds_[1]->GetWorldPosition().z);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

#endif // _DEBUG
}
