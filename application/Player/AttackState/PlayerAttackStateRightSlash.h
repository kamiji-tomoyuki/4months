#pragma once
#include "PlayerAttackBaseState.h"

class PlayerAttackStateRightSlash : public PlayerAttackBaseState
{
	public:
	// コンストラクタ
	PlayerAttackStateRightSlash(Player* player) : PlayerAttackBaseState("RightSkash", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

