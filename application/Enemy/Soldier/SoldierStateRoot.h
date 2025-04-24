#pragma once
#include "Soldier.h"

class SoldierStateRoot : public BaseEnemyState {
public:
	SoldierStateRoot(Enemy* soldier);
	//初期化
	void Initialize()override;
	//更新
	void Update()override;

private:

};
