#pragma once
#include "Soldier.h"

class SoldierStateAttack : public BaseEnemyState {
public:
	enum class AttackType {
		kDownSwing,		// 振り下ろし(上入力攻撃)
		kThrust,		// 突き(下入力攻撃)
		kRightSlash,	// 右振り抜き(左入力攻撃)
		kLeftSlash,		// 左振り抜き(右入力攻撃)
		kNullType,		// 未入力
	};
public:
	SoldierStateAttack(Enemy* soldier);

	//初期化
	void Initialize()override;
	//更新
	void Update()override;
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
	Soldier* soldier_;
	Player* player_;
	TimeManager* timeManager_;
	Soldier::Attack attack_;

	// 攻撃方向タイプ
	AttackType attackType_ = AttackType::kNullType;
	static void(SoldierStateAttack::* AttackTypeInitFuncTable[])();
	static void(SoldierStateAttack::* AttackTypeUpdateFuncTable[])();
	// 攻撃方向タイプのリクエスト
	std::optional<AttackType> attackTypeRequest_ = AttackType::kNullType;
};

