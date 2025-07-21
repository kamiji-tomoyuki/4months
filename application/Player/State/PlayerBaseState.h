#pragma once
#include <string>
#include <memory>
#include "PlayerAttackBaseState.h"


class Player;

class PlayerBaseState
{
public:
	// コンストラクタ
	PlayerBaseState(const std::string& name, Player* player) : name_(name), player_(player) {};

	// デストラクタ
	virtual ~PlayerBaseState() {};

	// ステートの初期化
	virtual void Initialize() = 0;
	// ステートの更新
	virtual void Update() = 0;

	// 攻撃ステートチェンジ
	void ChangeAttackState(std::unique_ptr<PlayerAttackBaseState> state);

#ifdef _DEBUG
	// デバッグログ出力
	virtual void DebugLog();
#endif // _DEBUG

protected:
	std::string name_;
	Player* player_ = nullptr;
	std::unique_ptr<PlayerAttackBaseState> attackState_ = nullptr; // 攻撃ステート
};

