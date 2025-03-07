#include "EnemyStateApproach.h"
#include "myMath.h"
#include "EnemyStateRoot.h"
#include "Player.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy)
	: BaseEnemyState("State Approach",enemy) {}

void EnemyStateApproach::Update() {
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	Vector3 velocity_ = player->GetCenterPosition() - enemy_->GetCenterPosition();
	velocity_ = velocity_.Normalize();
	enemy_->SetTranslation(enemy_->GetCenterPosition() + velocity_);

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		enemy_->SetVelocity({});
		enemy_->ChangeState(std::make_unique<EnemyStateRoot>(enemy_));
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		
	} else {

	}
}
