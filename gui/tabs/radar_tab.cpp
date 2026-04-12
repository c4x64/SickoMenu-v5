#include "pch-il2cpp.h"
#include "radar_tab.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "utility.h"
using namespace app;

namespace RadarTab {
	void Render() {
		ImGui::SameLine(100 * MenuState.dpiScale);
		ImGui::BeginChild("###Radar", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
		if (ToggleButton("Show Radar", &MenuState.ShowRadar)) {
			MenuState.Save();
		}

		ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
		ImGui::Separator();
		ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

		if (ToggleButton("Show Dead Bodies", &MenuState.ShowRadar_DeadBodies)) {
			MenuState.Save();
		}
		if (ToggleButton("Show Ghosts", &MenuState.ShowRadar_Ghosts)) {
			MenuState.Save();
		}
		if (ToggleButton("Right Click to Teleport", &MenuState.ShowRadar_RightClickTP)) {
			MenuState.Save();
		}

		ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
		ImGui::Separator();
		ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

		if (ToggleButton("Hide Radar During Meetings", &state.hideRadar_During_Meetings)) {
			MenuState.Save();
		}
		if (ToggleButton("Draw Player Icons", &MenuState.RadarDrawIcons)) {
			MenuState.Save();
		}
		/*if (MenuState.RadarDrawIcons && MenuState.RevealRoles) {
			ImGui::SameLine();
			if (ToggleButton("Show Role Color on Visor", &MenuState.RadarVisorRoleColor)) {
				MenuState.Save();
			}
		}*/

		if (ToggleButton("Lock Radar Position", &MenuState.LockRadar)) {
			MenuState.Save();
		}
		if (ToggleButton("Show Border", &MenuState.RadarBorder)) {
			MenuState.Save();
		}
		if (ImGui::ColorEdit4("Radar Color",
			(float*)&MenuState.SelectedColor,
			ImGuiColorEditFlags__OptionsDefault
			| ImGuiColorEditFlags_NoInputs
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_AlphaPreview)) {
			MenuState.Save();
		}
		if (ImGui::InputInt("Extra Width", &MenuState.RadarExtraWidth)) {
			MenuState.RadarExtraWidth = abs(MenuState.RadarExtraWidth); //prevent negatives
			MenuState.Save();
		}
		if (ImGui::InputInt("Extra Height", &MenuState.RadarExtraHeight)) {
			MenuState.RadarExtraHeight = abs(MenuState.RadarExtraHeight); //prevent negatives
			MenuState.Save();
		}

		ImGui::EndChild();
	}
}



