#include "ParticleEditor.h"
#include "imgui.h"

void ParticleEditor::Initialize(ViewProjection* vp) {

	viewProjection_ = vp;

	maxTime_ = 120.0f;

	timer_ = 0.0f;

	preTimer_ = 0.0f;

	isStart_ = false;

	isLoop_ = false;
}

void ParticleEditor::Update() {

	if (isStart_) {

		timer_ += 1.0f / 60.0f;
	}

	if (timer_ >= maxTime_) {

		if (isLoop_) {

			timer_ = 0.0f;
		} else {

			timer_ = maxTime_;
		}
	}

	for (const auto& emitter : emitters_) {

		emitter->Update();

		if (timer_ - preTimer_ >= 0.0f) {

			emitter->AddTime(timer_ - preTimer_);
		}
	}

	preTimer_ = timer_;
}

void ParticleEditor::Draw() {

	for (const auto& emitter : emitters_) {

		emitter->Draw();
	}
}

void ParticleEditor::ImGui() {

	ImGui::Begin("Timer");

	ImGui::Columns(2, "TimerColumns", false); // 2列レイアウト

	ImGui::Text("タイマー"); ImGui::NextColumn();
	ImGui::SliderFloat("##タイマー", &timer_, 0.0f, maxTime_);
	ImGui::NextColumn();

	ImGui::Text("スタート"); ImGui::NextColumn();
	ImGui::Checkbox("##スタート", &isStart_);
	ImGui::NextColumn();

	ImGui::Text("ループ"); ImGui::NextColumn();
	ImGui::Checkbox("##ループ", &isLoop_);
	ImGui::NextColumn();

	ImGui::Columns(1);

	ImGui::End();

	ImGui::Begin("ParticleEditor");

	if (ImGui::BeginTabBar("ParticleEditorTabBer")) {

		for (const auto& emitter : emitters_) {

			if (ImGui::BeginTabItem(emitter->GetName().c_str())) {

				emitter->ImGui();

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ParticleEditor::SaveEmitterData(ParticleEmitter* emitter) {

}

void ParticleEditor::LoadEmitterData(const std::string& fileName) {
}

void ParticleEditor::CreateEmitter(const std::string& name, const std::string& fileName) {

	std::unique_ptr<ParticleEmitter> newEmitter;

	newEmitter = std::make_unique<ParticleEmitter>();

	newEmitter->Initialize(name, fileName);

	emitters_.push_back(std::move(newEmitter));
}