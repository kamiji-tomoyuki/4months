#include "PlayerBaseState.h"
#ifdef _DEBUG
#include <Logger.h>

using namespace Logger;
#endif // _DEBUG

void PlayerBaseState::ChangeAttackState(std::unique_ptr<PlayerAttackBaseState> state)
{
	attackState_ = std::move(state);
	attackState_->Initialize();
}

#ifdef _DEBUG
// デバッグログ出力
void PlayerBaseState::DebugLog() {
	Log(name_);
}
#endif // _DEBUG

