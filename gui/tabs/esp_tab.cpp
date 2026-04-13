#include "pch-il2cpp.h"
#include "esp_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"
using namespace app;

namespace EspTab {

	void Render() {
		bool changed = false;
		ImGui::SameLine(100 * MenuState.dpiScale);
		ImGui::BeginChild("###ESP", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		changed |= ToggleButton("Enable", &MenuState.ShowEsp);

		changed |= ToggleButton("Show Ghosts", &MenuState.ShowEsp_Ghosts);
		//dead bodies for v3.1
		changed |= ToggleButton("Hide During Meetings", &MenuState.HideEsp_During_Meetings);

		changed |= ToggleButton("Show Boxes", &MenuState.ShowEsp_Box);
		changed |= ToggleButton("Show Tracers", &MenuState.ShowEsp_Tracers);
		changed |= ToggleButton("Show Distances", &MenuState.ShowEsp_Distance);
		//better esp (from noobuild) coming v3.1
		changed |= ToggleButton("Role-based", &MenuState.ShowEsp_RoleBased);

		if (MenuState.ShowEsp_RoleBased) {
			ImGui::SameLine();
			changed |= ToggleButton("Crewmates", &MenuState.ShowEsp_Crew);
			ImGui::SameLine();
			changed |= ToggleButton("Impostors", &MenuState.ShowEsp_Imp);
		}

		ImGui::EndChild();
		if (changed) {
			MenuState.Save();
		}
	}
}



