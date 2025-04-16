#include "SoldierStateRoot.h"
#include "Player.h"
#include "TimeManager.h"
#include "SoldierStateApproach.h"
#include "SoldierStateAttack.h"
#include "SoldierStateDefense.h"

SoldierStateRoot::SoldierStateRoot(Enemy* soldier)
	: BaseEnemyState("State Root", soldier){}

void SoldierStateRoot::Initialize(){
	Soldier* soldier = static_cast<Soldier*>(enemy_);
	soldier->GetSword()->SetIsAttack(false);
	soldier->GetSword()->SetIsDefense(false);
	soldier->GetSword()->SetTranslation({ 1.7f, 0.0f, 1.3f });
	soldier->SetIsAttack(false);
}

void SoldierStateRoot::Update(){
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
		enemy_->ChangeState(std::make_unique<SoldierStateAttack>(enemy_));
		return;
	}
	if (isDefense) {
		enemy_->ChangeState(std::make_unique<SoldierStateDefense>(enemy_));
		return;
	}

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	}
}
