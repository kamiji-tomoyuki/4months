#pragma once
#include "ParticleCommon.h"
#include "SrvManager.h"
#include "random"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "ParticleParamater.h"

// パーティクル管理
class ParticleManager {
public:

	static ParticleManager* instance;

	static ParticleManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SrvManager* srvManager);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const ViewProjection& viewProjeciton);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクルグループ生成
	/// </summary>
	/// <param name="name"></param>
	/// <param name="textureFilePath"></param>
	void CreateParticleGroup(const std::string name, const std::string& filename);

	void ChangeGroupName(const std::string& newName,const std::string& preName);

	void ChangeModel(const std::string name, const std::string& filename);

	std::vector<const char*> GetModelFiles();

private:

	/// <summary>
	/// 頂点データ作成
	/// </summary>
	void CreateVartexData(const std::string& filename);

private:

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct Particle {
		WorldTransform transform;
		WorldTransform emitterTransform;
		ParameterState positionState;
		EasingState positionEasingState;
		Parameter positionPara;
		ParameterState rotationState;
		EasingState rotationEasingState;
		Parameter rotationPara;
		ParameterState scaleState;
		EasingState scaleEasingState;
		Parameter scalePara;
		Vector4 color;
		Vector4 startColor;
		Vector4 endColor;
		Vector4 randomRangeColor;
		float lifeTime;
		float currentTime;
		bool isBillboard;
	};

	struct MaterialData {
		std::string textureFilePath;
	};

	struct ParticleGroup {
		MaterialData material;
		std::list<Particle> particles;
		uint32_t instancingSRVIndex = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
		uint32_t instanceCount = 0;
		ParticleForGPU* instancingData = nullptr;
	};

	ParticleCommon* particleCommon = nullptr;
	SrvManager* srvManager_;

	// --- 頂点データ ---
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	VertexData* vertexData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// --- マテリアルデータ ---
	struct Material {
		Vector4 color;
		Matrix4x4 uvTransform;
		float padding[3];
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Material* materialData = nullptr;

	// --- モデルデータ ---
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	ModelData modelData;

	static std::unordered_map<std::string, ModelData> modelCache;

	std::vector<std::string> modelFiles;

	std::unordered_map<std::string, ParticleGroup>particleGroups;

	// デルタタイム
	const float kDeltaTime = 1.0f / 60.0f;
	static const uint32_t kNumMaxInstance = 10000;

	std::random_device seedGenerator;
	std::mt19937 randomEngine;

	int id_;

public:

	std::list<Particle> Emit(
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
	);

private:
	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	///  .objファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterial();

	Particle MakeNewParticle(
		std::mt19937& randomEngine,
		const WorldTransform& parent,
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
	);

	Vector4 DistributionVector4(Vector4 num, Vector4 range);

	Vector3 DistributionVector3(Vector3 num, Vector3 range);

	float DistributionFloat(float num, float range);
};