#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "esp.hpp"
#include <iostream>
using namespace app;

static float camHeight = 3.f;
static bool refreshChat = true;

Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dCamera_ScreenToWorldPoint executed");
	try {
		if (!MenuState.PanicMode && (MenuState.GameLoaded || IsInLobby()))
		{
			//Figured it is better to restore the current camera height than using MenuState
			float orthographicSize = Camera_get_orthographicSize(__this, NULL);
			Camera_set_orthographicSize(__this, 3.0f, NULL);
			Vector3 ret = Camera_ScreenToWorldPoint(__this, position, method);
			Camera_set_orthographicSize(__this, orthographicSize, NULL);
			return ret;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in Camera_ScreenToWorldPoint (Camera)"); //better safe than sorry
	}

	return Camera_ScreenToWorldPoint(__this, position, method);
}

void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dFollowerCamera_Update executed");
	try {
		if (!MenuState.PanicMode) {
			if (auto playerToFollow = MenuState.playerToFollow.validate(); playerToFollow.has_value())
			{
				__this->fields.Target = (MonoBehaviour*)playerToFollow.get_PlayerControl();
			}
			else if (__this->fields.Target != (MonoBehaviour*)(*Game::pLocalPlayer)) {
				__this->fields.Target = (MonoBehaviour*)(*Game::pLocalPlayer);
			}
		}
		else __this->fields.Target = (MonoBehaviour*)(*Game::pLocalPlayer);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in FollowerCamera_Update (Camera)");
	}
	FollowerCamera_Update(__this, method);
}


