#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"
#include "replay.hpp"
#include "profiler.h"
using namespace app;

void dPolusShipStatus_OnEnable(PolusShipStatus* __this, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dPolusShipStatus_OnEnable executed");
	try {
		MenuState.BlinkPlayersTab = false;

		Replay::Reset();

		MenuState.MatchStart = std::chrono::system_clock::now();
		MenuState.MatchCurrent = MenuState.MatchStart;

		MenuState.selectedDoor = SystemTypes__Enum::Hallway;
		MenuState.mapDoors.clear();
		MenuState.pinnedDoors.clear();

		il2cpp::Array allDoors = __this->fields._.AllDoors;

		for (auto door : allDoors) {
			if (std::find(MenuState.mapDoors.begin(), MenuState.mapDoors.end(), door->fields.Room) == MenuState.mapDoors.end())
				MenuState.mapDoors.push_back(door->fields.Room);
		}

		std::sort(MenuState.mapDoors.begin(), MenuState.mapDoors.end());

		if (!MenuState.PanicMode && MenuState.confuser && MenuState.confuseOnStart)
			ControlAppearance(true);

		if (MenuState.AutoFakeRole) {
			if (!MenuState.SafeMode) MenuState.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, (RoleTypes__Enum)MenuState.FakeRole));
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PolusShipStatus_OnEnable (PolusShipStatus)");
	}
	PolusShipStatus_OnEnable(__this, method);
}


