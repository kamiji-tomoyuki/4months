#include "BossStateAttack.h"
#include "Player.h"
#include "TimeManager.h"
//#include "BossStateApproach.h"
//#include "BossStateAttack.h"
//#include "BossStateDefense.h"

BossStateAttack::BossStateAttack(Enemy* boss)
	: BaseEnemyState("State Root", boss){}

void BossStateAttack::Initialize(){
	Boss* boss = static_cast<Boss*>(enemy_);
}

void BossStateAttack::Update(){
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	bool isAttack = false;
	bool isDefense = false;

	float kAttack = enemy_->GetShortDistanceProbability().kAttack;
	float kDefense = enemy_->GetShortDistanceProbability().kDefense;

	if (player->GetAimingDirection().x != 0.0f || player->GetAimingDirection().y != 0.0f) {
		float kAttack = 0.1f;
		kDefense = 0.6f;
	} 

	if (!timeManager->GetTimer("CoolTime" + std::to_string(enemy_->GetSerialNumber())).isStart) {
		isAttack = enemy_->GetProbabilities(kAttack);
		if (!isAttack) {
			isDefense = enemy_->GetProbabilities(kDefense);
		}
		timeManager->SetTimer("CoolTime" + std::to_string(enemy_->GetSerialNumber()), enemy_->GetCoolTime());
	}
	if (isAttack) {
		//enemy_->ChangeState(std::make_unique<SoldierStateAttack>(enemy_));
		return;
	}
	if (isDefense) {
		//enemy_->ChangeState(std::make_unique<SoldierStateDefense>(enemy_));
		return;
	}

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	} else {
		//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	}
}
