#include "Soldier.h"
#include <CollisionTypeIdDef.h>
#include "myMath.h"
#include "SoldierStateRoot.h"
#include "Player.h"
#include "TimeManager.h"
#include <Easing.h>
using namespace std::numbers;

Soldier::Soldier(){

}
void Soldier::Init(){
	Enemy::Init();
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

	Enemy::ChangeState(std::make_unique<SoldierStateRoot>(this));
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
	if (GetSerialNumber() == GetNextSerialNumber() - 1) {
		return;
	}
	Enemy::Draw(viewProjection);
	sword_->Draw(viewProjection);
}
void Soldier::DrawParticle(const ViewProjection& viewProjection){
	if (GetSerialNumber() == GetNextSerialNumber() - 1) {
		return;
	}
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