#pragma once
#include "Enemy.h"
#include "EnemySword.h"

class Soldier : public Enemy{
public:
	enum class AttackType {
		kDownSwing,		// 振り下ろし(上入力攻撃)
		kThrust,		// 突き(下入力攻撃)
		kRightSlash,	// 右振り抜き(左入力攻撃)
		kLeftSlash,		// 左振り抜き(右入力攻撃)
		kNullType,		// 未入力
	};
	struct Attack {
		float kLimitTime = 0.4f;
		Vector3 armStart = { 0.0f };
		Vector3 armEnd = { 0.0f };
		float time = 0;
		bool isAttack = false;
		bool isLeft = false;
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

	//通常
	void RootInitialize()override;
	void RootUpdate()override;
	// 攻撃動作
	void AttackInitialize()override;
	void AttackUpdate()override;

	// 防御動作
	void ProtectionInitialize()override;
	void ProtectionUpdate()override;

	void ApplyGlobalVariables();
	void DirectionPreliminaryAction();
private:	// 攻撃方向タイプ
	// 振り下ろし(上入力攻撃)
	void AttackTypeDownSwingInitialize();
	void AttackTypeDownSwingUpdate();

	// 突き(下入力攻撃)
	void AttackTypeThrustInitialize();
	void AttackTypeThrustUpdate();

	// 右振り抜き(左入力攻撃)
	void AttackTypeLeftSwingInitialize();
	void AttackTypeLeftSwingUpdate();

	// 左振り抜き(右入力攻撃)
	void AttackTypeRightSwingInitialize();
	void AttackTypeRightSwingUpdate();

	// 未入力
	void AttackTypeNullInitialize();
	void AttackTypeNullUpdate();

private:
	const char* groupName = "Soldier";
	bool isMove_ = true;

	std::unique_ptr<EnemySword> sword_;

	// 攻撃方向タイプ
	AttackType attackType_ = AttackType::kNullType;
	static void(Soldier::* AttackTypeInitFuncTable[])();
	static void(Soldier::* AttackTypeUpdateFuncTable[])();
	// 攻撃方向タイプのリクエスト
	std::optional<AttackType> attackTypeRequest_ = AttackType::kNullType;

	Attack attack_;

	// 狙う方向
	Vector3 aimingDirection_{};
public:
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
};

