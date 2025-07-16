#pragma once
#include "PlayerBaseState.h"
class PlayerStateRoot : public PlayerBaseState
{
public:
	// コンストラクタ
	PlayerStateRoot(Player* player) : PlayerBaseState("Root", player) {};
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	
};

