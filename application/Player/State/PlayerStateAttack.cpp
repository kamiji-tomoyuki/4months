#include "PlayerStateAttack.h"
#include "Player.h"
#include "PlayerStateRoot.h"
#include "PlayerAttackStateNoInput.h"
#include "PlayerAttackStateDownSwing.h"
#include "PlayerAttackStateThrust.h"
#include "PlayerAttackStateLeftSlash.h"
#include "PlayerAttackStateRightSlash.h"


// 初期化
void PlayerStateAttack::Initialize()
{
	// プレイヤーのデータ取得
	WorldTransform transform = player_->GetTransform();
	Player::Attack attack = player_->GetAttackData();

	transform.UpdateMatrix();
	attack.time = 0;
	attack.swordStartTransform = player_->GetSword()->GetTranslation().z;
	
	// プレイヤーのデータセット
	player_->SetTransform(transform);
	player_->SetAttackData(attack);
	player_->GetSword()->SetIsAttack(true);

	// プレイヤーの攻撃ステートセット
	if( player_->GetAttackType() == Player::AttackType::kDownSwing) {
		attackState_ = std::make_unique<PlayerAttackStateDownSwing>(player_);
	} else if (player_->GetAttackType() == Player::AttackType::kThrust) {
		attackState_ = std::make_unique<PlayerAttackStateThrust>(player_);
	} else if (player_->GetAttackType() == Player::AttackType::kRightSlash) {
		attackState_ = std::make_unique<PlayerAttackStateRightSlash>(player_);
	} else if (player_->GetAttackType() == Player::AttackType::kLeftSlash) {
		attackState_ = std::make_unique<PlayerAttackStateLeftSlash>(player_);
	} else {
		attackState_ = std::make_unique<PlayerAttackStateNoInput>(player_);
	}
}

// 更新
void PlayerStateAttack::Update()
{
	// 開始角度
	float speed = player_->GetAcceleration() * 2.0f;
	Vector3 move{};
	Player::Attack attack_ = player_->GetAttackData();
	TimeManager* timeManager = player_->GetTimeManager();
	attack_.time += timeManager->deltaTime_;

	if (attack_.time / attack_.kLimitTime > 1.0f) {
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
		return;
	}

	attackState_->Update();
	player_->SetAttackData(attack_);
}