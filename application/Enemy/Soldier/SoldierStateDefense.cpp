#include "SoldierStateDefense.h"
#include "Player.h"
#include "TimeManager.h"
#include "SoldierStateApproach.h"
#include "SoldierStateRoot.h"
using namespace std::numbers;

SoldierStateDefense::SoldierStateDefense(Enemy* soldier)
: BaseEnemyState("State Defense", soldier) {}

void SoldierStateDefense::Initialize(){
	Soldier* soldier = static_cast<Soldier*>(enemy_);
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();
	Soldier::Attack attack = soldier->GetAttack();

	timeManager->SetTimer("DefenseCoolTime" + std::to_string(enemy_->GetSerialNumber()), attack.kLimitTimeDefense);
	attack.time = 0;
	attack.isAttack = false;
	attack.swordStartTransform = soldier->GetSword()->GetTranslation().z;
	soldier->SetAttack(attack);
	soldier->GetSword()->SetIsAttack(false);
	soldier->GetSword()->SetIsDefense(true);
	
	// 方向取得
	soldier->DirectionPreliminaryAction();
	if (player->GetAimingDirection().x == 0 && player->GetAimingDirection().z == 0) {
		//behaviorRequest_ = Behavior::kRoot;
	} else {

		soldier->SetAimingDirectionX(-player->GetAimingDirection().x);
		soldier->SetAimingDirectionZ(player->GetAimingDirection().z);
	}
}

void SoldierStateDefense::Update(){
	Soldier* soldier = static_cast<Soldier*>(enemy_);
	Player* player = enemy_->GetPlayer();
	TimeManager* timeManager = enemy_->GetTimeManager();

	// 入力方向によって角度をセット
	float cosTheta = atan2f(soldier->GetAimingDirection().z, soldier->GetAimingDirection().x);
	// 入力方向にセット
	if (cosTheta > 0.25f * pi && cosTheta < 0.75f * pi) {// 上
		// 座標
		soldier->GetSword()->SetTranslation({ soldier->GetAimingDirection().x * 0.6f + 2.0f, soldier->GetAimingDirection().z * 0.4f , 0.25f });

		// 角度
		soldier->GetSword()->SetRotation({ 0.0f, pi_v<float> *0.5f, pi_v<float> *0.5f });
	}else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {// 下
		// 座標
		soldier->GetSword()->SetTranslation({ soldier->GetAimingDirection().x * 0.6f + 2.0f, 0.0f , 1.5f });

		// 角度
		soldier->GetSword()->SetRotation({ 0.0f, pi_v<float> *0.5f, pi_v<float> *0.5f });
	}else {// 左右
		// 座標
		soldier->GetSword()->SetTranslation({ soldier->GetAimingDirection().x * 0.25f, soldier->GetAimingDirection().z * 0.25f , 0.0f });

		// 角度
		soldier->GetSword()->SetRotation({ 0.0f, 0.0f, 0.0f });
	}

	//プレイヤーの位置によって行動を変える
	if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		if (!timeManager->GetTimer("DefenseCoolTime" + std::to_string(enemy_->GetSerialNumber())).isStart) {
			enemy_->ChangeState(std::make_unique<SoldierStateRoot>(enemy_));
		} 
	} else if (Vector3(player->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	}
}
