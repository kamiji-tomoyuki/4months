#pragma once
#include "PlayerAttackBaseState.h"
class PlayerAttackStateLeftSlash : public PlayerAttackBaseState
{
	public:
	// コンストラクタ
		PlayerAttackStateLeftSlash(Player* player) : PlayerAttackBaseState("LeftSlash", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
};

