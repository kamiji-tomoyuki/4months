#include "PlayerAttackStateRightSlash.h"
#include "Player.h"

using namespace std::numbers;

// 初期化
void PlayerAttackStateRightSlash::Initialize()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();
	Vector3 aimingDirection = player_->GetAimingDirection();

	// 座標セット
	attack.swordStartTransform = sword->GetTranslation();
	attack.swordEndTransform = { -aimingDirection.x, sword->GetTranslation().y, sword->GetTranslation().z };

	attack.time = 0.0f;

	// プレイヤーのデータセット
	player_->SetAttackData(attack);
}

// 更新
void PlayerAttackStateRightSlash::Update()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();

	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi_v<float> *(attack.time / attack.kLimitTime));

	newPos = { attack.swordStartTransform.x * cosf(theta) - attack.swordStartTransform.z * sinf(theta), attack.swordStartTransform.y, attack.swordStartTransform.x * sinf(theta) + attack.swordStartTransform.z * cosf(theta) };

	sword->SetTranslation(newPos);

	// 角度の計算
	Quaternion q1 = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, pi_v<float> *0.5f);
	Quaternion q2 = Quaternion::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, -pi_v<float>);
	Quaternion q3 = Quaternion::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, 0.0f);
	Quaternion q4 = Quaternion::Sleap(q1 * q2, q1 * q3, attack.time / attack.kLimitTime);
	
	sword->SetRotation(q4.ToEulerAngles());
}

