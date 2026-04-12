#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"
#include "replay.hpp"
#include "profiler.h"
#include "game.h"
using namespace app;

float dShipStatus_CalculateLightRadius(ShipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_CalculateLightRadius executed");
	if (IsHost() && MenuState.TaskSpeedrun && MenuState.GameLoaded && MenuState.mapType != Settings::MapType::Airship)
		MenuState.SpeedrunTimer += Time_get_deltaTime(NULL);
	CosmeticsCache_PopulateFromPlayers(__this->fields._CosmeticsCache_k__BackingField, NULL);
	switch (__this->fields.Type) {
	case ShipStatus_MapType__Enum::Ship:
		if (MenuState.mapType != Settings::MapType::Airship) MenuState.mapType = Settings::MapType::Ship;
		break;
	case ShipStatus_MapType__Enum::Hq:
		MenuState.mapType = Settings::MapType::Hq;
		break;
	case ShipStatus_MapType__Enum::Pb:
		MenuState.mapType = Settings::MapType::Pb;
		break;
	case ShipStatus_MapType__Enum::Fungle:
		MenuState.mapType = Settings::MapType::Fungle;
		break;
	}

	if (!MenuState.PanicMode && MenuState.MaxVision)
		return 420.F;
	else
		return ShipStatus_CalculateLightRadius(__this, player, method);
}

void dShipStatus_OnEnable(ShipStatus* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_OnEnable executed");
	try {
		MenuState.BlinkPlayersTab = false;

		Replay::Reset();

		MenuState.MatchStart = std::chrono::system_clock::now();
		MenuState.MatchCurrent = MenuState.MatchStart;

		MenuState.selectedDoor = SystemTypes__Enum::Hallway;
		MenuState.mapDoors.clear();
		MenuState.pinnedDoors.clear();

		il2cpp::Array allDoors = __this->fields.AllDoors;

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

		//if (!MenuState.mapDoors.empty() && Constants_1_ShouldFlipSkeld(NULL))
			//MenuState.FlipSkeld = true; fix later

		if (MenuState.FlipSkeld && IsHost() && GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) != 3) {
			GameOptions().SetByte(app::ByteOptionNames__Enum::MapId, 3);
			auto gameOptionsManager = GameOptionsManager_get_Instance(NULL);
			GameManager* gameManager = GameManager_get_Instance(NULL);
			GameOptionsManager_set_GameHostOptions(gameOptionsManager, GameOptionsManager_get_CurrentGameOptions(gameOptionsManager, NULL), NULL);
			LogicOptions_SyncOptions(GameManager_get_LogicOptions(gameManager, NULL), NULL);
		}
		if (!IsHost()) MenuState.FlipSkeld = GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) == 3;
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ShipStatus_OnEnable (ShipStatus)");
	}
	ShipStatus_OnEnable(__this, method);
}

void dShipStatus_RpcUpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, int32_t amount, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcUpdateSystem executed");

	ShipStatus_RpcUpdateSystem(__this, systemType, amount, method);
}

void dShipStatus_RpcCloseDoorsOfType(ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcCloseDoorsOfType executed");
	ShipStatus_RpcCloseDoorsOfType(__this, type, method);
}

void dShipStatus_HandleRpc(ShipStatus* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_HandleRpc executed");
	if (callId != 27 && callId != 35) return;
	int32_t pos = reader->fields._position, head = reader->fields.readHead;
	auto systemType = (SystemTypes__Enum)MessageReader_ReadByte(reader, NULL);
	reader->fields._position = pos;
	reader->fields.readHead = head;
	if (systemType == SystemTypes__Enum::Ventilation ||
		(callId == 35 && systemType == SystemTypes__Enum::Security) ||
		systemType == SystemTypes__Enum::Decontamination ||
		systemType == SystemTypes__Enum::Decontamination2 ||
		systemType == SystemTypes__Enum::Decontamination3 ||
		(callId == 35 && systemType == SystemTypes__Enum::MedBay))
		return ShipStatus_HandleRpc(__this, callId, reader, method);
	if (!MenuState.PanicMode && MenuState.DisableSabotages) return;
	ShipStatus_HandleRpc(__this, callId, reader, method);
}

bool DetectCheatSabotageResult(PlayerControl* player, bool result) {
	if (MenuState.Enable_SMAC && MenuState.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
	return result;
}

bool DetectCheatSabotage(SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount) {
	/*uint8_t mapId = (uint8_t)MenuState.mapType;
	if (systemType == SystemTypes__Enum::Sabotage && PlayerIsImpostor(GetPlayerData(player)))
		return false;
	else if (systemType == SystemTypes__Enum::LifeSupp &&
		(mapId == 0 || mapId == 1) && (amount == 64 || amount == 65))
		return false;
	// Only Skeld and Mira have oxygen sabotage
	else if (systemType == SystemTypes__Enum::Comms) {
		if (amount == 0 && mapId != 1 && mapId != 4) return false;
		if ((amount == 64 || amount == 65 || amount == 32 || amount == 33 || amount == 16 || amount == 17)
			&& (mapId == 1 || mapId == 5)) return false;
	}
	else if (systemType == SystemTypes__Enum::Electrical) {
		if (mapId != 4 && amount < 5) return false;
		else if (amount >= 5 && !(MenuState.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	else if (systemType == SystemTypes__Enum::Laboratory &&
		mapId == 2 && (amount == 64 || amount == 65 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::Reactor &&
		mapId != 2 && mapId != 3 && (amount == 64 || amount == 65 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::HeliSabotage &&
		mapId == 3 && (amount == 64 || amount == 65 || amount == 16 || amount == 17 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::MushroomMixupSabotage) {
		if (mapId == 4 && !(MenuState.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	else if (MenuState.InMeeting && MeetingHud__TypeInfo->static_fields->Instance->fields.MenuState != MeetingHud_VoteStates__Enum::Animating) {
		if (!(MenuState.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	return DetectCheatSabotageResult(player, true);*/
	return false;
}

void dShipStatus_UpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_UpdateSystem executed");
	LOG_DEBUG(std::format("SystemType {} updated with amount {}", (std::string)TranslateSystemTypes(systemType), amount).c_str());
	if (systemType == SystemTypes__Enum::Ventilation ||
		systemType == SystemTypes__Enum::Security ||
		systemType == SystemTypes__Enum::Decontamination ||
		systemType == SystemTypes__Enum::Decontamination2 ||
		systemType == SystemTypes__Enum::Decontamination3 ||
		systemType == SystemTypes__Enum::MedBay)
		return ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
	if (!MenuState.PanicMode && MenuState.DisableSabotages) return;
	ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
}

void dShipStatus_AddTasksFromList(ShipStatus* __this, int32_t* start, int32_t count, void* tasks, void* usedTaskTypes, List_1_NormalPlayerTask_* unusedTasks, MethodInfo* method) {
	if (MenuState.DisableMedbayScan) {
		il2cpp::List<List_1_NormalPlayerTask_> tasks = unusedTasks;
		size_t max = tasks.size();
		size_t medScanIndex = max;
		for (size_t i = 0; i < max; i++) {
			if (tasks[i]->fields._.TaskType == TaskTypes__Enum::SubmitScan) {
				medScanIndex = i;
			}
		}
		if (medScanIndex != max) tasks.erase(medScanIndex);
		unusedTasks = tasks.get();
	}
	ShipStatus_AddTasksFromList(__this, start, count, tasks, usedTaskTypes, unusedTasks, method);
}


