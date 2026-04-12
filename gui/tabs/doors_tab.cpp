#include "pch-il2cpp.h"
#include "doors_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "imgui/imgui.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"
using namespace app;

using namespace std::string_view_literals;

namespace DoorsTab {
	void Render() {
		if (IsInGame() && !MenuState.mapDoors.empty()) {
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::BeginChild("doors#list", ImVec2(200, 0) * MenuState.dpiScale, true, ImGuiWindowFlags_NoBackground);
			bool shouldEndListBox = ImGui::ListBoxHeader("###doors#list", ImVec2(200, 150) * MenuState.dpiScale);
			for (auto systemType : MenuState.mapDoors) {
				if (systemType == SystemTypes__Enum::Decontamination
					|| systemType == SystemTypes__Enum::Decontamination2
					|| systemType == SystemTypes__Enum::Decontamination3) {
					continue;
				}
				bool isOpen;
				auto openableDoor = GetOpenableDoorByRoom(systemType);
				if ("PlainDoor"sv == openableDoor->klass->_0.parent->name
					|| "PlainDoor"sv == openableDoor->klass->_0.name) {
					isOpen = reinterpret_cast<PlainDoor*>(openableDoor)->fields.Open;
				}
				else if ("MushroomWallDoor"sv == openableDoor->klass->_0.name) {
					isOpen = reinterpret_cast<MushroomWallDoor*>(openableDoor)->fields.open;
				}
				else {
					continue;
				}
				if (!(std::find(MenuState.pinnedDoors.begin(), MenuState.pinnedDoors.end(), systemType) == MenuState.pinnedDoors.end()))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f });
					if (ImGui::Selectable(TranslateSystemTypes(systemType), MenuState.selectedDoor == systemType))
						MenuState.selectedDoor = systemType;
					ImGui::PopStyleColor(1);
				}
				else if (!isOpen)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, MenuState.RgbMenuTheme ? MenuState.RgbColor : MenuState.MenuThemeColor);
					if (ImGui::Selectable(TranslateSystemTypes(systemType), MenuState.selectedDoor == systemType))
						MenuState.selectedDoor = systemType;
					ImGui::PopStyleColor(1);
				}
				else
				{
					if (ImGui::Selectable(TranslateSystemTypes(systemType), MenuState.selectedDoor == systemType))
						MenuState.selectedDoor = systemType;
				}
			}
			if (shouldEndListBox)
				ImGui::ListBoxFooter();
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginChild("doors#options", ImVec2(300, 0) * MenuState.dpiScale, false, ImGuiWindowFlags_NoBackground);

			if (IsHost() && MenuState.DisableSabotages) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled.");
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Nothing can be sabotaged.");
			}

			if (AnimatedButton("Close All Doors"))
			{
				for (auto door : MenuState.mapDoors)
				{
					MenuState.rpcQueue.push(new RpcCloseDoorsOfType(door, false));
				}
			}

			if (AnimatedButton("Close Room Door"))
			{
				MenuState.rpcQueue.push(new RpcCloseDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer)), false));
			}

			if (MenuState.mapType == Settings::MapType::Pb || MenuState.mapType == Settings::MapType::Airship || MenuState.mapType == Settings::MapType::Fungle) {
				if (AnimatedButton("Open All Doors"))
				{
					for (auto door : MenuState.mapDoors)
					{
						MenuState.rpcQueue.push(new RpcOpenDoorsOfType(door));
					}
				}

				if (AnimatedButton("Open Room Door"))
				{
					MenuState.rpcQueue.push(new RpcOpenDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer))));
				}
			}

			if (AnimatedButton("Pin All Doors"))
			{
				for (auto door : MenuState.mapDoors)
				{
					if (std::find(MenuState.pinnedDoors.begin(), MenuState.pinnedDoors.end(), door) == MenuState.pinnedDoors.end())
					{
						if (door != SystemTypes__Enum::Decontamination && door != SystemTypes__Enum::Decontamination2 && door != SystemTypes__Enum::Decontamination3)
							MenuState.rpcQueue.push(new RpcCloseDoorsOfType(door, true));
					}
				}
			}
			if (AnimatedButton("Unpin All Doors"))
			{
				MenuState.pinnedDoors.clear();
			}

			ImGui::NewLine();
			if (MenuState.selectedDoor != SystemTypes__Enum::Hallway) {
				auto plainDoor = GetPlainDoorByRoom(MenuState.selectedDoor);

				if (AnimatedButton("Close Door")) {
					MenuState.rpcQueue.push(new RpcCloseDoorsOfType(MenuState.selectedDoor, false));
				}

				if (std::find(MenuState.pinnedDoors.begin(), MenuState.pinnedDoors.end(), MenuState.selectedDoor) == MenuState.pinnedDoors.end()) {
					if (AnimatedButton("Pin Door")) {
						MenuState.rpcQueue.push(new RpcCloseDoorsOfType(MenuState.selectedDoor, true));
					}
				}
				else {
					if (AnimatedButton("Unpin Door")) {
						MenuState.pinnedDoors.erase(std::remove(MenuState.pinnedDoors.begin(), MenuState.pinnedDoors.end(), MenuState.selectedDoor), MenuState.pinnedDoors.end());
					}
				}

				if ((MenuState.mapType == Settings::MapType::Pb || MenuState.mapType == Settings::MapType::Airship || MenuState.mapType == Settings::MapType::Fungle) && AnimatedButton("Open Door"))
				{
					MenuState.rpcQueue.push(new RpcOpenDoorsOfType(MenuState.selectedDoor));
				}
			}
			if (MenuState.mapType == Settings::MapType::Pb || MenuState.mapType == Settings::MapType::Airship || MenuState.mapType == Settings::MapType::Fungle)
			{
				ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
				if (ToggleButton("Auto Open Doors on Use", &MenuState.AutoOpenDoors)) MenuState.Save();

				if (ToggleButton("Spam Open/Close Doors", &MenuState.SpamDoors)) MenuState.Save();
			}
			ImGui::EndChild();
		}
	}
}


