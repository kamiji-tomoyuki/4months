#include "SoldierStateLeave.h"
#include "myMath.h"

SoldierStateLeave::SoldierStateLeave(Enemy* soldier) :
	BaseEnemyState("State Leave", soldier) {

}

void SoldierStateLeave::Initialize(){

}

void SoldierStateLeave::Update() {
	const float kCharacterSpeed = 0.2f;
	Vector3 move = {-kCharacterSpeed, kCharacterSpeed, 0.0f};
	enemy_->SetTranslation((enemy_->GetCenterPosition() + move));
}
