#include "Enemy.h"
#include "CollisionTypeIdDef.h"


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
	BaseObject::CreateModel("player/playerBody.obj");
}

void Enemy::Update() {
	BaseObject::Update();

	transform_.rotation_.y += 0.01f;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotation_.y);

	Vector3 move = Transformation(Vector3{ 0,0,0.1f }, rotateMatrix);

	// 移動
	transform_.translation_ = (transform_.translation_ + move);
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

void Enemy::SetTranslation(const Vector3& translation) {
	transform_.translation_ = translation;
	transform_.UpdateMatrix();
}

Vector3 Enemy::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 Enemy::GetCenterRotation() const
{
	return transform_.rotation_;
}
