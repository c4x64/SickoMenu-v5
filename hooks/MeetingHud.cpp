#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include <chrono>
using namespace app;

static app::Type* voteSpreaderType = nullptr;
static bool calloutOver = false;

void dMeetingHud_Awake(MeetingHud* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Awake executed");
	try {
		MenuState.BlinkPlayersTab = true;
		MenuState.voteMonitor.clear();
		MenuState.InMeeting = true;
		static std::string strVoteSpreaderType = translate_type_name("VoteSpreader, Assembly-CSharp");
		voteSpreaderType = app::Type_GetType_1(convert_to_string(strVoteSpreaderType), false, false, nullptr);
		if (MenuState.confuser && MenuState.confuseOnMeeting && !MenuState.PanicMode)
			ControlAppearance(true);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Awake (MeetingHud)");
	}
	MeetingHud_Awake(__this, method);
}

void dMeetingHud_Close(MeetingHud* __this, MethodInfo* method) {
	MenuState.vanishedPlayers.clear();
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Close executed");
	try {
		MenuState.BlinkPlayersTab = true;
		MenuState.InMeeting = false;
		calloutOver = false;
		if (IsHost() && MenuState.TournamentMode && !MenuState.tournamentFirstMeetingOver) MenuState.tournamentFirstMeetingOver = true;
		if (MenuState.Replay_ClearAfterMeeting) {
			Replay::Reset();
			synchronized(Replay::replayEventMutex) {
				MenuState.liveReplayEvents.clear();
			}
			MenuState.MatchStart = std::chrono::system_clock::now();
			MenuState.MatchCurrent = MenuState.MatchStart;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Close (MeetingHud)");
	}
	MeetingHud_Close(__this, method);
}

static void Transform_RemoveVotes(app::Transform* transform, size_t count) {
	auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((Component*)transform, voteSpreaderType, nullptr);
	if (!voteSpreader) return;
	il2cpp::List votes(voteSpreader->fields.Votes);
	const auto length = votes.size();
	if (length == 0) return;
	if (count >= length) {
		for (auto spriteRenderer : votes) {
			Object_DestroyImmediate((app::Object_1*)spriteRenderer, nullptr);
		}
		votes.clear();
		return;
	}
	for (size_t pos = length - 1; pos >= length - count; pos--) {
		Object_DestroyImmediate((app::Object_1*)votes[pos], nullptr);
		votes.erase(pos);
	}
}

static void Transform_RemoveAllVotes(app::Transform* transform) {
	Transform_RemoveVotes(transform, SIZE_MAX);
}

static void Transform_RevealAnonymousVotes(app::Transform* transform, Game::VotedFor votedFor) {
	if (!transform) return;
	auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((Component*)transform, voteSpreaderType, nullptr);
	if (!voteSpreader) return;
	auto votes = il2cpp::List(voteSpreader->fields.Votes);
	if (MenuState.RevealAnonymousVotes) {
		size_t idx = 0;
		for (auto& pair : MenuState.voteMonitor) {
			if (pair.second == votedFor) {
				if (idx >= votes.size()) {
					STREAM_ERROR("votedFor " << ToString(votedFor) << ", index " << idx << ", expected less than " << votes.size());
					break;
				}
				auto outfit = GetPlayerOutfit(GetPlayerDataById(pair.first));
				if (!outfit)
					continue;
				auto ColorId = outfit->fields.ColorId;
				auto spriteRenderer = votes[idx++];
				app::PlayerMaterial_SetColors(ColorId, (app::Renderer*)spriteRenderer, nullptr);
			}
		}
	}
	else {
		for (auto spriteRenderer : votes) {
			app::PlayerMaterial_SetColors_1(
				app::Palette__TypeInfo->static_fields->DisabledGrey,
				(app::Renderer*)spriteRenderer, nullptr);
		}
	}
}

void ManageCallout(uint8_t playerId, uint8_t suspectIdx) {
	if (!(!MenuState.PanicMode && IsHost() && MenuState.TournamentMode)) return;
	/*for (auto p : GetAllPlayerControl()) {
		if (p != *Game::pLocalPlayer) {
			auto playerData = GetPlayerData(p);
			PlayerControl_CmdCheckName(p, GetPlayerOutfit(playerData)->fields.PlayerName, NULL);
		}
	}*/
	uint8_t alivePlayers = 0;
	for (auto p : GetAllPlayerData()) {
		if (!p->fields.IsDead) alivePlayers++;
	}
	if (alivePlayers < 7) return;
	if (suspectIdx == Game::DeadVote || suspectIdx == Game::SkippedVote || suspectIdx == Game::MissedVote || suspectIdx == Game::HasNotVoted)
		return LOG_DEBUG("Callout failed: suspectIdx == Game::DeadVote || suspectIdx == Game::SkippedVote || suspectIdx == Game::MissedVote || suspectIdx == Game::HasNotVoted");
	// Shadow voting
	auto voter = GetPlayerDataById(playerId), target = GetPlayerDataById(suspectIdx);
	if (voter == NULL || target == NULL || target->fields.IsDead)
		return LOG_DEBUG("Callout failed: voter == NULL || target == NULL || target->fields.IsDead");
	std::string voterFc = convert_from_string(voter->fields.FriendCode), targetFc = convert_from_string(target->fields.FriendCode);
	if (!PlayerIsImpostor(voter) &&
		std::find(MenuState.tournamentCallers.begin(), MenuState.tournamentCallers.end(), voterFc) == MenuState.tournamentCallers.end() &&
		std::find(MenuState.tournamentCalledOut.begin(), MenuState.tournamentCalledOut.end(), targetFc) == MenuState.tournamentCalledOut.end()) {
		if (PlayerIsImpostor(target)) {
			UpdatePoints(voter, 1.f); //CorrectCallout
			MenuState.tournamentCalloutPoints[voterFc] += 1.f;
			LOG_DEBUG("Correct callout by " + ToString(target));
			MenuState.tournamentCorrectCallers[voterFc] = target->fields.PlayerId;
		}
		else {
			UpdatePoints(voter, -1.f); //IncorrectCallout
			MenuState.tournamentCalloutPoints[voterFc] -= 1.f;
			LOG_DEBUG("Incorrect callout by " + ToString(target));
		}
		MenuState.tournamentCallers.push_back(voterFc);
		MenuState.tournamentCalledOut.push_back(targetFc);
	}
	else if (PlayerIsImpostor(voter))
		LOG_DEBUG("Callout failed: " + ToString(target) + " is impostor");
	else if (std::find(MenuState.tournamentCallers.begin(), MenuState.tournamentCallers.end(), voterFc) != MenuState.tournamentCallers.end())
		LOG_DEBUG("Callout failed: " + ToString(target) + " has already used callout before");
	else if (std::find(MenuState.tournamentCalledOut.begin(), MenuState.tournamentCalledOut.end(), targetFc) != MenuState.tournamentCalledOut.end())
		LOG_DEBUG("Callout failed: " + ToString(target) + " has already been called out before");
}

void dMeetingHud_PopulateResults(MeetingHud* __this, MeetingHud_VoterState__Array* states, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_PopulateResults executed");
	try {// remove all votes before populating results
		for (auto votedForArea : il2cpp::Array(__this->fields.playerStates)) {
			if (!votedForArea) {
				// oops: game bug
				continue;
			}
			auto transform = app::Component_get_transform((Component*)votedForArea, nullptr);
			Transform_RemoveAllVotes(transform);
		}
		if (__this->fields.SkippedVoting) {
			auto transform = app::GameObject_get_transform(__this->fields.SkippedVoting, nullptr);
			Transform_RemoveAllVotes(transform);
		}

		if (auto exiled = __this->fields.exiledPlayer; exiled != nullptr) {
			synchronized(Replay::replayEventMutex) {
				MenuState.replayDeathTimePerPlayer[exiled->fields.PlayerId] = std::chrono::system_clock::now();
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_PopulateResults (MeetingHud)");
	}
	MeetingHud_PopulateResults(__this, states, method);
}

void RevealAnonymousVotes() {
	if (MenuState.PanicMode || (!MenuState.InMeeting
		|| !app::MeetingHud__TypeInfo
		|| !app::MeetingHud__TypeInfo->static_fields->Instance
		|| !GameOptions().GetBool(app::BoolOptionNames__Enum::AnonymousVotes)))
		return;
	auto meetingHud = app::MeetingHud__TypeInfo->static_fields->Instance;
	for (auto votedForArea : il2cpp::Array(meetingHud->fields.playerStates)) {
		if (!votedForArea) continue;
		auto transform = app::Component_get_transform((Component*)votedForArea, nullptr);
		Transform_RevealAnonymousVotes(transform, votedForArea->fields.TargetPlayerId);
	}
	if (meetingHud->fields.SkippedVoting) {
		auto transform = app::GameObject_get_transform(meetingHud->fields.SkippedVoting, nullptr);
		Transform_RevealAnonymousVotes(transform, Game::SkippedVote);
	}
}

void dMeetingHud_Update(MeetingHud* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Update executed");
	try {
		const bool isBeforeResultsState = __this->fields.state < app::MeetingHud_VoteStates__Enum::Results;
		il2cpp::Array playerStates(__this->fields.playerStates);
		for (auto playerVoteArea : playerStates) {
			if (!playerVoteArea) {
				// oops: game bug
				continue;
			}
			auto playerData = GetPlayerDataById(playerVoteArea->fields.TargetPlayerId);
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			auto playerControl = GetPlayerControlById(playerVoteArea->fields.TargetPlayerId);
			auto playerNameTMP = playerVoteArea->fields.NameText;
			auto outfit = GetPlayerOutfit(playerData);
			std::string playerName = convert_from_string(outfit->fields.PlayerName);
			static uint8_t offset = 0; // This should be unsigned or modulo will give unexpected negative results
			static int offsetCooldown = 0;
			if (!MenuState.PanicMode && (playerData == GetPlayerData(*Game::pLocalPlayer) || MenuState.CustomNameForEveryone) && MenuState.CustomName && !MenuState.ServerSideCustomName && !MenuState.userName.empty()) {
				if (MenuState.CustomName && !MenuState.ServerSideCustomName) {
					playerName = GetCustomName(playerName, false, 0, offset);
					if (offsetCooldown <= 0) {
						offset++;
						offsetCooldown = int(0.2 * GetFps());
					}
					else offsetCooldown--;
				}
			}

			if (playerData && localData && outfit) {
				if (!MenuState.PanicMode && MenuState.PlayerColoredDots)
				{
					Color32&& nameColor = GetPlayerColor(outfit->fields.ColorId);
					std::string dot = std::format("<#{:02x}{:02x}{:02x}{:02x}> ●</color>",
						nameColor.r, nameColor.g, nameColor.b,
						nameColor.a);

					playerName = "<#0000>● </color>" + playerName + dot;
				}
				if (!MenuState.PanicMode && MenuState.RevealRoles)
				{
					std::string roleName = GetRoleName(playerData->fields.Role, MenuState.AbbreviatedRoleNames);
					if (!playerData->fields.Disconnected) {
						int completedTasks = 0;
						int totalTasks = 0;
						auto tasks = GetNormalPlayerTasks(playerControl);
						for (auto task : tasks)
						{
							if (task == nullptr) continue;
							if (task->fields.taskStep == task->fields.MaxStep) {
								completedTasks++;
								totalTasks++;
							}
							else
								totalTasks++;
						}
						std::string tasksText = std::format("({}/{})", completedTasks, totalTasks);
						if (totalTasks == 0 || (PlayerIsImpostor(playerData) && completedTasks == 0))
							playerName = "<size=1.2>" + roleName + "\n</size>" + playerName + "\n<size=1.2><#0000>0";
						else
							playerName = "<size=1.2>" + roleName + " " + tasksText + "\n</size>" + playerName + "\n<size=1.2><#0000>0";
					}
					else
						playerName = "<size=1.2>" + roleName + " (D/C)\n</size>" + playerName + "\n<size=1.2><#0000>0";
					Color32&& roleColor = app::Color32_op_Implicit(GetRoleColor(playerData->fields.Role), NULL);

					playerName = std::format("<color=#{:02x}{:02x}{:02x}{:02x}>{}",
						roleColor.r, roleColor.g, roleColor.b,
						roleColor.a, playerName);
				}

				String* playerNameStr = convert_to_string(playerName);
				TMP_Text_set_text((app::TMP_Text*)playerNameTMP, playerNameStr, NULL);
			}

			if (playerData)
			{
				bool didVote = (playerVoteArea->fields.VotedFor != Game::HasNotVoted);
				// We are goign to check to see if they voted, then we are going to check to see who they voted for, finally we are going to check to see if we already recorded a vote for them
				// votedFor will either contain the id of the person they voted for, 254 if they missed, or 255 if they didn't vote. We don't want to record people who didn't vote
				if (didVote && playerVoteArea->fields.VotedFor != Game::MissedVote
					&& playerVoteArea->fields.VotedFor != Game::DeadVote
					&& MenuState.voteMonitor.find(playerData->fields.PlayerId) == MenuState.voteMonitor.end())
				{
					synchronized(Replay::replayEventMutex) {
						MenuState.liveReplayEvents.emplace_back(std::make_unique<CastVoteEvent>(GetEventPlayer(playerData).value(), GetEventPlayer(GetPlayerDataById(playerVoteArea->fields.VotedFor))));
						MenuState.liveConsoleEvents.emplace_back(std::make_unique<CastVoteEvent>(GetEventPlayer(playerData).value(), GetEventPlayer(GetPlayerDataById(playerVoteArea->fields.VotedFor))));
					}
					MenuState.voteMonitor[playerData->fields.PlayerId] = playerVoteArea->fields.VotedFor;
					STREAM_DEBUG(ToString(playerData) << " voted for " << ToString(playerVoteArea->fields.VotedFor));
					ManageCallout(playerData->fields.PlayerId, playerVoteArea->fields.VotedFor);

					// avoid duplicate votes

					if (isBeforeResultsState) {
						if (playerVoteArea->fields.VotedFor != Game::SkippedVote) {
							for (auto votedForArea : playerStates) {
								if (votedForArea->fields.TargetPlayerId == playerVoteArea->fields.VotedFor) {
									auto transform = app::Component_get_transform((Component*)votedForArea, nullptr);
									MeetingHud_BloopAVoteIcon(__this, playerData, 0, transform, nullptr);
									break;
								}
							}
						}
						else if (__this->fields.SkippedVoting) {
							auto transform = app::GameObject_get_transform(__this->fields.SkippedVoting, nullptr);
							MeetingHud_BloopAVoteIcon(__this, playerData, 0, transform, nullptr);
						}
						RevealAnonymousVotes();
					}
				}
				else if (!didVote && MenuState.voteMonitor.find(playerData->fields.PlayerId) != MenuState.voteMonitor.end())
				{
					auto it = MenuState.voteMonitor.find(playerData->fields.PlayerId);
					auto dcPlayer = it->second;
					MenuState.voteMonitor.erase(it); //Likely disconnected player

					// Remove all votes for disconnected player 
					for (auto votedForArea : playerStates) {
						if (votedForArea->fields.TargetPlayerId == dcPlayer) {
							auto transform = app::Component_get_transform((Component*)votedForArea, nullptr);
							Transform_RemoveVotes(transform, 1); // remove a vote
							break;
						}
					}
				}
			}
		}

		if (isBeforeResultsState) {
			for (auto votedForArea : playerStates) {
				if (!votedForArea) {
					// oops: game bug
					continue;
				}
				auto transform = app::Component_get_transform((Component*)votedForArea, nullptr);
				auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((Component*)transform, voteSpreaderType, nullptr);
				if (!voteSpreader) continue;
				for (auto spriteRenderer : il2cpp::List(voteSpreader->fields.Votes)) {
					auto gameObject = app::Component_get_gameObject((Component*)spriteRenderer, nullptr);
					app::GameObject_SetActive(gameObject, !MenuState.PanicMode && MenuState.RevealVotes, nullptr);
				}
			}

			if (__this->fields.SkippedVoting) {
				bool showSkipped = false;
				for (const auto& pair : MenuState.voteMonitor) {
					if (pair.second == Game::SkippedVote) {
						showSkipped = !MenuState.PanicMode && MenuState.RevealVotes;
						break;
					}
				}
				app::GameObject_SetActive(__this->fields.SkippedVoting, showSkipped, nullptr);
			}
		}
		il2cpp::Array playerStates2(__this->fields.playerStates);
		for (auto voteArea : playerStates2) {
			std::string namePlate = convert_from_string(GetPlayerOutfit(GetPlayerDataById(voteArea->fields.TargetPlayerId))->fields.NamePlateId);
			std::unordered_set<std::string> noPlateSet = { "", "nameplate_NoPlate", "nameplate_Transparent", "nameplate_bsb2_notes", "nameplate_bsb2_breach", "nameplate_bsb2_frame", "nameplate_bsb2_error", "nameplate_racing_beanCar" };
			if (!noPlateSet.contains(namePlate)) continue;
			if (!MenuState.PanicMode && MenuState.CustomGameTheme) {
				auto bg = Color(MenuState.GameBgColor.x, MenuState.GameBgColor.y, MenuState.GameBgColor.z, MenuState.GameBgColor.w);
				SpriteRenderer_set_color(voteArea->fields.Background, bg, NULL);
			}
			else {
				auto col = (!MenuState.PanicMode && MenuState.DarkMode)
					? Color(0.133f, 0.133f, 0.133f, 1.f) : Palette__TypeInfo->static_fields->White;
				SpriteRenderer_set_color(voteArea->fields.Background, col, NULL);
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Update (MeetingHud)");
	}
	app::MeetingHud_Update(__this, method);
}

void dMeetingHud_RpcVotingComplete(MeetingHud* __this, MeetingHud_VoterState__Array* states, NetworkedPlayerInfo* exiled, bool tie, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_RpcVotingComplete executed");
	if (!MenuState.PanicMode) {
		if (MenuState.VoteOffPlayerId == Game::SkippedVote || (MenuState.GodMode && exiled == GetPlayerData(*Game::pLocalPlayer))) {
			exiled = NULL;
		}
		else {
			auto toExileData = GetPlayerDataById(MenuState.VoteOffPlayerId);
			if (toExileData != NULL) {
				exiled = toExileData;
				tie = false;
			}
		}
		MenuState.VoteOffPlayerId = Game::HasNotVoted;
	}
	MeetingHud_RpcVotingComplete(__this, states, exiled, tie, method);
}

bool dLogicOptions_GetAnonymousVotes(LogicOptions* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dLogicOptions_GetAnonymousVotes executed");
	return LogicOptions_GetAnonymousVotes(__this, method);
}


void dMeetingHud_CastVote(MeetingHud* __this, uint8_t playerId, uint8_t suspectIdx, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dLogicOptions_GetAnonymousVotes executed");
	MeetingHud_CastVote(__this, playerId, suspectIdx, method);
}


