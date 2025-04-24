#pragma once
#include "Enemy.h"
#include "BaseEnemyState.h"
#include "EnemySword.h"

class Soldier : public Enemy{
public:
	struct Attack {
		float kLimitTime = 0.4f;
		Vector3 swordStartTransform = { 0.0f };
		Vector3 swordEndTransform = { 0.0f };
		Vector3 swordStartRotate = { 0.0f };
		Vector3 swordEndRotate = { 0.0f };
		float time = 0;
		bool isAttack = false;
		float kLimitTimeDefense = 1.0f;
	};
public:
	Soldier();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	void UpdateParticle(const ViewProjection& viewProjection) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection)override;
	void DrawParticle(const ViewProjection& viewProjection)override;
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
	void DirectionPreliminaryAction();

private:
	const char* groupName = "Soldier";
	bool isMove_ = true;

	std::unique_ptr<EnemySword> sword_;

	Attack attack_;

	// 狙う方向
	Vector3 aimingDirection_{};
public:
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;

	EnemySword* GetSword() { return sword_.get(); }
	const Attack& GetAttack() { return attack_; }
	Vector3 GetAimingDirection() const { return aimingDirection_; }

	void SetAttack(const Attack& attack) { attack_ = attack; }
	void SetAttackTime(float attackTime) { attack_.time = attackTime; }
	void SetIsAttack(bool isAttack) { attack_.isAttack = isAttack; }
	void SetAimingDirection(Vector3 aimingDirection) { aimingDirection_ = aimingDirection; }
	void SetAimingDirectionX(float aimingDirectionX) { aimingDirection_.x = aimingDirectionX; }
	void SetAimingDirectionY(float aimingDirectionY) { aimingDirection_.y = aimingDirectionY; }
	void SetAimingDirectionZ(float aimingDirectionZ) { aimingDirection_.x = aimingDirectionZ; }
};