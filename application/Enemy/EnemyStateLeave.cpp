#include "EnemyStateLeave.h"
#include "myMath.h"

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy) {

}

void EnemyStateLeave::Update() {
	const float kCharacterSpeed = 0.2f;
	Vector3 move = {-kCharacterSpeed, kCharacterSpeed, 0.0f};
	enemy_->SetTranslation((enemy_->GetCenterPosition() + move));
}
