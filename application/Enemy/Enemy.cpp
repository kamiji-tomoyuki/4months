#include "Enemy.h"
#include "CollisionTypeIdDef.h"
#include "Player.h"

uint32_t Enemy::nextSerialNumber_ = 0;

Enemy::Enemy() {
	//シリアルナンバーをふる
	serialNumber_ = nextSerialNumber_;
	//次の番号を加算
	++nextSerialNumber_;
}

void Enemy::Init(){
	BaseObject::Init();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
}

void Enemy::Update() {
	BaseObject::Update();
	// キャラ移動
	state_->Update();
}

void Enemy::Draw(const ViewProjection& viewProjection) { BaseObject::Draw(viewProjection); }

void Enemy::OnCollision(Collider* other) {
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

	}
}

void Enemy::OnCollisionEnter(Collider* other){

}

void Enemy::OnCollisionOut(Collider* other){

}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state){
	state_ = std::move(state);
}

void Enemy::SetTranslation(const Vector3& translation) {
	transform_.translation_ = translation;
	transform_.UpdateMatrix();
}