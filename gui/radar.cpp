#include "pch-il2cpp.h"
#include "radar.hpp"
#include "DirectX.h"
#include "utility.h"
#include "state.hpp"
#include "gui-helpers.hpp"
using namespace app;

namespace Radar {
	ImU32 GetRadarPlayerColor(NetworkedPlayerInfo* playerData) {
		auto outfit = GetPlayerOutfit(playerData);
		if (outfit == NULL) return ImU32(0);

		return ImGui::ColorConvertFloat4ToU32(AmongUsColorToImVec4((GetPlayerColor(outfit->fields.ColorId))));
	}

	ImU32 GetRadarPlayerColorStatus(NetworkedPlayerInfo* playerData) {
		if (MenuState.RevealRoles && playerData->fields.Role != nullptr)
			return ImGui::ColorConvertFloat4ToU32(AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role)));
		else if (playerData->fields.IsDead)
			return ImGui::ColorConvertFloat4ToU32(AmongUsColorToImVec4(app::Palette__TypeInfo->static_fields->HalfWhite));
		else
			return ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0));
	}

	void SquareConstraint(ImGuiSizeCallbackData* data)
	{
		data->DesiredSize = ImVec2(data->DesiredSize.x, data->DesiredSize.y);
	}

	void OnClick() {
		if (!ImGui::IsKeyPressed(VK_SHIFT) && !ImGui::IsKeyDown(VK_SHIFT) && !ImGui::IsKeyDown(VK_CONTROL) && (ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseDown(ImGuiMouseButton_Right))) {
			ImVec2 mouse = ImGui::GetMousePos();
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 winsize = ImGui::GetWindowSize();

			if (mouse.x < winpos.x
				|| mouse.x > winpos.x + winsize.x
				|| mouse.y < winpos.y
				|| mouse.y > winpos.y + winsize.y)
				return;

			const auto& map = maps[(size_t)MenuState.mapType];
			float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)MenuState.RadarExtraWidth;
			float yOffset = map.y_offset + (float)MenuState.RadarExtraHeight;

			Vector2 target = {
				((mouse.x - winpos.x) / MenuState.dpiScale - xOffset) / map.scale,
				(((mouse.y - winpos.y) / MenuState.dpiScale - yOffset) * -1.F) / map.scale
			};

			static int tpDelay = 0;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) MenuState.rpcQueue.push(new RpcSnapTo(target));
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
				if (tpDelay <= 0) {
					MenuState.rpcQueue.push(new RpcSnapTo(target));
					tpDelay = int(0.1 * GetFps());
				}
				else tpDelay--;
			}
		}
		if (MenuState.TeleportEveryone && !(ImGui::IsKeyDown(VK_CONTROL) || ImGui::IsKeyDown(VK_SHIFT)) && (ImGui::IsKeyPressed(0x12) || ImGui::IsKeyDown(0x12)) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImVec2 mouse = ImGui::GetMousePos();
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 winsize = ImGui::GetWindowSize();

			if (mouse.x < winpos.x
				|| mouse.x > winpos.x + winsize.x
				|| mouse.y < winpos.y
				|| mouse.y > winpos.y + winsize.y)
				return;

			const auto& map = maps[(size_t)MenuState.mapType];
			float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)MenuState.RadarExtraWidth;
			float yOffset = map.y_offset + (float)MenuState.RadarExtraHeight;

			Vector2 target = {
				((mouse.x - winpos.x) / MenuState.dpiScale - xOffset) / map.scale,
				(((mouse.y - winpos.y) / MenuState.dpiScale - yOffset) * -1.F) / map.scale
			};

			for (auto player : GetAllPlayerControl()) {
				MenuState.rpcQueue.push(new RpcForceSnapTo(player, target));
			}
		}
	}

	void Init() {
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), SquareConstraint);
		ImGui::SetNextWindowBgAlpha(0.F);
	}

	bool init = false;
	void Render() {
		if (!init)
			Radar::Init();

		const auto& map = maps[(size_t)MenuState.mapType];
		ImGui::SetNextWindowSize(ImVec2((float)map.mapImage.imageWidth * 0.5F + 10.F + 2.f * MenuState.RadarExtraWidth, (float)map.mapImage.imageHeight * 0.5f + 10.f + 2.f * MenuState.RadarExtraHeight) * MenuState.dpiScale, ImGuiCond_None);

		if (MenuState.LockRadar)
			ImGui::Begin("Radar", &MenuState.ShowRadar, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
		else
			ImGui::Begin("Radar", &MenuState.ShowRadar, ImGuiWindowFlags_NoDecoration);

		ImVec2 winpos = ImGui::GetWindowPos();

		if (MenuState.RadarBorder) {
			const ImVec2 points[] = { {winpos.x + 3.f * MenuState.dpiScale, winpos.y + 1.f * MenuState.dpiScale}, {winpos.x - 5.f * MenuState.dpiScale + ImGui::GetWindowWidth(), winpos.y + 1.f * MenuState.dpiScale}, {winpos.x - 5.f * MenuState.dpiScale + ImGui::GetWindowWidth(), winpos.y + ImGui::GetWindowHeight() - 4.f * MenuState.dpiScale}, {winpos.x + 3.f * MenuState.dpiScale, winpos.y + ImGui::GetWindowHeight() - 4.f * MenuState.dpiScale}, {winpos.x + 3.f * MenuState.dpiScale, winpos.y + 1.f * MenuState.dpiScale} };
			for (size_t i = 0; i < std::size(points); i++) {
				ImGui::GetCurrentWindow()->DrawList->AddLine(points[i], points[i + 1], MenuState.RgbMenuTheme ? ImGui::GetColorU32(ImVec4(MenuState.RgbColor.x, MenuState.RgbColor.y, MenuState.RgbColor.z, MenuState.SelectedColor.w)) : ImGui::GetColorU32(MenuState.SelectedColor), 2.f);
			}
		}

		ImVec4 RadarColor = ImVec4(1.f, 1.f, 1.f, 0.75f);
		if (MenuState.RgbMenuTheme)
			RadarColor = { MenuState.RgbColor.x, MenuState.RgbColor.y, MenuState.RgbColor.z, MenuState.SelectedColor.w };
		else
			RadarColor = MenuState.SelectedColor;

		GameOptions options;

		ImGui::Image((void*)map.mapImage.shaderResourceView,
			ImVec2((float)map.mapImage.imageWidth * 0.5F, (float)map.mapImage.imageHeight * 0.5F) * MenuState.dpiScale,
			ImVec2((float)MenuState.RadarExtraWidth * MenuState.dpiScale, (float)MenuState.RadarExtraHeight * MenuState.dpiScale),
			(MenuState.FlipSkeld && MenuState.mapType == Settings::MapType::Ship) ? ImVec2(1.0f, 0.0f) : ImVec2(0.0f, 0.0f),
			(MenuState.FlipSkeld && MenuState.mapType == Settings::MapType::Ship) ? ImVec2(0.0f, 1.0f) : ImVec2(1.0f, 1.0f),
			RadarColor);

		for (auto player : GetAllPlayerControl()) {
			auto playerData = GetPlayerData(player);

			if (!playerData || (!MenuState.ShowRadar_Ghosts && playerData->fields.IsDead))
				continue;

			if (MenuState.RadarDrawIcons)
				drawPlayerIcon(player, winpos, GetRadarPlayerColor(playerData));
			else
				drawPlayerDot(player, winpos, GetRadarPlayerColor(playerData), GetRadarPlayerColorStatus(playerData));
		}

		if (MenuState.ShowRadar_DeadBodies) {
			for (auto deadBody : GetAllDeadBodies()) {
				auto playerData = GetPlayerDataById(deadBody->fields.ParentId);

				if (MenuState.RadarDrawIcons)
					drawDeadPlayerIcon(deadBody, winpos, GetRadarPlayerColor(playerData));
				else
					drawDeadPlayerDot(deadBody, winpos, GetRadarPlayerColor(playerData));
			}
		}

		if (MenuState.ShowRadar_RightClickTP)
			OnClick();

		ImGui::End();
	}
}


