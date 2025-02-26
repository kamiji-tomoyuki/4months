#include "Soldier.h"
#include <CollisionTypeIdDef.h>
#include "myMath.h"
#include "EnemyStateApproach.h"

Soldier::Soldier(){

}
void Soldier::Init(){
	Enemy::Init();
	BaseObject::CreateModel("player/playerBody.obj");
	Enemy::ChangeState(std::make_unique<EnemyStateApproach>(this));
}
void Soldier::Update(){
	Enemy::Update();
	//transform_.rotation_.y += 0.01f;
	//Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotation_.y);
	//Vector3 move = Transformation(Vector3{ 0,0,0.1f }, rotateMatrix);
	//// 移動
	//transform_.translation_ = (transform_.translation_ + move);
}
void Soldier::Draw(const ViewProjection& viewProjection){
	Enemy::Draw(viewProjection);
}
void Soldier::OnCollision(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

	}
}
void Soldier::OnCollisionEnter(Collider* other){
	
}
void Soldier::OnCollisionOut(Collider* other){

}

Vector3 Soldier::GetCenterPosition() const{
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 Soldier::GetCenterRotation() const{
	return transform_.rotation_;
}