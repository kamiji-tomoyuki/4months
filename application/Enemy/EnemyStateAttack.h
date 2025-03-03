#pragma once
#include "Enemy.h"

class EnemyStateAttack : public BaseEnemyState {
public:
	EnemyStateAttack(Enemy* enemy);

	void Update();

private:

};

