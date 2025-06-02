#pragma once
#include "Soldier.h"

class SoldierStateDefense : public BaseEnemyState {
public:
	SoldierStateDefense(Enemy* soldier);

	//初期化
	void Initialize()override;
	//更新
	void Update()override;
private:

};

