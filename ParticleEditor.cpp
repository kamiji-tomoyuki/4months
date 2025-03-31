#include "ParticleEditor.h"
#include "imgui.h"
#include "fstream"
#include "filesystem"
void ParticleEditor::Initialize(ViewProjection* vp) {

	// ビュープロジェクションの設定
	viewProjection_ = vp;

	// 最大タイマーの初期化
	maxTime_ = 120.0f;

	// タイマーの初期化
	timer_ = 0.0f;

	// 前フレームのタイマーの初期化
	preTimer_ = 0.0f;

	// スタートフラグの初期化
	isStart_ = false;

	// ループフラグの初期化
	isLoop_ = false;

	// エミッターファイルの読み込み
	for (const auto& entry : std::filesystem::directory_iterator(kDirectoryPath_)) {

		// jsonファイルのみ取得
		if (entry.path().extension() == ".json") {

			// ファイル名を追加
			emitterFiles_.push_back(entry.path().filename().string());
		}
	}
}

void ParticleEditor::Update() {

	// タイマースタートされていたら
	if (isStart_) {

		// タイマーを進める
		timer_ += 1.0f / 60.0f;
	}

	// タイマーが最大時間を超えたら
	if (timer_ >= maxTime_) {

		// ループフラグが立っていたら
		if (isLoop_) {

			// タイマーをリセット
			timer_ = 0.0f;
		} else {

			// タイマーを最大時間でとめる
			timer_ = maxTime_;
		}
	}

	// エミッターの更新
	for (const auto& emitter : emitters_) {

		emitter->Update();

	}
}

void ParticleEditor::Draw() {

	// エミッターの描画
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

	// 新規エミッターの作成
	std::unique_ptr<ParticleEmitter> newEmitter;

	// エミッターの生成
	newEmitter = std::make_unique<ParticleEmitter>();

	// エミッターの初期化
	newEmitter->Initialize();

	// エミッターリストに追加
	emitters_.push_back(std::move(newEmitter));
}

void ParticleEditor::LoadEmitter(const std::string& fileName) {

	// 新規エミッターの作成
	std::unique_ptr<ParticleEmitter> newEmitter;

	// エミッターの生成
	newEmitter = std::make_unique<ParticleEmitter>();

	// エミッターの初期化
	newEmitter->Initialize();

	// エミッターの読み込み
	newEmitter->LoadEmitterData(fileName);

	// エミッターリストに追加
	emitters_.push_back(std::move(newEmitter));
}