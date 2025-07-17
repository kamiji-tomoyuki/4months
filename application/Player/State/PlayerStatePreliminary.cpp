#include "PlayerStatePreliminary.h"
#include <Input.h>
#include "Player.h"
#include "PlayerStateRoot.h"
#include "PlayerStateAttack.h"
#include "PlayerStateGuard.h"
#include "PlayerAttackStateNoInput.h"
#include "PlayerAttackStateDownSwing.h"
#include "PlayerAttackStateThrust.h"
#include "PlayerAttackStateLeftSlash.h"
#include "PlayerAttackStateRightSlash.h"

using namespace std::numbers;

// 初期化
void PlayerStatePreliminary::Initialize()
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
	player_->GetSword()->SetIsAttack(false);

	// プレイヤーの攻撃ステートセット
	attackState_ = std::make_unique<PlayerAttackStateNoInput>(player_);
}

// 更新
void PlayerStatePreliminary::Update()
{
	// 移動処理
	player_->Move();
	// 方向取得
	player_->SetInputDirection();
	
	if (player_->InputDirection() == Player::Nothing) {
		player_->SetAimingDirection({ 0.0f, 0.0f, 0.0f });
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
	}

	// プレイヤーのデータ取得
	PlayerSword* sword = player_->GetSword();
	Vector3 attackDirection = player_->GetAimingDirection();
	// 上
	if (player_->InputDirection() == Player::TOP) {
		// 座標
		sword->SetTranslation({ attackDirection.x, attackDirection.y , 0.0f });

		// 角度
		sword->SetRotation({ 0.0f, 0.0f, 0.0f });

		// プレイヤーの攻撃ステートセット
		ChangeAttackState(std::make_unique<PlayerAttackStateDownSwing>(player_));
		player_->SetAttackType(Player::AttackType::kDownSwing);
	}
	// 下
	if (player_->InputDirection() == Player::DOWN) {
		// 座標
		sword->SetTranslation({ 1.5f, 0.0f , -1.5f });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, 0.5f * pi_v<float>);
		sword->SetRotation(q.ToEulerAngles());

		// プレイヤーの攻撃ステートセット
		ChangeAttackState(std::make_unique<PlayerAttackStateThrust>(player_));
		player_->SetAttackType(Player::AttackType::kThrust);
	}
	// 左
	if (player_->InputDirection() == Player::LEFT) {
		// 座標
		sword->SetTranslation({ attackDirection.x, 0.0f , attackDirection.y });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, 0.5f * pi_v<float>);
		sword->SetRotation(q.ToEulerAngles());

		// プレイヤーの攻撃ステートセット
		ChangeAttackState(std::make_unique<PlayerAttackStateRightSlash>(player_));
		player_->SetAttackType(Player::AttackType::kRightSlash);
	}
	// 右
	if (player_->InputDirection() == Player::RIGHT) {
		// 座標
		sword->SetTranslation({ attackDirection.x, 0.0f , attackDirection.y });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, -0.5f * pi_v<float>);
		sword->SetRotation(q.ToEulerAngles());

		// プレイヤーの攻撃ステートセット
		ChangeAttackState(std::make_unique<PlayerAttackStateLeftSlash>(player_));
		player_->SetAttackType(Player::AttackType::kLeftSlash);
	}

	// ゲームパッド入力処理
	XINPUT_STATE joyState;

	// プレイヤーの攻撃データ取得
	Player::Attack attack = player_->GetAttackData();
	Vector3 aimingDirection = player_->GetAimingDirection();

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 攻撃入力がされていない時
		if (!(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			attack.isAttack = false;
			player_->SetAttackData(attack);
			player_->SetAttackType(Player::AttackType::kNullType);
		}
		// 攻撃の処理
		else if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
			!attack.isAttack) {
			aimingDirection = player_->GetAttackDirection();
			player_->SetAimingDirection(aimingDirection);
			player_->ChangeState(std::make_unique<PlayerStateAttack>(player_));
		}
		// 防御の処理
		else if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
			player_->ChangeState(std::make_unique<PlayerStateGuard>(player_));
			// プレイヤーの攻撃ステートセット
			attackState_ = std::make_unique<PlayerAttackStateNoInput>(player_);
			player_->SetAttackType(Player::AttackType::kNullType);
		}
	}
}