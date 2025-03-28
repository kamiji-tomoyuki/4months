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
	sword_->Initialize("player/playerArm.gltf", "sword/sword.obj");
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
void Soldier::UpdateParticle(const ViewProjection& viewProjection){
	Enemy::UpdateParticle(viewProjection);
	if (timeManager_->GetTimer("Smoke" + std::to_string(GetSerialNumber())).isStart &&
		!timeManager_->GetTimer("SmokeCoolTime" + std::to_string(GetSerialNumber())).isStart) {
		//emitters_[1]->Start();
		timeManager_->SetTimer("SmokeCoolTime" + std::to_string(GetSerialNumber()), 0.1f);
	}
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->SetPosition(GetCenterPosition());
		emitter_->Update();
	}
}
void Soldier::Draw(const ViewProjection& viewProjection){
	Enemy::Draw(viewProjection);
	sword_->Draw(viewProjection);
}
void Soldier::DrawParticle(const ViewProjection& viewProjection){
	Enemy::DrawParticle(viewProjection);
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->Draw();
		//emitter_->DrawEmitter();
	}
}
void Soldier::DrawAnimation(const ViewProjection& viewProjection){
	//sword_->DrawAnimation(viewProjection);
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

void Soldier::RootInitialize(){
	Enemy::RootInitialize();
	sword_->SetIsAttack(false);
	sword_->SetIsDefense(false);
	sword_->SetTranslation({ 1.7f, 0.0f, 1.3f });
}

void Soldier::RootUpdate(){
	Enemy::RootUpdate();
}

void Soldier::AttackInitialize(){
	Enemy::AttackInitialize();
	attackTypeRequest_ = AttackType::kNullType;
	// 方向取得
	DirectionPreliminaryAction();
}

void Soldier::AttackUpdate(){
	Enemy::AttackUpdate();
	attack_.time += timeManager_->deltaTime_;
	if (attackTypeRequest_) {
		//振るまいを変更する
		attackType_ = attackTypeRequest_.value();

		attack_.time = 0;
		attack_.isAttack = true;
		attack_.swordStartTransform = sword_->GetTranslation().z;
		sword_->SetIsAttack(true);

		(this->*AttackTypeInitFuncTable[static_cast<size_t>(attackType_)])();

		attackTypeRequest_ = std::nullopt;
	}

	(this->*AttackTypeUpdateFuncTable[static_cast<size_t>(attackType_)])();

	if (attack_.time / attack_.kLimitTime >= 1.0f) {
		attack_.time = attack_.kLimitTime;
		attack_.isAttack = false;
		sword_->SetTranslation({ 1.7f, 0.0f, 1.3f });
	}
}

void Soldier::ProtectionInitialize(){
	Enemy::ProtectionInitialize();
	attack_.time = 0;
	attack_.isAttack = false;
	attack_.isLeft = !attack_.isLeft;
	if (attack_.isLeft) {
		attack_.swordStartTransform = sword_->GetTranslation().z;
		sword_->SetIsAttack(false);
		sword_->SetIsDefense(true);

	} else {
		attack_.swordStartTransform = sword_->GetTranslation().z;
		sword_->SetIsAttack(false);
		sword_->SetIsDefense(true);
		//attack_.armStart = arms_[kRArm]->GetTranslation().z;
		//arms_[kRArm]->SetIsAttack(true);
	}
	// 方向取得
	DirectionPreliminaryAction();
	if (player_->GetAimingDirection().x == 0 && player_->GetAimingDirection().z == 0) {
		//behaviorRequest_ = Behavior::kRoot;
	} else {
		aimingDirection_.x = -player_->GetAimingDirection().x;
		aimingDirection_.z = player_->GetAimingDirection().z;
	}
}

void Soldier::ProtectionUpdate(){
	Enemy::ProtectionUpdate();
	// 入力方向にセット

	// 入力方向によって角度をセット
	float cosTheta = atan2f(aimingDirection_.z, aimingDirection_.x);
	// 上
	if (cosTheta > 0.25f * pi && cosTheta < 0.75f * pi) {
		// 座標
		sword_->SetTranslation({ aimingDirection_.x * 0.6f + 2.0f, aimingDirection_.z * 0.4f , 0.25f });

		// 角度
		sword_->SetRotation({ 0.0f, pi_v<float> *0.5f, pi_v<float> *0.5f });
	}
	// 下
	else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {
		// 座標
		sword_->SetTranslation({ aimingDirection_.x * 0.6f + 2.0f, 0.0f , 1.5f });

		// 角度
		sword_->SetRotation({ 0.0f, pi_v<float> *0.5f, pi_v<float> *0.5f });
	}
	// 左右
	else {
		// 座標
		sword_->SetTranslation({ aimingDirection_.x * 0.25f, aimingDirection_.z * 0.25f , 0.0f });

		// 角度
		sword_->SetRotation({ 0.0f, 0.0f, 0.0f });
	}

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
	// 座標セット
	attack_.swordStartTransform = sword_->GetTranslation();
	attack_.swordEndTransform = { sword_->GetTranslation().x + aimingDirection_.x, sword_->GetTranslation().y - aimingDirection_.z, sword_->GetTranslation().z + aimingDirection_.z };

	// 角度セット
	attack_.swordStartRotate = sword_->GetRotate();
	attack_.swordEndRotate = { 0.6f * pi_v<float>, sword_->GetRotate().y, sword_->GetRotate().z };

	attack_.time = 0.0f;
	sword_->SetIsAttack(true);
}

// 振り下ろし(上入力攻撃)の更新
void Soldier::AttackTypeDownSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber())).isStart) {
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * 0.5f * (attack_.time / attack_.kLimitTime));

	newPos = { Lerp(attack_.swordStartTransform.x, attack_.swordEndTransform.x, attack_.time / attack_.kLimitTime), attack_.swordStartTransform.y * cosf(theta) - attack_.swordStartTransform.z * sinf(theta), attack_.swordStartTransform.y * sinf(theta) + attack_.swordStartTransform.z * cosf(theta) };

	sword_->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;
	theta = float(pi * 0.5f * (attack_.time / attack_.kLimitTime));

	newRotate = { Lerp(attack_.swordStartRotate.x, attack_.swordEndRotate.x, attack_.time / attack_.kLimitTime), attack_.swordEndRotate.y, attack_.swordEndRotate.z };

	sword_->SetRotation(newRotate);
}

// 突き(下入力攻撃)の初期化
void Soldier::AttackTypeThrustInitialize(){
	attack_.swordStartTransform = sword_->GetTranslation();
	attack_.swordEndTransform = { sword_->GetTranslation().x - (aimingDirection_.x * 0.2f * 6.0f), sword_->GetTranslation().y, -aimingDirection_.z * 0.2f * 6.0f };
	attack_.time = 0.0f;
	sword_->SetIsAttack(true);
}

// 突き(下入力攻撃)の更新
void Soldier::AttackTypeThrustUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber())).isStart) {
		return;
	}
	Vector3 newPos = EaseInOutExpo(attack_.swordStartTransform, attack_.swordEndTransform, attack_.time, attack_.kLimitTime);

	sword_->SetTranslation(newPos);
}

// 右振り抜き(左入力攻撃)の初期化
void Soldier::AttackTypeLeftSwingInitialize(){
	// 座標セット
	attack_.swordStartTransform = sword_->GetTranslation();
	attack_.swordEndTransform = { -aimingDirection_.x, sword_->GetTranslation().y, sword_->GetTranslation().z };

	// 角度セット
	attack_.swordStartRotate = sword_->GetRotate();
	attack_.swordEndRotate = { sword_->GetRotate().x - pi_v<float>, sword_->GetRotate().y, sword_->GetRotate().z };

	attack_.time = 0.0f;
	sword_->SetIsAttack(true);
}

// 右振り抜き(左入力攻撃)の更新
void Soldier::AttackTypeLeftSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber())).isStart) {
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.swordStartTransform.x * cosf(-theta) - attack_.swordStartTransform.z * sinf(-theta), attack_.swordStartTransform.y, attack_.swordStartTransform.x * sinf(-theta) + attack_.swordStartTransform.z * cosf(-theta) };

	sword_->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;

	newRotate = { attack_.swordStartRotate.x + theta, attack_.swordStartRotate.y, attack_.swordStartRotate.z };

	sword_->SetRotation(newRotate);
}

// 左振り抜き(右入力攻撃)の初期化
void Soldier::AttackTypeRightSwingInitialize(){
	// 座標セット
	attack_.swordStartTransform = sword_->GetTranslation();
	attack_.swordEndTransform = { -aimingDirection_.x, sword_->GetTranslation().y, sword_->GetTranslation().z };

	// 角度セット
	attack_.swordStartRotate = sword_->GetRotate();
	attack_.swordEndRotate = { sword_->GetRotate().x + pi_v<float>, sword_->GetRotate().y, sword_->GetRotate().z };

	attack_.time = 0.0f;
	sword_->SetIsAttack(true);
}

// 左振り抜き(右入力攻撃)の更新
void Soldier::AttackTypeRightSwingUpdate(){
	if (!timeManager_->GetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber())).isStart) {
		return;
	}
	// 座標の計算
	Vector3 newPos = 0.0f;
	float theta = float(pi * (attack_.time / attack_.kLimitTime));

	newPos = { attack_.swordStartTransform.x * cosf(theta) - attack_.swordStartTransform.z * sinf(theta), attack_.swordStartTransform.y, attack_.swordStartTransform.x * sinf(theta) + attack_.swordStartTransform.z * cosf(theta) };

	sword_->SetTranslation(newPos);

	// 角度の計算
	Vector3 newRotate = 0.0f;

	newRotate = { attack_.swordStartRotate.x - theta, attack_.swordStartRotate.y, attack_.swordStartRotate.z };

	sword_->SetRotation(newRotate);
}

// 未入力
void Soldier::AttackTypeNullInitialize(){
	attack_.time = 0;
	attack_.isAttack = false;
	attack_.isLeft = !attack_.isLeft;
	attack_.swordStartTransform = sword_->GetTranslation().z;
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
		timeManager_->SetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber()), 1.0f);
	}
	// 下
	else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {
		sword_->SetTranslationX(1.2f);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(-0.75f);
		attackTypeRequest_ = AttackType::kThrust;
		timeManager_->SetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber()), 1.0f);
	}
	// 左
	else if (cosTheta >= 0.75f * pi || cosTheta <= -0.75f * pi) {
		sword_->SetTranslationX(aimingDirection_.x);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kRightSlash;
		timeManager_->SetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber()), 1.0f);
	}
	// 右
	else {
		sword_->SetTranslationX(aimingDirection_.x);
		sword_->SetTranslationY(0.0f);
		sword_->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kLeftSlash;
		timeManager_->SetTimer("PostureAttackCoolTime" + std::to_string(GetSerialNumber()), 1.0f);
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