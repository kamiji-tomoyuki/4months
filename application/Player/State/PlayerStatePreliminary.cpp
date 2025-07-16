#include "PlayerStatePreliminary.h"
#include "Player.h"
#include "PlayerStateRoot.h"

// 初期化
void PlayerStatePreliminary::Initialize()
{
	WorldTransform transform = player_->GetTransform();
	transform.UpdateMatrix();
	player_->SetTransform(transform);
	Player::Attack attack = player_->GetAttackData();
	attack.time = 0;
	attack.swordStartTransform = player_->GetSword()->GetTranslation().z;
	player_->SetAttackData(attack);
	player_->GetSword()->SetIsAttack(false);
}

// 更新
void PlayerStatePreliminary::Update()
{
	// 移動処理
	player_->Move();
	// 方向取得
	player_->SetInputDirection();
	
	if (player_->InputDirection() == Player::Nothing) {
		aimingDirection_ = { 0.0f, 0.0f, 0.0f };
		behaviorRequest_ = Behavior::kRoot;
	}

	// 上
	if (InputDirection() == TOP) {
		// 座標
		sword_->SetTranslation({ attackDirection_.x, attackDirection_.y , 0.0f });

		// 角度
		sword_->SetRotation({ 0.0f, 0.0f, 0.0f });

		attackTypeRequest_ = AttackType::kDownSwing;
	}
	// 下
	if (InputDirection() == DOWN) {
		// 座標
		sword_->SetTranslation({ 1.5f, 0.0f , -1.5f });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, 0.5f * pi_v<float>);
		sword_->SetRotation(q.ToEulerAngles());

		attackTypeRequest_ = AttackType::kThrust;
	}
	// 左
	if (InputDirection() == LEFT) {
		// 座標
		sword_->SetTranslation({ attackDirection_.x, 0.0f , attackDirection_.y });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, 0.5f * pi_v<float>);
		sword_->SetRotation(q.ToEulerAngles());

		attackTypeRequest_ = AttackType::kRightSlash;
	}
	// 右
	if (InputDirection() == RIGHT) {
		// 座標
		sword_->SetTranslation({ attackDirection_.x, 0.0f , attackDirection_.y });

		// 角度
		Quaternion q = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, -0.5f * pi_v<float>);
		sword_->SetRotation(q.ToEulerAngles());

		attackTypeRequest_ = AttackType::kLeftSlash;
	}

	// ゲームパッド入力処理
	XINPUT_STATE joyState;
	// 攻撃入力がされていない時
	if (Input::GetInstance()->GetJoystickState(0, joyState) && !(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		attack_.isAttack = false;
	}
	// 攻撃の処理
	if ((Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
		!attack_.isAttack) {
		behaviorRequest_ = Behavior::kAttack;
		aimingDirection_ = attackDirection_;
	}
	// 防御の処理
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		behaviorRequest_ = Behavior::kProtection;
	}
}