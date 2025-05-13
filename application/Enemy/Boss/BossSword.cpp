#include "BossSword.h"
#include "Boss.h"
#include "CollisionTypeIdDef.h"
#include "TimeManager.h"
#include "Player.h"
#include "PlayerSword.h"

void BossSword::Initialize(std::string filePath){
	BaseEnemySword::Initialize(filePath);
}

void BossSword::Update(){
	SetRadius(0);
	SetAABBScale({ 0.0f, 0.0f, 0.0f });
	SetOBBScale({ 0.4f,8.0f,1.0f });

	BaseEnemySword::Update();
}

void BossSword::Draw(const ViewProjection& viewProjection){
	BaseEnemySword::Draw(viewProjection);
}

void BossSword::DrawAnimation(const ViewProjection& viewProjection){
	BaseEnemySword::DrawAnimation(viewProjection);
}

void BossSword::OnCollision([[maybe_unused]] Collider* other){

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
	Vector3 rotate = transform_.rotation_ + enemy_->GetCenterRotation();
	return  rotate;
}