#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "myMath.h"

class LockOn;
class FollowCamera {
public:
	struct Shake {
		Vector2 move;
		Vector2 preMove;
		float kTime;
		float time;
		bool isShake = false;
	};
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void Reset();

	void ShakeStart(Vector2 move, float kTime);
private:
	void ApplyGlobalVariables();
	void Shaking();
	float LerpShortAngle(const float& v1, const float& v2, float t);
	//追従対象からのオフセットを計算する
	Vector3 MakeOffset();
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;
	//追従対象の残像座標
	Vector3 interTarget_{};
	//追従対象からのオフセット
	Vector3 offset_ = {0.0f,2.0f,-100.0f};

	float destinationAngleX_ = -2.7f;
	float destinationAngleY_ = 0.0f;

	Quaternion destinationAngle{};

	Shake shake_{};
	//ロックオン
	const LockOn* lockOn_ = nullptr;
public:
	void SetTarget(const WorldTransform* target);
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
};
