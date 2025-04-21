#pragma once
#include "Boss.h"

class BossStateAttack : public BaseEnemyState {
public:
	BossStateAttack(Enemy* boss);
	//初期化
	void Initialize()override;
	//更新
	void Update()override;

private:

};
