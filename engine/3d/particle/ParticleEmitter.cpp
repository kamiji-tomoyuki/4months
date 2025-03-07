#include "ParticleEmitter.h"
#include"line/DrawLine3D.h"

// コンストラクタ
ParticleEmitter::ParticleEmitter() {}

void ParticleEmitter::Initialize(const std::string& name, const std::string& fileName) {
	//パーティクルマネージャーの生成
	Manager_ = std::make_unique<ParticleManager>();

	//パーティクルマネージャーの初期化
	Manager_->Initialize(SrvManager::GetInstance());

	//パーティクルマネージャーにパーティクルグループを生成
	Manager_->CreateParticleGroup(name_, fileName);

	//名前の初期化
	name_ = name;

	//ワールド座標の初期化
	transform_.Initialize();

	//パーティクルの最大数の初期化
	count_ = 3;

	//パーティクルの発生頻度の初期化
	emitFrequency_ = 0.1f;

	//経過時間の初期化
	elapsedTime_ = 0.0f;

	//パーティクルの生存時間の最小値の初期化
	lifeTimeMin_ = { 1.0f };

	//パーティクルの生存時間の最大値の初期化
	lifeTimeMax_ = { 3.0f };

	//パーティクルの透明度の最小値の初期化
	alphaMin_ = 1.0f;

	//パーティクルの透明度の最大値の初期化
	alphaMax_ = 1.0f;

	//パーティクルの速度の最小値の初期化
	velocityMin_ = { -1.0f, -1.0f, -1.0f };

	//パーティクルの速度の最大値の初期化
	velocityMax_ = { 1.0f, 1.0f, 1.0f };

	//パーティクルの初期加速度の初期化
	startAcce_ = { 1.0f,1.0f,1.0f };

	//パーティクルの終了加速度の初期化
	endAcce_ = { 1.0f,1.0f,1.0f };

	//パーティクルの回転速度の最小値の初期化
	rotateVelocityMin = { -0.07f,-0.07f,-0.07f };

	//パーティクルの回転速度の最大値の初期化
	rotateVelocityMax = { 0.07f,0.07f,0.07f };

	//パーティクルの初期スケールの初期化
	startScale_ = { 1.0f,1.0f,1.0f };

	//パーティクルの終了スケールの初期化
	endScale_ = { 1.0f,1.0f,1.0f };

	//描画フラグの初期化
	isEmitterVisible = true;

	//ビルボードフラグの初期化
	isBillBoard = false;

	//アクティブフラグの初期化
	isActive_ = false;

	//加速度の乗算フラグの初期化
	isAcceMultiply = false;

	//全体のスケールの最小値の初期化
	allScaleMin = { 1.0f,1.0f,1.0f };

	//全体のスケールの最大値の初期化
	allScaleMax = { 1.0f,1.0f,1.0f };

	//グローバル値の設定
	AddItem();

	//グローバル値の適用
	ApplyGlobalVariables();
}

// Update関数
void ParticleEmitter::Update(const ViewProjection& vp_) {

	SetEmitValue();

	// 経過時間を進める
	elapsedTime_ += deltaTime;

	// 発生頻度に基づいてパーティクルを発生させる
	while (elapsedTime_ >= emitFrequency_) {

		// パーティクルを発生させる
		Emit();

		// 過剰に進んだ時間を考慮
		elapsedTime_ -= emitFrequency_;
	}

	//パーティクルの更新
	Manager_->Update(vp_);

	//エミッターの座標の更新
	transform_.UpdateMatrix();
}

void ParticleEmitter::UpdateOnce(const ViewProjection& vp_) {
	SetEmitValue();
	if (isActive_) {
		Emit();  // パーティクルを発生させる
		isActive_ = false;
	}
	Manager_->Update(vp_);
	transform_.UpdateMatrix();
}

void ParticleEmitter::Draw() {

	Manager_->SetRandomRotate(isRandomRotate);
	Manager_->SetAcceMultipy(isAcceMultiply);
	Manager_->SetBillBorad(isBillBoard);
	Manager_->SetRandomSize(isRandomScale);
	Manager_->SetAllRandomSize(isAllRamdomScale);
	Manager_->SetSinMove(isSinMove);

	//パーティクルの描画
	Manager_->Draw();
}

void ParticleEmitter::DrawEmitter() {

	// isEmitterVisibleがtrueのときだけ描画
	if (!isEmitterVisible) return;

	// 立方体のローカル座標での基本頂点（スケーリング済み）
	std::array<Vector3, 8> localVertices = {
		Vector3{-1.0f, -1.0f, -1.0f}, // 左下手前
		Vector3{ 1.0f, -1.0f, -1.0f}, // 右下手前
		Vector3{-1.0f,  1.0f, -1.0f}, // 左上手前
		Vector3{ 1.0f,  1.0f, -1.0f}, // 右上手前
		Vector3{-1.0f, -1.0f,  1.0f}, // 左下奥
		Vector3{ 1.0f, -1.0f,  1.0f}, // 右下奥
		Vector3{-1.0f,  1.0f,  1.0f}, // 左上奥
		Vector3{ 1.0f,  1.0f,  1.0f}  // 右上奥
	};

	// ワールド変換結果を格納する配列
	std::array<Vector3, 8> worldVertices;

	// ワールド行列の計算
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);

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
		name_,						// パーティクルの名前を引数として渡す
		transform_.translation_,	// 位置を引数として渡す
		count_,						// パーティクルの最大数を引数として渡す
		transform_.scale_,			// スケールを引数として渡す
		velocityMin_,				// 最小速度を引数として渡す
		velocityMax_,				// 最大速度を引数として渡す
		lifeTimeMin_,				// 最小ライフタイムを引数として渡す
		lifeTimeMax_,				// 最大ライフタイムを引数として渡す
		startScale_,				// 初期スケールを引数として渡す
		endScale_,					// 終了スケールを引数として渡す
		startAcce_,					// 初期加速度を引数として渡す
		endAcce_,					// 終了加速度を引数として渡す
		startRote_,					// 初期回転を引数として渡す
		endRote_,					// 終了回転を引数として渡す
		isRandomColor,				// ランダムカラーを引数として渡す
		alphaMin_,					// 最小アルファ値を引数として渡す
		alphaMax_,					// 最大アルファ値を引数として渡す
		rotateVelocityMin,			// 最小回転速度を引数として渡す
		rotateVelocityMax,			// 最大回転速度を引数として渡す
		allScaleMax,				// 全体のスケールの最大値を引数として渡す
		allScaleMin,				// 全体のスケールの最小値を引数として渡す
		scaleMin,					// スケールの最小値を引数として渡す
		scaleMax,					// スケールの最大値を引数として渡す
		transform_.rotation_		// 回転を引数として渡す
	);
}

void ParticleEmitter::ApplyGlobalVariables() {
	emitFrequency_ = globalVariables->GetFloatValue(groupName, "emitFrequency");
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
	isEmitterVisible = globalVariables->GetBoolValue(groupName, "isEmitterVisible");
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
	isSinMove = globalVariables->GetBoolValue(groupName, "isSinMove");
}

void ParticleEmitter::SetEmitValue() {

	globalVariables->SetValue(groupName, "emitFrequency", emitFrequency_);
	globalVariables->SetValue(groupName, "count", count_);
	globalVariables->SetValue(groupName, "Emit translation", transform_.translation_);
	globalVariables->SetValue(groupName, "Emit scale", transform_.scale_);
	globalVariables->SetValue(groupName, "Emit rotation", transform_.rotation_);
	globalVariables->SetValue(groupName, "Particle StartScale", startScale_);
	globalVariables->SetValue(groupName, "Particle StartRote", startRote_);
	globalVariables->SetValue(groupName, "Particle EndRote", endRote_);
	globalVariables->SetValue(groupName, "Particle EndScale", endScale_);
	globalVariables->SetValue(groupName, "Particle StartAcce", startAcce_);
	globalVariables->SetValue(groupName, "Particle EndAcce", endAcce_);
	globalVariables->SetValue(groupName, "minVelocity", velocityMin_);
	globalVariables->SetValue(groupName, "maxVelocity", velocityMax_);
	globalVariables->SetValue(groupName, "lifeTimeMax", lifeTimeMax_);
	globalVariables->SetValue(groupName, "lifeTimeMin", lifeTimeMin_);
	globalVariables->SetValue(groupName, "isEmitterVisible", isEmitterVisible);
	globalVariables->SetValue(groupName, "isBillBoard", isBillBoard);
	globalVariables->SetValue(groupName, "isRamdomColor", isRandomColor);
	globalVariables->SetValue(groupName, "alphaMin", alphaMin_);
	globalVariables->SetValue(groupName, "alphaMax", alphaMax_);
	globalVariables->SetValue(groupName, "isRandomRotate", isRandomRotate);
	globalVariables->SetValue(groupName, "RotationVelo Min", rotateVelocityMin);
	globalVariables->SetValue(groupName, "isAcceMultiply", isAcceMultiply);
	globalVariables->SetValue(groupName, "RotationVelo Max", rotateVelocityMax);
	globalVariables->SetValue(groupName, "AllScale Max", allScaleMax);
	globalVariables->SetValue(groupName, "AllScale Min", allScaleMin);
	globalVariables->SetValue(groupName, "Scale Min", scaleMin);
	globalVariables->SetValue(groupName, "Scale Max", scaleMax);
	globalVariables->SetValue(groupName, "isRandomScale", isRandomScale);
	globalVariables->SetValue(groupName, "isAllRamdomScale", isAllRamdomScale);
	globalVariables->SetValue(groupName, "isSinMove", isSinMove);
}

void ParticleEmitter::AddItem() {
	groupName = name_.c_str();
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
	globalVariables->AddItem(groupName, "isEmitterVisible", isEmitterVisible);
	globalVariables->AddItem(groupName, "isBillBoard", isBillBoard);
	globalVariables->AddItem(groupName, "isRandomRotate", isRandomRotate);
	globalVariables->AddItem(groupName, "isAcceMultiply", isAcceMultiply);
	globalVariables->AddItem(groupName, "RotationVelo Min", rotateVelocityMin);
	globalVariables->AddItem(groupName, "RotationVelo Max", rotateVelocityMax);
	globalVariables->AddItem(groupName, "isRandomScale", isRandomScale);
	globalVariables->AddItem(groupName, "isAllRamdomScale", isAllRamdomScale);
	globalVariables->AddItem(groupName, "isSinMove", isSinMove);
}

// ImGuiで値を動かす関数
void ParticleEmitter::imgui() {
#ifdef _DEBUG
	ImGui::Begin(name_.c_str());

	// 基本データセクション
	if (ImGui::CollapsingHeader("エミッターデータ")) {
		// トランスフォームデータをフレーム内に配置
		ImGui::Text("Transformデータ:");
		ImGui::Separator();
		ImGui::Columns(2, "TransformColumns", false); // 2列レイアウト
		ImGui::Text("位置"); ImGui::NextColumn();
		ImGui::DragFloat3("##位置", &transform_.translation_.x, 0.1f);
		ImGui::NextColumn();
		ImGui::Text("回転"); ImGui::NextColumn();
		float rotationDegrees[3] = {
		radiansToDegrees(transform_.rotation_.x),
		radiansToDegrees(transform_.rotation_.y),
		radiansToDegrees(transform_.rotation_.z)
		};

		// ドラッグUIを使用し、度数法で値を操作
		if (ImGui::DragFloat3("##回転 (度)", rotationDegrees, 0.1f, -360.0f, 360.0f)) {
			// 操作後、度数法からラジアンに戻して保存
			transform_.rotation_.x = degreesToRadians(rotationDegrees[0]);
			transform_.rotation_.y = degreesToRadians(rotationDegrees[1]);
			transform_.rotation_.z = degreesToRadians(rotationDegrees[2]);
		}

		ImGui::NextColumn();
		ImGui::Text("大きさ"); ImGui::NextColumn();
		ImGui::DragFloat3("##大きさ", &transform_.scale_.x, 0.1f, 0.0f);
		ImGui::Columns(1); // 列終了
		ImGui::Separator();

		// 可視性フラグ
		ImGui::Checkbox("表示", &isEmitterVisible);
	}

	// パーティクルデータセクション
	if (ImGui::CollapsingHeader("パーティクルデータ")) {
		// LifeTimeを折りたたみ可能にする
		if (ImGui::TreeNode("寿命")) {
			ImGui::Text("寿命設定:");
			ImGui::Separator();
			ImGui::DragFloat("最大値", &lifeTimeMax_, 0.1f, 0.0f);
			ImGui::DragFloat("最小値", &lifeTimeMin_, 0.1f, 0.0f);
			lifeTimeMin_ = std::clamp(lifeTimeMin_, 0.0f, lifeTimeMax_);
			lifeTimeMax_ = std::clamp(lifeTimeMax_, lifeTimeMin_, 10.0f);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 速度と加速度
		if (ImGui::TreeNode("速度、加速度")) {
			ImGui::Text("速度:");
			ImGui::DragFloat3("最大値", &velocityMax_.x, 0.1f);
			ImGui::DragFloat3("最小値", &velocityMin_.x, 0.1f);
			velocityMin_.x = std::clamp(velocityMin_.x, -FLT_MAX, velocityMax_.x);
			velocityMax_.x = std::clamp(velocityMax_.x, velocityMin_.x, FLT_MAX);
			velocityMin_.y = std::clamp(velocityMin_.y, -FLT_MAX, velocityMax_.y);
			velocityMax_.y = std::clamp(velocityMax_.y, velocityMin_.y, FLT_MAX);
			velocityMin_.z = std::clamp(velocityMin_.z, -FLT_MAX, velocityMax_.z);
			velocityMax_.z = std::clamp(velocityMax_.z, velocityMin_.z, FLT_MAX);
			ImGui::Text("加速度:");
			ImGui::DragFloat3("最初", &startAcce_.x, 0.001f);
			ImGui::DragFloat3("最後", &endAcce_.x, 0.001f);
			ImGui::Checkbox("乗算", &isAcceMultiply);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// サイズ
		if (ImGui::TreeNode("大きさ")) {
			ImGui::Text("大きさ:");
			if (isAllRamdomScale) {
				ImGui::DragFloat3("最大値", &allScaleMax.x, 0.1f, 0.0f);
				ImGui::DragFloat3("最小値", &allScaleMin.x, 0.1f, 0.0f);
				allScaleMin.x = std::clamp(allScaleMin.x, -FLT_MAX, allScaleMax.x);
				allScaleMax.x = std::clamp(allScaleMax.x, allScaleMin.x, FLT_MAX);
				allScaleMin.y = std::clamp(allScaleMin.y, -FLT_MAX, allScaleMax.y);
				allScaleMax.y = std::clamp(allScaleMax.y, allScaleMin.y, FLT_MAX);
				allScaleMin.z = std::clamp(allScaleMin.z, -FLT_MAX, allScaleMax.z);
				allScaleMax.z = std::clamp(allScaleMax.z, allScaleMin.z, FLT_MAX);
			} else if (isRandomScale) {
				ImGui::DragFloat("最大値", &scaleMax, 0.1f, 0.0f);
				ImGui::DragFloat("最小値", &scaleMin, 0.1f, 0.0f);
				scaleMax = std::clamp(scaleMax, scaleMin, FLT_MAX);
				scaleMin = std::clamp(scaleMin, 0.0f, scaleMax);
			} else if (isSinMove) {
				ImGui::DragFloat3("最初", &startScale_.x, 0.1f, 0.0f);
			} else {
				ImGui::DragFloat3("最初", &startScale_.x, 0.1f, 0.0f);
			}
			if (!isSinMove) {
				ImGui::DragFloat3("最後", &endScale_.x, 0.1f);
			}
			ImGui::Checkbox("均等にランダムな大きさ", &isRandomScale);
			ImGui::Checkbox("ばらばらにランダムな大きさ", &isAllRamdomScale);
			ImGui::Checkbox("sin波の動き", &isSinMove);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// 回転
		if (ImGui::TreeNode("回転")) {
			if (!isRandomRotate) {
				float startRotationDegrees[3] = {
				  radiansToDegrees(startRote_.x),
				  radiansToDegrees(startRote_.y),
				  radiansToDegrees(startRote_.z)
				};
				float endRotationDegrees[3] = {
				  radiansToDegrees(endRote_.x),
				  radiansToDegrees(endRote_.y),
				  radiansToDegrees(endRote_.z)
				};
				if (ImGui::DragFloat3("最初", startRotationDegrees, 0.1f)) {
					startRote_.x = degreesToRadians(startRotationDegrees[0]);
					startRote_.y = degreesToRadians(startRotationDegrees[1]);
					startRote_.z = degreesToRadians(startRotationDegrees[2]);
				}
				if (ImGui::DragFloat3("最後", endRotationDegrees, 0.1f)) {
					endRote_.x = degreesToRadians(endRotationDegrees[0]);
					endRote_.y = degreesToRadians(endRotationDegrees[1]);
					endRote_.z = degreesToRadians(endRotationDegrees[2]);
				}
			}
			if (isRandomRotate) {
				ImGui::DragFloat3("最大値", &rotateVelocityMax.x, 0.01f);
				ImGui::DragFloat3("最小値", &rotateVelocityMin.x, 0.01f);
				rotateVelocityMin.x = std::clamp(rotateVelocityMin.x, -FLT_MAX, rotateVelocityMax.x);
				rotateVelocityMax.x = std::clamp(rotateVelocityMax.x, rotateVelocityMin.x, FLT_MAX);
				rotateVelocityMin.y = std::clamp(rotateVelocityMin.y, -FLT_MAX, rotateVelocityMax.y);
				rotateVelocityMax.y = std::clamp(rotateVelocityMax.y, rotateVelocityMin.y, FLT_MAX);
				rotateVelocityMin.z = std::clamp(rotateVelocityMin.z, -FLT_MAX, rotateVelocityMax.z);
				rotateVelocityMax.z = std::clamp(rotateVelocityMax.z, rotateVelocityMin.z, FLT_MAX);
			}
			ImGui::Checkbox("ランダムな回転", &isRandomRotate);
			ImGui::TreePop();
		}

		ImGui::Separator();

		// Alphaを折りたたみ可能にする
		if (ImGui::TreeNode("透明度")) {
			ImGui::Text("透明度の設定:");
			ImGui::DragFloat("最大値", &alphaMax_, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("最小値", &alphaMin_, 0.1f, 0.0f, 1.0f);
			alphaMin_ = std::clamp(alphaMin_, 0.0f, alphaMax_);
			alphaMax_ = std::clamp(alphaMax_, alphaMin_, 1.0f);
			ImGui::TreePop();
		}
	}

	// エミット設定セクション
	if (ImGui::CollapsingHeader("パーティクルの数、間隔")) {
		ImGui::DragFloat("間隔", &emitFrequency_, 0.1f, 0.1f, 100.0f);
		ImGui::InputInt("数", &count_, 1, 100);
		count_ = std::clamp(count_, 0, 10000);
	}

	// その他の設定セクション
	if (ImGui::CollapsingHeader("各状態の設定")) {
		ImGui::Checkbox("ビルボード", &isBillBoard);
		ImGui::Checkbox("ランダムカラー", &isRandomColor);
	}

	ImGui::End();

#endif
}
