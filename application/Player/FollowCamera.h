#pragma once
#include "BaseObject.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "myMath.h"

class LockOn;
class FollowCamera : public BaseObject {
public:
	struct Shake {
		Vector2 move;
		Vector2 preMove;
		float kTime;
		float time;
		bool isShake = false;
	};
public:

	void Init() override;

	void Init(const std::string& fileName);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	void Reset();

	void ShakeStart(Vector2 move, float kTime);

	void Draw(const ViewProjection& viewProjection) override;

	/// <summary>
	/// 当たってる間
	/// </summary>
	/// <param name="other"></param>
	void OnCollision([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="other"></param>
	void OnCollisionEnter([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たり終わった瞬間
	/// </summary>
	/// <param name="other"></param>
	void OnCollisionOut([[maybe_unused]] Collider* other) override;
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
	Vector3 GetCenterPosition() const override;

	Vector3 GetCenterRotation() const override;
	void SetPosition(Vector3 position);

	void SetTarget(const WorldTransform* target);
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
};
