#pragma once
#include "PlayerAttackBaseState.h"
class PlayerAttackStateNoInput : public PlayerAttackBaseState
{
	public:
	// コンストラクタ
	PlayerAttackStateNoInput(Player* player) : PlayerAttackBaseState("NoInput", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

