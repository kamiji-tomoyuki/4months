#include "EnemySword.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "CollisionTypeIdDef.h"
#include "Player.h"
#include "PlayerSword.h"

void EnemySword::Initialize(std::string filePath) {
	BaseEnemySword::Initialize(filePath);

	for (int i = 0; i < 2; ++i) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		emitters_.push_back(std::move(emitter_));
	}

	emitters_[0]->Initialize("Block.json");
}
void EnemySword::Update() {
	SetRadius(0);
	SetAABBScale({ 0.0f, 0.0f, 0.0f });
	SetOBBScale({0.4f,8.0f,1.0f});
	
	BaseEnemySword::Update();
}

void EnemySword::UpdateParticle(const ViewProjection& viewProjection) {
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->SetPosition(GetCenterPosition());
		emitter_->Update();
	}
}

void EnemySword::Draw(const ViewProjection& viewProjection) {
	BaseEnemySword::Draw(viewProjection);
}

void EnemySword::DrawAnimation(const ViewProjection& viewProjection){
	BaseEnemySword::DrawAnimation(viewProjection);
}

void EnemySword::OnCollision([[maybe_unused]] Collider* other) {
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	if (GetEnemy()->GetSerialNumber() == GetEnemy()->GetNextSerialNumber() - 1) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerSword* playerSwod = static_cast<PlayerSword*>(other);
		if (GetIsAttack() && playerSwod->GetIsDefence()) {
			SetIsAttack(false);
			emitters_[0]->Start();
			//enemy_->SetObjColor({ 0.0f,0.0f,1.0f,1.0f });
			//emitters_[0]->SetEmitActive(true);
			Vector3 newVelocity = playerSwod->GetPlayer()->GetCenterPosition() - enemy_->GetCenterPosition();

			playerSwod->GetPlayer()->SetVelocity(playerSwod->GetPlayer()->GetVelocity() + newVelocity.Normalize() * 30.0f);
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);

		if (GetIsAttack()) {
			Vector3 newVelocity = player->GetCenterPosition() - enemy_->GetCenterPosition();

			player->SetVelocity(player->GetVelocity() + newVelocity.Normalize() * 300.0f);

			player->SetHP(player->GetHP() - int(1000));
			if (player->GetHP() <= 0) {
				player->SetGameOver(true);
			}
			SetIsAttack(false);
		}
	}
}

void EnemySword::OnCollisionEnter([[maybe_unused]] Collider* other) {
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	if (GetEnemy()->GetSerialNumber() == GetEnemy()->GetNextSerialNumber() - 1) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerSword* playerSwod = static_cast<PlayerSword*>(other);
		if (GetIsAttack() && playerSwod->GetIsDefence()) {
			SetIsAttack(false);
			emitters_[0]->Start();
			Vector3 newVelocity = playerSwod->GetPlayer()->GetCenterPosition() - enemy_->GetCenterPosition();

			playerSwod->GetPlayer()->SetVelocity(playerSwod->GetPlayer()->GetVelocity() + newVelocity.Normalize() * 30.0f);
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);

		if (GetIsAttack()) {
			Vector3 newVelocity = player->GetCenterPosition() - enemy_->GetCenterPosition();

			player->SetVelocity(player->GetVelocity() + newVelocity.Normalize() * 300.0f);

			player->SetHP(player->GetHP() - int(1000));
			if (player->GetHP() <= 0) {
				player->SetGameOver(true);
			}
			SetIsAttack(false);
		}
	}
}

void EnemySword::OnCollisionOut([[maybe_unused]] Collider* other) {

}

Vector3 EnemySword::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 2.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 EnemySword::GetCenterRotation() const {
	//OBBのローカルローテーション
	//Vector3 rotate = transform_.rotation_ + enemy_->GetCenterRotation();
	Quaternion enemyQuaternion = Quaternion::FromEulerAngles(enemy_->GetCenterRotation());
	Quaternion swordQuaternion = Quaternion::FromEulerAngles(transform_.rotation_);
	swordQuaternion = enemyQuaternion * swordQuaternion;
	return  swordQuaternion.ToEulerAngles();
}