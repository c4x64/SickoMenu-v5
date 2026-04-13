#include "pch-il2cpp.h"
#include "host_tab.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"
#include "gui-helpers.hpp"
using namespace app;

bool editingAutoStartPlayerCount = false;

namespace HostTab {
	enum Groups {
		Utils,
		Settings,
		Tournaments
	};

	static bool openUtils = true; //default to utils tab group
	static bool openSettings = false;
	static bool openTournaments = false;

	static bool hideRolesList = false;

	void CloseOtherGroups(Groups group) {
		openUtils = group == Groups::Utils;
		openSettings = group == Groups::Settings;
		openTournaments = group == Groups::Tournaments;
	}

	/*std::string GetPlayerNameFromFriendCode(std::string friendCode) {
		for (auto p : GetAllPlayerData()) {
			if (p->fields.FriendCode == convert_to_string(friendCode))
				return convert_from_string(GetPlayerOutfit(p)->fields.PlayerName);
		}
		return "";
	}*/ //use if needed

	std::string DisplayScore(float f) {
		return std::format("{}", f == (int)f ? (int)f : f);
	}

	static void SetRoleAmount(RoleTypes__Enum type, int amount) {
		auto&& options = GameOptions().GetRoleOptions();
		auto maxCount = options.GetNumPerGame(type);
		if (amount > maxCount)
			options.SetRoleRate(type, amount, 100);
		else if (amount > 0)
			options.SetRoleRate(type, maxCount, 100);
	}

	void SyncAllSettings() {
		if (IsInGame()) MenuState.rpcQueue.push(new RpcSyncSettings());
		if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcSyncSettings());
	}

	const ptrdiff_t GetRoleCount(RoleType role)
	{
		return std::count_if(MenuState.assignedRoles.cbegin(), MenuState.assignedRoles.cend(), [role](RoleType i) {return i == role; });
	}

	void Render() {
		if (IsHost()) {
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::BeginChild("###Host", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			if (SickoTabGroup("Utils", openUtils)) {
				CloseOtherGroups(Groups::Utils);
			}
			if (GameOptions().HasOptions()) {
				ImGui::SameLine();
				if (SickoTabGroup("Settings", openSettings)) {
					CloseOtherGroups(Groups::Settings);
				}
			}
			if (MenuState.TournamentMode) {
				ImGui::SameLine();
				if (SickoTabGroup("Tournaments", openTournaments)) {
					CloseOtherGroups(Groups::Tournaments);
				}
			}
			GameOptions options;
			if (openUtils) {
				if (IsInLobby()) {
					ImGui::BeginChild("host#list", ImVec2(200, 0) * MenuState.dpiScale, true, ImGuiWindowFlags_NoBackground);
					if (!MenuState.DisableRoleManager && (!hideRolesList || !MenuState.TournamentMode)) {
						bool shouldEndListBox = ImGui::ListBoxHeader("Choose Roles", ImVec2(200, 290) * MenuState.dpiScale);
						auto allPlayers = GetAllPlayerData();
						auto playerAmount = allPlayers.size();
						auto maxImpostorAmount = GetMaxImpostorAmount((int)playerAmount);
						for (size_t index = 0; index < playerAmount; index++) {
							auto playerData = allPlayers[index];
							if (playerData == nullptr) continue;
							PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);
							if (playerCtrl == nullptr) continue;
							MenuState.assignedRolesPlayer[index] = playerCtrl;
							if (MenuState.assignedRolesPlayer[index] == nullptr)
								continue;

							auto outfit = GetPlayerOutfit(playerData);
							if (outfit == NULL) continue;
							const std::string& playerName = convert_from_string(outfit->fields.PlayerName);
							//player colors in host tab by gdjkhp (https://github.com/GDjkhp/AmongUsMenu/commit/53b017183bac503c546f198e2bc03539a338462c)
							if (CustomListBoxInt((playerName + "###" + ToString(playerData)).c_str(), reinterpret_cast<int*>(&MenuState.assignedRoles[index]), ROLE_NAMES, 80 * MenuState.dpiScale, AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId)), 0, RemoveHtmlTags(playerName).c_str()))
							{
								MenuState.engineers_amount = (int)GetRoleCount(RoleType::Engineer);
								MenuState.scientists_amount = (int)GetRoleCount(RoleType::Scientist);
								MenuState.trackers_amount = (int)GetRoleCount(RoleType::Tracker);
								MenuState.noisemakers_amount = (int)GetRoleCount(RoleType::Noisemaker);
								MenuState.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
								MenuState.phantoms_amount = (int)GetRoleCount(RoleType::Phantom);
								MenuState.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
								if (MenuState.impostors_amount + MenuState.shapeshifters_amount + MenuState.phantoms_amount > maxImpostorAmount)
								{
									if (MenuState.assignedRoles[index] == RoleType::Impostor)
										MenuState.assignedRoles[index] = RoleType::Random;
									else if (MenuState.assignedRoles[index] == RoleType::Shapeshifter)
										MenuState.assignedRoles[index] = RoleType::Random;
									else if (MenuState.assignedRoles[index] == RoleType::Phantom)
										MenuState.assignedRoles[index] = RoleType::Random;
									MenuState.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
									MenuState.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
									MenuState.crewmates_amount = (int)GetRoleCount(RoleType::Crewmate);
								}

								if (MenuState.assignedRoles[index] == RoleType::Engineer || MenuState.assignedRoles[index] == RoleType::Scientist ||
									MenuState.assignedRoles[index] == RoleType::Tracker || MenuState.assignedRoles[index] == RoleType::Noisemaker ||
									MenuState.assignedRoles[index] == RoleType::Crewmate) {
									if (MenuState.engineers_amount + MenuState.scientists_amount + MenuState.trackers_amount + MenuState.noisemakers_amount + MenuState.crewmates_amount >= (int)playerAmount)
										MenuState.assignedRoles[index] = RoleType::Random;
								} //Some may set all players to non imps. This hangs the game on beginning. Leave space to Random so we have imps.

								if (options.GetGameMode() == GameModes__Enum::HideNSeek)
								{
									if (MenuState.assignedRoles[index] == RoleType::Shapeshifter)
										MenuState.assignedRoles[index] = RoleType::Impostor;
									else if (MenuState.assignedRoles[index] == RoleType::Phantom)
										MenuState.assignedRoles[index] = RoleType::Impostor;
									else if (MenuState.assignedRoles[index] == RoleType::Tracker)
										MenuState.assignedRoles[index] = RoleType::Engineer;
									else if (MenuState.assignedRoles[index] == RoleType::Noisemaker)
										MenuState.assignedRoles[index] = RoleType::Engineer;
									else if (MenuState.assignedRoles[index] == RoleType::Scientist)
										MenuState.assignedRoles[index] = RoleType::Engineer;
									else if (MenuState.assignedRoles[index] == RoleType::Crewmate)
										MenuState.assignedRoles[index] = RoleType::Engineer;
									else if (MenuState.assignedRoles[index] == RoleType::Engineer) // what?! lmao (see line 98)
										MenuState.assignedRoles[index] = RoleType::Engineer;
								} //Assign other roles in hidenseek causes game bug.
								//These are organized. Do not change the order unless you find it necessary.

								if (!IsInGame()) {
									if (options.GetGameMode() == GameModes__Enum::HideNSeek)
										SetRoleAmount(RoleTypes__Enum::Engineer, 15);
									else
										SetRoleAmount(RoleTypes__Enum::Engineer, MenuState.engineers_amount);
									SetRoleAmount(RoleTypes__Enum::Scientist, MenuState.scientists_amount);
									SetRoleAmount(RoleTypes__Enum::Tracker, MenuState.trackers_amount);
									SetRoleAmount(RoleTypes__Enum::Noisemaker, MenuState.noisemakers_amount);
									SetRoleAmount(RoleTypes__Enum::Shapeshifter, MenuState.shapeshifters_amount);
									SetRoleAmount(RoleTypes__Enum::Phantom, MenuState.phantoms_amount);
									if (options.GetNumImpostors() <= MenuState.impostors_amount + MenuState.shapeshifters_amount + MenuState.phantoms_amount)
										options.SetInt(app::Int32OptionNames__Enum::NumImpostors, MenuState.impostors_amount + MenuState.shapeshifters_amount + MenuState.phantoms_amount);
								}
							}
						}
						if (shouldEndListBox)
							ImGui::ListBoxFooter();
					}
					if (!MenuState.DisableRoleManager) ImGui::NewLine();
					ToggleButton("Disable Role Selection", &MenuState.DisableRoleManager);

					if (MenuState.TournamentMode) {
						if (!MenuState.DisableRoleManager || !hideRolesList) ImGui::NewLine();
						if (AnimatedButton("Randomize Roles")) {
							std::vector<Game::PlayerId> playerIds = {};
							std::vector<Game::PlayerId> impostorIds = {};
							for (auto p : GetAllPlayerControl()) {
								if (p == NULL || GetPlayerData(p) == NULL) continue;
								playerIds.push_back(p->fields.PlayerId);
							}
							int maxImpostors = (std::min)((int)GetAllPlayerControl().size(), GetMaxImpostorAmount(GetAllPlayerControl().size()));
							for (int i = 0; i < maxImpostors; ++i) {
								Game::PlayerId randImpostorId = playerIds[randi(0, playerIds.size() - 1)];
								impostorIds.push_back(randImpostorId);
								playerIds.erase(std::find(playerIds.begin(), playerIds.end(), randImpostorId));
								MenuState.assignedRoles[randImpostorId] = RoleType::Impostor;
							}
							for (auto i : playerIds)
								MenuState.assignedRoles[i] = RoleType::Crewmate;
						}
						ToggleButton("Hide Roles List", &hideRolesList);
					}
					ImGui::EndChild();
				}
				if (IsInLobby()) ImGui::SameLine();
				ImGui::BeginChild("host#actions", ImVec2(300, 0) * MenuState.dpiScale, true, ImGuiWindowFlags_NoBackground);

				if (!MenuState.DisableRoleManager && IsInLobby()) {
					if (ToggleButton("Custom Impostor Amount", &MenuState.CustomImpostorAmount))
						MenuState.Save();
					MenuState.ImpostorCount = std::clamp(MenuState.ImpostorCount, 0, int(Game::MAX_PLAYERS));
					if (MenuState.CustomImpostorAmount && ImGui::InputInt("Impostor Count", &MenuState.ImpostorCount))
						MenuState.Save();

					if (ToggleButton("Always", &MenuState.AutoHostRole)) {
						MenuState.Save();

						if (!MenuState.AutoHostRole) {
							auto allPlayers = GetAllPlayerData();
							for (size_t index = 0; index < allPlayers.size(); index++) {
								auto playerData = allPlayers[index];
								if (playerData == nullptr) continue;
								PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);
								if (playerCtrl == nullptr) continue;

								if (*Game::pLocalPlayer == playerCtrl) {
									MenuState.assignedRoles[index] = RoleType::Random;
									break;
								}
							}
						}
					}
					ImGui::SameLine();
					int hostRoleInt = (int)MenuState.HostRoleToSet;
					if (CustomListBoxInt("###RoleSelector", &hostRoleInt, ROLE_NAMES, 80 * MenuState.dpiScale, ImVec4(1.f, 1.f, 1.f, 0.f), 0, "")) {
						if (MenuState.HostRoleToSet == RoleType::Impostor || MenuState.HostRoleToSet == RoleType::Shapeshifter || MenuState.HostRoleToSet == RoleType::Phantom) {
							if (MenuState.impostors_amount + MenuState.shapeshifters_amount + MenuState.phantoms_amount + 1 > GetMaxImpostorAmount((int)GetAllPlayerData().size())) {
								MenuState.AutoHostRole = false;
							}
							else {
								if (options.GetGameMode() == GameModes__Enum::HideNSeek) MenuState.HostRoleToSet = RoleType::Impostor;
							}
						}
						else {
							if (MenuState.engineers_amount + MenuState.scientists_amount + MenuState.trackers_amount + MenuState.noisemakers_amount + MenuState.crewmates_amount + 1 >= (int)GetAllPlayerData().size()) {
								MenuState.AutoHostRole = false;
							}
							else {
								if (options.GetGameMode() == GameModes__Enum::HideNSeek) MenuState.HostRoleToSet = RoleType::Engineer;
							}
						}
						MenuState.HostRoleToSet = (RoleType)hostRoleInt;
						MenuState.Save();
					}
				}

				const int32_t currentMaxPlayers = options.GetMaxPlayers();
				const int32_t minPlayers = 4, maxAllowedPlayers = static_cast<int32_t>(Game::MAX_PLAYERS);
				int32_t newMaxPlayers = std::clamp(currentMaxPlayers, minPlayers, maxAllowedPlayers);
#define LocalInLobby (((*Game::pAmongUsClient)->fields._.NetworkMode == NetworkModes__Enum::LocalGame) && ((*Game::pAmongUsClient)->fields._.GameState == InnerNetClient_GameStates__Enum::Joined))
				if ((LocalInLobby || !MenuState.SafeMode) && IsInLobby() && ImGui::InputInt("Max Players", &newMaxPlayers)) {
					newMaxPlayers = std::clamp(newMaxPlayers, minPlayers, maxAllowedPlayers);
					GameOptions().SetInt(app::Int32OptionNames__Enum::MaxPlayers, newMaxPlayers);
					SyncAllSettings();
				}


				/*if (IsInLobby() && ToggleButton("Flip Skeld", &MenuState.FlipSkeld))
					MenuState.Save();*/ //to be fixed later
				if (IsInLobby()) ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
				if (IsInLobby() && AnimatedButton("Force Start of Game")) {
					app::InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
				}
				if (IsInLobby() && MenuState.IsStartCountdownActive &&
					ColoredButton(ImVec4(1.f, 0.f, 0.f, 1.f), "Cancel Start of Game")) {
					MenuState.CancelingStartGame = true;
				}

				if (ToggleButton("Always Allow Start Button", &MenuState.AlwaysAllowStart))
					MenuState.Save();

				if (ToggleButton("Modify Start Countdown", &MenuState.ModifyStartCountdown))
					MenuState.Save();

				if (MenuState.ModifyStartCountdown && ImGui::InputInt("Countdown Time", &MenuState.StartCountdown)) {
					MenuState.StartCountdown = std::clamp(MenuState.StartCountdown, 1, 127);
					MenuState.Save();
				}

				if (ToggleButton("Disable Meetings", &MenuState.DisableMeetings))
					MenuState.Save();

				if (ToggleButton("Disable Sabotages", &MenuState.DisableSabotages))
					MenuState.Save();

				if (ToggleButton("Disable All Votekicks", &MenuState.DisableAllVotekicks))
					MenuState.Save();

				std::vector<const char*> GAMEMODES = { "Default", "Task Speedrun" };
				if (MenuState.DisableHostAnticheat) GAMEMODES = { "Default", "Task Speedrun", "Battle Royale" };
				MenuState.GameMode = std::clamp(MenuState.GameMode, 0, MenuState.DisableHostAnticheat ? 2 : 1);
				if (IsInLobby() && CustomListBoxInt("Game Mode", &MenuState.GameMode, GAMEMODES, 100 * MenuState.dpiScale)) {
					if (MenuState.GameMode == 1) {
						MenuState.TaskSpeedrun = true;
						MenuState.BattleRoyale = false;
					}
					else if (MenuState.GameMode == 2) {
						MenuState.TaskSpeedrun = false;
						MenuState.BattleRoyale = true;
					}
					else {
						MenuState.TaskSpeedrun = false;
						MenuState.BattleRoyale = false;
					}
				}

				if (ToggleButton("Spectator Mode", &MenuState.SpectatorMode))
					MenuState.Save();

				if (ToggleButton("Show Lobby Timer", &MenuState.ShowLobbyTimer))
					MenuState.Save();

				if (ToggleButton("Auto Start Game", &MenuState.AutoStartGame))
					MenuState.Save();

				if (MenuState.AutoStartGame) {
					ImGui::Text("Start After");
					ImGui::SameLine();
					if (ImGui::InputInt("sec", &MenuState.AutoStartTimer))
						MenuState.Save();
				}

                if (ToggleButton("Auto Start Game (By Player Count)", &MenuState.AutoStartGamePlayers))
                    MenuState.Save();
                if (MenuState.AutoStartGamePlayers) {
                    ImGui::Text("Start at");
                    ImGui::SameLine();
                    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
                    editingAutoStartPlayerCount = ImGui::IsItemActive();
                    if (ImGui::InputInt("players##autostart", &MenuState.AutoStartPlayerCount, 1, 100, flags)) {
                        MenuState.AutoStartPlayerCount = std::clamp(MenuState.AutoStartPlayerCount, 1, 15);
                        MenuState.Save();
                    }
                    editingAutoStartPlayerCount = ImGui::IsItemActive();
                }

				if (ToggleButton("Ignore RPCs", &MenuState.IgnoreRPCs))
					MenuState.Save();

				//if (MenuState.DisableKills) ImGui::Text("Note: Cheaters can still bypass this feature!");

				/*if (ToggleButton("Disable Specific RPC Call ID", &MenuState.DisableCallId))
					MenuState.Save();
				int callId = MenuState.ToDisableCallId;
				if (ImGui::InputInt("ID to Disable", &callId)) {
					MenuState.ToDisableCallId = (uint8_t)callId;
					MenuState.Save();
				}*/

				if ((MenuState.mapType == Settings::MapType::Airship) && IsInGame() && AnimatedButton("Switch Moving Platform Side"))
				{
					MenuState.rpcQueue.push(new RpcUsePlatform());
				}

				if ((MenuState.mapType == Settings::MapType::Airship) && IsInGame()) {
					if (ToggleButton("Spam Moving Platform", &MenuState.SpamMovingPlatform)) {
						MenuState.Save();
					}
				}

				if (MenuState.InMeeting && AnimatedButton("End Meeting")) {
					MenuState.rpcQueue.push(new RpcEndMeeting());
					MenuState.InMeeting = false;
				}

				if (MenuState.CurrentScene.compare("Tutorial") || IsInLobby()) { //lobby isn't possible in freeplay
					if (ToggleButton("Disable Game Ending", &MenuState.NoGameEnd)) {
						MenuState.Save();
					}

					if (IsInGame()) {
						CustomListBoxInt("Reason", &MenuState.SelectedGameEndReasonId, GAMEENDREASON, 120.0f * MenuState.dpiScale);

						ImGui::SameLine();

						if (AnimatedButton("End Game")) {
							MenuState.rpcQueue.push(new RpcEndGame(GameOverReason__Enum(std::clamp(MenuState.SelectedGameEndReasonId, 0, 8))));
						}
					}
				}

				CustomListBoxInt(" ­", &MenuState.HostSelectedColorId, HOSTCOLORS, 85.0f * MenuState.dpiScale);

				if (ToggleButton("Force Color for Everyone", &MenuState.ForceColorForEveryone)) {
					MenuState.Save();
				}

				if (ToggleButton("Force Name for Everyone", &MenuState.ForceNameForEveryone)) {
					MenuState.Save();
				}
				if (InputString("Username", &MenuState.hostUserName)) {
					MenuState.Save();
				}

				/*if (IsHost() && IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && AnimatedButton("Revive Yourself"))
				{
					if (PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) {
						if (IsInGame()) MenuState.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
						if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
					}
					else {
						if (IsInGame()) MenuState.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
						if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
					}
				}*/

				if (ToggleButton("Unlock Kill Button", &MenuState.UnlockKillButton)) {
					MenuState.Save();
				}

				if (ToggleButton("Kill While Vanished", &MenuState.KillInVanish)) {
					MenuState.Save();
				}

				if (ToggleButton("Disable Medbay Scan", &MenuState.DisableMedbayScan)) {
					MenuState.Save();
				}

				/*if (ToggleButton("Bypass Guardian Angel Protections", &MenuState.BypassAngelProt)) {
					MenuState.Save();
				}*/

				ImGui::EndChild();
			}

			if (openSettings) {
				// AU v2022.8.24 has been able to change maps in lobby.
				MenuState.mapHostChoice = MenuState.FlipSkeld ? 3 : options.GetByte(app::ByteOptionNames__Enum::MapId);
				/*if (MenuState.mapHostChoice > 3)
					MenuState.mapHostChoice--;*/
				MenuState.mapHostChoice = std::clamp(MenuState.mapHostChoice, 0, (int)MAP_NAMES.size() - 1);
				if (IsInLobby() && CustomListBoxInt("Map", &MenuState.mapHostChoice, MAP_NAMES, 75 * MenuState.dpiScale)) {
					//if (!IsInGame()) {
						// disable flip
					if (MenuState.mapHostChoice == 3) {
						options.SetByte(app::ByteOptionNames__Enum::MapId, 0);
						MenuState.FlipSkeld = true;
						SyncAllSettings();
					}
					else {
						options.SetByte(app::ByteOptionNames__Enum::MapId, MenuState.mapHostChoice);
						MenuState.FlipSkeld = false;
						SyncAllSettings();
					}
					/*auto id = MenuState.mapHostChoice;
					if (id >= 3) id++;
					options.SetByte(app::ByteOptionNames__Enum::MapId, id);
					SyncAllSettings();*/
					//}
				}
				auto gamemode = options.GetGameMode();

				auto MakeBool = [&](const char* str, bool& v, BoolOptionNames__Enum opt) {
					if (ToggleButton(str, &v)) {
						options.SetBool(opt, v);
						SyncAllSettings();
					}
					else v = options.GetBool(opt);
					};

				auto MakeInt = [&](const char* str, int& v, Int32OptionNames__Enum opt) {
					if (ImGui::InputInt(str, &v)) {
						options.SetInt(opt, v);
						SyncAllSettings();
					}
					else v = options.GetInt(opt);
					};

				auto MakeFloat = [&](const char* str, float& v, FloatOptionNames__Enum opt) {
					if (ImGui::InputFloat(str, &v)) {
						options.SetFloat(opt, v);
						SyncAllSettings();
					}
					else v = options.GetFloat(opt);
					};

				if (gamemode == GameModes__Enum::Normal || gamemode == GameModes__Enum::NormalFools) {
					static int emergencyMeetings = 1, emergencyCooldown = 1, discussionTime = 1,
						votingTime = 1, killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, taskBarMode = 1;

					static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f;

					static bool ejects = false, anonVotes = false, visualTasks = false;

					// AU v2022.8.24 has been able to change maps in lobby.
					MenuState.mapHostChoice = options.GetByte(app::ByteOptionNames__Enum::MapId);
					if (MenuState.mapHostChoice > 3)
						MenuState.mapHostChoice--;
					MenuState.mapHostChoice = std::clamp(MenuState.mapHostChoice, 0, (int)MAP_NAMES.size() - 1);
					auto gamemode = options.GetGameMode();

					auto MakeBool = [&](const char* str, bool& v, BoolOptionNames__Enum opt) {
						if (ToggleButton(str, &v)) {
							options.SetBool(opt, v);
							SyncAllSettings();
						}
						else v = options.GetBool(opt);
					};

					auto MakeInt = [&](const char* str, int& v, Int32OptionNames__Enum opt) {
						if (ImGui::InputInt(str, &v)) {
							options.SetInt(opt, v);
							SyncAllSettings();
						}
						else v = options.GetInt(opt);
					};

					auto MakeFloat = [&](const char* str, float& v, FloatOptionNames__Enum opt) {
						if (ImGui::InputFloat(str, &v)) {
							options.SetFloat(opt, v);
							SyncAllSettings();
						}
						else v = options.GetFloat(opt);
					};

					if (gamemode == GameModes__Enum::Normal || gamemode == GameModes__Enum::NormalFools) {
						static int emergencyMeetings = 1, emergencyCooldown = 1, discussionTime = 1,
							votingTime = 1, killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, taskBarMode = 1;

						static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f;

						static bool ejects = false, anonVotes = false, visualTasks = false;

#pragma region General
						MakeBool("Confirm Ejects", ejects, BoolOptionNames__Enum::ConfirmImpostor);
						MakeInt("# Emergency Meetings", emergencyMeetings, Int32OptionNames__Enum::NumEmergencyMeetings);
						MakeBool("Anonymous Votes", anonVotes, BoolOptionNames__Enum::AnonymousVotes);
						MakeInt("Emergency Cooldown", emergencyCooldown, Int32OptionNames__Enum::EmergencyCooldown);
						MakeInt("Discussion Time", discussionTime, Int32OptionNames__Enum::DiscussionTime);
						MakeInt("Voting Time", votingTime, Int32OptionNames__Enum::VotingTime);
						MakeFloat("Player Speed", playerSpeed, FloatOptionNames__Enum::PlayerSpeedMod);
						MakeInt("Task Bar Updates", taskBarMode, Int32OptionNames__Enum::TaskBarMode);
						MakeBool("Visual Tasks", visualTasks, BoolOptionNames__Enum::VisualTasks);
						MakeFloat("Crewmate Vision", crewVision, FloatOptionNames__Enum::CrewLightMod);
						MakeFloat("Impostor Vision", impVision, FloatOptionNames__Enum::ImpostorLightMod);
						MakeFloat("Kill Cooldown", killCooldown, FloatOptionNames__Enum::KillCooldown);
						/*if (ImGui::InputFloat("Kill Cooldown", &killCooldown)) {
							if (killCooldown == 0.f) killCooldown = 0.0001f;
							options.SetFloat(FloatOptionNames__Enum::KillCooldown, killCooldown);
							SyncAllSettings();
						}
						else killCooldown = options.GetFloat(FloatOptionNames__Enum::KillCooldown);*/
						MakeInt("Kill Distance", killDistance, Int32OptionNames__Enum::KillDistance);
						MakeInt("# Short Tasks", shortTasks, Int32OptionNames__Enum::NumShortTasks);
						MakeInt("# Common Tasks", commonTasks, Int32OptionNames__Enum::NumCommonTasks);
						MakeInt("# Long Tasks", longTasks, Int32OptionNames__Enum::NumLongTasks);
#pragma endregion
#pragma region Scientist
						ImGui::Text("Scientist");
						static float vitalsCooldown = 1.f, batteryDuration = 1.f;

						MakeFloat("Vitals Display Cooldown", vitalsCooldown, FloatOptionNames__Enum::ScientistCooldown);
						MakeFloat("Battery Duration", batteryDuration, FloatOptionNames__Enum::ScientistBatteryCharge);
#pragma endregion
#pragma region Engineer
						ImGui::Text("Engineer");
						static float ventCooldown = 1.f, ventDuration = 1.f;

						MakeFloat("Vent Use Cooldown", ventCooldown, FloatOptionNames__Enum::EngineerCooldown);
						MakeFloat("Max Time in Vents", ventDuration, FloatOptionNames__Enum::EngineerInVentMaxTime);
#pragma endregion
#pragma region Guardian Angel
						ImGui::Text("Guardian Angel");
						static float protectCooldown = 1.f, protectDuration = 1.f;
						static bool protectVisible = false;

						MakeFloat("Protect Cooldown", protectCooldown, FloatOptionNames__Enum::GuardianAngelCooldown);
						MakeFloat("Protection Duration", protectDuration, FloatOptionNames__Enum::ProtectionDurationSeconds);
						MakeBool("Protect Visible to Impostors", protectVisible, BoolOptionNames__Enum::ImpostorsCanSeeProtect);
#pragma endregion
#pragma region Shapeshifter
						ImGui::Text("Shapeshifter");
						static float shapeshiftDuration = 1.f, shapeshiftCooldown = 1.f;
						static bool shapeshiftEvidence = false;

						MakeFloat("Shapeshift Duration", shapeshiftDuration, FloatOptionNames__Enum::ShapeshifterDuration);
						MakeFloat("Shapeshift Cooldown", shapeshiftCooldown, FloatOptionNames__Enum::ShapeshifterCooldown);
						MakeBool("Leave Shapeshifting Evidence", shapeshiftEvidence, BoolOptionNames__Enum::ShapeshifterLeaveSkin);
#pragma endregion
#pragma region Noisemaker
						ImGui::Text("Noisemaker");
						static float alertDuration = 1.f;

						MakeFloat("Alert Duration", alertDuration, FloatOptionNames__Enum::NoisemakerAlertDuration);
#pragma endregion
#pragma region Tracker
						ImGui::Text("Tracker");
						static float trackerDuration = 1.f, trackerCooldown = 1.f, trackerDelay = 1.f;

						MakeFloat("Tracker Duration", trackerDuration, FloatOptionNames__Enum::TrackerDuration);
						MakeFloat("Tracker Cooldown", trackerCooldown, FloatOptionNames__Enum::TrackerCooldown);
						MakeFloat("Tracker Delay", trackerDelay, FloatOptionNames__Enum::TrackerDelay);
#pragma endregion
#pragma region Phantom
						ImGui::Text("Phantom");
						static float phantomDuration = 1.f, phantomCooldown = 1.f;

						MakeFloat("Phantom Duration", phantomDuration, FloatOptionNames__Enum::PhantomDuration);
						MakeFloat("Phantom Cooldown", phantomCooldown, FloatOptionNames__Enum::PhantomCooldown);
#pragma endregion
					}
				}
#pragma region Hide and Seek
				if (gamemode == GameModes__Enum::HideNSeek || gamemode == GameModes__Enum::SeekFools) {
					static int killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, maxVents = 1;

					static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f,
						hidingTime = 1.f, finalHideTime = 1.f, ventTime = 1.f, crewLight = 1.f, impLight = 1.f,
						finalImpSpeed = 1.f, pingInterval = 1.f;

					static bool flashlight = false, seekMap = false, hidePings = false, showNames = false;

					MakeFloat("Crewmate Vision", crewVision, FloatOptionNames__Enum::CrewLightMod);
					MakeFloat("Impostor Vision", impVision, FloatOptionNames__Enum::ImpostorLightMod);
					MakeFloat("Kill Cooldown", killCooldown, FloatOptionNames__Enum::KillCooldown);
					MakeInt("Kill Distance", killDistance, Int32OptionNames__Enum::KillDistance);
					MakeInt("# Short Tasks", shortTasks, Int32OptionNames__Enum::NumShortTasks);
					MakeInt("# Common Tasks", commonTasks, Int32OptionNames__Enum::NumCommonTasks);
					MakeInt("# Long Tasks", longTasks, Int32OptionNames__Enum::NumLongTasks);
					MakeFloat("Player Speed", playerSpeed, FloatOptionNames__Enum::PlayerSpeedMod);
					MakeFloat("Hiding Time", hidingTime, FloatOptionNames__Enum::EscapeTime);
					MakeFloat("Final Hide Time", finalHideTime, FloatOptionNames__Enum::FinalEscapeTime);
					MakeInt("Max Vent Uses", maxVents, Int32OptionNames__Enum::CrewmateVentUses);
					MakeFloat("Max Time In Vent", ventTime, FloatOptionNames__Enum::CrewmateTimeInVent);
					MakeBool("Flashlight Mode", flashlight, BoolOptionNames__Enum::UseFlashlight);
					MakeFloat("Crewmate Flashlight Size", crewLight, FloatOptionNames__Enum::CrewmateFlashlightSize);
					MakeFloat("Impostor Flashlight Size", impLight, FloatOptionNames__Enum::ImpostorFlashlightSize);
					MakeFloat("Final Hide Impostor Speed", finalImpSpeed, FloatOptionNames__Enum::SeekerFinalSpeed);
					MakeBool("Final Hide Seek Map", seekMap, BoolOptionNames__Enum::SeekerFinalMap);
					MakeBool("Final Hide Pings", hidePings, BoolOptionNames__Enum::SeekerPings);
					MakeFloat("Ping Interval", pingInterval, FloatOptionNames__Enum::MaxPingTime);
					MakeBool("Show Names", showNames, BoolOptionNames__Enum::ShowCrewmateNames);
				}
#pragma endregion
			}
			if (openTournaments && MenuState.TournamentMode) {
				if (AnimatedButton("Copy All Data") && MenuState.tournamentFriendCodes.size() != 0) {
					std::string data = "";
					for (auto i : MenuState.tournamentFriendCodes) {
						float points = MenuState.tournamentPoints[i], win = MenuState.tournamentWinPoints[i],
							callout = MenuState.tournamentCalloutPoints[i], death = MenuState.tournamentEarlyDeathPoints[i];
						std::string text = std::format("\n{}: {} Normal, {} +SV", i, DisplayScore(points), DisplayScore(callout)/*,
							DisplayScore(win), DisplayScore(death)).c_str()*/); // +W, +D are not required anymore
						data += text;
					}
					ClipboardHelper_PutClipboardString(convert_to_string(data.substr(1)), NULL);
				}
				ImGui::SameLine();
				if (ColoredButton(ImVec4(1.f, 0.f, 0.f, 1.f), "Clear All Data")) {
					MenuState.tournamentPoints.clear();
					MenuState.tournamentKillCaps.clear();
					MenuState.tournamentWinPoints.clear();
					MenuState.tournamentCalloutPoints.clear();
					MenuState.tournamentEarlyDeathPoints.clear();
				}

				for (auto i : MenuState.tournamentFriendCodes) {
					float points = MenuState.tournamentPoints[i], win = MenuState.tournamentWinPoints[i],
						callout = MenuState.tournamentCalloutPoints[i], death = MenuState.tournamentEarlyDeathPoints[i];
					std::string text = std::format("{}: {} Normal, {} +SV", i, DisplayScore(points), DisplayScore(callout)/*,
							DisplayScore(win), DisplayScore(death)).c_str()*/); // +W, +D are not required anymore
					if (IsInLobby() && MenuState.ChatCooldown >= 3.f && text.size() <= 120 && AnimatedButton("Send")) {
						//in ideal conditions a message longer than 120 characters should not be possible
						MenuState.lobbyRpcQueue.push(new RpcSendChat(*Game::pLocalPlayer, text));
						MenuState.MessageSent = true;
					}
					if (IsInLobby() && MenuState.ChatCooldown >= 3.f && text.size() <= 120) ImGui::SameLine();
					ImGui::Text(text.c_str());
				}
			}
			ImGui::EndChild();
		}
	}
}



