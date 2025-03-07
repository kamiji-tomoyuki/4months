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

///=====================================================/// 
/// パーティクルマネージャークラス
///=====================================================///
class ParticleManager
{

	///-------------------------------------------/// 
	/// 構造体
	///-------------------------------------------///
public:

	//GPUに渡すパーティクルデータ
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	//パーティクルデータ
	struct Particle {
		WorldTransform transform;
		Vector3 velocity;
		Vector3 Acce;
		Vector4 color;
		float lifeTime;
		float currentTime;
		Vector3 startScale;
		Vector3 endScale;
		Vector3 startAcce;
		Vector3 endAcce;
		Vector3 startRote;
		Vector3 endRote;
		Vector3 rotateVelocity;
		float initialAlpha;
	};

	//マテリアルデータ
	struct MaterialData {
		std::string textureFilePath;
	};

	//パーティクルグループ
	struct ParticleGroup {
		MaterialData material;
		std::list<Particle> particles;
		uint32_t instancingSRVIndex = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
		uint32_t instanceCount = 0;
		ParticleForGPU* instancingData = nullptr;
	};

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};

	//マテリアル
	struct Material {
		Vector4 color;
		Matrix4x4 uvTransform;
		float padding[3];
	};

	//モデルデータ
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="srvManager">SRVマネージャー</param>
	void Initialize(SrvManager* srvManager);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="viewProjeciton">ビュープロジェクション</param>
	void Update(const ViewProjection& viewProjeciton);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクルグループ生成
	/// </summary>
	/// <param name="name">グループ名</param>
	/// <param name="filename">使用するテクスチャ</param>
	void CreateParticleGroup(const std::string name, const std::string& filename);

	///-------------------------------------------/// 
	/// セッター
	///-------------------------------------------///
public:

	void SetBillBorad(bool isBillBoard) { isBillboard = isBillBoard; }
	void SetRandomRotate(bool isRandomRotate) { isRandomRotate_ = isRandomRotate; }
	void SetAcceMultipy(bool isAcceMultipy) { isAcceMultipy_ = isAcceMultipy; }
	void SetRandomSize(bool isRandomSize) { isRandomSize_ = isRandomSize; }
	void SetAllRandomSize(bool isAllRandomSize) { isRandomAllSize_ = isAllRandomSize; }
	void SetSinMove(bool isSinMove) { isSinMove_ = isSinMove; }

	///-------------------------------------------/// 
	/// クラス内関数
	///-------------------------------------------///
private:

	/// <summary>
	/// 頂点データ生成
	/// </summary>
	/// <param name="filename">使用するモデル名</param>
	void CreateVertexData(const std::string& filename);

	///-------------------------------------------/// 
	/// メンバ変数
	///-------------------------------------------///
private:

	//パーティクル共通
	ParticleCommon* particleCommon = nullptr;

	//SRVマネージャー
	SrvManager* srvManager_;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;

	//頂点データ
	VertexData* vertexData = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;

	//マテリアルデータ
	Material* materialData = nullptr;

	//モデルデータ
	ModelData modelData;

	//モデルキャッシュ
	static std::unordered_map<std::string, ModelData> modelCache;

	//パーティクルグループ
	std::unordered_map<std::string, ParticleGroup>particleGroups;

	// デルタタイム
	const float kDeltaTime = 1.0f / 60.0f;

	// パーティクルインスタンスの最大数
	static const uint32_t kNumMaxInstance = 10000;

	// 乱数生成
	std::random_device seedGenerator;

	// 乱数エンジン
	std::mt19937 randomEngine;

	// ビルボードフラグ
	bool isBillboard = false;

	// ランダム回転フラグ
	bool isRandomRotate_ = false;

	// 加速度乗算フラグ
	bool isAcceMultipy_ = false;

	// ランダムサイズフラグ
	bool isRandomSize_ = false;

	// 全体ランダムサイズフラグ
	bool isRandomAllSize_ = false;

	// Sin波移動フラグ
	bool isSinMove_ = false;

public:
	
	/// <summary>
	/// 指定した名前のパーティクルグループにパーティクルを発生させる
	/// </summary>
	std::list<Particle> Emit(const std::string name, const Vector3& position, uint32_t count, const Vector3& scale,
		const Vector3& velocityMin, const Vector3& velocityMax, float lifeTimeMin, float lifeTimeMax,
		const Vector3& particleStartScale, const Vector3& particleEndScale, const Vector3& startAcce, const Vector3& endAcce,
		const Vector3& startRote, const Vector3& endRote, bool isRandomColor, float alphaMin, float alphaMax,
		const Vector3& rotateVelocityMin, const Vector3& rotateVelocityMax,
		const Vector3& allScaleMax, const Vector3& allScaleMin,
		const float& scaleMin, const float& scaleMax, const Vector3& rotation);

private:
	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>マテリアルデータ</returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	///  .objファイルの読み取り
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>モデルデータ</returns>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterial();

	/// <summary>
	/// 新しいパーティクルを生成
	/// </summary>
	/// <returns>パーティクルインスタンス</returns>
	Particle MakeNewParticle(std::mt19937& randomEngine,
		const Vector3& translate,
		const Vector3& rotation,
		const Vector3& scale,
		const Vector3& velocityMin, const Vector3& velocityMax,
		float lifeTimeMin, float lifeTimeMax, const Vector3& particleStartScale, const Vector3& particleEndScale,
		const Vector3& startAcce, const Vector3& endAcce, const Vector3& startRote, const Vector3& endRote
		, bool isRamdomColor, float alphaMin, float alphaMax, const Vector3& rotateVelocityMin, const Vector3& rotateVelocityMax,
		const Vector3& allScaleMax, const Vector3& allScaleMin,
		const float& scaleMin, const float& scaleMax);
};

