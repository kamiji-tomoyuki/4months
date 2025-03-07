#include "PlayerArm.h"
#include "Player.h"
#include "CollisionTypeIdDef.h"
#include "Enemy.h"
#include "TimeManager.h"

void PlayerArm::Initialize(){
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	obj3d_ = std::make_unique<Object3d>();
	palm_ = std::make_unique<Object3d>();
	
}

void PlayerArm::Initialize(std::string filePath, std::string palmFilePath){
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
void PlayerArm::Update(){
	//元となるワールドトランスフォームの更新
	transform_.UpdateMatrix();
	transformPalm_.translation_ = transform_.translation_;
	transformPalm_.rotation_ = transform_.rotation_;
	transformPalm_.UpdateMatrix();
	/// 色転送
	objColor_.TransferMatrix();

	obj3d_->AnimationUpdate(true);
}

void PlayerArm::Draw(const ViewProjection& viewProjection){
	palm_->Draw(transformPalm_, viewProjection);
}

void PlayerArm::DrawAnimation(const ViewProjection& viewProjection)
{
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

void PlayerArm::OnCollision([[maybe_unused]] Collider* other){
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (GetIsAttack()) {
			Vector3 newVelocity = enemy->GetCenterPosition() - player_->GetCenterPosition();

			enemy->SetVelocity(enemy->GetVelocity() + newVelocity.Normalize() * 100.0f);
			enemy->SetHP(enemy->GetHP() - int(1000));
			if (enemy->GetHP() <= 0) {
				enemy->SetIsAlive(false);
			}
			//SetIsAttack(false);
		}
	}
}

void PlayerArm::OnCollisionEnter([[maybe_unused]] Collider* other){
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (GetIsAttack()) {
			Vector3 newVelocity = enemy->GetCenterPosition() - player_->GetCenterPosition();

			enemy->SetVelocity(enemy->GetVelocity() + newVelocity.Normalize() * 100.0f);
			enemy->SetHP(enemy->GetHP() - int(1000));
			if (enemy->GetHP() <= 0) {
				enemy->SetIsAlive(false);
			}
			//SetIsAttack(false);
		}
	}
}

void PlayerArm::OnCollisionOut([[maybe_unused]] Collider* other){

}

Vector3 PlayerArm::GetCenterPosition() const{
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 PlayerArm::GetCenterRotation() const{
	return transform_.rotation_;
}

void PlayerArm::SetModel(const std::string& filePath){
	obj3d_->SetModel(filePath);
}

void PlayerArm::SetPlayer(Player* player) {
	player_ = player;
	transform_.parent_ = &player->GetWorldTransform();
	transformPalm_.parent_ = transform_.parent_;
}