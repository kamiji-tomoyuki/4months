#pragma once
#include "PlayerAttackBaseState.h"

class PlayerAttackStateThrust : public PlayerAttackBaseState
{
	public:
	// コンストラクタ
	PlayerAttackStateThrust(Player* player) : PlayerAttackBaseState("Thrust", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

