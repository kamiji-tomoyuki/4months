#pragma once
#include "Boss.h"

class BossStateApproach : public BaseEnemyState {
public:
public:
	BossStateApproach(Enemy* boss);

	//初期化
	void Initialize()override;
	//更新
	void Update()override;
private:	// 攻撃方向タイプ
	Boss* boss_;
	Player* player_;
	TimeManager* timeManager_;
};

