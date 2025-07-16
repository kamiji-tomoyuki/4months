#pragma once
#include "PlayerBaseState.h"
class PlayerStatePreliminary : public PlayerBaseState
{
	public:
	// コンストラクタ
	PlayerStatePreliminary(Player* player) : PlayerBaseState("Preliminary", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

