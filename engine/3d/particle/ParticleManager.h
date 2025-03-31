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

	// シングルトンインスタンス
	static ParticleManager* instance;

	// シングルトンインスタンスの取得
	static ParticleManager* GetInstance();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

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
	/// パーティクルグループの生成
	/// </summary>
	/// <param name="name">グループ名</param>
	/// <param name="modelFileName">モデルのファイル名</param>
	void CreateParticleGroup(std::string& name, const std::string& modelFileName);

	/// <summary>
	/// パーティクルグループの名前変更
	/// </summary>
	/// <param name="name">変更後の名前</param>
	/// <param name="preName">変更前の名前</param>
	void ChangeGroupName(const std::string& name, const std::string& preName);

	/// <summary>
	/// パーティクルグループの再生成
	/// </summary>
	/// <param name="name">再生成後の名前</param>
	/// <param name="preName">再生成前の名前</param>
	/// <param name="modelFileName">モデルのファイル名</param>
	void ReloadGroup(std::string& name, const std::string& preName, const std::string& modelFileName);

	/// <summary>
	/// パーティクルグループの全削除
	/// </summary>
	void ClearParticles();

	/// <summary>
	/// モデルのファイル名取得
	/// </summary>
	/// <returns></returns>
	std::vector<const char*> GetModelFiles();

private:

	// --- 頂点データ ---
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};

	// --- マテリアルデータ ---
	struct Material {
		Vector4 color;
		Matrix4x4 uvTransform;
		float padding[3];
	};

	// --- マテリアルのファイル名 ---
	struct MaterialData {
		std::string textureFilePath;
	};

	// --- モデルデータ ---
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	// --- パーティクルのGPUデータ ---
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	// --- パーティクル ---
	struct Particle {
		/// === ワールド座標 === ///
		WorldTransform transform;
		WorldTransform emitterTransform;
		/// === 座標のパラメータ === ///
		ParameterState positionState;
		EasingState positionEasingState;
		Parameter positionPara;
		/// === 回転のパラメータ === ///
		ParameterState rotationState;
		EasingState rotationEasingState;
		Parameter rotationPara;
		/// === 拡縮のパラメータ === ///
		ParameterState scaleState;
		EasingState scaleEasingState;
		Parameter scalePara;
		/// === 色 === ///
		Vector4 color;
		Vector4 startColor;
		Vector4 endColor;
		Vector4 randomRangeColor;
		/// === タイマー === ///
		float lifeTime;
		float currentTime;
		/// === その他の項目 === ///
		bool isBillboard;
	};

	// --- パーティクルグループ ---
	struct ParticleGroup {
		/// === パーティクル === ///
		std::list<Particle> particles;
		/// === インスタンスデータ === ///
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
		ParticleForGPU* instancingData = nullptr;
		uint32_t instancingSRVIndex = 0;
		uint32_t instanceCount = 0;
		/// === 頂点データ === ///
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
		VertexData* vertexData = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		/// === マテリアルデータ === ///
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
		Material* materialData = nullptr;
		ModelData modelData;
	};

	// パーティクル共通
	ParticleCommon* particleCommon = nullptr;

	// シェーダーリソースビューマネージャ
	SrvManager* srvManager_;

	// モデルキャッシュ
	static std::unordered_map<std::string, ModelData> modelCache;

	// モデルファイル名
	std::vector<std::string> modelFiles;

	// パーティクルグループ
	std::unordered_map<std::string, ParticleGroup> particleGroups;

	// デルタタイム
	const float kDeltaTime = 1.0f / 60.0f;

	// 最大インスタンス数
	static const uint32_t kNumMaxInstance = 10000;

	// 乱数のシード値
	std::random_device seedGenerator;

	// 乱数エンジン
	std::mt19937 randomEngine;

	// ディレクトリのパス
	const std::string kDirectoryPath = "resources/models/GameScene/";

public:

	/// <summary>
	/// パーティクルの発生
	/// </summary>
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
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="modelFileName">ファイル名</param>
	/// <returns>マテリアルデータ</returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& modelFileName);

	/// <summary>
	///  .objファイルの読み取り
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>モデルデータ</returns>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& modelFileName);

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterial();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <returns>パーティクル</returns>
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

	/// <summary>
	/// パーティクルパラメータの更新
	/// </summary>
	/// <param name="para">パラメータ</param>
	/// <param name="state">状態</param>
	/// <param name="easingState">イージング状態</param>
	/// <param name="lifeTime">生存時間</param>
	/// <param name="t">現在時間</param>
	/// <returns>パラメータ更新の値</returns>
	Vector3 UpdateParameter(Parameter& para, ParameterState& state, EasingState& easingState, float lifeTime, float t);

	/// <summary>
	/// ランダム値の生成(Vector4)
	/// </summary>
	/// <param name="num">基準値</param>
	/// <param name="range">ランダム幅</param>
	/// <returns>生成した値</returns>
	Vector4 DistributionVector4(Vector4 num, Vector4 range);

	/// <summary>
	/// ランダム値の生成(Vector3)
	/// </summary>
	/// <param name="num">基準値</param>
	/// <param name="range">ランダム幅</param>
	/// <returns>生成した値</returns>
	Vector3 DistributionVector3(Vector3 num, Vector3 range);

	/// <summary>
	/// ランダム値の生成(float)
	/// </summary>
	/// <param name="num">基準値</param>
	/// <param name="range">ランダム幅</param>
	/// <returns>生成した値</returns>
	float DistributionFloat(float num, float range);
};