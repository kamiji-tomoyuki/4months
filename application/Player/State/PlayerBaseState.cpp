#include "PlayerBaseState.h"
#ifdef _DEBUG
#include <Logger.h>

using namespace Logger;
#endif // _DEBUG

#ifdef _DEBUG
// デバッグログ出力
void PlayerBaseState::DebugLog() {
	Log(name_);
}
#endif // _DEBUG

