#include "EnemyStateRoot.h"
#include "Player.h"
#include "TimeManager.h"
#include "EnemyStateApproach.h"
#include "EnemyStateAttack.h"
#include "EnemyStateDefense.h"

EnemyStateRoot::EnemyStateRoot(Enemy* enemy)
	: BaseEnemyState("State Root", enemy){}

void EnemyStateRoot::Update(){
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	bool isAttack = false;
	bool isDefense = false;

	enemy_->SetVelocity({});

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		if (!timeManager->GetTimer("CoolTime").isStart) {
			isAttack = enemy_->GetProbabilities(enemy_->GetShortDistanceProbability().kAttack);
			if (!isAttack) {
				isDefense = enemy_->GetProbabilities(enemy_->GetShortDistanceProbability().kDefense);
			}
			timeManager->SetTimer("CoolTime", enemy_->GetCoolTime());
		}
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<EnemyStateApproach>(enemy_));
	}
	if (isAttack) {
		enemy_->ChangeState(std::make_unique<EnemyStateAttack>(enemy_));
	}
	if (isDefense) {
		enemy_->ChangeState(std::make_unique<EnemyStateDefense>(enemy_));
	}
}
