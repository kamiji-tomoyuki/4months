#include "BossStateApproach.h"
#include "Player.h"
#include "TimeManager.h"
#include "BossStateRoot.h"
using namespace std::numbers;

BossStateApproach::BossStateApproach(Enemy* boss)
	: BaseEnemyState("State Attack", boss) {}

void BossStateApproach::Initialize(){
	boss_ = static_cast<Boss*>(enemy_);
	player_ = enemy_->GetPlayer();
	timeManager_ = enemy_->GetTimeManager();

}

void BossStateApproach::Update(){
	Vector3 velocity_ = player_->GetCenterPosition() - enemy_->GetCenterPosition();
	velocity_ = velocity_.Normalize();
	enemy_->SetTranslation(enemy_->GetCenterPosition() + velocity_);

	//プレイヤーの位置によって行動を変える
	if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		enemy_->ChangeState(std::make_unique<BossStateRoot>(enemy_));
	} else if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {

	} else {

	}
}
