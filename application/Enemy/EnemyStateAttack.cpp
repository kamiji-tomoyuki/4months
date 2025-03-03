#include "EnemyStateAttack.h"
#include "Player.h"
#include "TimeManager.h"
#include "EnemyStateApproach.h"
#include "EnemyStateRoot.h"

EnemyStateAttack::EnemyStateAttack(Enemy* enemy)
	: BaseEnemyState("State Attack", enemy){}

void EnemyStateAttack::Update(){
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	enemy_->SetVelocity({});
	if (!timeManager->GetTimer("AttackCoolTime").isStart) {
		timeManager->SetTimer("AttackCoolTime", enemy_->GetCoolTime());
	}

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		if (timeManager->GetTimer("AttackCoolTime").isStart) {

		} else {
			enemy_->ChangeState(std::make_unique<EnemyStateRoot>(enemy_));
		}
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	}
}
