#pragma once
#include "Soldier.h"

class SoldierStateLeave : public BaseEnemyState {
public:
	SoldierStateLeave(Enemy* soldier);

	//初期化
	void Initialize()override;
	//更新
	void Update()override;
};
