#include "TitleEvent.h"

#include "Easing.h"

void TitleEvent::Initialize() {

	state_ = TITLE;

	maxTime_ = 2.0f;

	timer_ = 0.0f;

	titleTranslation_ = { 0.0f, 1.0f, 0.0f };
	titleRotation_ = { -0.1f, 0.0f, 0.0f };

	stageSelectTranslation_ = { 0.0f, 6000.0f, -7000.0f };
	stageSelectRotation_ = { 0.7f, 0.0f, 0.0f };

	gameStartTranslation_ = { 0.0f, 1.0f, 0.0f };
	gameStartRotation_ = { 0.7f, 0.0f, 0.0f };
}

void TitleEvent::Update() {

	if (timer_ <= maxTime_) {
		timer_ += 1.0f / 60.0f;
	}

	switch (state_) {
	case TITLE:

		vp_->translation_ = EaseOutQuad(vp_->translation_, titleTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseOutQuad(vp_->rotation_, titleRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(EaseOutQuad(ground->GetCenterRotation(), { 0.0f,0.0f,0.0f }, timer_, maxTime_));
		}

		break;
	case STAGESELECT:

		vp_->translation_ = EaseInCubic(vp_->translation_, stageSelectTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseInCubic(vp_->rotation_, stageSelectRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(ground->GetCenterRotation() + Vector3(0.0f, 0.01f, 0.0f));
		}

		break;
	case GAMESTART:

		vp_->translation_ = EaseInOutQuad(vp_->translation_, gameStartTranslation_, timer_, maxTime_);

		vp_->rotation_ = EaseInOutQuad(vp_->rotation_, gameStartRotation_, timer_, maxTime_);

		for (auto& ground : grounds_) {
			ground->SetRotation(ground->GetCenterRotation() + Vector3(0.0f, 0.01f, 0.0f));
		}
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

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

#endif // _DEBUG
}
