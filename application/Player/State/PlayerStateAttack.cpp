#include "PlayerStateAttack.h"
#include "Player.h"

// 初期化
void PlayerStateAttack::Initialize()
{
	WorldTransform transform = player_->GetTransform();
	transform.UpdateMatrix();
	player_->SetTransform(transform);
	
	Player::Attack attack = player_->GetAttackData();
	attack.time = 0;
	attack.swordStartTransform = player_->GetSword()->GetTranslation().z;
	player_->SetAttackData(attack);
	
	player_->GetSword()->SetIsAttack(true);
}

// 更新
void PlayerStateAttack::Update()
{
	// 攻撃動作の更新
	player_->BehaviorAttackUpdate();
	
	// 方向取得
	player_->SetInputDirection();
	player_->SetAimingDirection(player_->GetAttackDirection());
	
	if (player_->InputDirection() == Player::Nothing) {
		player_->SetBehavior(Player::Behavior::kRoot);
		return;
	}
	
	// 入力方向によって角度をセット
	float cosTheta = atan2f(player_->GetAimingDirection().y, player_->GetAimingDirection().x);
	player_->GetSword()->SetRotation({ 0.0f, cosTheta, 0.0f });
}