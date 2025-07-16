#pragma once
#include "PlayerBaseState.h"
class PlayerStateDash : public PlayerBaseState
{
	public:
	// コンストラクタ
	PlayerStateDash(Player* player) : PlayerBaseState("Dash", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

