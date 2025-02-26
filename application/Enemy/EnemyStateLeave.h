#pragma once
#include "Enemy.h"

class EnemyStateLeave : public BaseEnemyState {
public:
	EnemyStateLeave(Enemy* enemy);

	void Update();
};
