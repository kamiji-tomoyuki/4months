#include "EnemyStateDefense.h"
#include "Player.h"
#include "TimeManager.h"
#include "EnemyStateApproach.h"
#include "EnemyStateRoot.h"

EnemyStateDefense::EnemyStateDefense(Enemy* enemy)
: BaseEnemyState("State Defense", enemy) {}

void EnemyStateDefense::Update(){
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	float kDefense = 0.1f;

	enemy_->SetVelocity({});
	if (!timeManager->GetTimer("DefenseCoolTime").isStart) {
		timeManager->SetTimer("DefenseCoolTime", kDefense);
	}

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		if (timeManager->GetTimer("DefenseCoolTime").isStart) {
			
		} else {
			enemy_->ChangeState(std::make_unique<EnemyStateRoot>(enemy_));
		}
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	}
}
