#include "ParticleManager.h"
#include "TextureManager.h"
#include "fstream"

// モデルキャッシュ
std::unordered_map<std::string, ParticleManager::ModelData> ParticleManager::modelCache;

void ParticleManager::Initialize(SrvManager* srvManager) {
	// パーティクル共通の初期化
	particleCommon = ParticleCommon::GetInstance();

	// SRVマネージャーの設定
	srvManager_ = srvManager;

	// 乱数エンジンの初期化
	randomEngine.seed(seedGenerator());
}

void ParticleManager::Update(const ViewProjection& viewProjection) {
	/// === 各行列の初期化・計算 === ///

	// ビュー・プロジェクション行列の計算
	Matrix4x4 viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;

	// ビルボード行列の設定
	Matrix4x4 billboardMatrix = viewProjection.matView_;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	billboardMatrix.m[3][3] = 1.0f;

	billboardMatrix = Inverse(billboardMatrix);

	/// === パーティクルグループの更新 === ///

	for (auto& [groupName, particleGroup] : particleGroups) {

		// 合計インスタンス数の初期化
		uint32_t numInstance = 0;

		/// === パーティクルインスタンスの更新 === ///

		for (auto particleIterator : particleGroup.particles) {

			// パーティクルの生存時間が終了していたら削除
			if (particleIterator.lifeTime <= particleIterator.currentTime) {

				// パーティクルの削除
				particleGroup.particles.remove(particleIterator);

				// 次のパーティクルへ
				continue;
			}

			// パーティクルの生存時間 t の計算
			float t = particleIterator.currentTime / particleIterator.lifeTime;
			t = std::clamp(t, 0.0f, 1.0f);

			/// === 拡散処理 === ///

			// フラグがtrueの場合
			if (isSinMove_) {

				// Sin波の周波数制御 (速度調整)
				float waveScale = 0.5f * (sin(t * DirectX::XM_PI * 18.0f) + 1.0f);  // 0 ~ 1

				// 最大スケールが寿命に応じて縮小し、最終的に0になる
				float maxScale = (1.0f - t);

				// Sin波スケールと最大スケールの積を適用
				particleIterator.transform.scale_ = particleIterator.startScale * waveScale * maxScale;

			} else {

				// 通常の線形補間
				particleIterator.transform.scale_ = (1.0f - t) * particleIterator.startScale + t * particleIterator.endScale;

				// アルファ値の計算
				particleIterator.color.w = particleIterator.initialAlpha - (particleIterator.currentTime / particleIterator.lifeTime);
			}

			/// ===パーティクルの回転=== ///

			// フラグがtrueの場合
			if (isRandomRotate_) {

				// 回転量分回転させる
				particleIterator.transform.rotation_ += particleIterator.rotateVelocity;
			} else {

				// 初期回転量と終了回転量の線形補間
				particleIterator.transform.rotation_ = (1.0f - t) * particleIterator.startRote + t * particleIterator.endRote;
			}

			/// === パーティクルの移動 === ///

			// 加速度の計算
			particleIterator.Acce = (1.0f - t) * particleIterator.startAcce + t * particleIterator.endAcce;

			// フラグがtrueの場合
			if (isAcceMultipy_) {

				// 加速度乗算
				particleIterator.velocity *= particleIterator.Acce;
			} else {

				// 加速度加算
				particleIterator.velocity += particleIterator.Acce;
			}

			// 移動量分移動させる
			particleIterator.transform.translation_ += particleIterator.velocity * kDeltaTime;

			// 経過時間を加算
			particleIterator.currentTime += kDeltaTime;

			/// === ワールド行列に変換 === ///

			// ワールド行列
			Matrix4x4 worldMatrix{};

			// フラグがtrueの場合
			if (isBillboard) {

				// ビルボード状態で変換
				worldMatrix = MakeScaleMatrix(particleIterator.transform.scale_) * billboardMatrix *
					MakeTranslateMatrix(particleIterator.transform.translation_);
			} else {

				// 通常状態で変換
				worldMatrix = MakeAffineMatrix(particleIterator.transform.scale_,
					particleIterator.transform.rotation_,
					particleIterator.transform.translation_);
			}

			// ワールド・ビュー・プロジェクション行列計算
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			/// === インスタンスのデータ登録 === ///

			if (numInstance < kNumMaxInstance) {
				particleGroup.instancingData[numInstance].WVP = worldViewProjectionMatrix;
				particleGroup.instancingData[numInstance].World = worldMatrix;
				particleGroup.instancingData[numInstance].color = particleIterator.color;
				particleGroup.instancingData[numInstance].color.w = particleIterator.color.w;
				++numInstance;
			}
		}

		// インスタンス数更新
		particleGroup.instanceCount = numInstance;
	}
}

void ParticleManager::Draw() {
	// 頂点バッファの設定
	particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	// パーティクルグループの描画
	for (auto& [groupName, particleGroup] : particleGroups) {

		// インスタンス数が0より大きい場合
		if (particleGroup.instanceCount > 0) {

			// マテリアルデータの設定
			particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			// インスタンスデータの設定
			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.instancingSRVIndex);

			// テクスチャデータの設定
			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.material.textureFilePath));

			// 描画
			particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), particleGroup.instanceCount, 0, 0);
		}
	}
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string& filename) {
	//すでに登録されている場合は登録しない
	if (particleGroups.contains(name)) {
		return;
	}

	//パーティクルグループの生成
	particleGroups[name] = ParticleGroup();

	//新しく生成したパーティクルグループ
	ParticleGroup& particleGroup = particleGroups[name];

	/// === 頂点データの初期化 === ///

	//頂点データの生成
	CreateVertexData(filename);

	/// === テクスチャの初期化 === ///

	//テクスチャのファイルパスを取得
	particleGroup.material.textureFilePath = modelData.material.textureFilePath;

	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);

	/// === インスタンスデータの初期化 === ///

	//インスタンスリソースの生成
	particleGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	//SRVのインデックスを取得
	particleGroup.instancingSRVIndex = srvManager_->Allocate() + 1;

	//インスタンスデータのマップ
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));

	//インスタンスSRVのバッファーの生成
	srvManager_->CreateSRVforStructuredBuffer(particleGroup.instancingSRVIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	/// === マテリアルの初期化 === ///

	//マテリアルの生成
	CreateMaterial();

	//インスタンス数の初期化
	particleGroup.instanceCount = 0;
}

void ParticleManager::CreateVertexData(const std::string& filename) {
	//モデルデータを読み込む
	modelData = LoadObjFile("resources/models/", filename);

	//頂点リソースの生成
	vertexResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	//頂点バッファビューの設定
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点データのマップ
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//モデルデータから頂点データをコピー
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

ParticleManager::Particle ParticleManager::MakeNewParticle(
	std::mt19937& randomEngine,
	const Vector3& translate,
	const Vector3& rotation,
	const Vector3& scale,
	const Vector3& velocityMin, const Vector3& velocityMax,
	float lifeTimeMin, float lifeTimeMax,
	const Vector3& particleStartScale, const Vector3& particleEndScale,
	const Vector3& startAcce, const Vector3& endAcce,
	const Vector3& startRote, const Vector3& endRote,
	bool isRamdomColor, float alphaMin, float alphaMax,
	const Vector3& rotateVelocityMin, const Vector3& rotateVelocityMax,
	const Vector3& allScaleMax, const Vector3& allScaleMin,
	const float& scaleMin, const float& scaleMax
) {
	//乱数の生成
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distVelocityX(velocityMin.x, velocityMax.x);
	std::uniform_real_distribution<float> distVelocityY(velocityMin.y, velocityMax.y);
	std::uniform_real_distribution<float> distVelocityZ(velocityMin.z, velocityMax.z);
	std::uniform_real_distribution<float> distLifeTime(lifeTimeMin, lifeTimeMax);
	std::uniform_real_distribution<float> distAlpha(alphaMin, alphaMax);

	//パーティクル
	Particle particle;

	// スケールを考慮してランダムな位置を生成
	Vector3 randomTranslate = {
		distribution(randomEngine) * scale.x,
		distribution(randomEngine) * scale.y,
		distribution(randomEngine) * scale.z
	};

	// 角度から回転行列を生成
	Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(rotation);

	// ランダム位置を回転行列で変換
	Vector3 rotatedPosition = {
		randomTranslate.x * rotationMatrix.m[0][0] + randomTranslate.y * rotationMatrix.m[1][0] + randomTranslate.z * rotationMatrix.m[2][0],
		randomTranslate.x * rotationMatrix.m[0][1] + randomTranslate.y * rotationMatrix.m[1][1] + randomTranslate.z * rotationMatrix.m[2][1],
		randomTranslate.x * rotationMatrix.m[0][2] + randomTranslate.y * rotationMatrix.m[1][2] + randomTranslate.z * rotationMatrix.m[2][2]
	};

	// 回転されたランダムな位置をパーティクルの座標に加算
	particle.transform.translation_ = translate + rotatedPosition;

	// フラグがtrueの場合
	if (isRandomAllSize_) {

		//ランダムなスケールを生成
		std::uniform_real_distribution<float> distScaleX(allScaleMin.x, allScaleMax.x);
		std::uniform_real_distribution<float> distScaleY(allScaleMin.y, allScaleMax.y);
		std::uniform_real_distribution<float> distScaleZ(allScaleMin.z, allScaleMax.z);

		// パーティクルのスケールをランダムに設定
		particle.startScale = { distScaleX(randomEngine),distScaleY(randomEngine),distScaleZ(randomEngine) };

	} else if (isRandomSize_) {

		// ランダムのスケールを生成
		std::uniform_real_distribution<float> distScale(scaleMin, scaleMax);

		//X軸のみランダムなスケールを適用
		particle.startScale.x = distScale(randomEngine);
		particle.startScale.y = particle.startScale.x;
		particle.startScale.z = particle.startScale.x;

	} else {

		//初期スケールを設定
		particle.startScale = particleStartScale;
	}

	// 終了スケールを設定
	particle.endScale = particleEndScale;

	//初期加速度を設定
	particle.startAcce = startAcce;

	//終了加速度を設定
	particle.endAcce = endAcce;

	// パーティクルの速度をランダムに設定
	Vector3 randomVelocity = {
		distVelocityX(randomEngine),
		distVelocityY(randomEngine),
		distVelocityZ(randomEngine)
	};

	// エミッターの回転を速度ベクトルに適用
	particle.velocity = {
		randomVelocity.x * rotationMatrix.m[0][0] + randomVelocity.y * rotationMatrix.m[1][0] + randomVelocity.z * rotationMatrix.m[2][0],
		randomVelocity.x * rotationMatrix.m[0][1] + randomVelocity.y * rotationMatrix.m[1][1] + randomVelocity.z * rotationMatrix.m[2][1],
		randomVelocity.x * rotationMatrix.m[0][2] + randomVelocity.y * rotationMatrix.m[1][2] + randomVelocity.z * rotationMatrix.m[2][2]
	};

	// フラグがtrueの場合
	if (isRandomRotate_) {

		// ランダムな回転速度を生成
		std::uniform_real_distribution<float> distRotateXVelocity(rotateVelocityMin.x, rotateVelocityMax.x);
		std::uniform_real_distribution<float> distRotateYVelocity(rotateVelocityMin.y, rotateVelocityMax.y);
		std::uniform_real_distribution<float> distRotateZVelocity(rotateVelocityMin.z, rotateVelocityMax.z);
		std::uniform_real_distribution<float> distRotateX(0.0f, 2.0f);
		std::uniform_real_distribution<float> distRotateY(0.0f, 2.0f);
		std::uniform_real_distribution<float> distRotateZ(0.0f, 2.0f);

		// 回転速度をランダムに設定
		particle.rotateVelocity.x = distRotateXVelocity(randomEngine);
		particle.rotateVelocity.y = distRotateYVelocity(randomEngine);
		particle.rotateVelocity.z = distRotateZVelocity(randomEngine);
		particle.transform.rotation_.x = distRotateX(randomEngine);
		particle.transform.rotation_.y = distRotateY(randomEngine);
		particle.transform.rotation_.z = distRotateZ(randomEngine);
	} else {

		//初期回転を設定
		particle.startRote = startRote;

		//終了回転を設定
		particle.endRote = endRote;
	}

	// フラグがtrueの場合
	if (isRamdomColor) {

		// ランダムな色を生成
		std::uniform_real_distribution<float> distColor(0.0f, 1.0f);

		// パーティクルの色をランダムに設定
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), distAlpha(randomEngine) };
	} else {

		// パーティクルの色を設定
		particle.color = { 1.0f,1.0f,1.0f, distAlpha(randomEngine) };
	}

	//パーティクルの透明度をランダムな値で設定
	particle.initialAlpha = distAlpha(randomEngine);

	// パーティクルの寿命をランダムな値で設定
	particle.lifeTime = distLifeTime(randomEngine);

	// パーティクルの経過時間を初期化
	particle.currentTime = 0.0f;

	return particle;
}

ParticleManager::MaterialData ParticleManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {

	//マテリアルデータ
	MaterialData materialData;

	//1行分のデータ
	std::string line;

	//ファイルの内容
	std::ifstream file(directoryPath + "/" + filename);

	//ファイルを開く
	assert(file.is_open());

	//ファイルの内容を1行ずつ読み込む
	while (std::getline(file, line)) {

		//識別子
		std::string identifier;

		//1行分のデータを読み込む
		std::istringstream s(line);

		//識別子を取得
		s >> identifier;

		//識別子が一致していれば
		if (identifier == "map_Kd") {

			//テクスチャファイル名
			std::string textureFilename;

			//テクスチャファイル名を取得
			s >> textureFilename;

			//テクスチャファイルパスを設定
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	//テクスチャファイルパスが空の場合
	if (materialData.textureFilePath.empty()) {

		//白色のテクスチャを設定
		materialData.textureFilePath = directoryPath + "/../images/white1x1.png";
	}

	return materialData;
}


ParticleManager::ModelData ParticleManager::LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	//ファイルのフルパス
	std::string fullPath = directoryPath + filename;

	//モデルキャッシュに存在しているかの確認
	auto it = modelCache.find(fullPath);

	//モデルキャッシュに存在している場合
	if (it != modelCache.end()) {

		//モデルデータを返す
		return it->second;
	}

	//モデルデータ
	ModelData modelData;

	//頂点データ
	std::vector<Vector4> positions;

	//テクスチャ座標データ
	std::vector<Vector2> texcoords;

	//1行分のデータ
	std::string line;

	//フォルダーパス
	std::string folderPath;

	//ファイルの最後のスラッシュがある位置を取得
	size_t lastSlashPos = filename.find_last_of("/\\");

	//ファイルの最後のスラッシュがある場合
	if (lastSlashPos != std::string::npos) {

		//フォルダーパスを取得
		folderPath = filename.substr(0, lastSlashPos);
	}

	//ファイルの内容
	std::ifstream file(fullPath);

	//ファイルを開く
	assert(file.is_open());

	//ファイルの内容を1行ずつ読み込む
	while (std::getline(file, line)) {

		//識別子
		std::string identifier;

		//1行分のデータを読み込む
		std::istringstream s(line);

		//識別子を取得
		s >> identifier;

		//識別子が一致していれば
		if (identifier == "v") {

			//位置座標
			Vector4 position;

			//位置座標を取得
			s >> position.x >> position.y >> position.z;

			//位置座標を修正
			position.x *= -1.0f;
			position.w = 1.0f;

			//位置座標を登録
			positions.push_back(position);
		} else if (identifier == "vt") {

			//テクスチャ座標
			Vector2 texcoord;

			//テクスチャ座標を取得
			s >> texcoord.x >> texcoord.y;

			//テクスチャ座標を修正
			texcoord.y = 1.0f - texcoord.y;

			//テクスチャ座標を登録
			texcoords.push_back(texcoord);
		} else if (identifier == "f") {

			//三角形の頂点データ
			VertexData triangle[3];

			//面の頂点データを取得
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {

				//頂点データ
				std::string vertexDefinition;

				//頂点データを取得
				s >> vertexDefinition;

				//頂点データを分割
				std::istringstream v(vertexDefinition);

				//要素のインデックス
				uint32_t elementIndices[3];

				//要素のインデックスを取得
				for (int32_t element = 0; element < 3; ++element) {

					//要素のインデックス
					std::string index;

					//要素のインデックスを取得
					std::getline(v, index, '/');

					//要素のインデックスを登録
					elementIndices[element] = std::stoi(index);
				}

				//1頂点分のデータを取得
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				VertexData vertex = { position, texcoord };

				//頂点データを登録
				modelData.vertices.push_back(vertex);

				//三角形の頂点データを登録
				triangle[faceVertex] = { position, texcoord };
			}

		} else if (identifier == "mtllib") {

			//マテリアルファイル名
			std::string materialFilename;

			//マテリアルデータを取得
			s >> materialFilename;

			//フォルダパスがある場合
			if (!folderPath.empty()) {

				//フォルダパスと、マテリアルファイルパスからマテリアルデータを取得
				modelData.material = LoadMaterialTemplateFile(directoryPath + folderPath, materialFilename);
			} else {

				//マテリアルファイルパスからマテリアルデータを取得
				modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			}
		}
	}

	//モデルキャッシュに登録
	modelCache[fullPath] = modelData;

	return modelData;
}


void ParticleManager::CreateMaterial() {

	//マテリアルリソースの生成
	materialResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//マテリアルデータのマップ
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//マテリアルデータの初期化
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	//UV変換行列の初期化
	materialData->uvTransform = MakeIdentity4x4();
}

std::list<ParticleManager::Particle> ParticleManager::Emit(
	const std::string name,
	const Vector3& position,
	uint32_t count,
	const Vector3& scale,
	const Vector3& velocityMin, const Vector3& velocityMax,
	float lifeTimeMin, float lifeTimeMax,
	const Vector3& particleStartScale, const Vector3& particleEndScale,
	const Vector3& startAcce, const Vector3& endAcce,
	const Vector3& startRote, const Vector3& endRote,
	bool isRandomColor, float alphaMin, float alphaMax,
	const Vector3& rotateVelocityMin, const Vector3& rotateVelocityMax,
	const Vector3& allScaleMax, const Vector3& allScaleMin,
	const float& scaleMin, const float& scaleMax, const Vector3& rotation) {
	assert(particleGroups.find(name) != particleGroups.end() && "Error: パーティクルグループが存在しません。");

	// パーティクルグループの取得
	ParticleGroup& particleGroup = particleGroups[name];

	// 新しいパーティクルの生成
	std::list<Particle> newParticles;

	// 生成するパーティクルの数だけ繰り返す
	for (uint32_t nowCount = 0; nowCount < count; ++nowCount) {

		// パーティクルの生成
		Particle particle = MakeNewParticle(
			randomEngine,
			position,
			rotation,
			scale,
			velocityMin,
			velocityMax,
			lifeTimeMin,
			lifeTimeMax,
			particleStartScale,
			particleEndScale,
			startAcce,
			endAcce,
			startRote,
			endRote,
			isRandomColor,
			alphaMin,
			alphaMax,
			rotateVelocityMin,
			rotateVelocityMax,
			allScaleMax, allScaleMin,
			scaleMin, scaleMax
		);

		// 新しいパーティクルをリストに追加
		newParticles.push_back(particle);
	}

	//パーティクルグループに登録
	particleGroup.particles.splice(particleGroup.particles.end(), newParticles);

	return newParticles;
}
