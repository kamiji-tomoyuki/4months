#include "BaseEnemySword.h"
#include "CollisionTypeIdDef.h"
#include "Enemy.h"
#include "TimeManager.h"

void BaseEnemySword::Initialize(){
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemyWeapon));
	obj3d_ = std::make_unique<Object3d>();
}

void BaseEnemySword::Initialize(std::string filePath){
	Initialize();
	obj3d_->Initialize(filePath);
	/// ワールドトランスフォームの初期化
	transform_.Initialize();
	transformPalm_.Initialize();
	//カラーのセット
	objColor_.Initialize();
	objColor_.SetColor(Vector4(1, 1, 1, 1));
}

void BaseEnemySword::Update(){
	//元となるワールドトランスフォームの更新
	transform_.UpdateMatrix();
	transformPalm_.translation_ = transform_.translation_;
	transformPalm_.rotation_ = transform_.rotation_;
	transformPalm_.UpdateMatrix();
	/// 色転送
	objColor_.TransferMatrix();

	obj3d_->AnimationUpdate(true);
}

void BaseEnemySword::Draw(const ViewProjection& viewProjection){
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

void BaseEnemySword::DrawAnimation(const ViewProjection& viewProjection){
	obj3d_->Draw(transform_, viewProjection, &objColor_);
}

void BaseEnemySword::SetEnemy(Enemy* enemy){
	enemy_ = enemy;
	transform_.parent_ = &enemy_->GetWorldTransform();
	transformPalm_.parent_ = transform_.parent_;
}
