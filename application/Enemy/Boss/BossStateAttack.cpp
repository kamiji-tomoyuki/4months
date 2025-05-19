#include "BossStateAttack.h"
#include "Player.h"
#include "TimeManager.h"
#include "BossStateRoot.h"
using namespace std::numbers;

BossStateAttack::BossStateAttack(Enemy* boss)
	: BaseEnemyState("State Attack", boss) {}

void BossStateAttack::Initialize(){
	boss_ = static_cast<Boss*>(enemy_);
	player_ = enemy_->GetPlayer();
	timeManager_ = enemy_->GetTimeManager();

	Quaternion quaternion = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), pi_v<float> *0.5f);
	quaternion = quaternion * Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), pi_v<float> *0.5f);
	
	boss_->GetSword()->SetQuaternion(quaternion);
	
	Vector3 newRotate = quaternion.ToEulerAngles();
	boss_->GetSword()->SetRotation(newRotate);
	boss_->GetSword()->SetIsAttack(true);
	timeManager_->SetTimer("AttackCoolTime" + std::to_string(enemy_->GetSerialNumber()), enemy_->GetCoolTime());
}

void BossStateAttack::Update(){
	Quaternion quaternion = boss_->GetSword()->GetQuaternion();
	
	float theta = pi_v<float> * 0.05f * boss_->GetAimingDirection().y;

	Vector3 newPos = boss_->GetSword()->GetTranslation();
	Vector3 newRotate = boss_->GetSword()->GetRotate();
	quaternion = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), theta) * quaternion;
	
	boss_->GetSword()->SetQuaternion(quaternion);
	
	// 回転
	newRotate = quaternion.ToEulerAngles();
	boss_->GetSword()->SetRotation(newRotate);
	// 座標の計算
	newPos = Transformation(newPos, MakeRotateYMatrix(theta));
	boss_->GetSword()->SetTranslation(newPos);
	
	if (!timeManager_->GetTimer("AttackCoolTime" + std::to_string(enemy_->GetSerialNumber())).isStart) {
		timeManager_->SetTimer("AttackCoolTime" + std::to_string(enemy_->GetSerialNumber()), enemy_->GetCoolTime());
		boss_->GetSword()->SetIsAttack(true);
	}
	////プレイヤーの位置によって行動を変える
	//if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
	//	enemy_->ChangeState(std::make_unique<BossStateRoot>(enemy_));
	//} else if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
	//	//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	//} else {
	//	//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	//}
}
