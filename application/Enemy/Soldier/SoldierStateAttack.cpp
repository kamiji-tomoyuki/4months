#include "SoldierStateAttack.h"
#include "Player.h"
#include "TimeManager.h"
#include "SoldierStateApproach.h"
#include "SoldierStateRoot.h"
#include <Easing.h>
using namespace std::numbers;

SoldierStateAttack::SoldierStateAttack(Enemy* soldier)
	: BaseEnemyState("State Attack", soldier){}

void SoldierStateAttack::Initialize(){
	soldier_ = static_cast<Soldier*>(enemy_);
	player_ = enemy_->GetPlayer();
	timeManager_ = enemy_->GetTimeManager();
	float kAttack = soldier_->GetCoolTime() + soldier_->GetAttack().kLimitTime;
	timeManager_->SetTimer("AttackCoolTime" + std::to_string(enemy_->GetSerialNumber()), kAttack);
	// 方向取得
	soldier_->DirectionPreliminaryAction();
}

void SoldierStateAttack::Update(){
	attack_ = soldier_->GetAttack();

	attack_.time += timeManager_->deltaTime_;


	if (attackTypeRequest_) {
		//振るまいを変更する
		attackType_ = attackTypeRequest_.value();

		attack_.time = 0;
		attack_.isAttack = true;
		attack_.swordStartTransform = soldier_->GetSword()->GetTranslation().z;
		soldier_->GetSword()->SetIsAttack(true);

		(this->*AttackTypeInitFuncTable[static_cast<size_t>(attackType_)])();

		attackTypeRequest_ = std::nullopt;
	}

	(this->*AttackTypeUpdateFuncTable[static_cast<size_t>(attackType_)])();

	if (attack_.time / attack_.kLimitTime >= 1.0f) {
		attack_.time = attack_.kLimitTime;
		soldier_->GetSword()->SetTranslation({ 1.7f, 0.0f, 1.3f });
		soldier_->GetSword()->SetRotation({ 0.0f, 0.0f, 0.0f });
	}
	soldier_->SetAttack(attack_);

	//プレイヤーの位置によって行動を変える
	if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetShortDistance()) {
		if (!timeManager_->GetTimer("AttackCoolTime" + std::to_string(enemy_->GetSerialNumber())).isStart) {
			attackTypeRequest_ = AttackType::kNullType;
			enemy_->ChangeState(std::make_unique<SoldierStateRoot>(enemy_));
		} 
	} else if (Vector3(player_->GetCenterPosition() - enemy_->GetCenterPosition()).Length() < enemy_->GetMiddleDistance()) {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	} else {
		enemy_->ChangeState(std::make_unique<SoldierStateApproach>(enemy_));
	}
}
// 振り下ろし(上入力攻撃)の初期化
void SoldierStateAttack::AttackTypeDownSwingInitialize() {
	// 座標セット
	attack_.swordStartTransform = soldier_->GetSword()->GetTranslation();
	attack_.swordEndTransform = { soldier_->GetSword()->GetTranslation().x + soldier_->GetAimingDirection().x, soldier_->GetSword()->GetTranslation().y - soldier_->GetAimingDirection().z, soldier_->GetSword()->GetTranslation().z + soldier_->GetAimingDirection().z };

	// 角度セット
	attack_.swordStartRotate = soldier_->GetSword()->GetRotate();
	attack_.swordEndRotate = { 0.6f * pi_v<float>, soldier_->GetSword()->GetRotate().y, soldier_->GetSword()->GetRotate().z };

	attack_.time = 0.0f;
	soldier_->SetAttack(attack_);
	soldier_->GetSword()->SetIsAttack(true);
}

// 振り下ろし(上入力攻撃)の更新
void SoldierStateAttack::AttackTypeDownSwingUpdate() {
	if (timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(soldier_->GetSerialNumber())).isStart) {
		attack_.time = 0.0f;
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * 0.5f * (attack_.time / attack_.kLimitTime));

	newPos = { Lerp(attack_.swordStartTransform.x, attack_.swordEndTransform.x, attack_.time / attack_.kLimitTime), attack_.swordStartTransform.y * cosf(theta) - attack_.swordStartTransform.z * sinf(theta), attack_.swordStartTransform.y * sinf(theta) + attack_.swordStartTransform.z * cosf(theta) };

	soldier_->GetSword()->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;
	theta = float(pi * 0.5f * (attack_.time / attack_.kLimitTime));

	newRotate = { Lerp(attack_.swordStartRotate.x, attack_.swordEndRotate.x, attack_.time / attack_.kLimitTime), attack_.swordEndRotate.y, attack_.swordEndRotate.z };

	soldier_->GetSword()->SetRotation(newRotate);
}

// 突き(下入力攻撃)の初期化
void SoldierStateAttack::AttackTypeThrustInitialize() {
	attack_.swordStartTransform = soldier_->GetSword()->GetTranslation();
	attack_.swordEndTransform = { soldier_->GetSword()->GetTranslation().x - (soldier_->GetAimingDirection().x * 0.2f * 6.0f), soldier_->GetSword()->GetTranslation().y, -soldier_->GetAimingDirection().z * 0.2f * 6.0f };
	attack_.time = 0.0f;
	soldier_->GetSword()->SetIsAttack(true);
}

// 突き(下入力攻撃)の更新
void SoldierStateAttack::AttackTypeThrustUpdate() {
	if (timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(soldier_->GetSerialNumber())).isStart) {
		attack_.time = 0.0f;
		return;
	}
	Vector3 newPos = EaseInOutExpo(attack_.swordStartTransform, attack_.swordEndTransform, attack_.time, attack_.kLimitTime);

	soldier_->GetSword()->SetTranslation(newPos);
}

// 右振り抜き(左入力攻撃)の初期化
void SoldierStateAttack::AttackTypeLeftSwingInitialize() {
	// 座標セット
	attack_.swordStartTransform = soldier_->GetSword()->GetTranslation();
	attack_.swordEndTransform = { -soldier_->GetAimingDirection().x, soldier_->GetSword()->GetTranslation().y, soldier_->GetSword()->GetTranslation().z };

	// 角度セット
	attack_.swordStartRotate = soldier_->GetSword()->GetRotate();
	attack_.swordEndRotate = { soldier_->GetSword()->GetRotate().x - pi_v<float>, soldier_->GetSword()->GetRotate().y, soldier_->GetSword()->GetRotate().z };

	attack_.time = 0.0f;
	soldier_->GetSword()->SetIsAttack(true);
}

// 右振り抜き(左入力攻撃)の更新
void SoldierStateAttack::AttackTypeLeftSwingUpdate() {
	if (timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(soldier_->GetSerialNumber())).isStart) {
		attack_.time = 0.0f;
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.swordStartTransform.x * cosf(-theta) - attack_.swordStartTransform.z * sinf(-theta), attack_.swordStartTransform.y, attack_.swordStartTransform.x * sinf(-theta) + attack_.swordStartTransform.z * cosf(-theta) };

	soldier_->GetSword()->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;

	newRotate = { attack_.swordStartRotate.x + theta, attack_.swordStartRotate.y, attack_.swordStartRotate.z };

	soldier_->GetSword()->SetRotation(newRotate);
}

// 左振り抜き(右入力攻撃)の初期化
void SoldierStateAttack::AttackTypeRightSwingInitialize() {
	// 座標セット
	attack_.swordStartTransform = soldier_->GetSword()->GetTranslation();
	attack_.swordEndTransform = { -soldier_->GetAimingDirection().x, soldier_->GetSword()->GetTranslation().y, soldier_->GetSword()->GetTranslation().z };

	// 角度セット
	attack_.swordStartRotate = soldier_->GetSword()->GetRotate();
	attack_.swordEndRotate = { soldier_->GetSword()->GetRotate().x + pi_v<float>, soldier_->GetSword()->GetRotate().y, soldier_->GetSword()->GetRotate().z };

	attack_.time = 0.0f;
	soldier_->GetSword()->SetIsAttack(true);
}

// 左振り抜き(右入力攻撃)の更新
void SoldierStateAttack::AttackTypeRightSwingUpdate() {
	if (timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(soldier_->GetSerialNumber())).isStart) {
		attack_.time = 0.0f;
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.swordStartTransform.x * cosf(theta) - attack_.swordStartTransform.z * sinf(theta), attack_.swordStartTransform.y, attack_.swordStartTransform.x * sinf(theta) + attack_.swordStartTransform.z * cosf(theta) };

	soldier_->GetSword()->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;

	newRotate = { attack_.swordStartRotate.x - theta, attack_.swordStartRotate.y, attack_.swordStartRotate.z };

	soldier_->GetSword()->SetRotation(newRotate);
}

// 未入力
void SoldierStateAttack::AttackTypeNullInitialize() {
	attack_.time = 0;
	attack_.isAttack = false;
	attack_.swordStartTransform = soldier_->GetSword()->GetTranslation().z;
	soldier_->GetSword()->SetIsAttack(false);
}
void SoldierStateAttack::AttackTypeNullUpdate() {
	// 武器を入力方向に移動
	float cosTheta = atan2f(soldier_->GetAimingDirection().z, soldier_->GetAimingDirection().x);
	// 上
	if (cosTheta > 0.25f * pi && cosTheta < 0.75f * pi) {
		// 座標
		soldier_->GetSword()->SetTranslation({ soldier_->GetAimingDirection().x, soldier_->GetAimingDirection().z , 0.0f });

		// 角度
		soldier_->GetSword()->SetRotation({ 0.0f, 0.5f * pi_v<float> -cosTheta, 0.0f });

		attackTypeRequest_ = AttackType::kDownSwing;
	}
	// 下
	else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {
		// 座標
		soldier_->GetSword()->SetTranslation({ 1.5f, 0.0f , -1.5f });

		// 角度
		soldier_->GetSword()->SetRotation({ pi_v<float> *0.5f, -(0.5f * pi_v<float> +cosTheta), 0.0f });

		attackTypeRequest_ = AttackType::kThrust;
	}
	// 左
	else if (cosTheta >= 0.75f * pi || cosTheta <= -0.75f * pi) {
		// 座標
		soldier_->GetSword()->SetTranslation({ soldier_->GetAimingDirection().x, 0.0f , soldier_->GetAimingDirection().z });

		// 角度
		soldier_->GetSword()->SetRotation({ cosTheta >= 0.75f * pi ? pi_v<float> *1.0f - cosTheta : pi_v<float> *1.0f - cosTheta, 0.0f, pi_v<float> *0.5f });

		attackTypeRequest_ = AttackType::kRightSlash;
	}
	// 右
	else {
		// 座標
		soldier_->GetSword()->SetTranslation({ soldier_->GetAimingDirection().x, 0.0f , soldier_->GetAimingDirection().z });

		// 角度
		soldier_->GetSword()->SetRotation({ cosTheta >= 0.0f ? pi_v<float> *1.0f - cosTheta : pi_v < float> *1.0f + -cosTheta, 0.0f, pi_v<float> *0.5f });

		attackTypeRequest_ = AttackType::kLeftSlash;
	}
	timeManager_->SetTimer("PostureAttackCoolTime" + std::to_string(soldier_->GetSerialNumber()), 1.0f);
}

void(SoldierStateAttack::*SoldierStateAttack::AttackTypeInitFuncTable[])() = {
	&SoldierStateAttack::AttackTypeDownSwingInitialize,
	&SoldierStateAttack::AttackTypeThrustInitialize,
	&SoldierStateAttack::AttackTypeLeftSwingInitialize,
	&SoldierStateAttack::AttackTypeRightSwingInitialize,
	&SoldierStateAttack::AttackTypeNullInitialize,
};
void(SoldierStateAttack::*SoldierStateAttack::AttackTypeUpdateFuncTable[])() = {
	&SoldierStateAttack::AttackTypeDownSwingUpdate,
	&SoldierStateAttack::AttackTypeThrustUpdate,
	&SoldierStateAttack::AttackTypeLeftSwingUpdate,
	&SoldierStateAttack::AttackTypeRightSwingUpdate,
	&SoldierStateAttack::AttackTypeNullUpdate,
};

