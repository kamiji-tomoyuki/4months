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

	damageEmitter_ = std::make_unique<ParticleEmitter>();
	damageEmitter_->Initialize("Damage.json");
}

void Enemy::Update() {
	BaseObject::Update();

	damageEmitter_->Update();
}

void Enemy::UpdateParticle(const ViewProjection& viewProjection){

}

void Enemy::Draw(const ViewProjection& viewProjection) { BaseObject::Draw(viewProjection); }

void Enemy::DrawParticle(const ViewProjection& viewProjection){

}

void Enemy::DrawAnimation(const ViewProjection& viewProjection){

}

void Enemy::OnCollision(Collider* other) {
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
			return;
		}
		// 衝突後の新しい速度を計算
		auto [newVelocity1, newVelocity2] = ComputeCollisionVelocities(
			1.0f, GetVelocity(), 1.0f, enemy->GetVelocity(), 1.0f, Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize()
		);

		// 計算した速度でボールの速度を更新
		SetVelocity(newVelocity1);
		enemy->SetVelocity(newVelocity2);

		float distance = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Length();

		Vector3 correction = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize() * (GetRadius() + enemy->GetRadius() - distance) * 0.55f;
		transform_.translation_ += correction;
		enemy->SetTranslation(enemy->GetTransform().translation_ - correction);

		//timeManager_->SetTimer("collision", timeManager_->deltaTime_ * 3.0f);
	}

	transform_.UpdateMatrix();
}

void Enemy::OnCollisionEnter(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
			return;
		}
		// 衝突後の新しい速度を計算
		auto [newVelocity1, newVelocity2] = ComputeCollisionVelocities(
			1.0f, GetVelocity(), 1.0f, enemy->GetVelocity(), 1.0f, Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize()
		);

		// 計算した速度でボールの速度を更新
		SetVelocity(newVelocity1);
		enemy->SetVelocity(newVelocity2);

		float distance = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Length();

		Vector3 correction = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize() * (GetRadius() + enemy->GetRadius() - distance) * 0.55f;
		transform_.translation_ += correction;
		enemy->SetTranslation(enemy->GetTransform().translation_ - correction);

		//timeManager_->SetTimer("collision", timeManager_->deltaTime_ * 3.0f);
	}

	transform_.UpdateMatrix();
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
void Enemy::RootInitialize(){

}
void Enemy::RootUpdate(){

}
void Enemy::AttackInitialize(){

}
void Enemy::AttackUpdate(){

}
void Enemy::ProtectionInitialize(){

}
void Enemy::ProtectionUpdate(){

}
void Enemy::Damage() {

	damageEmitter_->SetPosition(GetCenterPosition());

	damageEmitter_->Start();
}
void Enemy::SetTranslation(const Vector3& translation) {
	transform_.translation_ = translation;
	transform_.UpdateMatrix();
}