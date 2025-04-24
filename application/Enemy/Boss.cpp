#include "Boss.h"
#include <CollisionTypeIdDef.h>
#include "myMath.h"
#include "BossStateRoot.h"
#include "Player.h"

Boss::Boss() {

}
void Boss::Init() {
	Enemy::Init();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kBoss));
	BaseObject::CreateModel("enemy/enemyBody.obj");
	Enemy::ChangeState(std::make_unique<BossStateRoot>(this));
	Collider::SetRadius(10.0f);
	Collider::SetAABBScale({ 0.0f,0.0f,0.0f });
	Enemy::SetScale({ 10.0f,10.0f,10.0f });
	shortDistance_ = (player_->GetRadius() + GetRadius()) * 200.0f;
	middleDistance_ = (player_->GetRadius() + GetRadius()) * 400.0f;
	//imgui
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "kHp_", kHp_);

	globalVariables->AddItem(groupName, "shortDistanceProbability_.kAttack", shortDistanceProbability_.kAttack);
	globalVariables->AddItem(groupName, "shortDistanceProbability_.kDefense", shortDistanceProbability_.kDefense);
	globalVariables->AddItem(groupName, "middleDistanceProbability_.kAttack", middleDistanceProbability_.kAttack);
	globalVariables->AddItem(groupName, "middleDistanceProbability_.kDefense", middleDistanceProbability_.kDefense);
	globalVariables->AddItem(groupName, "longDistanceProbability_.kAttack", longDistanceProbability_.kAttack);
	globalVariables->AddItem(groupName, "longDistanceProbability_.kDefense", longDistanceProbability_.kDefense);

	globalVariables->AddItem(groupName, "shortDistance", shortDistance_);
	globalVariables->AddItem(groupName, "middleDistance", middleDistance_);

	globalVariables->AddItem(groupName, "CoolTime", kCoolTime_);
	//確認用
	globalVariables->AddItem(groupName, "isMove_", isMove_);
	//更新
	ApplyGlobalVariables();
	hp_ = kHp_;
}
void Boss::Update() {
	ApplyGlobalVariables();
	Enemy::VectorRotation(player_->GetCenterPosition() - GetCenterPosition());
	Enemy::Update();
	if (!isMove_) {
		return;
	}
	// キャラ移動
	state_->Update();

	velocity_ *= 1.0f - kAttenuation_;

	transform_.translation_ += velocity_ * timeManager_->deltaTime_;

	transform_.translation_.y = GetRadius();
	Enemy::Update();
}
void Boss::Draw(const ViewProjection& viewProjection) {
	Enemy::Draw(viewProjection);
}
void Boss::OnCollision(Collider* other) {
	Enemy::OnCollision(other);
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
	}
}
void Boss::OnCollisionEnter(Collider* other) {
	Enemy::OnCollisionEnter(other);
}
void Boss::OnCollisionOut(Collider* other) {
	Enemy::OnCollisionOut(other);
}

void Boss::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	kHp_ = globalVariables->GetIntValue(groupName, "kHp_");

	shortDistanceProbability_.kAttack = globalVariables->GetFloatValue(groupName, "shortDistanceProbability_.kAttack");
	shortDistanceProbability_.kDefense = globalVariables->GetFloatValue(groupName, "shortDistanceProbability_.kDefense");
	middleDistanceProbability_.kAttack = globalVariables->GetFloatValue(groupName, "middleDistanceProbability_.kAttack");
	middleDistanceProbability_.kDefense = globalVariables->GetFloatValue(groupName, "middleDistanceProbability_.kDefense");
	longDistanceProbability_.kAttack = globalVariables->GetFloatValue(groupName, "longDistanceProbability_.kAttack");
	longDistanceProbability_.kDefense = globalVariables->GetFloatValue(groupName, "longDistanceProbability_.kDefense");

	shortDistance_ = globalVariables->GetFloatValue(groupName, "shortDistance");
	middleDistance_ = globalVariables->GetFloatValue(groupName, "middleDistance");

	kCoolTime_ = globalVariables->GetFloatValue(groupName, "CoolTime");
	isMove_ = globalVariables->GetBoolValue(groupName, "isMove_");
}

Vector3 Boss::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 Boss::GetCenterRotation() const {
	return transform_.rotation_;
}