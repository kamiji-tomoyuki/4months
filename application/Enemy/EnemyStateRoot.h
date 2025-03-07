#pragma once
#include "Enemy.h"

class EnemyStateRoot : public BaseEnemyState {
public:
	EnemyStateRoot(Enemy* enemy);

	void Update();

private:

};
