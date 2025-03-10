#pragma once
#include <string>
#include "WorldTransform.h"  // Transformの定義があるヘッダーをインクルード
#include "ParticleManager.h" // ParticleManagerのインクルード
#include "ViewProjection.h"
#ifdef _DEBUG
#include "imgui.h" // ImGuiのインクルード
#endif // _DEBUG
#include"GlobalVariables.h"
#include"ParticleParamater.h"

class ParticleEmitter {
public:
	// コンストラクタでメンバ変数を初期化
	ParticleEmitter();

	void Initialize(const std::string& name, const std::string& fileName);

	// 更新処理を行うUpdate関数
	void Update(const ViewProjection& vp_);

	void UpdateOnce(const ViewProjection& vp_);

	void Draw();

	void DrawEmitter();

	void imgui(); // ImGui用の関数を追加

private:
	// パーティクルを発生させるEmit関数
	void Emit();

	void AddItem();
	void SetEmitValue();
	void ApplyGlobalVariables();

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

	//現在時間
	float time_;

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
	Vector4 randomColor_;

	//ビルボードフラグ
	bool isBillboard_;

	std::unique_ptr<ParticleManager> Manager_;

	GlobalVariables* globalVariables = nullptr;
	const char* groupName = nullptr;

};