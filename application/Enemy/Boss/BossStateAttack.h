#pragma once
#include "Boss.h"

class BossStateAttack : public BaseEnemyState {
public:
	struct Attack {
		Vector3 swordStartTransform = { 0.0f };
		Vector3 swordEndTransform = { 0.0f };
		Vector3 swordStartRotate = { 0.0f };
		Vector3 swordEndRotate = { 0.0f };
	};
public:
	BossStateAttack(Enemy* boss);

	//初期化
	void Initialize()override;
	//更新
	void Update()override;
private:	// 攻撃方向タイプ
	Boss* boss_;
	Player* player_;
	TimeManager* timeManager_;
	Attack attack_;
};

