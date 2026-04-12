#include "pch-il2cpp.h"
#include "replay_tab.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include <chrono>
using namespace app;

namespace ReplayTab {
	void Render() {
		ImGui::SameLine(100 * MenuState.dpiScale);
		ImGui::BeginChild("###Replay", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
		if (ToggleButton("Show Replay", &MenuState.ShowReplay)) {
			MenuState.Save();
		}
		if (ToggleButton("Show Only Last", &MenuState.Replay_ShowOnlyLastSeconds))
		{
			MenuState.Save();
		}
		ImGui::SameLine();
		if (SliderIntV2("Seconds", &MenuState.Replay_LastSecondsValue, 1, 1200, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			MenuState.Save();
		}

		if (ToggleButton("Clear After Meeting", &MenuState.Replay_ClearAfterMeeting))
		{
			MenuState.Save();
		}

		if (ToggleButton("Draw Player Icons", &MenuState.Replay_DrawIcons))
		{
			MenuState.Save();
		}

		if (ImGui::ColorEdit4("Replay Map Color",
			(float*)&MenuState.SelectedReplayMapColor,
			ImGuiColorEditFlags__OptionsDefault
			| ImGuiColorEditFlags_NoInputs
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_AlphaPreview)) {
			MenuState.Save();
		}
		ImGui::EndChild();
	}
}


