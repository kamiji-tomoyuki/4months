#pragma once
#include "PlayerAttackBaseState.h"

class PlayerAttackStateDownSwing : public PlayerAttackBaseState
{
	public:
	// コンストラクタ
	PlayerAttackStateDownSwing(Player* player) : PlayerAttackBaseState("DownSwing", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

