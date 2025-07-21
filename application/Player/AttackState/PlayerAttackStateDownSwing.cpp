#include "PlayerAttackStateDownSwing.h"
#include "Player.h"

using namespace std::numbers;

// 初期化
void PlayerAttackStateDownSwing::Initialize()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();
	Vector3 aimingDirection = player_->GetAimingDirection();

	// 座標セット
	attack.swordStartTransform = sword->GetTranslation();
	attack.swordEndTransform = { sword->GetTranslation().x + aimingDirection.x, sword->GetTranslation().y - aimingDirection.y, sword->GetTranslation().z + aimingDirection.y };
	attack.time = 0.0f;

	// プレイヤーのデータセット
	player_->SetAttackData(attack);
}

// 更新
void PlayerAttackStateDownSwing::Update()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();

	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi_v<float> *0.5f * (attack.time / attack.kLimitTime));

	newPos = { Lerp(attack.swordStartTransform.x, attack.swordEndTransform.x, attack.time / attack.kLimitTime), attack.swordStartTransform.y * cosf(theta) - attack.swordStartTransform.z * sinf(theta), attack.swordStartTransform.y * sinf(theta) + attack.swordStartTransform.z * cosf(theta) };

	sword->SetTranslation(newPos);

	// 角度の計算
	Quaternion q1 = Quaternion::MakeRotateAxisAngleQuaternion({ 1.0f, 0.0f, 0.0f }, pi_v<float> *0.6f);
	Quaternion q2 = Quaternion::Sleap(Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f }, q1, attack.time / attack.kLimitTime);
	sword->SetRotation(q2.ToEulerAngles());
}
