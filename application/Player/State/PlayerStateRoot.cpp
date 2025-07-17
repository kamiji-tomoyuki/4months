#include "PlayerStateRoot.h"
#include "Player.h"
#include "PlayerStateDash.h"
#include "PlayerStatePreliminary.h"
#include <Input.h>
#include "PlayerAttackStateNoInput.h"

// 初期化
void PlayerStateRoot::Initialize()
{
	player_->GetSword()->SetIsAttack(false);
	player_->GetSword()->SetTranslation({ 1.5f, 0.0f, 0.0f });
	player_->GetSword()->SetIsAttack(false);
	player_->GetSword()->SetIsDefence(false);
	player_->GetSword()->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	player_->GetSword()->SetTranslation(Vector3(1.5f, 0.0f, 0.0f));
	player_->GetSword()->ContactRecordClear();

	// プレイヤーの攻撃ステートセット
	attackState_ = std::make_unique<PlayerAttackStateNoInput>(player_);
	player_->SetAttackType(Player::AttackType::kNullType);
}

// 更新
void PlayerStateRoot::Update()
{
	// 移動処理
	player_->Move();

	// 入力処理
	// 攻撃入力がされていない時
	if (!player_->IsAttackInput()) {
		Player::Attack attack = player_->GetAttackData();
		attack.isAttack = false;
		player_->SetAttackData(attack);
	}
	// ダッシュ入力判定
	if (player_->IsDashInput()) {
		player_->ChangeState(std::make_unique<PlayerStateDash>(player_));
		return;
	}
	// 予備動作入力判定
	if (player_->IsPreliminaryInput()) {
		player_->ChangeState(std::make_unique <PlayerStatePreliminary>(player_));
		return;
	}

#ifdef _DEBUG
	// 攻撃入力がされていない時
	if (Input::GetInstance()->ReleaseKey(DIK_SPACE)) {
		Player::Attack attack = player_->GetAttackData();
		attack.isAttack = false;
		player_->SetAttackData(attack);
	}
	// ダッシュ処理
	if (Input::GetInstance()->TriggerKey(DIK_RSHIFT)) {
		player_->ChangeState(std::make_unique<PlayerStateDash>(player_));
		return;
	}
	// 予備動作(攻撃防御方向入力)
	if (Input::GetInstance()->TriggerKey(DIK_UP) ||
		Input::GetInstance()->TriggerKey(DIK_DOWN) ||
		Input::GetInstance()->TriggerKey(DIK_LEFT) ||
		Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
		player_->ChangeState(std::make_unique<PlayerStatePreliminary>(player_));
		return;
	}
#endif // _DEBUG
}
