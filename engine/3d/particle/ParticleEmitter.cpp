#include "ParticleEmitter.h"
#include"line/DrawLine3D.h"

// コンストラクタ
ParticleEmitter::ParticleEmitter() {}

void ParticleEmitter::Initialize(const std::string& name, const std::string& fileName) {

	Manager_ = std::make_unique<ParticleManager>();
	Manager_->Initialize(SrvManager::GetInstance());
	Manager_->CreateParticleGroup(name, fileName);

	startTime_ = 0.0f;
	endTime_ = 100.0f;
	isLoop_ = true;
	isStart_ = false;

	name_ = name;
	count_ = 100;
	frequency_ = 0.01f;
	lifeTime_ = 1.0f;
	lifeTimeRandomRange_ = 0.0f;
	isActive_ = true;
	isDrawEmitter_ = true;
	isBillboard_ = true;
	transform_.Initialize();

	positionState_ = START;
	positionEasingState_ = LERP;
	position_.startNum = { 0.0f, 0.0f, 0.0f };
	position_.startRandomRange = { 0.0f,5.0f,0.0f };
	position_.endNum = { 0.0f, 0.0f, 0.0f };
	position_.endRandomRange = { 0.0f, 0.0f, 0.0f };
	position_.velocity = { 0.0f, 0.0f, 0.0f };
	position_.velocityRandomRange = { 0.0f, 0.0f, 0.0f };
	position_.acceleration = { 0.0f, 0.0f, 0.0f };
	position_.accelerationRandomRange = { 0.0f, 0.0f, 0.0f };

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

	startColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	endColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	randomColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	AddItem();
	ApplyGlobalVariables();
}

// Update関数
void ParticleEmitter::Update(const ViewProjection& vp_) {

	if (isStart_) {

		// 時間を進める
		currentTimer_ += 1.0f / 60.0f;
	}

	// アクティブなら
	if (isActive_) {

		if (currentTimer_ >= frequency_) {

			// パーティクルを発生させる
			Emit();

			currentTimer_ -= frequency_;
		}
	}

	Manager_->Update(vp_);

	transform_.UpdateMatrix();
}

void ParticleEmitter::UpdateOnce(const ViewProjection& vp_) {

	if (isActive_) {

		Emit();
	}

	Manager_->Update(vp_);
	transform_.UpdateMatrix();
}

void ParticleEmitter::Draw() {

	Manager_->Draw();

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
	// ParticleManagerのEmit関数を呼び出す
	Manager_->Emit(
		name_,
		transform_,
		count_,
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
		randomColor_,
		isBillboard_
	);
}

void ParticleEmitter::AddItem() {
	/*groupName = name_.c_str();
	globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "emitFrequency", emitFrequency_);
	globalVariables->AddItem(groupName, "count", count_);
	globalVariables->AddItem(groupName, "Emit translation", transform_.translation_);
	globalVariables->AddItem(groupName, "Emit scale", transform_.scale_);
	globalVariables->AddItem(groupName, "Emit rotation", transform_.rotation_);
	globalVariables->AddItem(groupName, "Particle StartScale", startScale_);
	globalVariables->AddItem(groupName, "Particle EndScale", endScale_);
	globalVariables->AddItem(groupName, "Particle StartRote", startRote_);
	globalVariables->AddItem(groupName, "Particle EndRote", endRote_);
	globalVariables->AddItem(groupName, "Particle StartAcce", startAcce_);
	globalVariables->AddItem(groupName, "Particle EndAcce", endAcce_);
	globalVariables->AddItem(groupName, "minVelocity", velocityMin_);
	globalVariables->AddItem(groupName, "maxVelocity", velocityMax_);
	globalVariables->AddItem(groupName, "lifeTimeMax", lifeTimeMax_);
	globalVariables->AddItem(groupName, "lifeTimeMin", lifeTimeMin_);
	globalVariables->AddItem(groupName, "isRamdomColor", isRandomColor);
	globalVariables->AddItem(groupName, "alphaMin", alphaMin_);
	globalVariables->AddItem(groupName, "alphaMax", alphaMax_);
	globalVariables->AddItem(groupName, "AllScale Max", allScaleMax);
	globalVariables->AddItem(groupName, "AllScale Min", allScaleMin);
	globalVariables->AddItem(groupName, "Scale Min", scaleMin);
	globalVariables->AddItem(groupName, "Scale Max", scaleMax);
	globalVariables->AddItem(groupName, "isVisible", isVisible);
	globalVariables->AddItem(groupName, "isBillBoard", isBillBoard);
	globalVariables->AddItem(groupName, "isRandomRotate", isRandomRotate);
	globalVariables->AddItem(groupName, "isAcceMultiply", isAcceMultiply);
	globalVariables->AddItem(groupName, "RotationVelo Min", rotateVelocityMin);
	globalVariables->AddItem(groupName, "RotationVelo Max", rotateVelocityMax);
	globalVariables->AddItem(groupName, "isRandomScale", isRandomScale);
	globalVariables->AddItem(groupName, "isAllRamdomScale", isAllRamdomScale);
	globalVariables->AddItem(groupName, "isSinMove", isSinMove);*/
}

void ParticleEmitter::SetEmitValue() {

	//globalVariables->SetValue(groupName, "emitFrequency", emitFrequency_);
	//globalVariables->SetValue(groupName, "count", count_);
	//globalVariables->SetValue(groupName, "Emit translation", transform_.translation_);
	//globalVariables->SetValue(groupName, "Emit scale", transform_.scale_);
	//globalVariables->SetValue(groupName, "Emit rotation", transform_.rotation_);
	//globalVariables->SetValue(groupName, "Particle StartScale", startScale_);
	//globalVariables->SetValue(groupName, "Particle StartRote", startRote_);
	//globalVariables->SetValue(groupName, "Particle EndRote", endRote_);
	//globalVariables->SetValue(groupName, "Particle EndScale", endScale_);
	//globalVariables->SetValue(groupName, "Particle StartAcce", startAcce_);
	//globalVariables->SetValue(groupName, "Particle EndAcce", endAcce_);
	//globalVariables->SetValue(groupName, "minVelocity", velocityMin_);
	//globalVariables->SetValue(groupName, "maxVelocity", velocityMax_);
	//globalVariables->SetValue(groupName, "lifeTimeMax", lifeTimeMax_);
	//globalVariables->SetValue(groupName, "lifeTimeMin", lifeTimeMin_);
	//globalVariables->SetValue(groupName, "isVisible", isVisible);
	//globalVariables->SetValue(groupName, "isBillBoard", isBillBoard);
	//globalVariables->SetValue(groupName, "isRamdomColor", isRandomColor);
	//globalVariables->SetValue(groupName, "alphaMin", alphaMin_);
	//globalVariables->SetValue(groupName, "alphaMax", alphaMax_);
	//globalVariables->SetValue(groupName, "isRandomRotate", isRandomRotate);
	//globalVariables->SetValue(groupName, "RotationVelo Min", rotateVelocityMin);
	//globalVariables->SetValue(groupName, "isAcceMultiply", isAcceMultiply);
	//globalVariables->SetValue(groupName, "RotationVelo Max", rotateVelocityMax);
	//globalVariables->SetValue(groupName, "AllScale Max", allScaleMax);
	//globalVariables->SetValue(groupName, "AllScale Min", allScaleMin);
	//globalVariables->SetValue(groupName, "Scale Min", scaleMin);
	//globalVariables->SetValue(groupName, "Scale Max", scaleMax);
	//globalVariables->SetValue(groupName, "isRandomScale", isRandomScale);
	//globalVariables->SetValue(groupName, "isAllRamdomScale", isAllRamdomScale);
	//globalVariables->SetValue(groupName, "isSinMove", isSinMove);
}

void ParticleEmitter::ApplyGlobalVariables() {
	/*emitFrequency_ = globalVariables->GetFloatValue(groupName, "emitFrequency");
	count_ = globalVariables->GetIntValue(groupName, "count");
	transform_.translation_ = globalVariables->GetVector3Value(groupName, "Emit translation");
	transform_.scale_ = globalVariables->GetVector3Value(groupName, "Emit scale");
	transform_.rotation_ = globalVariables->GetVector3Value(groupName, "Emit rotation");
	startScale_ = globalVariables->GetVector3Value(groupName, "Particle StartScale");
	endScale_ = globalVariables->GetVector3Value(groupName, "Particle EndScale");
	startRote_ = globalVariables->GetVector3Value(groupName, "Particle StartRote");
	endRote_ = globalVariables->GetVector3Value(groupName, "Particle EndRote");
	startAcce_ = globalVariables->GetVector3Value(groupName, "Particle StartAcce");
	endAcce_ = globalVariables->GetVector3Value(groupName, "Particle EndAcce");
	velocityMin_ = globalVariables->GetVector3Value(groupName, "minVelocity");
	velocityMax_ = globalVariables->GetVector3Value(groupName, "maxVelocity");
	lifeTimeMax_ = globalVariables->GetFloatValue(groupName, "lifeTimeMax");
	lifeTimeMin_ = globalVariables->GetFloatValue(groupName, "lifeTimeMin");
	isVisible = globalVariables->GetBoolValue(groupName, "isVisible");
	isBillBoard = globalVariables->GetBoolValue(groupName, "isBillBoard");
	isRandomColor = globalVariables->GetBoolValue(groupName, "isRamdomColor");
	alphaMin_ = globalVariables->GetFloatValue(groupName, "alphaMin");
	alphaMax_ = globalVariables->GetFloatValue(groupName, "alphaMax");
	isRandomRotate = globalVariables->GetBoolValue(groupName, "isRandomRotate");
	isAcceMultiply = globalVariables->GetBoolValue(groupName, "isAcceMultiply");
	rotateVelocityMin = globalVariables->GetVector3Value(groupName, "RotationVelo Min");
	rotateVelocityMax = globalVariables->GetVector3Value(groupName, "RotationVelo Max");
	allScaleMax = globalVariables->GetVector3Value(groupName, "AllScale Max");
	allScaleMin = globalVariables->GetVector3Value(groupName, "AllScale Min");
	scaleMin = globalVariables->GetFloatValue(groupName, "Scale Min");
	scaleMax = globalVariables->GetFloatValue(groupName, "Scale Max");
	isRandomScale = globalVariables->GetBoolValue(groupName, "isRandomScale");
	isAllRamdomScale = globalVariables->GetBoolValue(groupName, "isAllRamdomScale");
	isSinMove = globalVariables->GetBoolValue(groupName, "isSinMove");*/
}

// ImGuiで値を動かす関数
void ParticleEmitter::imgui() {
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());

	if (ImGui::CollapsingHeader("タイマー")) {
		ImGui::Columns(2, "TimerColumns", false); // 2列レイアウト

		ImGui::Text("タイマー"); ImGui::NextColumn();
		ImGui::DragFloat("##タイマー", &currentTimer_, 0.1f, 0.0f);
		ImGui::NextColumn();

		ImGui::Text("ループ"); ImGui::NextColumn();
		ImGui::Checkbox("##ループ", &isLoop_);
		ImGui::NextColumn();

		ImGui::Text("スタート"); ImGui::NextColumn();
		ImGui::Checkbox("##スタート", &isStart_);
		ImGui::NextColumn();

		ImGui::Columns(1); // 列終了
	}

	/// === エミッター設定 === ///

	if (ImGui::CollapsingHeader("エミッター設定")) {
		ImGui::Columns(2, "EmitterColumns", false); // 2列レイアウト

		ImGui::Text("アクティブ"); ImGui::NextColumn();
		ImGui::Checkbox("##アクティブ", &isActive_);
		ImGui::NextColumn();

		ImGui::Text("エミッター描画"); ImGui::NextColumn();
		ImGui::Checkbox("##エミッター描画", &isDrawEmitter_);
		ImGui::NextColumn();

		ImGui::Text("名前"); ImGui::NextColumn();
		ImGui::InputText("##名前", const_cast<char*>(name_.c_str()), 256);
		ImGui::NextColumn();

		ImGui::Text("エミッター座標"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター座標", &transform_.translation_.x,0.1f);
		ImGui::NextColumn();

		ImGui::Text("エミッター角度"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター角度", &transform_.rotation_.x,0.1f);
		ImGui::NextColumn();

		ImGui::Text("エミッター拡縮"); ImGui::NextColumn();
		ImGui::DragFloat3("##エミッター拡縮", &transform_.scale_.x,0.1f);
		ImGui::NextColumn();

		ImGui::Separator();

		ImGui::Text("生成数"); ImGui::NextColumn();
		ImGui::DragInt("##生成数", &count_, 1, 1, 100);
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

		ImGui::Text("初期色"); ImGui::NextColumn();
		ImGui::ColorEdit4("##初期色", &startColor_.x);
		ImGui::NextColumn();

		ImGui::Text("終了色"); ImGui::NextColumn();
		ImGui::ColorEdit4("##終了色", &endColor_.x);
		ImGui::NextColumn();

		ImGui::Text("色のランダム幅"); ImGui::NextColumn();
		ImGui::ColorEdit4("##色のランダム幅", &randomColor_.x);
		ImGui::NextColumn();

		ImGui::Text("ビルボード"); ImGui::NextColumn();
		ImGui::Checkbox("##ビルボード", &isBillboard_);
		ImGui::NextColumn();

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

			ImGui::Text("終了座標"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了座標", &position_.endNum.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("終了座標のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了座標のランダム幅", &position_.endRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("イージング種類"); ImGui::NextColumn();
			ImGui::Combo("##イージング種類", (int*)&positionEasingState_, easingItems, sizeof(easingItems));
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

			ImGui::Text("終了角度"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了角度", &rotation_.endNum.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Text("終了角度のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了角度のランダム幅", &rotation_.endRandomRange.x, 0.01f, 0.0f, 3.14f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("イージング種類"); ImGui::NextColumn();
			ImGui::Combo("##イージング種類", (int*)&rotationEasingState_, easingItems, sizeof(easingItems));
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

			ImGui::Text("終了拡大"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了拡大", &scale_.endNum.x, 0.1f);
			ImGui::NextColumn();

			ImGui::Text("終了拡大のランダム幅"); ImGui::NextColumn();
			ImGui::DragFloat3("##終了拡大のランダム幅", &scale_.endRandomRange.x, 0.1f, 0.0f);
			ImGui::NextColumn();

			ImGui::Separator();

			ImGui::Text("イージング種類"); ImGui::NextColumn();
			ImGui::Combo("##イージング種類", (int*)&scaleEasingState_, easingItems, sizeof(easingItems));
			ImGui::NextColumn();

			break;
		}

		ImGui::Columns(1); // 列終了
	}

	ImGui::End();

#endif
}
