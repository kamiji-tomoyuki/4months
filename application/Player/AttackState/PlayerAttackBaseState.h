#pragma once
#include <string>

class Player;

class PlayerAttackBaseState
{
public:
	// コンストラクタ
	PlayerAttackBaseState(const std::string& name, Player* player) : name_(name), player_(player) {}

	// デストラクタ
	virtual ~PlayerAttackBaseState() {};

	// ステートの初期化
	virtual void Initialize() = 0;

	// ステートの更新
	virtual void Update() = 0;

#ifdef _DEBUG
	// デバッグログ出力
	virtual void DebugLog();
#endif // _DEBUG

protected:
	std::string name_;
	Player* player_ = nullptr;
};

