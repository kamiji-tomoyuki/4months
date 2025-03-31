#include "ParticleManager.h"
#include "TextureManager.h"
#include "fstream"
#include "filesystem"
#include "Vector3.h"

// モデルキャッシュ
std::unordered_map<std::string, ParticleManager::ModelData> ParticleManager::modelCache;

// シングルトンインスタンス
ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance() {

	// インスタンスがない場合
	if (instance == nullptr) {

		// インスタンスを生成
		instance = new ParticleManager();
	}

	// インスタンスを返す
	return instance;
}

void ParticleManager::Finalize() {

	// インスタンスを削除
	delete instance;

	// インスタンスをnullptrで初期化
	instance = nullptr;
}

void ParticleManager::Initialize(SrvManager* srvManager) {

	// パーティクル共通の取得
	particleCommon = ParticleCommon::GetInstance();

	// シェーダーリソースビューマネージャの取得
	srvManager_ = srvManager;

	// シード値の生成
	randomEngine.seed(seedGenerator());

	// モデルファイル名の取得
	for (const auto& entry : std::filesystem::directory_iterator(kDirectoryPath)) {

		// objファイルのみ取得
		if (entry.path().extension() == ".obj") {

			// ファイル名を追加
			modelFiles.push_back(entry.path().filename().string());
		}
	}
}

void ParticleManager::Update(const ViewProjection& viewProjection) {

	/// === 行列の生成 === ///

	// ビュープロジェクション行列の生成
	Matrix4x4 viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;

	// ビルボード行列の生成
	Matrix4x4 billboardMatrix = viewProjection.matView_;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	billboardMatrix.m[3][3] = 1.0f;

	billboardMatrix = Inverse(billboardMatrix);

	/// === パーティクルの更新 === ///

	// パーティクルグループごとに更新
	for (auto& [groupName, particleGroup] : particleGroups) {

		// パーティクルの合計生成数
		uint32_t numInstance = 0;

		// パーティクルごとに更新
		for (auto particleIterator = particleGroup.particles.begin();
			particleIterator != particleGroup.particles.end();) {

			/// === 削除処理 === ///

			// ライフタイムを超えたパーティクルの削除
			if (particleIterator->lifeTime <= particleIterator->currentTime) {

				// パーティクルの削除
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			// パーティクルの進行度
			float t = particleIterator->currentTime / particleIterator->lifeTime;

			//0~1の範囲内に収める
			t = std::clamp(t, 0.0f, 1.0f);

			/// === 移動処理 === ///

			// パーティクルの座標更新
			particleIterator->transform.translation_ = UpdateParameter(
				particleIterator->positionPara,
				particleIterator->positionState,
				particleIterator->positionEasingState,
				particleIterator->lifeTime,
				t
			);

			/// === 回転処理 === ///

			// パーティクルの回転更新
			particleIterator->transform.rotation_ = UpdateParameter(
				particleIterator->rotationPara,
				particleIterator->rotationState,
				particleIterator->rotationEasingState,
				particleIterator->lifeTime,
				t
			);

			/// === 拡縮処理 === ///

			// パーティクルの拡縮更新
			particleIterator->transform.scale_ = UpdateParameter(
				particleIterator->scalePara,
				particleIterator->scaleState,
				particleIterator->scaleEasingState,
				particleIterator->lifeTime,
				t
			);

			/// === 色 === ///

			// パーティクルの色更新
			particleIterator->color = Lerp(particleIterator->startColor, particleIterator->endColor, t);

			/// === ワールド座標の更新 === ///

			// ワールドトランスフォームの更新
			particleIterator->transform.UpdateMatrix();

			// パーティクルのワールド座標にエミッターのワールド座標を掛ける
			particleIterator->transform.matWorld_ *= particleIterator->emitterTransform.matWorld_;

			// ワールド座標の送信
			particleIterator->transform.TransferMatrix();

			// ワールド行列の計算
			Matrix4x4 worldMatrix{};

			// ビルボード処理が必要な場合
			if (particleIterator->isBillboard) {

				// パーティクルの座標取得
				Vector3 objPos = {
					particleIterator->transform.matWorld_.m[3][0],
					particleIterator->transform.matWorld_.m[3][1],
					particleIterator->transform.matWorld_.m[3][2]
				};

				// ビルボード行列の計算
				worldMatrix =
					MakeScaleMatrix(particleIterator->transform.scale_) *
					billboardMatrix *
					MakeTranslateMatrix(objPos);

			} else {

				// ビルボードなしのワールド行列の計算
				worldMatrix = particleIterator->transform.matWorld_;
			}

			// ワールド・ビュー・プロジェクション行列計算
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			/// === インスタンスデータ設定 === ///

			// インスタンス数が最大数未満の場合
			if (numInstance < kNumMaxInstance) {

				// WVPの設定
				particleGroup.instancingData[numInstance].WVP = worldViewProjectionMatrix;

				// ワールド座標の設定
				particleGroup.instancingData[numInstance].World = worldMatrix;

				// 色の設定
				particleGroup.instancingData[numInstance].color = (*particleIterator).color;

				// アルファ値の設定
				particleGroup.instancingData[numInstance].color.w = (*particleIterator).color.w;

				// 生成数の更新
				++numInstance;
			}

			// パーティクルの時間更新
			particleIterator->currentTime += kDeltaTime;

			// 次のパーティクルへ
			++particleIterator;
		}

		// インスタンス数更新
		particleGroup.instanceCount = numInstance;
	}
}

void ParticleManager::Draw() {

	// パーティクルグループごとに描画
	for (auto& [groupName, particleGroup] : particleGroups) {

		// インスタンス数が0より大きい場合
		if (particleGroup.instanceCount > 0) {

			particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &particleGroup.vertexBufferView);

			particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, particleGroup.materialResource->GetGPUVirtualAddress());

			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.instancingSRVIndex);
			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.modelData.material.textureFilePath));

			particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(particleGroup.modelData.vertices.size()), particleGroup.instanceCount, 0, 0);
		}
	}
}

std::list<ParticleManager::Particle> ParticleManager::Emit(
	const std::string name,
	const WorldTransform& parent,
	const int count,
	const float lifeTime,
	const float lifeTimeRandomRange,
	const ParameterState& positionState,
	const EasingState& positionEasingState,
	const Parameter& position,
	const ParameterState& rotationState,
	const EasingState& rotationEasingState,
	const Parameter& rotation,
	const ParameterState& scaleState,
	const EasingState& scaleEasingState,
	const Parameter& scale,
	const Vector4& startColor,
	const Vector4& endColor,
	const Vector4& randomColor,
	const bool& isBillboard
) {

	// 指定された名前のパーティクルグループが存在するか確認
	assert(particleGroups.find(name) != particleGroups.end() && "Error: パーティクルグループが存在しません。");

	// パーティクルグループの取得
	ParticleGroup& particleGroup = particleGroups[name];

	// 新しいパーティクルのリスト
	std::list<Particle> newParticles;

	// 新しいパーティクルの生成
	if (particleGroup.instanceCount <= static_cast<uint32_t>(count)) {

		// パーティクルの生成
		Particle particle = MakeNewParticle(
			randomEngine,
			parent,
			positionState,
			positionEasingState,
			position,
			rotationState,
			rotationEasingState,
			rotation,
			scaleState,
			scaleEasingState,
			scale,
			startColor,
			endColor,
			randomColor,
			lifeTime,
			isBillboard
		);

		// パーティクルリストに追加
		newParticles.push_back(particle);
	}

	// パーティクルグループのパーティクルリストに追加
	particleGroup.particles.splice(particleGroup.particles.end(), newParticles);

	return newParticles;
}

ParticleManager::Particle ParticleManager::MakeNewParticle(
	std::mt19937& randomEngine,
	const WorldTransform& emitterTransform,
	const ParameterState& positionState,
	const EasingState& positionEasingState,
	const Parameter& position,
	const ParameterState& rotationState,
	const EasingState& rotationEasingState,
	const Parameter& rotation,
	const ParameterState& scaleState,
	const EasingState& scaleEasingState,
	const Parameter& scale,
	const Vector4& startColor,
	const Vector4& endColor,
	const Vector4& randomRangeColor,
	const float& lifeTime,
	const bool& isBillboard
) {

	//新しく作成するパーティクル
	Particle newParticle;

	/// === 座標設定 === ///

	newParticle.positionPara.startNum = DistributionVector3(position.startNum, position.startRandomRange);

	newParticle.positionPara.endNum = DistributionVector3(position.endNum, position.endRandomRange);

	newParticle.positionPara.velocity = DistributionVector3(position.velocity, position.velocityRandomRange);

	newParticle.positionPara.acceleration = DistributionVector3(position.acceleration, position.accelerationRandomRange);

	newParticle.positionState = positionState;

	newParticle.positionEasingState = positionEasingState;

	/// === 角度設定 === ///

	newParticle.rotationPara.startNum = DistributionVector3(rotation.startNum, rotation.startRandomRange);

	newParticle.rotationPara.endNum = DistributionVector3(rotation.endNum, rotation.endRandomRange);

	newParticle.rotationPara.velocity = DistributionVector3(rotation.velocity, rotation.velocityRandomRange);

	newParticle.rotationPara.acceleration = DistributionVector3(rotation.acceleration, rotation.accelerationRandomRange);

	newParticle.rotationState = rotationState;

	newParticle.rotationEasingState = rotationEasingState;

	/// === 大きさ設定 === ///

	newParticle.scalePara.startNum = DistributionVector3(scale.startNum, scale.startRandomRange);

	newParticle.scalePara.endNum = DistributionVector3(scale.endNum, scale.endRandomRange);

	newParticle.scalePara.velocity = DistributionVector3(scale.velocity, scale.velocityRandomRange);

	newParticle.scalePara.acceleration = DistributionVector3(scale.acceleration, scale.accelerationRandomRange);

	newParticle.scaleState = scaleState;

	newParticle.scaleEasingState = scaleEasingState;

	/// === 色設定 === ///

	newParticle.startColor = DistributionVector4(startColor, randomRangeColor);

	newParticle.endColor = endColor;

	newParticle.color = newParticle.startColor;

	/// === ワールドトランスフォーム設定 === ///

	newParticle.emitterTransform = emitterTransform;

	newParticle.emitterTransform.UpdateMatrix();

	newParticle.transform.Initialize();

	newParticle.transform.translation_ = newParticle.positionPara.startNum;

	newParticle.transform.rotation_ = newParticle.rotationPara.startNum;

	newParticle.transform.scale_ = newParticle.scalePara.startNum;

	newParticle.transform.UpdateMatrix();

	/// === タイマー設定 === ///

	newParticle.lifeTime = lifeTime;

	newParticle.currentTime = 0.0f;

	newParticle.isBillboard = isBillboard;

	return newParticle;
}

Vector3 ParticleManager::UpdateParameter(Parameter& parameter, ParameterState& state, EasingState& easingState, float lifeTime, float t) {

	// イージング処理の初期値
	Vector3 startPos;

	// イージング処理の終了値
	Vector3 endPos;

	// 結果
	Vector3 result;

	switch (state) {

	case START:

		// パラメータの開始値をそのまま設定
		result = parameter.startNum;

		break;

	case VELOCITY:

		// パラメータの速度を加算
		parameter.velocity += parameter.acceleration;

		// イージング処理の開始値をパラメータの開始値で設定
		startPos = parameter.startNum;

		// イージング処理の終了値を移動処理後の値で設定
		endPos = parameter.startNum +
			parameter.velocity *
			(lifeTime / kDeltaTime);

		// イージング処理
		result =
			Lerp(
				startPos,
				endPos,
				t
			);

		break;

	case EASING:

		// イージング状態で処理を分岐
		switch (easingState) {

		case LERP:

			// 線形補間
			result =
				Lerp(
					parameter.startNum,
					parameter.endNum,
					t
				);

			break;

		case EASEINSINE:

			// EaseIn(等倍)
			result =
				EaseIn(
					parameter.startNum,
					parameter.endNum,
					t,
					1.0f
				);

			break;

		case EASEINQUAD:

			// EaseIn(2乗)
			result =
				EaseIn(
					parameter.startNum,
					parameter.endNum,
					t,
					2.0f
				);

			break;

		case EASEINCUBIC:

			// EaseIn(3乗)
			result =
				EaseIn(
					parameter.startNum,
					parameter.endNum,
					t,
					3.0f
				);

			break;

		case EASEINQUART:

			// EaseIn(4乗)
			result =
				EaseIn(
					parameter.startNum,
					parameter.endNum,
					t,
					4.0f
				);

			break;

		case EASEOUTSINE:

			// EaseOut(等倍)
			result =
				EaseOut(
					parameter.startNum,
					parameter.endNum,
					t,
					1.0f
				);

			break;

		case EASEOUTQUAD:

			// EaseOut(2乗)
			result =
				EaseOut(
					parameter.startNum,
					parameter.endNum,
					t,
					2.0f
				);

			break;

		case EASEOUTCUBIC:

			// EaseOut(3乗)
			result =
				EaseOut(
					parameter.startNum,
					parameter.endNum,
					t,
					3.0f
				);

			break;

		case EASEOUTQUART:

			// EaseOut(4乗)
			result =
				EaseOut(
					parameter.startNum,
					parameter.endNum,
					t,
					4.0f
				);

			break;

		case EASEINOUTSINE:

			// EaseInOut(等倍)
			result =
				EaseInOut(
					parameter.startNum,
					parameter.endNum,
					t,
					1.0f
				);

			break;

		case EASEINOUTQUAD:

			// EaseInOut(2乗)
			result =
				EaseInOut(
					parameter.startNum,
					parameter.endNum,
					t,
					2.0f
				);

			break;

		case EASEINOUTCUBIC:

			// EaseInOut(3乗)
			result =
				EaseInOut(
					parameter.startNum,
					parameter.endNum,
					t,
					3.0f
				);

			break;

		case EASEINOUTQUART:

			// EaseInOut(4乗)
			result =
				EaseInOut(
					parameter.startNum,
					parameter.endNum,
					t,
					4.0f
				);

			break;

		}

		break;
	}

	return result;
}

void ParticleManager::CreateParticleGroup(std::string& name, const std::string& modelFileName) {

	// グループ名重複時のID
	int id = 0;

	// グループ名を取得
	std::string groupName = name;

	// グループ名が重複している場合
	while (particleGroups.contains(groupName)) {

		// IDをインクリメント
		id++;

		// グループ名を再設定
		groupName = name + std::to_string(id);
	}

	// 再設定した名前をグループ名として設定
	name = groupName;

	// パーティクルグループの生成
	particleGroups[groupName] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups[groupName];

	// --- モデルデータ生成 ---
	particleGroup.modelData = LoadObjFile(kDirectoryPath, modelFileName);

	// --- 頂点リソース生成 ---
	particleGroup.vertexResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * particleGroup.modelData.vertices.size());

	// --- 頂点バッファビュー生成 ---
	particleGroup.vertexBufferView.BufferLocation = particleGroup.vertexResource->GetGPUVirtualAddress();
	particleGroup.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * particleGroup.modelData.vertices.size());
	particleGroup.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// --- 書き込み ---
	particleGroup.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.vertexData));
	std::memcpy(particleGroup.vertexData, particleGroup.modelData.vertices.data(), sizeof(VertexData) * particleGroup.modelData.vertices.size());

	// --- テクスチャ読み込み ---
	TextureManager::GetInstance()->LoadTexture(particleGroup.modelData.material.textureFilePath);

	// --- インスタンスリソース生成 ---
	particleGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	particleGroup.instancingSRVIndex = srvManager_->Allocate() + 1;
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));
	srvManager_->CreateSRVforStructuredBuffer(particleGroup.instancingSRVIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	// --- マテリアルリソース生成 ---
	particleGroup.materialResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	particleGroup.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.materialData));
	particleGroup.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	particleGroup.materialData->uvTransform = MakeIdentity4x4();

	// --- インスタンス数初期化 ---
	particleGroup.instanceCount = 0;
}

void ParticleManager::ChangeGroupName(const std::string& name, const std::string& preName) {

	// 旧名が存在しない場合
	if (!particleGroups.contains(preName)) {
		return;
	}

	// 新名のパーティクルグループに旧名のパーティクルグループを移動
	particleGroups[name] = particleGroups[preName];

	// 旧名のパーティクルグループを削除
	particleGroups.erase(preName);
}

void ParticleManager::ReloadGroup(std::string& name, const std::string& preName, const std::string& modelFileName) {

	// 旧名が存在しない場合
	if (!particleGroups.contains(preName)) {
		return;
	}

	// 旧名のパーティクルグループを削除
	particleGroups.erase(preName);

	// パーティクルグループの生成
	CreateParticleGroup(name, modelFileName);
}

void ParticleManager::ClearParticles() {

	// パーティクルグループの全削除
	particleGroups.clear();
}

std::vector<const char*> ParticleManager::GetModelFiles() {

	// モデルファイル名を返す
	std::vector<const char*> items;

	// モデルファイル名を追加
	for (const auto& file : modelFiles) {
		items.push_back(file.c_str());
	}

	return items;
}

ParticleManager::MaterialData ParticleManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& modelFileName) {

	// マテリアルデータ
	MaterialData materialData;

	// ファイル1行分の文字列
	std::string line;

	// ファイルパスを設定
	std::ifstream file(directoryPath + "/" + modelFileName);

	// ファイルを開く
	assert(file.is_open());

	// ファイルの内容を1行ずつ読み込む
	while (std::getline(file, line)) {

		// 識別子
		std::string identifier;

		// 1行を読み込む
		std::istringstream s(line);

		// 識別子を取得
		s >> identifier;

		// 識別子が"map_Kd"の場合
		if (identifier == "map_Kd") {

			// テクスチャファイル名を取得
			std::string textureFilename;
			s >> textureFilename;

			// テクスチャファイルパスを設定
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	// テクスチャファイルパス指定されていない場合
	if (materialData.textureFilePath.empty()) {

		// テクスチャファイルパスを白色の1x1テクスチャに設定
		materialData.textureFilePath = directoryPath + "/../images/white1x1.png";
	}

	return materialData;
}


ParticleManager::ModelData ParticleManager::LoadObjFile(const std::string& directoryPath, const std::string& modelFileName) {

	// フルパスの取得
	std::string fullPath = directoryPath + modelFileName;

	// モデルキャッシュから検索
	auto it = modelCache.find(fullPath);

	// モデルキャッシュに存在する場合
	if (it != modelCache.end()) {

		// キャッシュから取得
		return it->second;
	}

	// モデルデータ
	ModelData modelData;

	// 頂点データ
	std::vector<Vector4> positions;

	// テクスチャ座標データ
	std::vector<Vector2> texcoords;

	// ファイル1行分の文字列
	std::string line;

	// ファイルパスを設定
	std::string folderPath;

	// ファイル名の部分のスラッシュ位置を取得
	size_t lastSlashPos = modelFileName.find_last_of("/\\");

	// 値が見つかった場合
	if (lastSlashPos != std::string::npos) {

		// フォルダパスを取得
		folderPath = modelFileName.substr(0, lastSlashPos);
	}

	// ファイルパスを設定
	std::ifstream file(fullPath);

	// ファイルを開く
	assert(file.is_open());

	// ファイルの内容を1行ずつ読み込む
	while (std::getline(file, line)) {

		// 識別子
		std::string identifier;

		// 1行を読み込む
		std::istringstream s(line);

		// 識別子を取得
		s >> identifier;

		if (identifier == "v") {

			/// === 識別子が「v」の場合 === ///

			// 頂点座標
			Vector4 position;

			// 頂点座標を取得
			s >> position.x >> position.y >> position.z;

			// X軸を反転
			position.x *= -1.0f;

			// W座標を設定
			position.w = 1.0f;

			// 頂点座標をデータに追加
			positions.push_back(position);

		} else if (identifier == "vt") {

			/// === 識別子が「vt」の場合 === ///

			// テクスチャ座標
			Vector2 texcoord;

			// テクスチャ座標を取得
			s >> texcoord.x >> texcoord.y;

			// Y軸座標を調整
			texcoord.y = 1.0f - texcoord.y;

			// テクスチャ座標をデータに追加
			texcoords.push_back(texcoord);

		} else if (identifier == "f") {

			/// === 識別子が「f」の場合 === ///

			// 面の頂点データ
			VertexData triangle[3];

			// 面の頂点データを取得
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {

				// 頂点定義
				std::string vertexDefinition;

				// 頂点定義を取得
				s >> vertexDefinition;

				// 頂点定義をストリームに変換
				std::istringstream v(vertexDefinition);

				// 頂点データ
				uint32_t elementIndices[3];

				// 頂点データを取得
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}

				// 座標データを取得
				Vector4 position = positions[elementIndices[0] - 1];

				// テクスチャ座標データを取得
				Vector2 texcoord = texcoords[elementIndices[1] - 1];

				// 頂点データを作成
				VertexData vertex = { position, texcoord };

				// 頂点データをデータに追加
				modelData.vertices.push_back(vertex);

				// 三角形データに追加
				triangle[faceVertex] = { position, texcoord };
			}
		} else if (identifier == "mtllib") {

			/// === 識別子が「mtllib」の場合 === ///

			// マテリアルファイル名
			std::string materialFilename;

			// マテリアルファイル名を取得
			s >> materialFilename;

			// フォルダパスが存在する場合
			if (!folderPath.empty()) {

				// フォルダパスからマテリアルファイルを読み込む
				modelData.material = LoadMaterialTemplateFile(directoryPath + folderPath, materialFilename);
			} else {

				// マテリアルファイルを読み込む
				modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			}
		}
	}

	// モデルキャッシュに追加
	modelCache[fullPath] = modelData;

	return modelData;
}

Vector4 ParticleManager::DistributionVector4(Vector4 num, Vector4 range) {

	Vector4 result;

	std::uniform_real_distribution<float> distNumX(num.x - range.x, num.x + range.x);
	std::uniform_real_distribution<float> distNumY(num.y - range.y, num.y + range.y);
	std::uniform_real_distribution<float> distNumZ(num.z - range.z, num.z + range.z);
	std::uniform_real_distribution<float> distNumW(num.w - range.w, num.w + range.w);

	result.x = distNumX(randomEngine);
	result.y = distNumY(randomEngine);
	result.z = distNumZ(randomEngine);
	result.w = distNumW(randomEngine);

	return result;
}

Vector3 ParticleManager::DistributionVector3(Vector3 num, Vector3 range) {

	Vector3 result;

	Vector3 num1 = num - range;
	Vector3 num2 = num + range;

	Vector3 min;
	Vector3 max;

	std::uniform_real_distribution<float> distNumX(num.x - range.x, num.x + range.x);
	std::uniform_real_distribution<float> distNumY(num.y - range.y, num.y + range.y);
	std::uniform_real_distribution<float> distNumZ(num.z - range.z, num.z + range.z);

	result.x = distNumX(randomEngine);
	result.y = distNumY(randomEngine);
	result.z = distNumZ(randomEngine);

	return result;
}

float ParticleManager::DistributionFloat(float num, float range) {

	std::uniform_real_distribution<float> distNum(num - range, num + range);

	float result;

	result = distNum(randomEngine);

	return result;
}