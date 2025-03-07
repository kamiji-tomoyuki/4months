#pragma once
#include <string>
#include "WorldTransform.h"  // Transformの定義があるヘッダーをインクルード
#include "ParticleManager.h" // ParticleManagerのインクルード
#include "ViewProjection.h"
#ifdef _DEBUG
#include "imgui.h" // ImGuiのインクルード
#endif // _DEBUG
#include"GlobalVariables.h"

///=====================================================/// 
/// パーティクルエミッタークラス
///=====================================================///
class ParticleEmitter {

	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name">パーティクル名</param>
	/// <param name="fileName">テクスチャファイル名</param>
	void Initialize(const std::string& name, const std::string& fileName);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="vp_">ビュープロジェクション</param>
	void Update(const ViewProjection& vp_);

	/// <summary>
	/// 一回更新
	/// </summary>
	/// <param name="vp_">ビュープロジェクション</param>
	void UpdateOnce(const ViewProjection& vp_);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// エミッターの描画
	/// </summary>
	void DrawEmitter();

	/// <summary>
	/// ImGuiの値を動かす関数
	/// </summary>
	void imgui(); // ImGui用の関数を追加

	/// === エミッターのセッター === ///

	void SetEmitPosition(const Vector3& position) { transform_.translation_ = position; }
	void SetEmitScale(const Vector3& scale) { transform_.scale_ = scale; }
	void SetEmitCount(const int& count) { count_ = count; }
	void SetEmitActive(bool isActive) { isActive_ = isActive; }
	void SetEmitValue();

	/// === パーティクルのセッター === ///

	void SetLifeTime(float min, float max) { lifeTimeMin_ = min; lifeTimeMax_ = max; }
	void SetVelocity(Vector3 min, Vector3 max) { velocityMin_ = min; velocityMax_ = max; }
	void SetAcceleration(Vector3 start, Vector3 end) { startAcce_ = start; endAcce_ = end; }
	void SetScale(Vector3 min, Vector3 max) { allScaleMin = min; allScaleMax = max; }
	void SetRandScale(float min, float max) { isRandomScale = true; scaleMin = min; scaleMax = max; }
	void SetRotate(Vector3 start, Vector3 end) { startRote_ = start; endRote_ = end; }
	void SetRandRotate(Vector3 min, Vector3 max) { isRandomRotate = true; rotateVelocityMin = min; rotateVelocityMax = max; }
	void SetAlpha(float min, float max) { alphaMin_ = min; alphaMax_ = max; }
	void SetFrequency(float frequency) { emitFrequency_ = frequency; }
	void SetCount(int count) { count_ = count; }
	void SetIsBillBoard(bool flag) { isBillBoard = flag; }
	void SetIsRandColor(bool flag) { isRandomColor = flag; }

	///-------------------------------------------/// 
	/// クラス内関数
	///-------------------------------------------///
private:

	/// <summary>
	/// パーティクルを生成
	/// </summary>
	void Emit();

	/// <summary>
	/// グローバル値を適用
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// グローバル値に追加
	/// </summary>
	void AddItem();

	///-------------------------------------------/// 
	/// メンバ変数
	///-------------------------------------------///
private:

	//パーティクルマネージャー
	std::unique_ptr<ParticleManager> Manager_;

	//グローバル変数
	GlobalVariables* globalVariables = nullptr;

	//グループ名
	const char* groupName = nullptr;

	//エミッターの名前
	std::string name_;

	//エミッターのワールド座標
	WorldTransform transform_;

	//パーティクルの最大数
	int count_;

	//パーティクルの発生頻度
	float emitFrequency_;

	//経過時間
	float elapsedTime_;

	//デルタタイム
	float deltaTime = 1.0f / 60.0f;

	//パーティクルの生存時間の最小値
	float lifeTimeMin_;

	//パーティクルの生存時間の最大値
	float lifeTimeMax_;

	//パーティクルの透明度の最小値
	float alphaMin_;

	//パーティクルの透明度の最大値
	float alphaMax_;

	//パーティクルのスケールの最小値
	float scaleMin;

	//パーティクルのスケールの最大値
	float scaleMax;

	//パーティクルの速度の最小値
	Vector3 velocityMin_;

	//パーティクルの速度の最大値
	Vector3 velocityMax_;

	//パーティクルの開始加速度
	Vector3 startAcce_;

	//パーティクルの終了加速度
	Vector3 endAcce_;

	//パーティクルの開始回転
	Vector3 startRote_ = {};

	//パーティクルの終了回転
	Vector3 endRote_ = {};

	//パーティクルの回転速度
	Vector3 rotateVelocityMin;

	//パーティクルの回転速度
	Vector3 rotateVelocityMax;

	//パーティクルの開始スケール
	Vector3 startScale_;

	//パーティクルの終了スケール
	Vector3 endScale_;

	//パーティクルの全体スケールの最小値
	Vector3 allScaleMin;

	//パーティクルの全体スケールの最大値
	Vector3 allScaleMax;

	//スケールのランダム生成フラグ
	bool isRandomScale = false;

	//全体スケールのランダム生成フラグ
	bool isAllRamdomScale = false;

	//色のランダム生成フラグ
	bool isRandomColor = true;

	//回転のランダム生成フラグ
	bool isRandomRotate = false;

	//エミッター描画フラグ
	bool isEmitterVisible;

	//ビルボードフラグ
	bool isBillBoard = true;

	//エミッターのアクティブフラグ
	bool isActive_ = false;

	//加速度の乗算フラグ
	bool isAcceMultiply = false;

	//sin波の動きフラグ
	bool isSinMove = false;

};