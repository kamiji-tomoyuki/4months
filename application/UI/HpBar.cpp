#include "HpBar.h"

// 初期化
void HpBar::Initialize(const std::string& textureFile, const Vector2& position, const Vector2& size, const float& rotate, const int maxHp)
{
	Sprite_ = std::make_unique<Sprite>();
	Sprite_->Initialize(textureFile, position, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }, false, false);
	Sprite_->SetSize(size);
	Sprite_->SetRotation(rotate); // 回転を設定

	// テクスチャの切り取りサイズを設定
	maxHp_ = maxHp; // 最大HPを設定
	hp_ = maxHp;	// 初期HPは最大HPに設定
	maxSize_ = size; // 最大サイズを保存
}

// 更新
void HpBar::Update()
{
	// HPに応じてバーの高さを更新
	hpRatio_ = static_cast<float>(hp_) / maxHp_;
	float newHeight = maxSize_.y * hpRatio_; // 元の高さに比例して計算
	Sprite_->SetSize(Vector2(Sprite_->GetSize().x, newHeight)); // 横幅はそのまま、高さだけ更新
}

// 描画
void HpBar::Draw()
{
	if (hpRatio_ >= 0.0f) {
		Sprite_->Draw();
	}
}
