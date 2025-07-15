#pragma once
#include "Sprite.h"
#include <memory>

class HpBar
{
public:	// メンバ関数
	HpBar() = default;
	~HpBar() = default;
	// 初期化
	void Initialize(const std::string& textureFile, const Vector2& position, const Vector2& size, const float& rotate, const int maxHp);
	// 更新
	void Update();
	// 描画
	void Draw();

public:	// getter
	const Vector2& GetPosition() const { return Sprite_->GetPosition(); }
	const Vector2& GetSize() const { return Sprite_->GetSize(); }
	float GetHpRatio() const { return hpRatio_; }
	int GetMaxHp() const { return maxHp_; }
	int GetHP() const { return hp_; }

public: //setter
	void SetPosition(const Vector2& position) { Sprite_->SetPosition(position); }
	void SetSize(const Vector2& size) { Sprite_->SetSize(size); }
	void SetHpRatio(float ratio) { hpRatio_ = ratio; } // HP比率を設定
	void SetHP(int hp) { hp_ = hp; }

private:	// メンバ変数
	std::unique_ptr<Sprite> Sprite_; // HPバーのスプライト

	int maxHp_ = 100;	// 最大HP
	int hp_ = 0;		// 現在のHP
	float hpRatio_ = 1.0f; // HP比率（0.0fから1.0fの範囲）

	Vector2 maxSize_;	// 最大サイズ
};

