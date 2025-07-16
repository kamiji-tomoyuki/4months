#pragma once
#include "PlayerBaseState.h"
class PlayerStateGuard : public PlayerBaseState
{
	public:
	// コンストラクタ
	PlayerStateGuard(Player* player) : PlayerBaseState("Guard", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

