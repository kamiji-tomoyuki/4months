#include "ParticleEmitter.h"
#include"line/DrawLine3D.h"
#include "array"
#include "fstream"
#include "Input.h"

// コンストラクタ
ParticleEmitter::ParticleEmitter() {}

void ParticleEmitter::Initialize() {

	//マネージャーを取得
	Manager_ = ParticleManager::GetInstance();

	emitTimer = 0.0f;

	/// === エミッター設定 === ///

	name_ = "newEmitter";
	maxEmitNum_ = 1;
	count_ = 0;
	frequency_ = 0.01f;
	lifeTime_ = 1.0f;
	lifeTimeRandomRange_ = 0.0f;
	startTime_ = 0.0f;
	endTime_ = 3.0f;
	isActive_ = false;
	isLoop_ = false;
	isDrawEmitter_ = true;

	transform_.Initialize();

	/// === 座標設定 === ///

	positionState_ = START;
	positionEasingState_ = LERP;
	position_.startNum = { 0.0f, 0.0f, 0.0f };
	position_.startRandomRange = { 0.0f,0.0f,0.0f };
	position_.endNum = { 0.0f, 0.0f, 0.0f };
	position_.endRandomRange = { 0.0f, 0.0f, 0.0f };
	position_.velocity = { 0.0f, 0.0f, 0.0f };
	position_.velocityRandomRange = { 0.0f, 0.0f, 0.0f };
	position_.acceleration = { 0.0f, 0.0f, 0.0f };
	position_.accelerationRandomRange = { 0.0f, 0.0f, 0.0f };

	/// === 角度設定 === ///

	rotationState_ = START;
	rotationEasingState_ = LERP;
	rotation_.startNum = { 0.0f, 0.0f, 0.0f };
	rotation_.startRandomRange = { 0.0f, 0.0f, 0.0f };
	rotation_.endNum = { 0.0f, 0.0f, 0.0f };
	rotation_.endRandomRange = { 0.0f, 0.0f, 0.0f };
	rotation_.velocity = { 0.0f, 0.0f, 0.0f };
	rotation_.velocityRandomRange = { 0.0f, 0.0f, 0.0f };
	rotation_.acceleration = { 0.0f, 0.0f, 0.0f };
	rotation_.accelerationRandomRange = { 0.0f, 0.0f, 0.0f };

	/// === 拡縮設定 === ///

	scaleState_ = START;
	scaleEasingState_ = LERP;
	scale_.startNum = { 1.0f, 1.0f, 1.0f };
	scale_.startRandomRange = { 0.0f, 0.0f, 0.0f };
	scale_.endNum = { 1.0f, 1.0f, 1.0f };
	scale_.endRandomRange = { 0.0f, 0.0f, 0.0f };
	scale_.velocity = { 0.0f, 0.0f, 0.0f };
	scale_.velocityRandomRange = { 0.0f, 0.0f, 0.0f };
	scale_.acceleration = { 0.0f, 0.0f, 0.0f };
	scale_.accelerationRandomRange = { 0.0f, 0.0f, 0.0f };

	/// === モデル設定 === ///

	modelName_ = "star.obj";
	startColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	endColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	colorRandomRange_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	isBillboard_ = true;

	//設定を基にパーティクルグループを生成
	Manager_->CreateParticleGroup(name_, modelName_);
}

// Update関数
void ParticleEmitter::Update() {

	// アクティブなら
	if (isActive_) {

		totalTimer_ += 1.0f / 60.0f;
	}

	if (totalTimer_ >= endTime_) {

		isActive_ = false;

		if (isLoop_) {

			Start();
		}
	}

	if (totalTimer_ >= startTime_) {

		if (totalTimer_ - emitTimer >= frequency_) {

			// パーティクルを発生させる
			Emit();

			emitTimer = totalTimer_;
		}
	}

	transform_.UpdateMatrix();
}

void ParticleEmitter::UpdateOnce() {

	if (isActive_) {

		Emit();
	}

	transform_.UpdateMatrix();
}

void ParticleEmitter::Draw() {

	DrawEmitter();
}

void ParticleEmitter::DrawEmitter() {
	// isDrawEmitter_がtrueのときだけ描画
	if (!isDrawEmitter_) return;

	Vector3 range = position_.startRandomRange;

	// 立方体のローカル座標での基本頂点（スケーリング済み）
	std::array<Vector3, 8> localVertices = {
		Vector3{-1.0f - range.x, -1.0f - range.y, -1.0f - range.z}, // 左下手前
		Vector3{ 1.0f + range.x, -1.0f - range.y, -1.0f - range.z}, // 右下手前
		Vector3{-1.0f - range.x,  1.0f + range.y, -1.0f - range.z}, // 左上手前
		Vector3{ 1.0f + range.x,  1.0f + range.y, -1.0f - range.z}, // 右上手前
		Vector3{-1.0f - range.x, -1.0f - range.y,  1.0f + range.z}, // 左下奥
		Vector3{ 1.0f + range.x, -1.0f - range.y,  1.0f + range.z}, // 右下奥
		Vector3{-1.0f - range.x,  1.0f + range.y,  1.0f + range.z}, // 左上奥
		Vector3{ 1.0f + range.x,  1.0f + range.y,  1.0f + range.z}  // 右上奥
	};

	// ワールド変換結果を格納する配列
	std::array<Vector3, 8> worldVertices;

	// ワールド行列の計算
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_ + position_.startNum);

	// 頂点のワールド変換
	for (size_t i = 0; i < localVertices.size(); i++) {
		worldVertices[i] = Transformation(localVertices[i], worldMatrix);
	}

	// エッジリスト（線の接続順）
	constexpr std::array<std::pair<int, int>, 12> edges = {
		std::make_pair(0, 1), std::make_pair(1, 3), std::make_pair(3, 2), std::make_pair(2, 0), // 前面
		std::make_pair(4, 5), std::make_pair(5, 7), std::make_pair(7, 6), std::make_pair(6, 4), // 背面
		std::make_pair(0, 4), std::make_pair(1, 5), std::make_pair(2, 6), std::make_pair(3, 7)  // 側面
	};

	// エッジ描画
	for (const auto& edge : edges) {
		DrawLine3D::GetInstance()->SetPoints(worldVertices[edge.first], worldVertices[edge.second]);
	}
}

// Emit関数
void ParticleEmitter::Emit() {

	if (isLoop_) {
		count_ = 0;
	}

	while (1) {

		if (maxEmitNum_ <= count_) {
			return;
		}

		if (totalTimer_ <= emitTimer) {
			return;
		}

		Manager_->Emit(
			name_,
			transform_,
			maxEmitNum_,
			lifeTime_,
			lifeTimeRandomRange_,
			positionState_,
			positionEasingState_,
			position_,
			rotationState_,
			rotationEasingState_,
			rotation_,
			scaleState_,
			scaleEasingState_,
			scale_,
			startColor_,
			endColor_,
			colorRandomRange_,
			isBillboard_
		);

		count_++;

		emitTimer += frequency_;
	}

}

void ParticleEmitter::ReloadGroup(const std::string& fileName) {

	Manager_->ReloadGroup(name_, name_, fileName);
}

// ImGuiで値を動かす関数
void ParticleEmitter::ImGui() {
#ifdef _DEBUG

	/// === エミッター設定 === ///

	if (ImGui::CollapsingHeader("エミッター設定")) {
		ImGui::Columns(2, "EmitterColumns", false); // 2列レイアウト

		ImGui::Text("アクティブ"); ImGui::NextColumn();
		ImGui::Checkbox("##アクティブ", &isActive_);
		ImGui::NextColumn();

		ImGui::Text("ループ"); ImGui::NextColumn();
		ImGui::Checkbox("##ループ", &isLoop_);
		ImGui::NextColumn();

		ImGui::Text("エミッター描画"); ImGui::NextColumn();
		ImGui::Checkbox("##エミッター描画", &isDrawEmitter_);
		ImGui::NextColumn();

		std::string currentName = name_;

		ImGui::Text("名前"); ImGui::NextColumn();
		if (ImGui::InputText("##名前", currentName.data(), 256)) {
			if (Input::GetInstance()->PushKey(DIK_RETURN)) {
				currentName = currentName.c_str();  // 余分な '\0' を削除
				Manager_->ChangeGroupName(currentName, name_);
				name_ = currentName;
			}
		}
		ImGui::NextColumn();

		ImGui::Text("エミッター座標"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター座標", &transform_.translation_.x, 0.1f);
		ImGui::NextColumn();

		ImGui::Text("エミッター角度"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター角度", &transform_.rotation_.x, 0.1f);
		ImGui::NextColumn();

		ImGui::Text("エミッター拡縮"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター拡縮", &transform_.scale_.x, 0.1f);
		ImGui::NextColumn();

		ImGui::Separator();

		ImGui::Text("生成数"); ImGui::NextColumn();
		ImGui::DragInt("##生成数", &maxEmitNum_, 1, 1, 100);
		ImGui::NextColumn();

		ImGui::Text("発生頻度"); ImGui::NextColumn();
		ImGui::DragFloat("##発生頻度", &frequency_, 0.01f, 0.0f);
		ImGui::NextColumn();

		ImGui::Separator();

		ImGui::Text("生存時間"); ImGui::NextColumn();
		ImGui::DragFloat("##生存時間", &lifeTime_, 0.1f, 0.0f);
		ImGui::NextColumn();

		ImGui::Text("生存時間のランダム幅"); ImGui::NextColumn();
		ImGui::DragFloat("##生存時間のランダム幅", &lifeTimeRandomRange_, 0.1f, 0.0f);
		ImGui::NextColumn();

		ImGui::Text("生成開始時間"); ImGui::NextColumn();
		ImGui::DragFloat("##生成開始時間", &startTime_, 0.1f, 0.0f);
		ImGui::NextColumn();

		if (!isLoop_) {

			ImGui::Text("生成終了時間"); ImGui::NextColumn();
			ImGui::DragFloat("##生成終了時間", &endTime_, 0.1f, 0.0f);
			ImGui::NextColumn();
		}

		ImGui::Columns(1); // 列終了
	}

	/// === パーティクル設定 === ///

	if (ImGui::CollapsingHeader("座標設定")) {
		ImGui::Columns(2, "PositionColumns", false); // 2列レイアウト

		const char* items[] = { "位置","位置・速度・加速度","イージング" };

		int currentItem = static_cast<int>(positionState_);

		ImGui::Text("座標ステート"); ImGui::NextColumn();
		if (ImGui::Combo("##座標ステート", &currentItem, items, IM_ARRAYSIZE(items))) {
			positionState_ = static_cast<ParameterState>(currentItem);
		}
		ImGui::NextColumn();

		ImGui::Text("初期座標"); ImGui::NextColumn();
		ImGui::DragFloat3("##初期座標", &position_.startNum.x, 0.1f);
		ImGui::NextColumn();

		ImGui::Text("初期座標のランダム幅"); ImGui::NextColumn();
		ImGui::DragFloat3("##初期座標のランダム幅", &position_.startRandomRange.x, 0.1f, 0.0f);
		ImGui::NextColumn();

		ImGui::Separator();

		switch (positionState_) {
		case START:
			break;
		case VELOCITY:

			ImGui::Text("速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##速度", &position_.velocity.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##速度のランダム幅", &position_.velocityRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("加速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##加速度", &position_.acceleration.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("加速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##加速度のランダム幅", &position_.accelerationRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			break;
		case EASING:

			const char* easingItems[] = { "Lerp","EaseIn","EaseOut" };

			int currentItem = static_cast<int>(positionEasingState_);

			ImGui::Text("終了座標"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了座標", &position_.endNum.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("終了座標のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了座標のランダム幅", &position_.endRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("座標のイージング種類"); ImGui::NextColumn();
			if (ImGui::Combo("##座標のイージング種類", &currentItem, easingItems, sizeof(easingItems))) {
				positionEasingState_ = static_cast<EasingState>(currentItem);
			}
			ImGui::NextColumn();

			break;
		}

		ImGui::Columns(1); // 列終了
	}

	if (ImGui::CollapsingHeader("角度設定")) {
		ImGui::Columns(2, "RorationColumns", false); // 2列レイアウト

		const char* items[] = { "角度","角度・角速度・角加速度","イージング" };

		int currentItem = static_cast<int>(rotationState_);

		ImGui::Text("角度ステート"); ImGui::NextColumn();
		if (ImGui::Combo("##角度ステート", &currentItem, items, IM_ARRAYSIZE(items))) {
			rotationState_ = static_cast<ParameterState>(currentItem);
		}
		ImGui::NextColumn();

		ImGui::Text("初期角度"); ImGui::NextColumn();
		ImGui::DragFloat3("##角度", &rotation_.startNum.x, 0.01f, 0.0f, 3.14f);
		ImGui::NextColumn();

		ImGui::Text("初期角度のランダム幅"); ImGui::NextColumn();
		ImGui::DragFloat3("##角度のランダム幅", &rotation_.startRandomRange.x, 0.01f, 0.0f, 3.14f);
		ImGui::NextColumn();

		ImGui::Separator();

		switch (rotationState_) {
		case START:
			break;
		case VELOCITY:

			ImGui::Text("角速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##角速度", &rotation_.velocity.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Text("角速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##角速度のランダム幅", &rotation_.velocityRandomRange.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("角加速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##角加速度", &rotation_.acceleration.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Text("角加速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##角加速度のランダム幅", &rotation_.accelerationRandomRange.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			break;
		case EASING:

			const char* easingItems[] = { "Lerp","EaseIn","EaseOut" };

			int currentItem = static_cast<int>(rotationEasingState_);

			ImGui::Text("終了角度"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了角度", &rotation_.endNum.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Text("終了角度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了角度のランダム幅", &rotation_.endRandomRange.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("角度のイージング種類"); ImGui::NextColumn();
			if (ImGui::Combo("##角度のイージング種類", &currentItem, easingItems, IM_ARRAYSIZE(easingItems))) {
				rotationEasingState_ = static_cast<EasingState>(currentItem);
			}
			ImGui::NextColumn();

			break;
		}

		ImGui::Columns(1); // 列終了
	}

	if (ImGui::CollapsingHeader("拡大設定")) {
		ImGui::Columns(2, "ScaleColumns", false); // 2列レイアウト

		const char* items[] = { "拡大","拡大・拡大速度・拡大加速度","イージング" };

		int currentItem = static_cast<int>(scaleState_);

		ImGui::Text("拡縮ステート"); ImGui::NextColumn();
		if (ImGui::Combo("##拡縮ステート", &currentItem, items, IM_ARRAYSIZE(items))) {
			scaleState_ = static_cast<ParameterState>(currentItem);
		}
		ImGui::NextColumn();

		ImGui::Text("初期拡大"); ImGui::NextColumn();
		ImGui::DragFloat3("##初期拡大", &scale_.startNum.x, 0.1f);
		ImGui::NextColumn();

		ImGui::Text("初期拡大のランダム幅"); ImGui::NextColumn();
		ImGui::DragFloat3("##初期拡大のランダム幅", &scale_.startRandomRange.x, 0.1f, 0.0f);
		ImGui::NextColumn();

		ImGui::Separator();

		switch (scaleState_) {
		case START:
			break;
		case VELOCITY:

			ImGui::Text("拡大速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##拡大速度", &scale_.velocity.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("拡大速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##拡大速度のランダム幅", &scale_.velocityRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("拡大加速度"); ImGui::NextColumn();
			ImGui::DragFloat3("##拡大加速度", &scale_.acceleration.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("拡大加速度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##拡大加速度のランダム幅", &scale_.accelerationRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			break;
		case EASING:

			const char* easingItems[] = { "Lerp","EaseIn","EaseOut" };

			int currentItem = static_cast<int>(scaleEasingState_);

			ImGui::Text("終了拡大"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了拡大", &scale_.endNum.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("終了拡大のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了拡大のランダム幅", &scale_.endRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("拡大のイージング種類"); ImGui::NextColumn();
			if (ImGui::Combo("##拡大のイージング種類", &currentItem, easingItems, IM_ARRAYSIZE(easingItems))) {
				scaleEasingState_ = static_cast<EasingState>(currentItem);
			}
			ImGui::NextColumn();

			break;
		}

		ImGui::Columns(1); // 列終了
	}

	if (ImGui::CollapsingHeader("モデル設定")) {
		ImGui::Columns(2, "RorationColumns", false); // 2列レイアウト

		std::vector<const char*> items = Manager_->GetModelFiles();

		items.insert(items.begin(), "モデルを選択");

		int currentItem = 0;

		ImGui::Text("モデルデータ"); ImGui::NextColumn();
		if (ImGui::Combo("##モデルデータ", &currentItem, items.data(), static_cast<int>(items.size()))) {
			Manager_->ReloadGroup(name_, name_, items[currentItem]);
			modelName_ = items[currentItem];
		}
		ImGui::NextColumn();

		ImGui::Text("初期色"); ImGui::NextColumn();
		ImGui::ColorEdit4("##初期色", &startColor_.x);
		ImGui::NextColumn();

		ImGui::Text("終了色"); ImGui::NextColumn();
		ImGui::ColorEdit4("##終了色", &endColor_.x);
		ImGui::NextColumn();

		ImGui::Text("色のランダム幅"); ImGui::NextColumn();
		ImGui::ColorEdit4("##色のランダム幅", &colorRandomRange_.x);
		ImGui::NextColumn();

		ImGui::Text("ビルボード"); ImGui::NextColumn();
		ImGui::Checkbox("##ビルボード", &isBillboard_);
		ImGui::NextColumn();

		ImGui::Columns(1); // 列終了
	}

	if (ImGui::Button("Start")) {
		Start();
	}

	if (ImGui::Button("Save")) {
		SaveEmitterData();
		std::string message = std::format("{}.json saved.", name_);
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
	}

#endif
}

void ParticleEmitter::SaveEmitterData() {

	nlohmann::json jsonData;

	std::string kDirectoryPath = "resources/jsons/particles/";

	std::string filePath = kDirectoryPath + name_ + ".json";

	jsonData["name"] = name_;

	jsonData["count"] = maxEmitNum_;
	jsonData["frequency"] = frequency_;
	jsonData["lifeTime"] = lifeTime_;
	jsonData["lifeTimeRandomRange"] = lifeTimeRandomRange_;
	jsonData["startTime"] = startTime_;
	jsonData["endTime"] = endTime_;
	jsonData["isLoop"] = isLoop_;

	jsonData["position"] = {
		{"startNum",{position_.startNum.x,position_.startNum.y,position_.startNum.z}},
		{"startRandomRange",{position_.startRandomRange.x,position_.startRandomRange.y,position_.startRandomRange.z}},
		{"endNum",{position_.endNum.x,position_.endNum.y,position_.endNum.z}},
		{"endRandomRange",{position_.endRandomRange.x,position_.endRandomRange.y,position_.endRandomRange.z}},
		{"velocity",{position_.velocity.x,position_.velocity.y,position_.velocity.z}},
		{"velocityRandomRange",{position_.velocityRandomRange.x,position_.velocityRandomRange.y,position_.velocityRandomRange.z}},
		{"acceleration",{position_.acceleration.x,position_.acceleration.y,position_.acceleration.z}},
		{"accelerationRandomRange",{position_.accelerationRandomRange.x,position_.accelerationRandomRange.y,position_.accelerationRandomRange.z}}
	};
	jsonData["positionState"] = positionState_;
	jsonData["positionEasingState"] = positionEasingState_;

	jsonData["rotation"] = {
		{"startNum",{rotation_.startNum.x,rotation_.startNum.y,rotation_.startNum.z}},
		{"startRandomRange",{rotation_.startRandomRange.x,rotation_.startRandomRange.y,rotation_.startRandomRange.z}},
		{"endNum",{rotation_.endNum.x,rotation_.endNum.y,rotation_.endNum.z}},
		{"endRandomRange",{rotation_.endRandomRange.x,rotation_.endRandomRange.y,rotation_.endRandomRange.z}},
		{"velocity",{rotation_.velocity.x,rotation_.velocity.y,rotation_.velocity.z}},
		{"velocityRandomRange",{rotation_.velocityRandomRange.x,rotation_.velocityRandomRange.y,rotation_.velocityRandomRange.z}},
		{"acceleration",{rotation_.acceleration.x,rotation_.acceleration.y,rotation_.acceleration.z}},
		{"accelerationRandomRange",{rotation_.accelerationRandomRange.x,rotation_.accelerationRandomRange.y,rotation_.accelerationRandomRange.z}}
	};
	jsonData["rotationState"] = rotationState_;
	jsonData["rotationEasingState"] = rotationEasingState_;

	jsonData["scale"] = {
		{"startNum",{scale_.startNum.x,scale_.startNum.y,scale_.startNum.z}},
		{"startRandomRange",{scale_.startRandomRange.x,scale_.startRandomRange.y,scale_.startRandomRange.z}},
		{"endNum",{scale_.endNum.x,scale_.endNum.y,scale_.endNum.z}},
		{"endRandomRange",{scale_.endRandomRange.x,scale_.endRandomRange.y,scale_.endRandomRange.z}},
		{"velocity",{scale_.velocity.x,scale_.velocity.y,scale_.velocity.z}},
		{"velocityRandomRange",{scale_.velocityRandomRange.x,scale_.velocityRandomRange.y,scale_.velocityRandomRange.z}},
		{"acceleration",{scale_.acceleration.x,scale_.acceleration.y,scale_.acceleration.z}},
		{"accelerationRandomRange",{scale_.accelerationRandomRange.x,scale_.accelerationRandomRange.y,scale_.accelerationRandomRange.z}}
	};
	jsonData["scaleState"] = scaleState_;
	jsonData["scaleEasingState"] = scaleEasingState_;

	jsonData["modelName"] = modelName_;
	jsonData["startColor"] = { startColor_.x,startColor_.y,startColor_.z,startColor_.w };
	jsonData["endColor"] = { endColor_.x,endColor_.y,endColor_.z,endColor_.w };
	jsonData["colorRandomRange"] = { colorRandomRange_.x,colorRandomRange_.y,colorRandomRange_.z,colorRandomRange_.w };
	jsonData["isBillboard"] = isBillboard_;

	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::ofstream file;

	file.open(filePath);

	if (file.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEmitter", 0);
		assert(0);
		return;
	}

	file << jsonData.dump(4);

	file.close();
}

void ParticleEmitter::LoadEmitterData(const std::string& fileName) {

	nlohmann::json jsonData;

	std::string kDirectoryPath = "resources/jsons/particles/";

	std::string filePath = kDirectoryPath + fileName;

	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "ParticleEmitter", 0);
		assert(0);
		return;
	}

	file >> jsonData;

	file.close();

	std::string newName = jsonData["name"];

	Manager_->ReloadGroup(newName, name_, jsonData["modelName"]);

	name_ = newName;

	if (jsonData.contains("count")) maxEmitNum_ = jsonData["count"];
	if (jsonData.contains("frequency")) frequency_ = jsonData["frequency"];
	if (jsonData.contains("lifeTime")) lifeTime_ = jsonData["lifeTime"];
	if (jsonData.contains("lifeTimeRandomRange")) lifeTimeRandomRange_ = jsonData["lifeTimeRandomRange"];
	if (jsonData.contains("startTime")) startTime_ = jsonData["startTime"];
	if (jsonData.contains("endTime")) endTime_ = jsonData["endTime"];
	if (jsonData.contains("isLoop")) isLoop_ = jsonData["isLoop"];

	if (jsonData.contains("position")) {
		auto pos = jsonData["position"];
		position_.startNum = { pos["startNum"][0],pos["startNum"][1],pos["startNum"][2] };
		position_.startRandomRange = { pos["startRandomRange"][0],pos["startRandomRange"][1],pos["startRandomRange"][2] };
		position_.endNum = { pos["endNum"][0],pos["endNum"][1],pos["endNum"][2] };
		position_.endRandomRange = { pos["endRandomRange"][0],pos["endRandomRange"][1],pos["endRandomRange"][2] };
		position_.velocity = { pos["velocity"][0],pos["velocity"][1],pos["velocity"][2] };
		position_.velocityRandomRange = { pos["velocityRandomRange"][0],pos["velocityRandomRange"][1],pos["velocityRandomRange"][2] };
		position_.acceleration = { pos["acceleration"][0],pos["acceleration"][1],pos["acceleration"][2] };
		position_.accelerationRandomRange = { pos["accelerationRandomRange"][0],pos["accelerationRandomRange"][1],pos["accelerationRandomRange"][2] };
	}
	if (jsonData.contains("positionState")) positionState_ = jsonData["positionState"];
	if (jsonData.contains("positionEasingState")) positionEasingState_ = jsonData["positionEasingState"];

	if (jsonData.contains("rotation")) {
		auto pos = jsonData["rotation"];
		rotation_.startNum = { pos["startNum"][0],pos["startNum"][1],pos["startNum"][2] };
		rotation_.startRandomRange = { pos["startRandomRange"][0],pos["startRandomRange"][1],pos["startRandomRange"][2] };
		rotation_.endNum = { pos["endNum"][0],pos["endNum"][1],pos["endNum"][2] };
		rotation_.endRandomRange = { pos["endRandomRange"][0],pos["endRandomRange"][1],pos["endRandomRange"][2] };
		rotation_.velocity = { pos["velocity"][0],pos["velocity"][1],pos["velocity"][2] };
		rotation_.velocityRandomRange = { pos["velocityRandomRange"][0],pos["velocityRandomRange"][1],pos["velocityRandomRange"][2] };
		rotation_.acceleration = { pos["acceleration"][0],pos["acceleration"][1],pos["acceleration"][2] };
		rotation_.accelerationRandomRange = { pos["accelerationRandomRange"][0],pos["accelerationRandomRange"][1],pos["accelerationRandomRange"][2] };
	}
	if (jsonData.contains("rotationState")) rotationState_ = jsonData["rotationState"];
	if (jsonData.contains("rotationEasingState")) rotationEasingState_ = jsonData["rotationEasingState"];

	if (jsonData.contains("scale")) {
		auto pos = jsonData["scale"];
		scale_.startNum = { pos["startNum"][0],pos["startNum"][1],pos["startNum"][2] };
		scale_.startRandomRange = { pos["startRandomRange"][0],pos["startRandomRange"][1],pos["startRandomRange"][2] };
		scale_.endNum = { pos["endNum"][0],pos["endNum"][1],pos["endNum"][2] };
		scale_.endRandomRange = { pos["endRandomRange"][0],pos["endRandomRange"][1],pos["endRandomRange"][2] };
		scale_.velocity = { pos["velocity"][0],pos["velocity"][1],pos["velocity"][2] };
		scale_.velocityRandomRange = { pos["velocityRandomRange"][0],pos["velocityRandomRange"][1],pos["velocityRandomRange"][2] };
		scale_.acceleration = { pos["acceleration"][0],pos["acceleration"][1],pos["acceleration"][2] };
		scale_.accelerationRandomRange = { pos["accelerationRandomRange"][0],pos["accelerationRandomRange"][1],pos["accelerationRandomRange"][2] };
	}
	if (jsonData.contains("scaleState")) scaleState_ = jsonData["scaleState"];
	if (jsonData.contains("scaleEasingState")) scaleEasingState_ = jsonData["scaleEasingState"];

	if (jsonData.contains("modelName")) modelName_ = jsonData["modelName"];
	if (jsonData.contains("startColor")) startColor_ = { jsonData["startColor"][0],jsonData["startColor"][1],jsonData["startColor"][2],jsonData["startColor"][3] };
	if (jsonData.contains("endColor")) endColor_ = { jsonData["endColor"][0],jsonData["endColor"][1],jsonData["endColor"][2],jsonData["endColor"][3] };
	if (jsonData.contains("colorRandomRange")) colorRandomRange_ = { jsonData["colorRandomRange"][0],jsonData["colorRandomRange"][1],jsonData["colorRandomRange"][2],jsonData["colorRandomRange"][3] };
	if (jsonData.contains("isBillboard")) isBillboard_ = jsonData["isBillboard"];
}

void ParticleEmitter::Start() {

	isActive_ = true;

	emitTimer = 0.0f;

	totalTimer_ = 0.0f;

	count_ = 0;
}