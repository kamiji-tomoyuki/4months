#include "PlayerSword.h"
#include "Player.h"
#include "CollisionTypeIdDef.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "EnemySword.h"

/// 初期化
void PlayerSword::Initialize()
{
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
	obj3d_ = std::make_unique<Object3d>();
	palm_ = std::make_unique<Object3d>();
}

/// 初期化
void PlayerSword::Initialize(std::string filePath, std::string palmFilePath)
{
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

/// 更新
void PlayerSword::Update()
{
	// 半径をセット
	SetRadius(4.0f);

	//元となるワールドトランスフォームの更新
	transform_.UpdateMatrix();
	transformPalm_.translation_ = transform_.translation_;
	transformPalm_.rotation_ = transform_.rotation_;
	transformPalm_.UpdateMatrix();
	/// 色転送
	objColor_.TransferMatrix();

	obj3d_->AnimationUpdate(true);
}

/// 描画
void PlayerSword::Draw(const ViewProjection& viewProjection)
{
	palm_->Draw(transformPalm_, viewProjection);
}

/// 描画
void PlayerSword::DrawAnimation(const ViewProjection& viewProjection)
{
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

/// 当たってる間
void PlayerSword::OnCollision(Collider* other)
{
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon)) {
		EnemySword* enemySwod = static_cast<EnemySword*>(other);
		if (GetIsAttack() && enemySwod->GetIsDefense()) {
			SetIsAttack(false);
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (enemy->GetSerialNumber() == enemy->GetNextSerialNumber() - 1) {
			return;
		}
		if (GetIsAttack()) {
			Vector3 newVelocity = enemy->GetCenterPosition() - player_->GetCenterPosition();

			enemy->SetVelocity(enemy->GetVelocity() + newVelocity.Normalize() * 30.0f);
			enemy->SetHP(enemy->GetHP() - int(1000));
			if (enemy->GetHP() <= 0) {
				enemy->SetIsAlive(false);
			}
			//SetIsAttack(false);
		}
	}
}

/// 当たった瞬間
void PlayerSword::OnCollisionEnter(Collider* other)
{
	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon)) {
		EnemySword* enemySwod = static_cast<EnemySword*>(other);
		if (GetIsAttack() && enemySwod->GetIsDefense()) {
			SetIsAttack(false);
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (enemy->GetSerialNumber() == enemy->GetNextSerialNumber() - 1) {
			return;
		}
		if (GetIsAttack()) {
			Vector3 newVelocity = enemy->GetCenterPosition() - player_->GetCenterPosition();

			enemy->SetVelocity(enemy->GetVelocity() + newVelocity.Normalize() * 30.0f);
			enemy->SetHP(enemy->GetHP() - int(1000));
			if (enemy->GetHP() <= 0) {
				enemy->SetIsAlive(false);
			}
			//SetIsAttack(false);
		}
	}
}

/// 当たり終わった瞬間
void PlayerSword::OnCollisionOut(Collider* other)
{
}

// 中心座標を取得
Vector3 PlayerSword::GetCenterPosition() const
{
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 2.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

// 中心座標を取得
Vector3 PlayerSword::GetCenterRotation() const
{
	return transform_.rotation_;
}

/// モデルセット
void PlayerSword::SetModel(const std::string& filePath)
{
	obj3d_->SetModel(filePath);
}

/// プレイヤーセット
void PlayerSword::SetPlayer(Player* player)
{
	player_ = player;
	transform_.parent_ = &player->GetWorldTransform();
	transformPalm_.parent_ = transform_.parent_;
}
