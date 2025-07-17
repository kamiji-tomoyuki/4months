#include "PlayerStateGuard.h"
#include "Player.h"
#include "PlayerStateRoot.h"
#include <Input.h>

using namespace std::numbers;

// 初期化
void PlayerStateGuard::Initialize()
{
	// プレイヤーのデータ取得
	WorldTransform transform = player_->GetTransform();
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();
	Player::Defence defence = player_->GetDefenceData();

	transform.UpdateMatrix();
	attack.isAttack = false;
	sword->SetIsAttack(false);
	transform.UpdateMatrix();
	defence.time = 0;
	defence.isDefence = true;
	sword->SetIsDefence(true);

	// プレイヤーのデータセット
	player_->SetTransform(transform);
	player_->SetAttackData(attack);
	player_->SetDefenceData(defence);
}

// 更新
void PlayerStateGuard::Update()
{
	// プレイヤーのデータ取得
	PlayerSword* sword = player_->GetSword();

	// 移動処理
	player_->Move();
	player_->SetAimingDirection(player_->GetAttackDirection());
	// 入力方向によって角度をセット
	float cosTheta = atan2f(player_->GetAimingDirection().y, player_->GetAimingDirection().x);
	if (player_->InputDirection() == Player::Nothing) {
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
		return;
	}

	// 上
	if (player_->InputDirection() == Player::TOP) {
		// 座標
		sword->SetTranslation({ player_->GetAimingDirection().x * 0.6f + 2.0f, player_->GetAimingDirection().y * 0.4f , 0.25f });

		// 角度
		Vector3 newRotate = { 0.0f, pi_v<float> *0.5f, pi_v<float> *0.5f };
		sword->SetRotation(newRotate);
	}
	// 下
	if (player_->InputDirection() == Player::DOWN) {
		// 座標
		sword->SetTranslation({ player_->GetAimingDirection().x * 0.6f + 2.0f, -0.50f , 1.5f });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, pi_v<float> *0.5f);
		sword->SetRotation(q.ToEulerAngles());
	}
	// 左右
	if (player_->InputDirection() == Player::LEFT || player_->InputDirection() == Player::RIGHT) {
		// 座標
		sword->SetTranslation({ player_->GetAimingDirection().x * 0.25f, player_->GetAimingDirection().y * 0.25f , 0.0f });

		// 角度
		sword->SetRotation({ 0.0f, 0.0f, 0.0f });
	}

	// 
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	// 防御解除の処理
	if (Input::GetInstance()->GetJoystickState(0, joyState) && !(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
		return;
	}
}