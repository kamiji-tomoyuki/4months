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

	Vector3 newRotate = { pi_v<float> *0.5f, 0.0f,0.0f};
	quaternion_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), pi_v<float> *0.5f);
	quaternion_ = quaternion_ * Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), pi_v<float> *0.5f);

	newRotate = quaternion_.ToEulerAngles();
	boss_->GetSword()->SetRotation(newRotate);
	boss_->GetSword()->SetIsAttack(true);
}

void BossStateAttack::Update(){
	// 座標の計算
	float theta = pi_v<float> * 0.05f;

	Vector3 newPos = boss_->GetSword()->GetTranslation();
	Vector3 newRotate = boss_->GetSword()->GetRotate();
	quaternion_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), theta) * quaternion_;
	newRotate = quaternion_.ToEulerAngles();
	boss_->GetSword()->SetRotation(newRotate);
	// 回転
	newPos = Transformation(newPos, MakeRotateYMatrix(theta));
	boss_->GetSword()->SetTranslation(newPos);

	////プレイヤーの位置によって行動を変える
	//if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
	//	enemy_->ChangeState(std::make_unique<BossStateRoot>(enemy_));
	//} else if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
	//	//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	//} else {
	//	//enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	//}
}
