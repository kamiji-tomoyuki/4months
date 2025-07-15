#pragma once
#include "HpBar.h"

class UIManager
{
public:	// メンバ関数
	// 初期化
	void Initialize(int playerMaxHp, int bossMaxHp = 0);
	// 更新
	void Update();
	// 描画
	void Draw();

public:	// getter
	const int GetPlayerHP() const { return playerHpBar_->GetHP(); }
	const int GetBossHP() const { return bossHpBar_ ? bossHpBar_->GetHP() : 0; }

public:	// setter
	void SetPlayerHP(int hp);
	void SetBossHP(int hp);

private:	// メンバ変数
	std::unique_ptr<HpBar> playerHpBar_; // プレイヤーのHPバー
	std::unique_ptr<HpBar> bossHpBar_; // 敵のHPバー
	std::unique_ptr<Sprite> howToPlay_; // 操作説明のスプライト

	bool isPlayerHpValue_ = false; // プレイヤーのHP値が変化したかどうか
	bool isBossHpValue_ = false; // ボスのHP値が変化したかどうか
};

