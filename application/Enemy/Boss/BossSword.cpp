#include "BossSword.h"
#include "Boss.h"
#include "CollisionTypeIdDef.h"
#include "TimeManager.h"
#include "Player.h"
#include "PlayerSword.h"
#include "BossStateRoot.h"
using namespace std::numbers;

void BossSword::Initialize(std::string filePath){
	BaseEnemySword::Initialize(filePath);
}

void BossSword::Update(){
	SetRadius(0);
	SetAABBScale({ 0.0f, 0.0f, 0.0f });
	Vector3 obbScale = { 0.4f,8.0f,1.0f };
	SetOBBScale(obbScale * 3.0f);

	BaseEnemySword::Update();
}

void BossSword::Draw(const ViewProjection& viewProjection){
	BaseEnemySword::Draw(viewProjection);
}

void BossSword::DrawAnimation(const ViewProjection& viewProjection){
	BaseEnemySword::DrawAnimation(viewProjection);
}

void BossSword::OnCollision([[maybe_unused]] Collider* other){
	if (GetEnemy()->GetSerialNumber() == GetEnemy()->GetNextSerialNumber() - 1) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	Boss* boss = static_cast<Boss*>(GetEnemy());
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerSword* playerSwod = static_cast<PlayerSword*>(other);
		if (GetIsAttack() && playerSwod->GetIsDefence()) {
			SetIsAttack(false);
			Vector3 aimingDirection = boss->GetAimingDirection();
			aimingDirection.y *= -1.0f;
			boss->SetAimingDirection(aimingDirection);
			//enemy_->SetObjColor({ 0.0f,0.0f,1.0f,1.0f });
			//emitters_[0]->SetEmitActive(true);
			Vector3 newVelocity = playerSwod->GetPlayer()->GetCenterPosition() - enemy_->GetCenterPosition();

			playerSwod->GetPlayer()->SetVelocity(playerSwod->GetPlayer()->GetVelocity() + newVelocity.Normalize() * 30.0f);
			float theta = pi_v<float> * 0.10f * aimingDirection.y;

			Vector3 newPos = GetTranslation();
			Vector3 newRotate = GetRotate();
			quaternion_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), theta) * quaternion_;

			// 回転
			newRotate = quaternion_.ToEulerAngles();
			SetRotation(newRotate);
			// 座標の計算
			newPos = Transformation(newPos, MakeRotateYMatrix(theta));
			SetTranslation(newPos);


			enemy_->ChangeState(std::make_unique<BossStateRoot>(enemy_));
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

void BossSword::OnCollisionEnter([[maybe_unused]] Collider* other){

}

void BossSword::OnCollisionOut([[maybe_unused]] Collider* other){

}

Vector3 BossSword::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 2.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 BossSword::GetCenterRotation() const {
	//OBBのローカルローテーション
	Quaternion enemyQuaternion = Quaternion::FromEulerAngles(enemy_->GetCenterRotation());
	Quaternion swordQuaternion = Quaternion::FromEulerAngles(transform_.rotation_);
	swordQuaternion = enemyQuaternion * swordQuaternion;
	return  swordQuaternion.ToEulerAngles();
}