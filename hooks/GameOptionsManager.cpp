#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
using namespace app;

void dGameOptionsManager_set_CurrentGameOptions(GameOptionsManager* __this, IGameOptions* value, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dGameOptionsManager_set_CurrentGameOptions executed");
	GameOptionsManager_set_CurrentGameOptions(__this, value, method);
	try {
		GameOptions gameOptions(value);
		if (gameOptions.HasOptions()) {
			SaveGameOptions(gameOptions);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in GameOptionsManager_set_CurrentGameOptions (GameOptionsManager)");
	}
}

int32_t dLogicOptionsHnS_GetCrewmateLeadTime(LogicOptionsHnS* __this, MethodInfo* method) {
	return !MenuState.PanicMode && MenuState.NoSeekerAnim ? 0 : 10; // Anyway it is hardcoded in the game itself to be 10
}


