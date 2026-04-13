#include "pch-il2cpp.h"
#include "settings_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "game.h"
#include "achievements.hpp"
#include "DirectX.h"
#include "imgui/imgui_impl_win32.h" // ImGui_ImplWin32_GetDpiScaleForHwnd
#include "theme.hpp" // ApplyTheme
using namespace app;

namespace SettingsTab {
	enum Groups {
		General,
		Spoofing,
		Customization,
		Keybinds
	};

	static bool openGeneral = true; //default to general tab group
	static bool openSpoofing = false;
	static bool openCustomization = false;
	static bool openKeybinds = false;

	void CloseOtherGroups(Groups group) {
		openGeneral = group == Groups::General;
		openSpoofing = group == Groups::Spoofing;
		openCustomization = group == Groups::Customization;
		openKeybinds = group == Groups::Keybinds;
	}

	void CheckKeybindEdit(bool hotKey) {
		MenuState.KeybindsBeingEdited = MenuState.KeybindsBeingEdited || hotKey;
	}

	void Render() {
		ImGui::SameLine(100 * MenuState.dpiScale);
		ImGui::BeginChild("###Settings", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		if (SickoTabGroup("General", openGeneral)) {
			CloseOtherGroups(Groups::General);
		}
		ImGui::SameLine();
		if (SickoTabGroup("Spoofing", openSpoofing)) {
			CloseOtherGroups(Groups::Spoofing);
		}
		ImGui::SameLine();
		if (SickoTabGroup("Customization", openCustomization)) {
			CloseOtherGroups(Groups::Customization);
		}
		ImGui::SameLine();
		if (SickoTabGroup("Keybinds", openKeybinds)) {
			CloseOtherGroups(Groups::Keybinds);
		}
		if (openGeneral) {
			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
			if (ToggleButton("Show Keybinds", &MenuState.ShowKeybinds)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Allow Activating Keybinds while Chatting", &MenuState.KeybindsWhileChatting)) {
				MenuState.Save();
			}
			if (ToggleButton("Always Show Menu on Startup", &MenuState.ShowMenuOnStartup)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Panic Warning", &MenuState.PanicWarning)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Extra Commands", &MenuState.ExtraCommands)) {
				MenuState.Save();
			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Type \"/help\" in chat to see all available commands.");
			}
			ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

			// sorry to anyone trying to read this code it is pretty messy
#pragma region New config menu, needs fixing
			/*
			std::vector<std::string> CONFIGS = GetAllConfigs();
			CONFIGS.push_back("[New]");
			CONFIGS.push_back("[Delete]");

			std::vector<const char*> CONFIGS_CHAR;

			for (const std::string& str : CONFIGS) {
				char* ch = new char[str.size() + 1];
				std::copy(str.begin(), str.end(), ch);
				ch[str.size()] = '\0';
				CONFIGS_CHAR.push_back(ch);
			}

			bool isNewConfig = CONFIGS.size() == 1;
			bool isDelete = false;

			int& selectedConfigInt = MenuState.selectedConfigInt;
			std::string selectedConfig = CONFIGS[selectedConfigInt];

			if (CustomListBoxInt("Configs", &selectedConfigInt, CONFIGS_CHAR), 100 * MenuState.dpiScale, ImVec4(0,0,0,0), ImGuiComboFlags_NoArrowButton) {
				isNewConfig = selectedConfigInt == CONFIGS.size() - 2;
				isDelete = selectedConfigInt == CONFIGS.size() - 1;
				if (!isNewConfig && !isDelete) MenuState.selectedConfig = CONFIGS[selectedConfigInt];
				MenuState.Save();
				MenuState.Load();
			}

			if (isNewConfig || isDelete) {
				InputString("Name", &MenuState.selectedConfig);
				if (isNewConfig && (AnimatedButton(CheckConfigExists(MenuState.selectedConfig) ? "Overwrite" : "Save"))) {
					MenuState.Save();
					CONFIGS = GetAllConfigs();

					selectedConfigInt = std::distance(CONFIGS.begin(), std::find(CONFIGS.begin(), CONFIGS.end(), MenuState.selectedConfig));
				}

				if (isDelete && CheckConfigExists(MenuState.selectedConfig)) {
					if (AnimatedButton("Delete")) {
						selectedConfigInt--;
						MenuState.Delete();
						CONFIGS = GetAllConfigs();
						if (selectedConfigInt < 0) selectedConfigInt = 0;
					}
				}
			}*/
#pragma endregion

			InputString("Config Name", &MenuState.selectedConfig);

			if (CheckConfigExists(MenuState.selectedConfig) && AnimatedButton("Load Config"))
			{
				MenuState.SaveConfig();
				MenuState.Load();
				MenuState.Save(); //actually save the selected config
			}
			if (CheckConfigExists(MenuState.selectedConfig)) ImGui::SameLine();
			if (AnimatedButton("Save Config"))
			{
				MenuState.Save();
			}
			if (!CheckConfigExists(MenuState.selectedConfig)) {
				ImGui::Text("Config name not found!");
				ImGui::SameLine();
			}

			/*if (ToggleButton("Adjust by DPI", &MenuState.AdjustByDPI)) {
				if (!MenuState.AdjustByDPI) {
					MenuState.dpiScale = 1.0f;
				}
				else {
					MenuState.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
				}
				MenuState.dpiChanged = true;
				MenuState.Save();
			}*/

			/*static const std::vector<const char*> DPI_SCALING_LEVEL = {"50%", "60%", "70%", "80%", "90%", "100%", "110%", "120%", "130%", "140%", "150%", "160%", "170%", "180%", "190%", "200%", "210%", "220%", "230%", "240%", "250%", "260%", "270%", "280%", "290%", "300%"};
			
			int scaleIndex = (int(std::clamp(MenuState.dpiScale, 0.5f, 3.0f) * 100.0f) - 50) / 5;
			if (CustomListBoxInt("Menu Scale", &scaleIndex, DPI_SCALING_LEVEL, 100 * MenuState.dpiScale)) {
				MenuState.dpiScale = (scaleIndex * 10 + 50) / 100.0f;
				MenuState.dpiChanged = true;
			}*/

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			if (ImGui::InputInt("FPS", &MenuState.GameFPS)) {
				MenuState.GameFPS = std::clamp(MenuState.GameFPS, 1, 2147483647);
				MenuState.Save();
			}

			ImGui::Dummy(ImVec2(1, 1) * MenuState.dpiScale);

			if (ToggleButton("Auto-Exit Due To Low FPS", &MenuState.LeaveDueLFPS)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(80);
			ImGui::InputInt("Minimum FPS", &MenuState.minFpsThreshold);
			if (MenuState.minFpsThreshold < 0)
				MenuState.minFpsThreshold = 0;
			ImGui::PopItemWidth();

			ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);

#ifdef _DEBUG
			if (ToggleButton("Show Debug Tab", &MenuState.showDebugTab)) {
				MenuState.Save();
			}
			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
#endif
			if (!IsHost() && !MenuState.SafeMode && !IsNameValid(MenuState.userName)) {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.f, 0.f, MenuState.MenuThemeColor.w));
				if (InputString("Username", &MenuState.userName)) MenuState.Save();
				ImGui::PopStyleColor();
			}
			else if (InputString("Username", &MenuState.userName)) MenuState.Save();

			if (!IsNameValid(MenuState.userName) && !IsHost() && MenuState.SafeMode) {
				if (MenuState.userName == "")
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Empty username gets detected by anticheat. This name will be ignored.");
				if (MenuState.userName.length() >= (size_t)13)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username is too long, gets detected by anticheat. This name will be ignored.");
				else if (!IsNameValid(MenuState.userName))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username contains characters blocked by anticheat. This name will be ignored.");
				else
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username gets detected by anticheat. This name will be ignored.");
			}

			if (IsNameValid(MenuState.userName) || IsHost() || !MenuState.SafeMode) {
				if ((IsInGame() || IsInLobby()) && AnimatedButton("Set Name")) {
					if (IsInGame())
						MenuState.rpcQueue.push(new RpcSetName(MenuState.userName));
					else if (IsInLobby())
						MenuState.lobbyRpcQueue.push(new RpcSetName(MenuState.userName));
					LOG_INFO("Successfully set in-game name to \"" + MenuState.userName + "\"");
				}
				if (IsNameValid(MenuState.userName)) {
					if ((IsInGame() || IsInLobby())) ImGui::SameLine();
					if (AnimatedButton("Set as Account Name")) {
						SetPlayerName(MenuState.userName);
						LOG_INFO("Successfully set account name to \"" + MenuState.userName + "\"");
					}
				}
				ImGui::SameLine();
			}
			if (ToggleButton("Automatically Set Name", &MenuState.SetName)) {
				MenuState.Save();
			}

			if (InputString("Custom Code", &MenuState.customCode)) {
				MenuState.Save();
			}
			if (ToggleButton("Replace Streamer Mode Lobby Code", &MenuState.HideCode)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("RGB Lobby Code", &MenuState.RgbLobbyCode)) {
				MenuState.Save();
			}

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

			static float timer = 0.0f;
			static bool CosmeticsNotification = false;

			if (ToggleButton("Unlock Cosmetics", &MenuState.UnlockCosmetics)) {
				MenuState.Save();
				CosmeticsNotification = true;
				timer = static_cast<float>(ImGui::GetTime());
			}

			if (CosmeticsNotification) {
				float currentTime = static_cast<float>(ImGui::GetTime());
				if (currentTime - timer < 5.0f) {
					ImGui::SameLine();
					if (MenuState.UnlockCosmetics)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Cosmetics Are Unlocked!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cosmetics Are Locked!");
				}
				else {
					CosmeticsNotification = false;
				}
			}

			if (Achievements::IsSupported())
			{
				ImGui::SameLine();
				if (AnimatedButton("Unlock All Achievements"))
					Achievements::UnlockAll();
			}

			if (ToggleButton("Allow other mod users to see you're using", &MenuState.ModDetection)) MenuState.Save();
			ImGui::SameLine();
			if (CustomListBoxInt(" ", &MenuState.BroadcastedMod, MODS, 100.f * MenuState.dpiScale)) MenuState.Save();
		}
		if (openSpoofing) {
			/*if (ToggleButton("Spoof Guest Account", &MenuState.SpoofGuestAccount)) {
				MenuState.Save();
			}
			if (MenuState.SpoofGuestAccount) {
				ImGui::SameLine();
				if (ToggleButton("Use Custom Guest Friend Code", &MenuState.UseNewFriendCode)) {
					MenuState.Save();
				}
				if (MenuState.UseNewFriendCode) {
					if (InputString("Guest Friend Code", &MenuState.NewFriendCode)) {
						MenuState.Save();
					}
					ImGui::Text("Guest friend code should be <= 10 characters long and cannot have a hashtag.");
				}
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Pro Tip: You can bypass the free chat restriction using a space after your custom friend");
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "code!");
			}*/
			/*if (AnimatedButton("Force Login as Guest")) {
				MenuState.ForceLoginAsGuest = true;
			}*/
			if (ToggleButton("Spoof Guest Account (Quick Chat ONLY)", &MenuState.SpoofGuestAccount)) {
				MenuState.Save();
			}
			if (ToggleButton("Use Custom Friend Code (For New/Guest Account ONLY)", &MenuState.UseNewFriendCode)) {
				MenuState.Save();
			}
			if (MenuState.UseNewFriendCode) {
				ImGui::SetNextItemWidth(150 * MenuState.dpiScale); // Adjust the width of the input box
				if (InputString("Friend Code (For New/Guest Account ONLY)", &MenuState.NewFriendCode)) {
					MenuState.Save();
				}
				ImGui::Text("This new friend code should be <= 10 characters long and cannot have spaces.");
			}
			if (ToggleButton("Spoof Level", &MenuState.SpoofLevel)) {
				MenuState.Save();
			}
			if (MenuState.SpoofLevel) {
				ImGui::SameLine();
				if (ImGui::InputInt("Level", &MenuState.FakeLevel, 0, 1)) {
					MenuState.Save();
				}

				if (MenuState.SafeMode && (MenuState.FakeLevel <= 0 || MenuState.FakeLevel > 100001))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Level will be detected by anticheat, your level will be between 0 and 100001.");
			}

			if (ToggleButton("Spoof Platform", &MenuState.SpoofPlatform)) {
				MenuState.Save();
			}
			if (MenuState.SpoofPlatform) {
				ImGui::SameLine();
				if (CustomListBoxInt("Platform", &MenuState.FakePlatform, PLATFORMS))
					MenuState.Save();
			}

			if (MenuState.FakePlatform == 9) {
				if (ToggleButton("Spoof PSN Platform ID", &MenuState.SpoofPsnId)) {
					MenuState.Save();
				}
				if (MenuState.SpoofPsnId)
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(150 * MenuState.dpiScale);
					ImGui::InputScalar("Fake PSN ID", ImGuiDataType_U64, &MenuState.FakePsnId);

					if (AnimatedButton("Random PSN ID")) {
						GeneratePlatformId();
					}
				}
			}

			if (MenuState.FakePlatform == 8) {
				if (ToggleButton("Spoof Xbox Platform ID", &MenuState.SpoofXboxId)) {
					MenuState.Save();
				}
				if (MenuState.SpoofXboxId)
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(150 * MenuState.dpiScale);
					ImGui::InputScalar("Fake Xbox ID", ImGuiDataType_U64, &MenuState.FakeXboxId);

					if (AnimatedButton("Random Xbox ID")) {
						GeneratePlatformId();
					}
				}
			}

			if (ToggleButton("Disable Anticheat While Hosting (+25 Mode)", &MenuState.DisableHostAnticheat)) {
				if (!MenuState.DisableHostAnticheat && MenuState.BattleRoyale) {
					MenuState.BattleRoyale = false;
					MenuState.GameMode = 0;
				}
				MenuState.Save();
			}
			/*if (MenuState.DisableHostAnticheat) {
				BoldText("Warning (+25 Mode)", ImVec4(1.f, 0.f, 0.f, 1.f));
				BoldText("With this option enabled, you can only find public lobbies with +25 enabled.");
				BoldText("You may not find any public lobbies in the game listing due to this.");
				BoldText("This is intended behaviour, do NOT report it as a bug.");
			}*/
			if (ToggleButton("Spoof Among Us Version", &MenuState.SpoofAUVersion))
				MenuState.Save();
			if (MenuState.SpoofAUVersion) {
				ImGui::SameLine();
				if (CustomListBoxInt("Version", &MenuState.FakeAUVersion, AUVERSIONS))
					MenuState.Save();
			}
		}

		if (openCustomization) {
			if (ToggleButton("Hide Watermark", &MenuState.HideWatermark)) {
				MenuState.Save();
			}

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			if (ToggleButton("Light Mode", &MenuState.LightMode)) MenuState.Save();
			ImGui::SameLine();
			if (!MenuState.GradientMenuTheme) {
				if (ImGui::ColorEdit3("Menu Theme Color", (float*)&MenuState.MenuThemeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					MenuState.Save();
				}
			}
			else {
				if (ImGui::ColorEdit3("Gradient Color 1", (float*)&MenuState.MenuGradientColor1, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					MenuState.Save();
				}
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Gradient Color 2", (float*)&MenuState.MenuGradientColor2, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					MenuState.Save();
				}
			}
			ImGui::SameLine();
			if (ToggleButton("Gradient Theme", &MenuState.GradientMenuTheme))
				MenuState.Save();

			if (ToggleButton("Match Background with Theme", &MenuState.MatchBackgroundWithTheme)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("RGB Menu Theme", &MenuState.RgbMenuTheme)) {
				MenuState.Save();
			}
			ImGui::SameLine();
			if (AnimatedButton("Reset Menu Theme"))
			{
				MenuState.MenuThemeColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
			}

			SteppedSliderFloat("Opacity", (float*)&MenuState.MenuThemeColor.w, 0.1f, 1.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			if (ToggleButton("Dark Game Theme", &MenuState.DarkMode)) MenuState.Save();
			ImGui::SameLine();
			if (ToggleButton("Custom Game Theme", &MenuState.CustomGameTheme)) MenuState.Save();

			if (MenuState.CustomGameTheme) {
				if (ImGui::ColorEdit3("Background Color", (float*)&MenuState.GameBgColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
					MenuState.Save();
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Text Color", (float*)&MenuState.GameTextColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
					MenuState.Save();
			}
			if (ToggleButton("Change Chat Font", &MenuState.ChatFont)) {
				MenuState.Save();
			}
			if (MenuState.ChatFont) {
				ImGui::SameLine();
				if (CustomListBoxInt(" ", &MenuState.ChatFontType, FONTS, 160.f * MenuState.dpiScale)) {
					MenuState.Save();
				}
			}

			ImGui::Dummy(ImVec2(4, 4)* MenuState.dpiScale);

			if (ImGui::CollapsingHeader("GUI")) {
				ImGui::SetNextItemWidth(50 * MenuState.dpiScale);
				if (ImGui::InputFloat("Menu Scale", &MenuState.dpiScale)) {
					MenuState.dpiScale = std::clamp(MenuState.dpiScale, 0.5f, 3.f);
					MenuState.dpiChanged = true;
					MenuState.Save();
				}
				if (ToggleButton("Disable Animations", &MenuState.DisableAnimations))
					MenuState.Save();
				if (ImGui::InputFloat("Animation Speed", &MenuState.AnimationSpeed)) {
					if (MenuState.AnimationSpeed <= 0) MenuState.AnimationSpeed = 1.f;
					MenuState.Save();
				}
				SteppedSliderFloat("Rounding Radius Multiplier", &MenuState.RoundingRadiusMultiplier, 0.f, 2.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);
			}

			if (ImGui::CollapsingHeader("Role Colors")) {
				ImGui::ColorEdit4("Crewmate Ghost", (float*)&MenuState.CrewmateGhostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Crewmate", (float*)&MenuState.CrewmateColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Engineer", (float*)&MenuState.EngineerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Guardian Angel", (float*)&MenuState.GuardianAngelColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Scientist", (float*)&MenuState.ScientistColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Impostor", (float*)&MenuState.ImpostorColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Shapeshifter", (float*)&MenuState.ShapeshifterColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Impostor Ghost", (float*)&MenuState.ImpostorGhostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Noisemaker", (float*)&MenuState.NoisemakerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Tracker", (float*)&MenuState.TrackerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Phantom", (float*)&MenuState.PhantomColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				if (AnimatedButton("Reset Role Colors")) {
					MenuState.CrewmateGhostColor = ImVec4(1.f, 1.f, 1.f, 0.5f);
					MenuState.CrewmateColor = ImVec4(1.f, 1.f, 1.f, 1.f);
					MenuState.EngineerColor = ImVec4(0.f, 1.f, 1.f, 1.f);
					MenuState.GuardianAngelColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
					MenuState.ScientistColor = ImVec4(0.2f, 0.2f, 1.f, 1.f);
					MenuState.ImpostorColor = ImVec4(1.f, 0.1f, 0.1f, 1.f);
					MenuState.ShapeshifterColor = ImVec4(1.f, 0.67f, 0.f, 1.f);
					MenuState.ImpostorGhostColor = ImVec4(0.25f, 0.25f, 0.25f, 0.5f);
					MenuState.NoisemakerColor = ImVec4(0.f, 1.f, 0.47f, 1.f);
					MenuState.TrackerColor = ImVec4(0.65f, 0.36f, 1.f, 1.f);
					MenuState.PhantomColor = ImVec4(0.53f, 0.f, 0.f, 1.f);
					MenuState.Save();
				}
			}

			if (ImGui::CollapsingHeader("Other Colors")) {
				ImGui::ColorEdit4("Lobby Host", (float*)&MenuState.HostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Player ID", (float*)&MenuState.PlayerIdColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Player Level", (float*)&MenuState.LevelColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Platform", (float*)&MenuState.PlatformColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Mod Usage", (float*)&MenuState.ModUsageColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Name-Checker", (float*)&MenuState.NameCheckerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Friend Code", (float*)&MenuState.FriendCodeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Dater Names", (float*)&MenuState.DaterNamesColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * MenuState.dpiScale);
				ImGui::ColorEdit4("Lobby Code", (float*)&MenuState.LobbyCodeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Lobby Age", (float*)&MenuState.AgeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				if (AnimatedButton("Reset Other Colors")) {
					MenuState.HostColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					MenuState.PlayerIdColor = ImVec4(1.f, 0.f, 0.f, 1.f);
					MenuState.LevelColor = ImVec4(0.f, 1.f, 0.f, 1.f);
					MenuState.PlatformColor = ImVec4(0.73f, 0.f, 1.f, 1.f);
					MenuState.ModUsageColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					MenuState.NameCheckerColor = ImVec4(1.f, 0.67f, 0.f, 1.f);
					MenuState.FriendCodeColor = ImVec4(0.2f, 0.6f, 1.f, 1.f);
					MenuState.DaterNamesColor = ImVec4(1.f, 0.f, 0.f, 1.f);
					MenuState.LobbyCodeColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					MenuState.AgeColor = ImVec4(0.f, 1.f, 0.f, 1.f);
					MenuState.Save();
				}
			}
		}

		if (openKeybinds) {
			MenuState.KeybindsBeingEdited = false; // This should not stay on permanently

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Menu));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Show/Hide Menu");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Console));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Show/Hide Console");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Radar));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Show/Hide Radar");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Replay));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Show/Hide Replay");

			ImGui::Dummy(ImVec2(4, 4)* MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_ChatAlwaysActive));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Show/Hide Chat");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_ReadGhostMessages));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Read Ghost Messages");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Sicko));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Panic Mode");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Hud));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Enable/Disable HUD");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Freecam));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Freecam");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Zoom));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Zoom");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Noclip));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("NoClip");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Toggle_Autokill));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Autokill");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Reset_Appearance));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Reset Appearance");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Randomize_Appearance));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Confuse Now");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Repair_Sabotage));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Repair All Sabotages");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Close_All_Doors));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Close All Doors");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Close_Current_Room_Door));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Close Current Room Door");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Complete_Tasks));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Complete All Tasks");

			ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);

			CheckKeybindEdit(HotKey(MenuState.KeyBinds.Cancel_Start));
			ImGui::SameLine(100 * MenuState.dpiScale);
			ImGui::Text("Cancel Start Game");
		}
		ImGui::EndChild();
	}
}



