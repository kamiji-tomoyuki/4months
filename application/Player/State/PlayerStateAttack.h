#pragma once
#include "PlayerBaseState.h"
class PlayerStateAttack : public PlayerBaseState
{
	public:
	// コンストラクタ
	PlayerStateAttack(Player* player) : PlayerBaseState("Attack", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

