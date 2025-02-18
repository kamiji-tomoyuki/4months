#include "Skydome.h"
#include <cassert>
#include <iostream> // デバッグ用

void Skydome::Init() {
	BaseObject::Init(); // 基底クラスの初期化
	CreateModel("debug/skydome.obj"); // モデルをロード
	transform_.scale_ = { 100.0f,100.0f,100.0f };
	transform_.scale_ = { 10.0f, 10.0f, 10.0f }; // スケールを設定
	transform_.UpdateMatrix();
	//transform_.translation_ = { 0.0f, 10.0f, -80.0f }; // 位置を設定
}

void Skydome::Update() {
	BaseObject::Update();

	//transform_.UpdateMatrix(); // ワールド行列の更新
}

void Skydome::Draw(const ViewProjection& viewProjection) {
	BaseObject::Draw(viewProjection);
}

Vector3 Skydome::GetCenterPosition() const
{
	return Vector3();
}

Vector3 Skydome::GetCenterRotation() const
{
	return Vector3();
}

void Skydome::SetViewProjection(ViewProjection* viewProjection)
{
	viewProjection_ = viewProjection;
}