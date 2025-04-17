#pragma once
#include "Boss.h"

class BossStateRoot : public BaseEnemyState {
public:
	BossStateRoot(Enemy* boss);
	//初期化
	void Initialize()override;
	//更新
	void Update()override;

private:

};
