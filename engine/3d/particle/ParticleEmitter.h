#pragma once
#include <string>
#include "WorldTransform.h"  // Transformの定義があるヘッダーをインクルード
#include "ParticleManager.h" // ParticleManagerのインクルード
#include "ViewProjection.h"
#include "ParticleParamater.h"
#include "json.hpp"

#ifdef _DEBUG
#include "imgui.h" // ImGuiのインクルード
#endif // _DEBUG

///=====================================================/// 
/// エミッター
///=====================================================///
class ParticleEmitter {

public:
	// コンストラクタでメンバ変数を初期化
	ParticleEmitter();

	// メンバ変数の初期化を行うInitialize関数
	void Initialize();

	// 更新処理を行うUpdate関数
	void Update();

	// 一度だけ更新を行うUpdate関数
	void UpdateOnce();

	// エミッターを描画するDraw関数
	void Draw();

	// エミッターを描画するDraw関数
	void DrawEmitter();

	// モデルを変更
	void ChangeModel(const std::string& fileName);

	// ImGui用の関数を追加
	void ImGui(); 

	// エミッターの内容をJsonファイルにセーブする
	void SaveEmitterData();

	// Jsonファイルからエミッターの内容を読み込む
	void LoadEmitterData(const std::string& fileName);

	void Start();

public:
	// 名前を取得
	std::string GetName() { return name_; }

	// タイマーを進める
	void AddTime(float time) { totalTimer_ += time; }

	void SetTime(float time) { totalTimer_ = time; }

	void SetPosition(const Vector3& pos) { transform_.translation_ = pos; }

private:
	// パーティクルを発生させるEmit関数
	void Emit();

	/// === タイマー === ///

	//現在時間
	float emitTimer;

	//合計時間
	float totalTimer_;

	/// === エミッター設定 === ///

	//アクティブフラグ
	bool isActive_;

	//エミッターの描画フラグ
	bool isDrawEmitter_;

	//名前
	std::string name_;

	//生成数
	int count_;

	//発生頻度
	float frequency_;

	//生存時間
	float lifeTime_;

	//生存時間のランダム幅
	float lifeTimeRandomRange_;

	//生成開始時間
	float startTime_;

	//生成終了時間
	float endTime_;

	//ループフラグ
	bool isLoop_;

	//エミッターのトランスフォーム
	WorldTransform transform_;

	/// === パーティクル設定 === ///

	//座標設定の状態
	ParameterState positionState_;

	//座標設定のイージング状態
	EasingState positionEasingState_;

	//座標
	Parameter position_;

	//回転設定の状態
	ParameterState rotationState_;

	//回転設定のイージング状態
	EasingState rotationEasingState_;

	//回転
	Parameter rotation_;

	//拡縮設定の状態
	ParameterState scaleState_;

	//拡縮設定のイージング状態
	EasingState scaleEasingState_;

	//拡縮
	Parameter scale_;

	/// === モデル設定 === ///

	//モデル
	std::string modelName_;

	//色
	Vector4 startColor_;

	//終了色
	Vector4 endColor_;

	//ランダムの色
	Vector4 colorRandomRange_;

	//ビルボードフラグ
	bool isBillboard_;

	// パーティクルマネージャー
	ParticleManager* Manager_;
};