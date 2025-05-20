#include "Coliseum.h"
#include "Player.h"
#include <cassert>
#include <iostream> // デバッグ用
#include <CollisionTypeIdDef.h>

void Coliseum::Init() {

	BaseObject::Init(); // 基底クラスの初期化
}

void Coliseum::Init(const std::string& fileName) {

	const std::string kDirectoryPath = "debug/"; // ディレクトリパス

	std::string filePath = kDirectoryPath + fileName; // ファイルパス

	BaseObject::Init(); // 基底クラスの初期化

	CreateModel(filePath); // モデルをロード

	transform_.scale_ = { 100.0f, 100.0f, 100.0f }; // スケールを設定

	transform_.UpdateMatrix();

	//transform_.translation_ = { 0.0f, 10.0f, -80.0f }; // 位置を設定
}

void Coliseum::Update() {
	BaseObject::Update();

	//transform_.UpdateMatrix(); // ワールド行列の更新
}

void Coliseum::Draw(const ViewProjection& viewProjection) {
	BaseObject::Draw(viewProjection);
}

/// 当たってる間
void Coliseum::OnCollision(Collider* other)
{

}

/// 当たった瞬間
void Coliseum::OnCollisionEnter(Collider* other)
{

}

/// 当たり終わった瞬間
void Coliseum::OnCollisionOut(Collider* other)
{
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);
		
		Vector3 playerToColiseum = player->GetCenterPosition() - transform_.translation_;
		playerToColiseum = playerToColiseum.Normalize();
		player->SetPosition(GetCenterPosition() + playerToColiseum * (GetRadius() + player->GetRadius() * 2.0f));
	
	}

	//transform_.UpdateMatrix();
}

Vector3 Coliseum::GetCenterPosition() const
{
	return transform_.translation_;
}

Vector3 Coliseum::GetCenterRotation() const
{
	return transform_.rotation_;
}

void Coliseum::SetViewProjection(ViewProjection* viewProjection)
{
	viewProjection_ = viewProjection;
}