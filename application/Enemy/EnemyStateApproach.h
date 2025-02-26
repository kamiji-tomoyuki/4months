#pragma once
#include "Enemy.h"

class EnemyStateApproach : public BaseEnemyState{
public:
	EnemyStateApproach(Enemy* enemy);

	void Update();
};
