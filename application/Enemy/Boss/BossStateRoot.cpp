#include "BossStateRoot.h"
#include "Player.h"
#include "TimeManager.h"
//#include "BossStateApproach.h"
#include "BossStateAttack.h"

BossStateRoot::BossStateRoot(Enemy* boss)
	: BaseEnemyState("State Root", boss){}

void BossStateRoot::Initialize(){
	Boss* boss = static_cast<Boss*>(enemy_);
	TimeManager* timeManager = enemy_->GetTimeManager();
	timeManager->SetTimer("CoolTime" + std::to_string(enemy_->GetSerialNumber()), enemy_->GetCoolTime());
}

void BossStateRoot::Update(){
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	bool isAttack = false;

	float kAttack = enemy_->GetShortDistanceProbability().kAttack;

	if (player->GetAimingDirection().x != 0.0f || player->GetAimingDirection().y != 0.0f) {
		float kAttack = 0.1f;
	} 

	if (!timeManager->GetTimer("CoolTime" + std::to_string(enemy_->GetSerialNumber())).isStart) {
		isAttack = enemy_->GetProbabilities(kAttack);
		timeManager->SetTimer("CoolTime" + std::to_string(enemy_->GetSerialNumber()), enemy_->GetCoolTime());
	}
	if (isAttack) {
		enemy_->ChangeState(std::make_unique<BossStateAttack>(enemy_));
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
