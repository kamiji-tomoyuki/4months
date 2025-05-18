#pragma once
#include "Enemy.h"

class Boss : public Enemy {
public:
	
public:
	Boss();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection)override;
	void DrawAnimation(const ViewProjection& viewProjection)override;
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

	void ApplyGlobalVariables();
private:
	const char* groupName = "Boss";
	bool isMove_ = true;

	// 狙う方向
	Vector3 aimingDirection_{0,1,0};
public:
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
	Vector3 GetAimingDirection() const { return aimingDirection_; }
	// 方向を取得
	void SetAimingDirection(Vector3 aimingDirection) { aimingDirection_ = aimingDirection; }
	void SetAimingDirectionX(float aimingDirectionX) { aimingDirection_.x = aimingDirectionX; }
	void SetAimingDirectionY(float aimingDirectionY) { aimingDirection_.y = aimingDirectionY; }
	void SetAimingDirectionZ(float aimingDirectionZ) { aimingDirection_.x = aimingDirectionZ; }
};
