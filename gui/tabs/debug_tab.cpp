#include "pch-il2cpp.h"
#include "debug_tab.h"
#include "imgui/imgui.h"
#include "state.hpp"
#include "main.h"
#include "game.h"
#include "profiler.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include "gui-helpers.hpp"
using namespace app;

namespace DebugTab {

	void Render() {
		ImGui::SameLine(100 * MenuState.dpiScale);
		ImGui::BeginChild("###Debug", ImVec2(500, 0) * MenuState.dpiScale, true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
#ifndef _VERSION
		if (AnimatedButton("Unload DLL"))
		{
			SetEvent(hUnloadEvent);
		}
		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
#endif
		ToggleButton("Enable Occlusion Culling", &MenuState.OcclusionCulling);
		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

		if (AnimatedButton("Force Load Settings"))
		{
			MenuState.Load();
		}
		if (AnimatedButton("Force Save Settings"))
		{
			MenuState.Save();
		}
		if (AnimatedButton("Clear RPC Queues"))
		{
			MenuState.rpcQueue = std::queue<RPCInterface*>();
			MenuState.lobbyRpcQueue = std::queue<RPCInterface*>();
		}

		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

		if (ToggleButton("SickoLog Unity Debug Messages", &MenuState.ShowUnityLogs)) MenuState.Save();
		if (ToggleButton("SickoLog Hook Debug Messages", &MenuState.ShowHookLogs)) MenuState.Save();

		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

		if (ImGui::CollapsingHeader("Replay##debug"))
		{
			synchronized(Replay::replayEventMutex) {
				size_t numWalkPoints = 0;
				for (const auto& pair : MenuState.replayWalkPolylineByPlayer) {
					numWalkPoints += pair.second.pendingPoints.size() + pair.second.simplifiedPoints.size();
				}
				ImGui::Text("Num Walk Points: %d", numWalkPoints);
				ImGui::Text("Num Live Replay Events: %d", MenuState.liveReplayEvents.size());
				ImGui::Text("Num Live Console Events: %d", MenuState.liveConsoleEvents.size());
			}

			ImGui::Text("ReplayMatchStart: %s", std::format("{:%OH:%OM:%OS}", MenuState.MatchStart).c_str());
			ImGui::Text("ReplayMatchCurrent: %s", std::format("{:%OH:%OM:%OS}", MenuState.MatchCurrent).c_str());
			ImGui::Text("ReplayMatchLive: %s", std::format("{:%OH:%OM:%OS}", std::chrono::system_clock::now()).c_str());
			ImGui::Text("ReplayIsLive: %s", (MenuState.Replay_IsLive) ? "True" : "False");
			ImGui::Text("ReplayIsPlaying: %s", (MenuState.Replay_IsPlaying) ? "True" : "False");

			if (AnimatedButton("Re-simplify polylines (check console)"))
			{
				SYNCHRONIZED(Replay::replayEventMutex);
				for (auto& playerPolylinePair : MenuState.replayWalkPolylineByPlayer)
				{
					std::vector<ImVec2> resimplifiedPoints;
					std::vector<std::chrono::system_clock::time_point> resimplifiedTimeStamps;
					Replay::WalkEvent_LineData& plrLineData = playerPolylinePair.second;
					size_t numOldSimpPoints = plrLineData.simplifiedPoints.size();
					DoPolylineSimplification(plrLineData.simplifiedPoints, plrLineData.simplifiedTimeStamps, resimplifiedPoints, resimplifiedTimeStamps, 50.f, false);
					STREAM_DEBUG("Player[" << playerPolylinePair.first << "]: Re-simplification could reduce " << numOldSimpPoints << " points to " << resimplifiedPoints.size());
				}
			}
		}

		if (ImGui::CollapsingHeader("Colors##debug"))
		{
			il2cpp::Array colArr = app::Palette__TypeInfo->static_fields->PlayerColors;
			auto colArr_raw = colArr.begin();
			size_t length = colArr.size();
			for (size_t i = 0; i < length; i++)
			{
				const app::Color32& col = colArr_raw[i];
				const ImVec4& conv_col = AmongUsColorToImVec4(col);
				static constexpr std::array COLORS = { "Red", "Blue", "Green", "Pink", "Orange", "Yellow", "Black", "White", "Purple", "Brown", "Cyan", "Lime", "Maroon", "Rose", "Banana", "Gray", "Tan", "Coral", "Fortegreen" };
				ImGui::TextColored(conv_col, "%s [%d]: (%d, %d, %d, %d)", COLORS.at(i), i, col.r, col.g, col.b, col.a);
			}
		}

		if (ImGui::CollapsingHeader("Profiler##debug"))
		{
			if (AnimatedButton("Clear Stats"))
			{
				Profiler::ClearStats();
			}

			std::stringstream statStream;
			Profiler::AppendStatStringStream("WalkEventCreation", statStream);
			Profiler::AppendStatStringStream("ReplayRender", statStream);
			Profiler::AppendStatStringStream("ReplayPolyline", statStream);
			Profiler::AppendStatStringStream("PolylineSimplification", statStream);
			Profiler::AppendStatStringStream("ReplayPlayerIcons", statStream);
			Profiler::AppendStatStringStream("ReplayEventIcons", statStream);
			// NOTE:
			// can also just do this to dump all stats, but i like doing them individually so i can control the order better:
			// Profiler::WriteStatsToStream(statStream);

			ImGui::TextUnformatted(statStream.str().c_str());
		}

		ImGui::Text(std::format("Active Scene: {}", MenuState.CurrentScene).c_str());

		ImGui::Text(std::format("Current FPS: {}", GetFps()).c_str());

		if (ImGui::CollapsingHeader("Experiments##debug")) {
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "These features are in development and can break at any time.");
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Use these at your own risk.");
			if (ToggleButton("Point System (Only for Hosting)", &MenuState.TournamentMode)) MenuState.Save();
			if (ToggleButton("April Fools' Mode", &MenuState.AprilFoolsMode)) MenuState.Save();
			static float timer = 0.0f;
			static bool SafeModeNotification = false;
			if (ToggleButton("Safe Mode", &MenuState.SafeMode)) {
				MenuState.Save();
				SafeModeNotification = true;
				timer = static_cast<float>(ImGui::GetTime());
			}

			if (SafeModeNotification) {
				float currentTime = static_cast<float>(ImGui::GetTime());

				if (currentTime - timer < 5.0f) {
					ImGui::SameLine();
					if (MenuState.SafeMode)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Safe Mode is Enabled!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Safe Mode is Disabled! (The likelihood of getting banned increases)");
				}
				else {
					SafeModeNotification = false;
				}
			}

			ImGui::Text("Keep safe mode on in official servers (NA, Europe, Asia) to prevent anticheat detection!");
		}

		ImGui::EndChild();
	}
}


