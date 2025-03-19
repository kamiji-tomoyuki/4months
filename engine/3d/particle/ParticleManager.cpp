#include "ParticleManager.h"
#include "TextureManager.h"
#include "fstream"
#include "filesystem"
#include "Vector3.h"

std::unordered_map<std::string, ParticleManager::ModelData> ParticleManager::modelCache;

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ParticleManager();
	}
	return instance;
}

void ParticleManager::Initialize(SrvManager* srvManager) {
	particleCommon = ParticleCommon::GetInstance();
	srvManager_ = srvManager;
	randomEngine.seed(seedGenerator());

	for (const auto& entry : std::filesystem::directory_iterator("resources/models/GameScene")) {
		if (entry.path().extension() == ".obj") {
			modelFiles.push_back(entry.path().filename().string());
		}
	}
}

void ParticleManager::Update(const ViewProjection& viewProjection) {
	// --- 各行列の初期化・計算 ---
	Matrix4x4 viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;

	Matrix4x4 billboardMatrix = viewProjection.matView_;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	billboardMatrix.m[3][3] = 1.0f;

	billboardMatrix = Inverse(billboardMatrix);

	for (auto& [groupName, particleGroup] : particleGroups) {

		uint32_t numInstance = 0;

		// --- パーティクルの更新 ---
		for (auto particleIterator = particleGroup.particles.begin();
			particleIterator != particleGroup.particles.end();) {

			/// === 削除処理 === ///

			if (particleIterator->lifeTime <= particleIterator->currentTime) {
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			// パーティクルの進行度
			float t = particleIterator->currentTime / particleIterator->lifeTime;

			//0~1の範囲内に収める
			t = std::clamp(t, 0.0f, 1.0f);

			/// === 移動処理 === ///

			switch (particleIterator->positionState) {

			case START:

				particleIterator->transform.translation_ = particleIterator->positionPara.startNum;

				break;

			case VELOCITY:

				particleIterator->transform.translation_ += particleIterator->positionPara.velocity;

				particleIterator->positionPara.velocity += particleIterator->positionPara.acceleration;

				break;

			case EASING:

				switch (particleIterator->positionEasingState) {

				case LERP:

					particleIterator->transform.translation_ =
						Lerp(
							particleIterator->positionPara.startNum,
							particleIterator->positionPara.endNum,
							t
						);

					break;

				case EASEIN:
					break;

				case EASEOUT:
					break;
				}

				break;
			}

			/// === 回転処理 === ///

			switch (particleIterator->rotationState) {

			case START:

				particleIterator->transform.rotation_ = particleIterator->rotationPara.startNum;

				break;

			case VELOCITY:

				particleIterator->transform.rotation_ += particleIterator->rotationPara.velocity;

				particleIterator->rotationPara.velocity += particleIterator->rotationPara.acceleration;

				break;

			case EASING:

				switch (particleIterator->positionEasingState) {

				case LERP:

					particleIterator->transform.rotation_ =
						Lerp(
							particleIterator->rotationPara.startNum,
							particleIterator->rotationPara.endNum,
							t
						);

					break;

				case EASEIN:
					break;

				case EASEOUT:
					break;
				}

				break;
			}

			/// === 拡縮処理 === ///

			switch (particleIterator->scaleState) {

			case START:

				particleIterator->transform.scale_ = particleIterator->scalePara.startNum;

				break;

			case VELOCITY:

				particleIterator->transform.scale_ += particleIterator->scalePara.velocity;

				particleIterator->scalePara.velocity += particleIterator->scalePara.acceleration;

				break;

			case EASING:

				switch (particleIterator->positionEasingState) {

				case LERP:

					particleIterator->transform.scale_ =
						Lerp(
							particleIterator->scalePara.startNum,
							particleIterator->scalePara.endNum,
							t
						);

					break;

				case EASEIN:
					break;

				case EASEOUT:
					break;
				}

				break;
			}

			particleIterator->color = Lerp(particleIterator->startColor, particleIterator->endColor, t);

			// ワールドトランスフォームの更新
			particleIterator->transform.UpdateMatrix();

			particleIterator->transform.matWorld_ *= particleIterator->emitterTransform.matWorld_;

			particleIterator->transform.TransferMatrix();

			// ワールド行列の計算
			Matrix4x4 worldMatrix{};

			if (particleIterator->isBillboard) {

				Vector3 objPos = {
					particleIterator->transform.matWorld_.m[3][0],
					particleIterator->transform.matWorld_.m[3][1],
					particleIterator->transform.matWorld_.m[3][2]
				};

				Vector3 forward = (viewProjection.translation_ - objPos).Normalize();

				Vector3 up = { 0.0f,1.0f,0.0f };

				Vector3 right = (up.Cross(forward)).Normalize();

				up = forward.Cross(right);

				// ビルボード
				worldMatrix = {
					right.x,up.x,forward.x,0,
					right.y,up.y,forward.y,0,
					right.z,up.z,forward.z,0,
					objPos.x,objPos.y,objPos.z,1
				};
			} else {
				// 通常
				worldMatrix = particleIterator->transform.matWorld_;
			}

			// ワールド・ビュー・プロジェクション行列計算
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

			// インスタンスデータ設定
			if (numInstance < kNumMaxInstance) {
				particleGroup.instancingData[numInstance].WVP = worldViewProjectionMatrix;
				particleGroup.instancingData[numInstance].World = worldMatrix;
				particleGroup.instancingData[numInstance].color = (*particleIterator).color;
				particleGroup.instancingData[numInstance].color.w = (*particleIterator).color.w;
				++numInstance;
			}

			particleIterator->currentTime += kDeltaTime;

			++particleIterator;
		}

		// インスタンス数更新
		particleGroup.instanceCount = numInstance;
	}
}

void ParticleManager::Draw() {
	particleCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	for (auto& [groupName, particleGroup] : particleGroups) {
		if (particleGroup.instanceCount > 0) {
			particleCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.instancingSRVIndex);
			srvManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.material.textureFilePath));

			particleCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), particleGroup.instanceCount, 0, 0);
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

	assert(particleGroups.find(name) != particleGroups.end() && "Error: パーティクルグループが存在しません。");

	ParticleGroup& particleGroup = particleGroups[name];

	std::list<Particle> newParticles;

	if (particleGroup.instanceCount <= static_cast<uint32_t>(count)) {

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

		newParticles.push_back(particle);
	}

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

void ParticleManager::CreateParticleGroup(const std::string name, const std::string& filename) {
	// --- パーティクルグループ生成 ---
	if (particleGroups.contains(name)) {
		return;
	}

	particleGroups[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups[name];
	CreateVartexData(filename);

	particleGroup.material.textureFilePath = modelData.material.textureFilePath;
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	particleGroup.instancingResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

	particleGroup.instancingSRVIndex = srvManager_->Allocate() + 1;
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));

	srvManager_->CreateSRVforStructuredBuffer(particleGroup.instancingSRVIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));

	CreateMaterial();
	particleGroup.instanceCount = 0;
}

void ParticleManager::ChangeModel(const std::string name, const std::string& filename) {

	if (!particleGroups.contains(name)) {
		return;
	}

	modelData = LoadObjFile("resources/models/GameScene/", filename);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	particleGroups[name].material.textureFilePath = modelData.material.textureFilePath;

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
}

std::vector<const char*> ParticleManager::GetModelFiles() {

	std::vector<const char*> items;

	for (const auto& file : modelFiles) {
		items.push_back(file.c_str());
	}

	return items;
}

void ParticleManager::CreateVartexData(const std::string& filename) {
	modelData = LoadObjFile("resources/models/", filename);

	// --- 頂点リソース生成 ---
	vertexResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// --- 頂点バッファビュー生成 ---
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// --- 書き込み ---
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

ParticleManager::MaterialData ParticleManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	if (materialData.textureFilePath.empty()) {
		materialData.textureFilePath = directoryPath + "/../images/white1x1.png";
	}

	return materialData;
}


ParticleManager::ModelData ParticleManager::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// --- .obj読み込み ---
	std::string fullPath = directoryPath + filename;

	auto it = modelCache.find(fullPath);
	if (it != modelCache.end()) {
		return it->second;
	}

	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector2> texcoords;
	std::string line;

	std::string folderPath;
	size_t lastSlashPos = filename.find_last_of("/\\");
	if (lastSlashPos != std::string::npos) {
		folderPath = filename.substr(0, lastSlashPos);
	}

	std::ifstream file(fullPath);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				VertexData vertex = { position, texcoord };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord };
			}
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			if (!folderPath.empty()) {
				modelData.material = LoadMaterialTemplateFile(directoryPath + folderPath, materialFilename);
			} else {
				modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			}
		}
	}

	modelCache[fullPath] = modelData;

	return modelData;
}


void ParticleManager::CreateMaterial() {
	// --- マテリアルリソース生成 ---
	materialResource = particleCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->uvTransform = MakeIdentity4x4();
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