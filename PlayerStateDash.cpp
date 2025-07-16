#include "PlayerStateDash.h"
#include "Player.h"

// 初期化
void PlayerStateDash::Initialize()
{
	player_->GetSword()->SetIsAttack(false);
	player_->GetSword()->SetIsDefence(false);
	player_->GetSword()->SetTranslation({ 1.5f, 0.0f, 0.0f });
	player_->GetSword()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	player_->GetSword()->ContactRecordClear();
}

// 更新
void PlayerStateDash::Update()
{
	// 入力処理
	// 攻撃入力がされていない時
	if (!player_->IsAttackInput()) {
		Player::Attack attack = player_->GetAttackData();
		attack.isAttack = false;
		player_->SetAttackData(attack);
	}
	// ダッシュ動作の更新
	player_->BehaviorDashUpdate();
	// 方向取得
	player_->SetInputDirection();
	if (player_->InputDirection() == Player::Nothing) {
		player_->SetBehavior(Player::Behavior::kRoot);
		return;
	}
	// 入力方向によって角度をセット
	float cosTheta = atan2f(player_->GetAttackDirection().y, player_->GetAttackDirection().x);
	player_->GetSword()->SetRotation({ 0.0f, cosTheta, 0.0f });
}