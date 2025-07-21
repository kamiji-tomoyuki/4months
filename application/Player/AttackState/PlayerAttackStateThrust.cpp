#include "PlayerAttackStateThrust.h"
#include <Easing.h>
#include "Player.h"

// 初期化
void PlayerAttackStateThrust::Initialize()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();
	Vector3 aimingDirection = player_->GetAimingDirection();

	attack.swordStartTransform = sword->GetTranslation();
	attack.swordEndTransform = { sword->GetTranslation().x - (aimingDirection.x * 0.2f * 6.0f), sword->GetTranslation().y, -aimingDirection.y * 0.2f * 6.0f };
	attack.time = 0.0f;

	// プレイヤーのデータセット
	player_->SetAttackData(attack);
}

// 更新
void PlayerAttackStateThrust::Update()
{
	// プレイヤーのデータ取得
	Player::Attack attack = player_->GetAttackData();
	PlayerSword* sword = player_->GetSword();

	// 座標の計算
	Vector3 newPos = EaseInOutExpo(attack.swordStartTransform, attack.swordEndTransform, attack.time, attack.kLimitTime);

	sword->SetTranslation(newPos);
}
