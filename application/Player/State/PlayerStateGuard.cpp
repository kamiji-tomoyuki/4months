#include "PlayerStateGuard.h"
#include "Player.h"

// 初期化
void PlayerStateGuard::Initialize()
{
	player_->GetSword()->SetIsDefence(true);
	player_->GetSword()->SetTranslation({ 1.5f, 0.0f, 0.0f });
	player_->GetSword()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	player_->GetSword()->ContactRecordClear();
}

// 更新
void PlayerStateGuard::Update()
{
	// 入力処理
	// 攻撃入力がされていない時
	if (!player_->IsAttackInput()) {
		Player::Attack attack = player_->GetAttackData();
		attack.isAttack = false;
		player_->SetAttackData(attack);
	}
	// 移動処理
	player_->Move();
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