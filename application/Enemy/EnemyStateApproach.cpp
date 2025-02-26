#include "EnemyStateApproach.h"
#include "myMath.h"
#include "EnemyStateLeave.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy)
	: BaseEnemyState("State Approach",enemy) {}

void EnemyStateApproach::Update() {
	const float kCharacterSpeed = 0.1f;
	Vector3 move = {0.0f, 0.0f, -kCharacterSpeed};
	enemy_->SetTranslation((enemy_->GetCenterPosition() + move));
	//
	if (enemy_->GetCenterPosition().z < 0.0f) {
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}
