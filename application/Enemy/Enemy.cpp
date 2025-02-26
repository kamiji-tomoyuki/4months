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
bool Enemy::GetProbabilities(float probabilities)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	float randomValue = dist(gen);
	if (randomValue <= probabilities) {
		return true;
	}
	return false;
}
void Enemy::VectorRotation(const Vector3& direction) {
	Vector3 move = direction;
	transform_.rotation_.y = std::atan2f(move.x, move.z);
	Vector3 velocityZ = Transformation(move, MakeRotateYMatrix(-transform_.rotation_.y));
	transform_.rotation_.x = std::atan2f(-velocityZ.y, velocityZ.z);
}
void Enemy::SetTranslation(const Vector3& translation) {
	transform_.translation_ = translation;
	transform_.UpdateMatrix();
}