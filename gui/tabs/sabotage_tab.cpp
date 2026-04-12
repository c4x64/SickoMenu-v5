#include "pch-il2cpp.h"
#include "sabotage_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace app;

namespace SabotageTab {
    // Begone, garbage code.

    void Render() {
        if (IsInGame()) {
            ImGui::SameLine(100 * MenuState.dpiScale);
            ImGui::BeginChild("###Sabotage", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
            ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
            if (IsHost() && ToggleButton("Disable Sabotages", &MenuState.DisableSabotages)) {
                ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
            }
            if (AnimatedButton("Repair Sabotage")) {
                RepairSabotage(*Game::pLocalPlayer);
            }

            if (ToggleButton("Auto Repair Sabotages", &MenuState.AutoRepairSabotage)) {
                MenuState.Save();
            }

            ImGui::NewLine();
            if (MenuState.DisableSabotages)
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled. Nothing can be sabotaged.");
            //i skidded some code from https://github.com/scp222thj/MalumMenu/

            if (AnimatedButton("Sabotage All")) {
                if (MenuState.mapType != Settings::MapType::Fungle) {
                    for (size_t i = 0; i < 5; i++)
                        MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                }

                if (MenuState.mapType == Settings::MapType::Ship || MenuState.mapType == Settings::MapType::Hq || MenuState.mapType == Settings::MapType::Fungle)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
                else if (MenuState.mapType == Settings::MapType::Pb)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
                else if (MenuState.mapType == Settings::MapType::Airship)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));

                if (MenuState.mapType == Settings::MapType::Ship || MenuState.mapType == Settings::MapType::Hq)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));

                if (MenuState.mapType == Settings::MapType::Fungle)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));

                MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128));
            }

            if (AnimatedButton("Random Sabotage")) {
                switch (MenuState.mapType) {
                case Settings::MapType::Pb:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128)); break;
                    case 3: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                case Settings::MapType::Airship:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128)); break;
                    case 3: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                case Settings::MapType::Fungle:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1)); break;
                    case 2: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128)); break;
                    case 3: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                default:
                {
                    int randIndex = randi(1, 4);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128)); break;
                    case 3: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128)); break;
                    case 4: MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                }
            }

            if (MenuState.mapType != Settings::MapType::Fungle && AnimatedButton("Sabotage Lights")) {
                for (size_t i = 0; i < 5; i++)
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
            }
            if (MenuState.mapType == Settings::MapType::Ship || MenuState.mapType == Settings::MapType::Hq || MenuState.mapType == Settings::MapType::Fungle) {
                if (AnimatedButton("Sabotage Reactor")) {
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
                }
            }
            else if (MenuState.mapType == Settings::MapType::Pb) {
                if (AnimatedButton("Sabotage Seismic Stabilizers")) {
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
                }
            }
            else if (MenuState.mapType == Settings::MapType::Airship) {
                if (AnimatedButton("Sabotage Crash Course")) {
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));
                }
            }
            if (MenuState.mapType == Settings::MapType::Ship || MenuState.mapType == Settings::MapType::Hq) {
                if (AnimatedButton("Sabotage Oxygen")) {
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));
                }
            }
            if (MenuState.mapType == Settings::MapType::Fungle) {
                if (AnimatedButton("Activate Mushroom Mixup")) {
                    MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));
                }
            }
            if (AnimatedButton("Sabotage Comms")) {
                MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128));
            }

            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

            if (MenuState.mapType != Settings::MapType::Fungle) {
                if (ToggleButton("Disable Lights", &MenuState.DisableLights)) {
                    if (auto switchSystem = (SwitchSystem*)il2cpp::Dictionary((*Game::pShipStatus)->fields.Systems)[SystemTypes__Enum::Electrical]) {
                        auto actualSwitches = switchSystem->fields.ActualSwitches;
                        auto expectedSwitches = switchSystem->fields.ExpectedSwitches;

                        auto switchMask = 1 << ((MenuState.DisableLights ? 0 : 5) & 0x1F);

                        if ((actualSwitches & switchMask) != ((MenuState.DisableLights ? ~expectedSwitches : expectedSwitches) & switchMask)) {
                            MenuState.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, 5));
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Disable Lights [Auto Moving Switches]", &MenuState.DisableLightSwitches)) MenuState.Save();

            if (ToggleButton("Disable Fix Comms", &MenuState.DisableComms)) MenuState.Save();

            if (ToggleButton("Spam Sabotage Reactor", &MenuState.DisableReactor)) MenuState.Save();

            if ((MenuState.mapType == Settings::MapType::Ship || MenuState.mapType == Settings::MapType::Hq) && ToggleButton("Spam Sabotage Oxygen", &MenuState.DisableOxygen))
                MenuState.Save();

            if (MenuState.mapType == Settings::MapType::Fungle && ToggleButton("Infinite Mushroom Mixup", &MenuState.InfiniteMushroomMixup))
                MenuState.Save();

            ImGui::EndChild();
        }
    }
}


