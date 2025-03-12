#include "Soldier.h"
#include <CollisionTypeIdDef.h>
#include "myMath.h"
#include "EnemyStateRoot.h"
#include "Player.h"
#include "TimeManager.h"
#include <Easing.h>
using namespace std::numbers;

Soldier::Soldier(){

}
void Soldier::Init(){
	Enemy::Init();
	Enemy::ChangeState(std::make_unique<EnemyStateRoot>(this));
	Collider::SetRadius(3.0f);
	Collider::SetAABBScale({ 0.0f,0.0f,0.0f });
	Enemy::SetScale({ 1.0f,1.0f,1.0f });
	Enemy::SetScale({ GetRadius(),GetRadius(),GetRadius() });
	shortDistance_ = (player_->GetRadius() + GetRadius()) * 2.0f;
	middleDistance_ = (player_->GetRadius() + GetRadius()) * 4.0f;

	//エネミーの剣
	sword_ = std::make_unique<EnemySword>();
	sword_->SetEnemy(this);
	sword_->SetTimeManager(timeManager_);

	BaseObject::CreateModel("player/playerBody.obj");
	sword_->Initialize("player/playerArm.gltf", "player/playerPalm.obj");
	sword_->SetTranslation(Vector3(1.7f, 0.0f, 1.3f));

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
	ApplyGlobalVariables();

	hp_ = kHp_;
}
void Soldier::Update(){
	ApplyGlobalVariables();
	Enemy::VectorRotation(player_->GetCenterPosition() - GetCenterPosition());
	Enemy::Update();
	if (aimingDirection_.x == 0 && aimingDirection_.z == 0) {
		sword_->SetTranslation({ 1.7f, 0.0f, 1.3f });
	}
	if (!isMove_) {
		return;
	}
	// キャラ移動
	state_->Update();
	
	velocity_ *= 1.0f - kAttenuation_;

	transform_.translation_ += velocity_ * timeManager_->deltaTime_;

	transform_.translation_.y = GetRadius();

	Enemy::Update();
	sword_->Update();
}
void Soldier::Draw(const ViewProjection& viewProjection){
	Enemy::Draw(viewProjection);
	sword_->Draw(viewProjection);
}
void Soldier::DrawAnimation(const ViewProjection& viewProjection){
	sword_->DrawAnimation(viewProjection);
}
void Soldier::OnCollision(Collider* other){
	Enemy::OnCollision(other);
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

	}
}
void Soldier::OnCollisionEnter(Collider* other){
	Enemy::OnCollisionEnter(other);
}
void Soldier::OnCollisionOut(Collider* other){
	Enemy::OnCollisionOut(other);
}

void Soldier::AttackInitialize(){
	Enemy::AttackInitialize();
	// 方向取得
	DirectionPreliminaryAction();
}

void Soldier::AttackUpdate(){
	Enemy::AttackUpdate();
	attack_.time += timeManager_->deltaTime_;
	if (attack_.time / attack_.kLimitTime >= 1.0f) {
		attack_.time = attack_.kLimitTime;
	}
	if (attackTypeRequest_) {

		//振るまいを変更する
		attackType_ = attackTypeRequest_.value();

		attack_.time = 0;
		attack_.isAttack = true;
		attack_.armStart = sword_->GetTranslation().z;
		sword_->SetIsAttack(true);

		(this->*AttackTypeInitFuncTable[static_cast<size_t>(attackType_)])();

		attackTypeRequest_ = std::nullopt;
	}

	(this->*AttackTypeUpdateFuncTable[static_cast<size_t>(attackType_)])();
}

void Soldier::ProtectionInitialize(){
	Enemy::ProtectionInitialize();

}

void Soldier::ProtectionUpdate(){
	Enemy::ProtectionUpdate();

}

void Soldier::ApplyGlobalVariables() {
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
// 予備動作の方向
void Soldier::DirectionPreliminaryAction(){
	// 方向をセット
	if (GetProbabilities(0.25f)){
		aimingDirection_ = { 1.0f, 0.0f, 0.0f };
	} else if (GetProbabilities(0.25f)) {
		aimingDirection_ = { -1.0f, 0.0f, 0.0f };
	} else if (GetProbabilities(0.25f)) {
		aimingDirection_ = { 0.0f, 0.0f, 1.0f };
	} else {
		aimingDirection_ = { 0.0f, 0.0f, -1.0f };
	}

	aimingDirection_ *= 5.0f;
}

// 振り下ろし(上入力攻撃)の初期化
void Soldier::AttackTypeDownSwingInitialize(){
	attack_.armStart = sword_->GetTranslation();
	attack_.armEnd = { sword_->GetTranslation().x + aimingDirection_.x, sword_->GetTranslation().y - aimingDirection_.z, sword_->GetTranslation().z + aimingDirection_.z };
	attack_.time = 0.0f;
}

// 振り下ろし(上入力攻撃)の更新
void Soldier::AttackTypeDownSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime").isStart) {
		return;
	}
	//Vector3 newPos = EaseInSine(attack_.armStart, attack_.armEnd, attack_.time, attack_.kLimitTime);
	Vector3 newPos = 0.0f;
	float theta = float(pi / 2.0f * (attack_.time / attack_.kLimitTime));

	newPos = { Lerp(attack_.armStart.x, attack_.armEnd.x, attack_.time / attack_.kLimitTime), attack_.armStart.y * cosf(theta) - attack_.armStart.z * sinf(theta), attack_.armStart.y * sinf(theta) + attack_.armStart.z * cosf(theta) };

	sword_->SetTranslation(newPos);
}

// 突き(下入力攻撃)の初期化
void Soldier::AttackTypeThrustInitialize(){
	attack_.armStart = sword_->GetTranslation();
	attack_.armEnd = { sword_->GetTranslation().x - aimingDirection_.x, sword_->GetTranslation().y, -aimingDirection_.z };
	attack_.time = 0.0f;
}

// 突き(下入力攻撃)の更新
void Soldier::AttackTypeThrustUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime").isStart) {
		return;
	}
	Vector3 newPos = EaseInExpo(attack_.armStart, attack_.armEnd, attack_.time, attack_.kLimitTime);

	sword_->SetTranslation(newPos);
}

// 右振り抜き(左入力攻撃)の初期化
void Soldier::AttackTypeLeftSwingInitialize(){
	attack_.armStart = sword_->GetTranslation();
	attack_.armEnd = { -aimingDirection_.x, sword_->GetTranslation().y, sword_->GetTranslation().z };
	attack_.time = 0.0f;
}

// 右振り抜き(左入力攻撃)の更新
void Soldier::AttackTypeLeftSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime").isStart) {
		return;
	}
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.armStart.x * cosf(-theta) - attack_.armStart.z * sinf(-theta), attack_.armStart.y, attack_.armStart.x * sinf(-theta) + attack_.armStart.z * cosf(-theta) };

	sword_->SetTranslation(newPos);
}

// 左振り抜き(右入力攻撃)の初期化
void Soldier::AttackTypeRightSwingInitialize(){
	attack_.armStart = sword_->GetTranslation();
	attack_.armEnd = { -aimingDirection_.x, sword_->GetTranslation().y, sword_->GetTranslation().z };
	attack_.time = 0.0f;
}

// 左振り抜き(右入力攻撃)の更新
void Soldier::AttackTypeRightSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime").isStart) {
		return;
	}
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.armStart.x * cosf(theta) - attack_.armStart.z * sinf(theta), attack_.armStart.y, attack_.armStart.x * sinf(theta) + attack_.armStart.z * cosf(theta) };

	sword_->SetTranslation(newPos);
}

// 未入力
void Soldier::AttackTypeNullInitialize(){
	attack_.time = 0;
	attack_.isAttack = false;
	attack_.isLeft = !attack_.isLeft;
	attack_.armStart = sword_->GetTranslation().z;
	sword_->SetIsAttack(false);
}
void Soldier::AttackTypeNullUpdate(){
	// 武器を入力方向に移動
	float cosTheta = atan2f(aimingDirection_.z, aimingDirection_.x);
	// 上
	if (cosTheta > 0.25f * pi && cosTheta < 0.75f * pi) {
		sword_->SetTranslationX(aimingDirection_.x);
		sword_->SetTranslationY(aimingDirection_.z);
		sword_->SetTranslationZ(0.0f);
		attackTypeRequest_ = AttackType::kDownSwing;
		timeManager_->SetTimer("PostureAttackCoolTime", 1.0f);
	}
	// 下
	else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {
		sword_->SetTranslationX(1.2f);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(-0.75f);
		attackTypeRequest_ = AttackType::kThrust;
		timeManager_->SetTimer("PostureAttackCoolTime", 1.0f);
	}
	// 左
	else if (cosTheta >= 0.75f * pi || cosTheta <= -0.75f * pi) {
		sword_->SetTranslationX(aimingDirection_.x);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kRightSlash;
		timeManager_->SetTimer("PostureAttackCoolTime", 1.0f);
	}
	// 右
	else {
		sword_->SetTranslationX(aimingDirection_.x);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kLeftSlash;
		timeManager_->SetTimer("PostureAttackCoolTime", 1.0f);
	}
}

void(Soldier::* Soldier::AttackTypeInitFuncTable[])() = {
	&Soldier::AttackTypeDownSwingInitialize,
	&Soldier::AttackTypeThrustInitialize,
	&Soldier::AttackTypeLeftSwingInitialize,
	&Soldier::AttackTypeRightSwingInitialize,
	&Soldier::AttackTypeNullInitialize,
};
void(Soldier::* Soldier::AttackTypeUpdateFuncTable[])() = {
	&Soldier::AttackTypeDownSwingUpdate,
	&Soldier::AttackTypeThrustUpdate,
	&Soldier::AttackTypeLeftSwingUpdate,
	&Soldier::AttackTypeRightSwingUpdate,
	&Soldier::AttackTypeNullUpdate,
};
Vector3 Soldier::GetCenterPosition() const{
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 Soldier::GetCenterRotation() const{
	return transform_.rotation_;
}