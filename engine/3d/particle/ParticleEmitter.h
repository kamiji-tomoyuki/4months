#pragma once
#include <string>
#include "WorldTransform.h"  // Transformの定義があるヘッダーをインクルード
#include "ParticleManager.h" // ParticleManagerのインクルード
#include "ViewProjection.h"
#ifdef _DEBUG
#include "imgui.h" // ImGuiのインクルード
#endif // _DEBUG
#include"GlobalVariables.h"

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

    void SetEmitPosition(const Vector3& position) { transform_.translation_ = position; }
    void SetEmitScale(const Vector3& scale) { transform_.scale_ = scale; }
    void SetEmitCount(const int& count) { count_ = count; }
    void SetEmitActive(bool isActive) { isActive_ = isActive; }
    void SetEmitValue();

    // 各パーティクルの設定
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

private:
    // パーティクルを発生させるEmit関数
    void Emit();

    void ApplyGlobalVariables();
    void AddItem();

    std::string name_;          // パーティクルの名前
    WorldTransform transform_;       // 位置や回転などのトランスフォーム
    int count_;                 // 一度に発生させるパーティクルの数

    float emitFrequency_;       // パーティクルの発生頻度
    float elapsedTime_;         // 経過時間
    // Emitに関連するメンバ変数を追加
    float lifeTimeMin_;         // ライフタイムの最小値
    float lifeTimeMax_;         // ライフタイムの最大値
    float alphaMin_;
    float alphaMax_;
    float deltaTime = 1.0f / 60.0f;
    float scaleMin;
    float scaleMax;

    Vector3 velocityMin_;       // 速度の最小値
    Vector3 velocityMax_;       // 速度の最大値
    Vector3 startScale_;
    Vector3 endScale_;
    Vector3 startAcce_;
    Vector3 endAcce_;
    Vector3 startRote_ = {};
    Vector3 endRote_ = {};
    Vector3 rotateVelocityMin;
    Vector3 rotateVelocityMax;
    Vector3 allScaleMin;
    Vector3 allScaleMax;

    bool isRandomScale = false;
    bool isAllRamdomScale = false;
    bool isRandomColor = true;
    bool isRandomRotate = false;
    bool isVisible;
    bool isBillBoard = true;
    bool isActive_ = false;
    bool isAcceMultiply = false;
    bool isSinMove = false;

    std::unique_ptr<ParticleManager> Manager_;

    GlobalVariables* globalVariables = nullptr;
    const char* groupName = nullptr;

};