#include "EnemySword.h"
#include "Enemy.h"
#include "CollisionTypeIdDef.h"
#include "TimeManager.h"
#include "Player.h"
#include "PlayerSword.h"

void EnemySword::Initialize() {
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon));
	obj3d_ = std::make_unique<Object3d>();
	palm_ = std::make_unique<Object3d>();

}

void EnemySword::Initialize(std::string filePath, std::string palmFilePath) {
	Initialize();
	obj3d_->Initialize(filePath);
	palm_->Initialize(palmFilePath);
	/// ワールドトランスフォームの初期化
	transform_.Initialize();
	transformPalm_.Initialize();
	//カラーのセット
	objColor_.Initialize();
	objColor_.SetColor(Vector4(1, 1, 1, 1));
}
void EnemySword::Update() {
	SetRadius(transform_.scale_.Length());
	//元となるワールドトランスフォームの更新
	transform_.UpdateMatrix();
	transformPalm_.translation_ = transform_.translation_;
	transformPalm_.rotation_ = transform_.rotation_;
	transformPalm_.UpdateMatrix();
	/// 色転送
	objColor_.TransferMatrix();

	obj3d_->AnimationUpdate(true);
}

void EnemySword::Draw(const ViewProjection& viewProjection) {
	palm_->Draw(transformPalm_, viewProjection);
}

void EnemySword::DrawAnimation(const ViewProjection& viewProjection){
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

void EnemySword::OnCollision([[maybe_unused]] Collider* other) {
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerSword* playerSwod = static_cast<PlayerSword*>(other);
		if (GetIsAttack() && playerSwod->GetIsDefence()) {
			SetIsAttack(false);
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
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerSword* playerSwod = static_cast<PlayerSword*>(other);
		if (GetIsAttack() && playerSwod->GetIsDefence()) {
			SetIsAttack(false);
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
	return transform_.rotation_;
}

void EnemySword::SetModel(const std::string& filePath) {
	obj3d_->SetModel(filePath);
}

void EnemySword::SetEnemy(Enemy* enemy) {
	enemy_ = enemy;
	transform_.parent_ = &enemy_->GetWorldTransform();
	transformPalm_.parent_ = transform_.parent_;
}
