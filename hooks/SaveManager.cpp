#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"
using namespace app;

// deprecated
bool dSaveManager_GetPurchase(String* itemKey, String* bundleKey, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dSaveManager_GetPurchase executed");
	return true;
}

// v2022.10.25s
bool dPlayerPurchasesData_GetPurchase(PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dPlayerPurchasesData_GetPurchase executed");
	if (MenuState.UnlockCosmetics) {
		return true;
	}
	return PlayerPurchasesData_GetPurchase(__this, itemKey, bundleKey, method);
}


