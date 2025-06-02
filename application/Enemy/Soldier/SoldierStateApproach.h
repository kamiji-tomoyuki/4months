#pragma once
#include "Soldier.h"

class SoldierStateApproach : public BaseEnemyState{
public:
	SoldierStateApproach(Enemy* soldier);
	//初期化
	void Initialize()override;
	//更新
	void Update()override;
};
