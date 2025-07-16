#include "UIManager.h"

void UIManager::Initialize(int playerMaxHp, int bossMaxHp)
{
	// プレイヤーのHPバーを初期化
	playerHpBar_ = std::make_unique<HpBar>();
	playerHpBar_->Initialize("hp.png", Vector2(400.0f, 700.0f), Vector2(100.0f, 500.0f), -1.57f, playerMaxHp); // 横幅を少し太く
	if (bossMaxHp > 0) {
		// 敵のHPバーを初期化
		bossHpBar_ = std::make_unique<HpBar>();
		bossHpBar_->Initialize("enemyHpBar.png", Vector2(400.0f, 80.0f), Vector2(100.0f, 500.0f), -1.57f, bossMaxHp); // 横幅を少し太く
	}
	// 操作説明のスプライトを初期化
	howToPlay_ = std::make_unique<Sprite>();
	howToPlay_->Initialize("HowToPlay.png", Vector2(0.0f, 0.0f));
}

void UIManager::Update()
{
	if(isPlayerHpValue_) {
		playerHpBar_->Update();
		isPlayerHpValue_ = false; // 更新後はフラグをリセット
	}
	if (bossHpBar_ && isBossHpValue_) {
		bossHpBar_->Update();
		isBossHpValue_ = false; // 更新後はフラグをリセット
	}
}

void UIManager::Draw()
{
	// プレイヤーのHPバーを描画
	playerHpBar_->Draw();
	// 敵のHPバーを描画
	if (bossHpBar_) {
		bossHpBar_->Draw();
	}
	// 操作説明のスプライトを描画
	howToPlay_->Draw();
}

void UIManager::SetPlayerHP(int hp)
{
	if (hp != playerHpBar_->GetHP()) {
		playerHpBar_->SetHP(hp);
		isPlayerHpValue_ = true;
	}
}

void UIManager::SetBossHP(int hp)
{
	if (hp != bossHpBar_->GetHP()) {
		bossHpBar_->SetHP(hp);
		isBossHpValue_ = true;
	}
}
