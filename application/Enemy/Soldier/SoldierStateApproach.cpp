#include "SoldierStateApproach.h"
#include "myMath.h"
#include "SoldierStateRoot.h"
#include "Player.h"

SoldierStateApproach::SoldierStateApproach(Enemy* soldier)
	: BaseEnemyState("State Approach",soldier) {}

void SoldierStateApproach::Initialize(){

}

void SoldierStateApproach::Update() {
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	Vector3 velocity_ = player->GetCenterPosition() - enemy_->GetCenterPosition();
	velocity_ = velocity_.Normalize();
	enemy_->SetTranslation(enemy_->GetCenterPosition() + velocity_);

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		enemy_->ChangeState(std::make_unique<SoldierStateRoot>(enemy_));
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		
	} else {

	}
}
