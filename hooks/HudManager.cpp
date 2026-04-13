#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "utility.h"
#include "game.h"
using namespace app;

static std::string strToLower(std::string str) {
	std::string new_str = "";
	for (auto i : str) {
		new_str += char(std::tolower(i));
	}
	return new_str;
}

static std::string strRev(std::string str) {
	std::string new_str = str;
	std::reverse(new_str.begin(), new_str.end());
	return new_str;
}

void dHudManager_Update(HudManager* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dHudManager_Update executed");
	try {
		static bool bChatAlwaysActivePrevious = false;
		if (bChatAlwaysActivePrevious != MenuState.ChatAlwaysActive)
		{
			if (MenuState.ChatAlwaysActive && !MenuState.PanicMode)
				ChatController_SetVisible(__this->fields.Chat, true, NULL);
			else if (!MenuState.InMeeting && !IsInLobby()) //You will lose chat ability in meeting otherwise
				ChatController_SetVisible(__this->fields.Chat, MenuState.ChatActiveOriginalState, NULL);
			bChatAlwaysActivePrevious = MenuState.ChatAlwaysActive;
		}
		__this->fields.PlayerCam->fields.Locked = MenuState.FreeCam && !MenuState.PanicMode;


		static bool DisableActivation = false; //so a ghost seek button doesn't show up

		if (MenuState.InMeeting)
			HudManager_SetHudActive_1(__this, false, NULL);
		else {
			if (MenuState.DisableHud && !MenuState.PanicMode) {
				HudManager_SetHudActive_1(__this, false, NULL);
				DisableActivation = false;
			}
			else if (!DisableActivation) {
				HudManager_SetHudActive_1(__this, true, NULL);
				DisableActivation = true;
			}
		}

		if ((IsInGame() || IsInLobby())) {
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			GameObject* shadowLayerObject = Component_get_gameObject((Component*)__this->fields.ShadowQuad, NULL);
			if (shadowLayerObject != NULL)
				GameObject_SetActive(shadowLayerObject,
					((!(MenuState.IsRevived || MenuState.FreeCam || MenuState.EnableZoom || MenuState.playerToFollow.has_value() || MenuState.Wallhack || (MenuState.MaxVision && IsInLobby()))))
					&& (localData != NULL && !localData->fields.IsDead),
					NULL);
			if (!localData) {
				// oops: game bug
				return;
			}
			if (IsInGame() || IsInLobby()) {

				if (MenuState.OutfitCooldown == 0) {
					if (!MenuState.CanChangeOutfit && IsInLobby() && !MenuState.PanicMode && MenuState.confuser && MenuState.confuseOnJoin)
						ControlAppearance(true);
					MenuState.CanChangeOutfit = true;
					if (MenuState.ProGamer) {
						std::string rofl = "sesaeler/uneMokciS/yta0g/moc.buhtig//:sptth morf unem eht dedaolnwod ev'uoy erus ekaM\n.uneMokciS fo noisrev dezirohtuanu na gnisu ma I";
						rofl = strRev(rofl);
						PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(rofl), NULL);
						CustomNetworkTransform_RpcSnapTo((*Game::pLocalPlayer)->fields.NetTransform, app::Vector2(0.f, 0.f), NULL);
						(*Game::pLocalPlayer)->fields.moveable = false;
						InnerNetClient_DisconnectInternal((InnerNetClient*)(*Game::pAmongUsClient), DisconnectReasons__Enum::Sanctions, convert_to_string(rofl), NULL);
						InnerNetClient_EnqueueDisconnect((InnerNetClient*)(*Game::pAmongUsClient), DisconnectReasons__Enum::Sanctions, convert_to_string(rofl), NULL);
						MenuState.OutfitCooldown = 50;
						if (MenuState.PanicMode && MenuState.TempPanicMode) {
							MenuState.PanicMode = false;
							MenuState.TempPanicMode = false;
						}
					}
				}
				else if (MenuState.OutfitCooldown == 25) {
					if (MenuState.PanicMode && MenuState.TempPanicMode) {
						MenuState.PanicMode = false;
						MenuState.TempPanicMode = false;
					}
					ChatController_SetVisible(__this->fields.Chat, true, NULL);
					MenuState.OutfitCooldown--;
				}
				else MenuState.OutfitCooldown--;
			}

			if (!MenuState.InMeeting && !MenuState.DisableHud)
			{
				app::RoleBehaviour* playerRole = localData->fields.Role; // Nullable
				app::RoleTypes__Enum role = playerRole != nullptr ? playerRole->fields.Role : app::RoleTypes__Enum::Crewmate;
				GameObject* ImpostorVentButton = app::Component_get_gameObject((Component*)__this->fields.ImpostorVentButton, NULL);

				if (ImpostorVentButton != NULL) {
					if (role == RoleTypes__Enum::Engineer && MenuState.UnlockVents && !MenuState.PanicMode)
					{
						app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
						if (engineerRole->fields.cooldownSecondsRemaining > 0.0f)
							engineerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
						engineerRole->fields.inVentTimeRemaining = 30.0f; //Can be anything as it will always be written
					}
					else if ((GetPlayerData(*Game::pLocalPlayer)->fields.IsDead || IsInLobby()))
					{
						app::GameObject_SetActive(ImpostorVentButton, false, nullptr);
					}
					else
					{
						app::GameObject_SetActive(ImpostorVentButton, (MenuState.UnlockVents && !MenuState.PanicMode) || (((*Game::pLocalPlayer)->fields.inVent && role != RoleTypes__Enum::Engineer)) || (PlayerIsImpostor(localData) && GameOptions().GetGameMode() == GameModes__Enum::Normal), nullptr);
					}
				}

				if ((IsInGame() || (IsInLobby() && MenuState.KillInLobbies))) {
					bool amImpostor = false;
					try {
						amImpostor = PlayerIsImpostor(localData);
					}
					catch (...) {
						LOG_ERROR("Exception occured while fetching whether player is impostor or not.");
					}

					for (auto player : GetAllPlayerControl())
					{
						auto playerInfo = GetPlayerData(player);
						if (!playerInfo) break; //This happens sometimes during loading


						if ((!IsInLobby()) && !MenuState.PanicMode && MenuState.KillImpostors && !playerInfo->fields.IsDead && amImpostor)
							playerInfo->fields.Role->fields.CanBeKilled = true;
						else if (PlayerIsImpostor(playerInfo))
							playerInfo->fields.Role->fields.CanBeKilled = false;
					}
					GameObject* KillButton = app::Component_get_gameObject((Component*)__this->fields.KillButton, NULL);
					if (KillButton != NULL && (IsInGame())) {
						if ((!MenuState.PanicMode && MenuState.UnlockKillButton && (IsHost() || !MenuState.SafeMode) && !localData->fields.IsDead) || amImpostor) {
							app::GameObject_SetActive(KillButton, true, nullptr);
							playerRole->fields.CanUseKillButton = true;
						}
						else {
							app::GameObject_SetActive(KillButton, false, nullptr);
							playerRole->fields.CanUseKillButton = false;
						}
					}
					else if (KillButton != NULL && IsInLobby()) {
						app::GameObject_SetActive(KillButton, false, nullptr);
					}
				}
			}
		}
	}
	catch (...) {
		//LOG_ERROR("Exception occurred in HudManager_Update (HudManager)");
	}
	HudManager_Update(__this, method);
}

void dVersionShower_Start(VersionShower* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dVersionShower_Start executed");
	MenuState.versionShower = __this;
	VersionShower_Start(__this, method);
	MenuState.versionShowerDefaultText = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));

	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm = {};
	localtime_s(&tm, &t);  // Safe version of localtime
	std::ostringstream oss;
	oss << std::put_time(&tm, "%m-%d");
	if (oss.str() == "04-01") MenuState.AprilFoolsMode = true;

	std::string wtf = "lld.unemokcis";
	std::string xd = "lld.noisrev";
	wtf = strRev(wtf);
	xd = strRev(xd);
	std::string lmao = strToLower(MenuState.lol);

	if (lmao != wtf && lmao != xd) {
		MenuState.ProGamer = true;
		if (!MenuState.TempPanicMode) MenuState.PanicMode = false;
		MenuState.HideWatermark = false;
	}

	if (MenuState.PanicMode) return;

	int watermarkSize = 100;
	if (!MenuState.HideWatermark) {
		if (MenuState.CurrentScene == "FindAGame") watermarkSize = 60;
		else if (MenuState.CurrentScene == "MainMenu") watermarkSize = 75;
	}
	std::string spoofVersionText = "";
	if (MenuState.SpoofAUVersion && !MenuState.HideWatermark) {
		switch (MenuState.FakeAUVersion) {
		case 0: // AU v16.0.0 / v16.0.2
			spoofVersionText = " <#fb0>[Spoofing v16.0.2]</color>";
			break;
		case 1: // AU v16.0.5 / v16.1.0
			spoofVersionText = " <#fb0>[Spoofing v16.1.0]</color>";
			break;
		}
	}
	std::string disableHostAnticheatText = MenuState.CurrentScene == "FindAGame" && MenuState.DisableHostAnticheat ? " • <#f00>+25 Mode is ON</color>" : "";
	std::string watermarkOffset = MenuState.CurrentScene == "MMOnline" ? "<#0000>00000</color>" : "";
	std::string sickoText = "<#ff006c>SickoMenu</color>";
	std::string goatText = "<#ef0143>g0aty</color>";
	/*if (!MenuState.HideWatermark) {
		sickoText = GetGradientUsername("SickoMenu", ImVec4(1.f, 0.f, 0.424f, 1.f), ImVec4(0.502f, 0.075f, 0.256f, 1.f));
		goatText = GetGradientUsername("g0aty", ImVec4(0.937f, 0.004f, 0.263f, 1.f), ImVec4(0.529f, 0.008f, 0.157f, 1.f));
	}*/
	std::string watermarkText = /*MenuState.AprilFoolsMode ? std::format(" • {} <#fb0>{}</color> <#ca08ff>[{} Mode]</color> by {}", sickoText,
		MenuState.SickoVersion, MenuState.DiddyPartyMode ? "Diddy Party" : (IsChatCensored() || IsStreamerMode() ? "F***son" : "Fuckson"), goatText) :*/
		std::format(" • {} <#fb0>{}</color> by {}", sickoText, MenuState.SickoVersion, goatText);
	const auto& versionText = std::format("<font=\"Barlow-Regular SDF\"><size={}%>{}{}{}{}{}{}</color></size></font>",
		watermarkSize, MenuState.DarkMode ? "<#666>" : "<#fff>", MenuState.versionShowerDefaultText, spoofVersionText,
		MenuState.HideWatermark ? "" : watermarkText, disableHostAnticheatText, watermarkOffset);
	TMP_Text_set_text((TMP_Text*)MenuState.versionShower->fields.text, convert_to_string(versionText), nullptr);
}

void dPingTracker_Update(PingTracker* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dPingTracker_Update executed");
	__this->fields.gamePos.x = 0.f, __this->fields.lobbyPos.x = -0.09f; // Make the PingTracker actually look centered
	bool isFreeplay = ((InnerNetClient*)(*Game::pAmongUsClient))->fields.NetworkMode == NetworkModes__Enum::FreePlay;
	app::PingTracker_Update(__this, method);
	float initialYdist = IsInGame() ? __this->fields.gamePos.y : __this->fields.lobbyPos.y;
	float camHeight = Camera_get_orthographicSize(MenuState.FollowerCam, NULL);
	if (!MenuState.PanicMode && MenuState.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y = initialYdist + 3 * (camHeight - 1);
	app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
	if (isFreeplay) {
		GameObject_SetActive(Component_get_gameObject((Component*)__this, NULL), true, NULL);
		if ((MenuState.PanicMode && !MenuState.TempPanicMode) || MenuState.OldStylePingText)
			return app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(""), nullptr);
		else {
			__this->fields.aspectPosition->fields.DistanceFromEdge = __this->fields.gamePos;
			if (!MenuState.PanicMode && MenuState.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y = initialYdist + 3 * (camHeight - 1);
		}
	}
	try {
		if (!MenuState.PanicMode || MenuState.TempPanicMode) {
			if (MenuState.OldStylePingText) {
				Vector3 oldDistFromEdge = Vector3(2.3f, 5.9f, 0.f);
				if (!MenuState.PanicMode && MenuState.EnableZoom) oldDistFromEdge.y = initialYdist + 3 * (camHeight - 1);
				__this->fields.aspectPosition->fields.DistanceFromEdge = oldDistFromEdge;
			}
			std::string sep = MenuState.OldStylePingText ? "\n" : " • ";
			std::string ping = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			static int fps = GetFps();
			static int fpsDelay = 0;
			if (fpsDelay <= 0 || GetFps() <= 30) {
				fps = GetFps();
				fpsDelay = int(0.5 * GetFps()); // 0.5 sec delay
			}
			else fpsDelay--;
			std::string fpsText = MenuState.ShowFps ? sep : "";
			if (MenuState.ShowFps) {
				if (fps <= 20) fpsText += std::format("FPS: <#f00>{}</color>", fps);
				else if (fps <= 40) fpsText += std::format("<#ff0>FPS: {}</color>", fps);
				else fpsText += std::format("<#0f0>FPS: {}</color>", fps);
			}
			std::string autoKill = MenuState.AutoKill ? (sep + "<#f00>Autokill</color>") : "";
			std::string noClip = MenuState.NoClip ? (sep + "NoClip") : "";
			std::string freeCam = MenuState.FreeCam ? (sep + "Freecam") : "";
			std::string spectating = "";
			if (auto playerToFollow = MenuState.playerToFollow.validate(); playerToFollow.has_value()) {
				app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(playerToFollow.get_PlayerData());
				Color32 playerColor = GetPlayerColor(outfit->fields.ColorId);
				std::string colorCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>",
					playerColor.r, playerColor.g, playerColor.b, playerColor.a);
				auto name = RemoveHtmlTags(convert_from_string(outfit->fields.PlayerName));
				if (name == "") spectating = sep + "Now Spectating";
				else spectating = sep + "Now Spectating: " + colorCode + name + "</color>";
			}
			uint8_t pingSize = 100;
			if (!MenuState.OldStylePingText) {
				if (!MenuState.HideWatermark || spectating != "") pingSize = 75;
				if (!MenuState.HideWatermark && spectating != "") pingSize = 50;
			}
			std::string hostText = MenuState.ShowHost && IsInGame() ?
				(IsHost() ? (sep + "You are Host") : std::format("{}Host: {}", sep, GetHostUsername(true))) : "";
			std::string voteKicksText = (MenuState.ShowVoteKicks && MenuState.VoteKicks > 0) ? std::format("{}Vote Kicks: {}", sep, MenuState.VoteKicks) : "";
			std::string sickoText = "";
			std::string goatText = "";
			if (!MenuState.HideWatermark) {
				static uint8_t gradientOffset = 0;
				static int gradientDelay = 0;
				sickoText = GetGradientUsername("SickoMenu", ImVec4(1.f, 0.f, 0.424f, 1.f), ImVec4(0.502f, 0.075f, 0.256f, 1.f), gradientOffset);
				goatText = GetGradientUsername("g0aty", ImVec4(0.937f, 0.004f, 0.263f, 1.f), ImVec4(0.529f, 0.008f, 0.157f, 1.f), gradientOffset);
				if (gradientDelay <= 0) {
					gradientOffset++;
					gradientDelay = (int)(0.1 * fps);
				}
				else gradientDelay--;
			}
			std::string watermarkText = /*MenuState.AprilFoolsMode ? std::format("<size={}%>{} <#fb0>{}</color> <#ca08ff>[{} Mode]</color> by {}{}",
				IsInGame() ? pingSize : 100, sickoText, MenuState.SickoVersion, MenuState.DiddyPartyMode ? "Diddy Party" : (IsChatCensored() || IsStreamerMode() ? "F***son" : "Fuckson"), goatText, sep) :*/
				std::format("<size={}%>{} <#fb0>{}</color> by {}{}", IsInGame() ? pingSize : 100, sickoText, MenuState.SickoVersion, goatText, sep);
			std::string pingText = (isFreeplay && !MenuState.OldStylePingText ? "<size=150%><#0000>0</color></size>\n" : "") +
				std::format("{}{}{}{}{}{}{}{}{}{}</color></size>", MenuState.DarkMode ? "<#666>" : "<#fff>",
					MenuState.HideWatermark ? "" : watermarkText, ping, fpsText, hostText, voteKicksText, autoKill, noClip, freeCam, spectating);
			app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, MenuState.OldStylePingText ? 
				app::TextAlignmentOptions__Enum::TopRight : app::TextAlignmentOptions__Enum::Top, nullptr);
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(pingText), nullptr);
		}
		else {
			std::string ping = (isFreeplay ? "<size=150%><#0000>0</color></size>\n" : "") +
				convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(ping), nullptr);
		}
		//"<#0000>00 00</color>" has been added to center the ping text
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PingTracker_Update (HudManager)");
	}
}

bool dLogicGameFlowNormal_IsGameOverDueToDeath(LogicGameFlowNormal* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dLogicGameFlowNormal_IsGameOverDueToDeath executed");
	return false; //fix black screen when you set fake role
}
bool dLogicGameFlowHnS_IsGameOverDueToDeath(LogicGameFlowHnS* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dLogicGameFlowHnS_IsGameOverDueToDeath executed");
	return false; //fix black screen when you set fake role
}

void dModManager_LateUpdate(ModManager* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dModManager_LateUpdate executed");
	ModManager_LateUpdate(__this, method);
}

void dEndGameNavigation_ShowDefaultNavigation(EndGameNavigation* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEndGameNavigation_ShowDefaultNavigation executed");
	EndGameNavigation_ShowDefaultNavigation(__this, method);
}

void dFriendsListUI_UpdateFriendCodeUI(FriendsListUI* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dFriendsListUI_UpdateFriendCodeUI executed");
	FriendsListUI_UpdateFriendCodeUI(__this, method);
}

void dMapCountOverlay_OnEnable(MapCountOverlay* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMapCountOverlay_OnEnable executed");
	MenuState.IsAdminMapOpen = true;
	MapCountOverlay_OnEnable(__this, method);
}

void dMapCountOverlay_OnDisable(MapCountOverlay* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMapCountOverlay_OnDisable executed");
	MenuState.IsAdminMapOpen = false;
	MapCountOverlay_OnDisable(__this, method);
}

void* dIntroCutscene_ShowTeam(IntroCutscene* __this, List_1_PlayerControl_* teamToShow, float duration, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dIntroCutscene_ShowTeam executed");
	return IntroCutscene_ShowTeam(__this, teamToShow, duration, method);
}



