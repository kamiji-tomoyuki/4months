#include "ParticleEditor.h"
#include "imgui.h"
#include "fstream"
#include "filesystem"
void ParticleEditor::Initialize(ViewProjection* vp) {

	viewProjection_ = vp;

	maxTime_ = 120.0f;

	timer_ = 0.0f;

	preTimer_ = 0.0f;

	isStart_ = false;

	isLoop_ = false;

	for (const auto& entry : std::filesystem::directory_iterator(kDirectoryPath_)) {
		if (entry.path().extension() == ".json") {
			emitterFiles_.push_back(entry.path().filename().string());
		}
	}
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

		//emitter->SetTime(timer_);

		emitter->Update();

	}
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

		if (ImGui::BeginTabItem("Create Emitter")) {

			std::vector<const char*> items;

			int currentItem = 0;

			for (const auto& file : emitterFiles_) {
				items.push_back(file.c_str());
			}

			items.insert(items.begin(), "ファイルを選択");

			ImGui::Text("新規データを生成");
			if (ImGui::Button("新規作成")) {
				CreateEmitter();
			}

			ImGui::Separator();

			ImGui::Text("既存データから生成");
			if (ImGui::Combo("##既存データから生成", &currentItem, items.data(), static_cast<int>(items.size()))) {
				LoadEmitter(items[currentItem]);
			}

			ImGui::EndTabItem();
		}

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

void ParticleEditor::CreateEmitter() {

	std::unique_ptr<ParticleEmitter> newEmitter;

	newEmitter = std::make_unique<ParticleEmitter>();

	newEmitter->Initialize();

	emitters_.push_back(std::move(newEmitter));
}

void ParticleEditor::LoadEmitter(const std::string& fileName) {

	std::unique_ptr<ParticleEmitter> newEmitter;

	newEmitter = std::make_unique<ParticleEmitter>();

	newEmitter->Initialize();

	newEmitter->LoadEmitterData(fileName);

	emitters_.push_back(std::move(newEmitter));
}