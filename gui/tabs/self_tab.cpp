#include "pch-il2cpp.h"
#include "self_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "utility.h"
#include "state.hpp"
#include "logger.h"
using namespace app;

extern void RevealAnonymousVotes(); // in MeetingHud.cpp

namespace SelfTab {
    enum Groups {
        Visuals,
        Utils,
        Randomizers,
        TextEditor
    };

    static bool openVisuals = true; //default to visual tab group
    static bool openUtils = false;
    static bool openRandomizers = false;
    static bool openTextEditor = false;

    static std::string originalText = "";
    static std::string editedText = "";

    static bool italicName = false;
    static bool underlineName = false;
    static bool strikethroughName = false;
    static bool boldName = false;
    static bool nobrName = false;
    static ImVec4 nameColor1 = ImVec4(1.f, 1.f, 1.f, 1.f);
    static ImVec4 nameColor2 = ImVec4(1.f, 1.f, 1.f, 1.f);
    static bool coloredName = false;
    static bool font = false;
    static int fontType = 0;
    static bool resizeName = false;
    static float nameSize = 0.f;
    static bool indentName = false;
    static float indentLevel = 0.f;
    static bool cspaceName = false;
    static float cspaceLevel = 0.f;
    static bool mspaceName = false;
    static float mspaceLevel = 0.f;
    static bool voffsetName = false;
    static float voffsetLevel = 0.f;
    static bool rotateName = false;
    static float rotateAngle = 0.f;

    void CloseOtherGroups(Groups group) {
        openVisuals = group == Groups::Visuals;
        openUtils = group == Groups::Utils;
        openRandomizers = group == Groups::Randomizers;
        openTextEditor = group == Groups::TextEditor;
    }

    std::string GetTextEditorName(std::string str) {
        str = RemoveHtmlTags(str);

        std::string opener = "", closer = "";

        if (coloredName) {
            str = GetGradientUsername(str, nameColor1, nameColor2);
        }

        if (italicName) {
            opener += "<i>";
            closer += "</i>";
        }

        if (underlineName && !coloredName) {
            opener += "<u>";
            closer += "</u>";
        }

        if (strikethroughName && !coloredName) {
            opener += "<s>";
            closer += "</s>";
        }

        if (boldName) {
            opener += "<b>";
            closer += "</b>";
        }

        if (nobrName) {
            opener += "<nobr>";
            closer += "</nobr>";
        }

        if (font) {
            switch (fontType) {
            case 0: {
                opener += "<font=\"Barlow-Italic SDF\">";
                break;
            }
            case 1: {
                opener += "<font=\"Barlow-Medium SDF\">";
                break;
            }
            case 2: {
                opener += "<font=\"Barlow-Bold SDF\">";
                break;
            }
            case 3: {
                opener += "<font=\"Barlow-SemiBold SDF\">";
                break;
            }
            case 4: {
                opener += "<font=\"Barlow-SemiBold Masked\">";
                break;
            }
            case 5: {
                opener += "<font=\"Barlow-ExtraBold SDF\">";
                break;
            }
            case 6: {
                opener += "<font=\"Barlow-BoldItalic SDF\">";
                break;
            }
            case 7: {
                opener += "<font=\"Barlow-BoldItalic Masked\">";
                break;
            }
            case 8: {
                opener += "<font=\"Barlow-Black SDF\">";
                break;
            }
            case 9: {
                opener += "<font=\"Barlow-Light SDF\">";
                break;
            }
            case 10: {
                opener += "<font=\"Barlow-Regular SDF\">";
                break;
            }
            case 11: {
                opener += "<font=\"Barlow-Regular Masked\">";
                break;
            }
            case 12: {
                opener += "<font=\"Barlow-Regular Outline\">";
                break;
            }
            case 13: {
                opener += "<font=\"Brook SDF\">";
                break;
            }
            case 14: {
                opener += "<font=\"LiberationSans SDF\">";
                break;
            }
            case 15: {
                opener += "<font=\"NotoSansJP-Regular SDF\">";
                break;
            }
            case 16: {
                opener += "<font=\"VCR SDF\">";
                break;
            }
            case 17: {
                opener += "<font=\"CONSOLA SDF\">";
                break;
            }
            case 18: {
                opener += "<font=\"digital-7 SDF\">";
                break;
            }
            case 19: {
                opener += "<font=\"OCRAEXT SDF\">";
                break;
            }
            case 20: {
                opener += "<font=\"DIN_Pro_Bold_700 SDF\">";
                break;
            }
            }
            closer += "</font>";
        }

        /*if (MenuState.Material) {
            switch (MenuState.MaterialType) {
            case 0: {
                opener += "<material=\"Barlow-Italic SDF Outline\">";
                break;
            }
            case 1: {
                opener += "<material=\"Barlow-BoldItalic SDF Outline\">";
                break;
            }
            case 2: {
                opener += "<material=\"Barlow-SemiBold SDF Outline\">";
                break;
            }
                    closer += "</material>";
            }
        }*/

        if (resizeName) {
            opener += std::format("<size={}%>", nameSize * 100);
            closer += "</size>";
        }

        if (indentName) {
            opener += std::format("<line-indent={}>", indentLevel);
            closer += "</line-indent>";
        }

        if (cspaceName) {
            opener += std::format("<cspace={}>", cspaceLevel);
            closer += "</cspace>";
        }

        if (mspaceName) {
            opener += std::format("<mspace={}>", mspaceLevel);
            closer += "</mspace>";
        }

        if (voffsetName) {
            opener += std::format("<voffset={}>", voffsetLevel);
            closer += "</voffset>";
        }

        if (rotateName) {
            opener += std::format("<rotate={}>", rotateAngle);
            closer += "<rotate=0>";
        }

        return opener + str + closer;
    }

    void Render() {
        ImGui::SameLine(100 * MenuState.dpiScale);
        ImGui::BeginChild("###Self", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (SickoTabGroup("Visuals", openVisuals)) {
            CloseOtherGroups(Groups::Visuals);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Utils", openUtils)) {
            CloseOtherGroups(Groups::Utils);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Randomizers", openRandomizers)) {
            CloseOtherGroups(Groups::Randomizers);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Text Editor", openTextEditor)) {
            CloseOtherGroups(Groups::TextEditor);
        }

        if (openVisuals) {
            ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
            if (ToggleButton("Max Vision", &MenuState.MaxVision)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Wallhack", &MenuState.Wallhack)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            ToggleButton("Disable HUD", &MenuState.DisableHud);

            if (ToggleButton("Freecam", &MenuState.FreeCam)) {
                MenuState.playerToFollow = {};
                MenuState.Save();
            }

            ImGui::SameLine(130.f * MenuState.dpiScale);
            SteppedSliderFloat("Speed", &MenuState.FreeCamSpeed, 0.f, 10.f, 0.05f, "%.2fx", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            if (ToggleButton("Zoom", &MenuState.EnableZoom)) {
                MenuState.Save();
                if (!MenuState.EnableZoom) RefreshChat();
            }

            ImGui::SameLine(130.f * MenuState.dpiScale);
            SteppedSliderFloat("Scale", &MenuState.CameraHeight, 0.5f, 10.0f, 0.1f, "%.2fx", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

            if (ToggleButton("Always show Chat Button", &MenuState.ChatAlwaysActive)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Allow Ctrl+(C/V) in Chat", &MenuState.ChatPaste)) { //add copying later
                MenuState.Save();
            }

            if (ToggleButton("Read Messages by Ghosts", &MenuState.ReadGhostMessages)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Read and Send SickoChat", &MenuState.ReadAndSendSickoChat)) {
                MenuState.Save();
            }
            if (MenuState.ReadAndSendSickoChat) ImGui::Text("Send SickoChat messages in regular chat by typing \"/sc [message]\"!");
            /*static int framesPassed = 0;
            if (AnimatedButton("Refresh Chat Button")) {
                MenuState.RefreshChatButton = true;
                framesPassed = 100;
            }

            if (framesPassed == 0) MenuState.RefreshChatButton = false;
            else framesPassed--;*/

            if (!IsHost() && MenuState.SafeMode) {
                ImGui::Text("Custom names are purely CLIENT-SIDED without host/disabled safe mode!");
            }
            if (ToggleButton("Custom Name", &MenuState.CustomName)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Custom Name for Everyone", &MenuState.CustomNameForEveryone)) {
                MenuState.Save();
            }

            if ((IsHost() || !MenuState.SafeMode)) {
                if (ToggleButton("Server-sided Custom Name", &MenuState.ServerSideCustomName)) {
                    MenuState.Save();
                }
            }

            if (MenuState.CustomName && ImGui::CollapsingHeader("Custom Name Options"))
            {
                if (ToggleButton("Italics", &MenuState.ItalicName)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Underline", &MenuState.UnderlineName)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Strikethrough", &MenuState.StrikethroughName)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Bold", &MenuState.BoldName)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Nobr", &MenuState.NobrName)) {
                    MenuState.Save();
                }

                if (ImGui::ColorEdit4("Starting Gradient Color", (float*)&MenuState.NameColor1, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ImGui::ColorEdit4("Ending Gradient Color", (float*)&MenuState.NameColor2, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Colored", &MenuState.ColoredName)) {
                    MenuState.Save();
                }

                if (ToggleButton("RGB", &MenuState.RgbName)) {
                    MenuState.Save();
                }

                if (CustomListBoxInt("Gradient Method", &MenuState.ColorMethod, { "Static", "Left-to-Right" }, 80.f * MenuState.dpiScale))
                    MenuState.Save();
                ImGui::SameLine();
                if (CustomListBoxInt("RGB Method", &MenuState.RgbMethod, { "All-at-Once", "Left-to-Right" }, 80.f * MenuState.dpiScale))
                    MenuState.Save();

                if (ToggleButton("Enable Prefix and Suffix", &MenuState.UsePrefixAndSuffix)) MenuState.Save();
                if (ToggleButton("New Lines for Prefix and Suffix", &MenuState.PrefixAndSuffixNewLines)) MenuState.Save();

                if (InputString("Name Prefix", &MenuState.NamePrefix)) MenuState.Save();
                if (InputString("Name Suffix", &MenuState.NameSuffix)) MenuState.Save();
                if (MenuState.UsePrefixAndSuffix) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Note: Prefix and/or suffix will be cleared from the ends of the name if it contains them."));
                if (MenuState.UsePrefixAndSuffix) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("This is done to prevent name overflowing."));

                if (ToggleButton("Font", &MenuState.Font)) {
                    MenuState.Save();
                }
                if (MenuState.Font) {
                    ImGui::SameLine();
                    if (CustomListBoxInt(" ", &MenuState.FontType, FONTS, 160.f * MenuState.dpiScale)) {
                        MenuState.Save();
                    }
                }
                if (MenuState.Font) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Note: The white nickname will not be visible in the chat"));

                /*if (ToggleButton("Material", &MenuState.Material)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (CustomListBoxInt(" Some materials are not supported", &MenuState.MaterialType, MATERIALS, 160.f * MenuState.dpiScale)) {
                    MenuState.Save();
                }*/

                if (ToggleButton("Size", &MenuState.ResizeName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Name Size", &MenuState.NameSize)) {
                    MenuState.Save();
                }

                if (ToggleButton("Indent", &MenuState.IndentName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Name Indent", &MenuState.NameIndent)) {
                    MenuState.Save();
                }

                if (ToggleButton("Cspace", &MenuState.CspaceName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Name Cspace", &MenuState.NameCspace)) {
                    MenuState.Save();
                }

                if (ToggleButton("Mspace", &MenuState.MspaceName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Name Mspace", &MenuState.NameMspace)) {
                    MenuState.Save();
                }

                if (ToggleButton("Voffset", &MenuState.VoffsetName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Name Voffset", &MenuState.NameVoffset)) {
                    MenuState.Save();
                }
                if (ToggleButton("Rotate", &MenuState.RotateName)) {
                    MenuState.Save();
                }

                ImGui::SameLine();
                if (ImGui::InputFloat("Rotation Angle", &MenuState.NameRotate)) {
                    MenuState.Save();
                }
                ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            }

            if (ToggleButton("Reveal Roles", &MenuState.RevealRoles)) {
                MenuState.Save();
            }
            ImGui::SameLine(120.0f * MenuState.dpiScale);
            if (ToggleButton("Abbrv. Role", &MenuState.AbbreviatedRoleNames))
            {
                MenuState.Save();
            }
            ImGui::SameLine(240.0f * MenuState.dpiScale);
            if (ToggleButton("Player Colored Dots Next To Names", &MenuState.PlayerColoredDots))
            {
                MenuState.Save();
            }

            if (ToggleButton("Show Player Info in Lobby", &MenuState.ShowPlayerInfo))
            {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Show Lobby Info", &MenuState.ShowLobbyInfo))
            {
                MenuState.Save();
            }

            if (ToggleButton("Reveal Votes", &MenuState.RevealVotes)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Reveal Anonymous Votes", &MenuState.RevealAnonymousVotes)) {
                MenuState.Save();
                RevealAnonymousVotes();
            }

            if (ToggleButton("See Ghosts", &MenuState.ShowGhosts)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("See Phantoms", &MenuState.ShowPhantoms)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("See Players In Vents", &MenuState.ShowPlayersInVents)) {
                MenuState.Save();
            }

            if (ToggleButton("See Protections", &MenuState.ShowProtections))
            {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("See Kill Cooldown", &MenuState.ShowKillCD)) {
                MenuState.Save();
            }

            if (ToggleButton("Disable Kill Animation", &MenuState.DisableKillAnimation)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Disable Lobby Music", &MenuState.DisableLobbyMusic)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Old Ping Text", &MenuState.OldStylePingText)) MenuState.Save();

            if (ToggleButton("Show Host", &MenuState.ShowHost)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Show Vote Kicks", &MenuState.ShowVoteKicks)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Show FPS", &MenuState.ShowFps)) {
                MenuState.Save();
            }

            if (ToggleButton("Show Chat Cooldown", &MenuState.ShowChatTimer)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Extend Chat Character Limit", &MenuState.ExtendChatLimit)) {
                MenuState.Save();
            }

            if (ToggleButton("Extend Chat History", &MenuState.ExtendChatHistory)) {
                MenuState.Save();
            }

            /*if (ToggleButton("Change Body Type", &MenuState.ChangeBodyType)) {
                MenuState.Save();
            }
            if (MenuState.ChangeBodyType) {
                ImGui::SameLine();
                if (CustomListBoxInt("Type", &MenuState.BodyType, BODYTYPES, 75.f * MenuState.dpiScale))
                    MenuState.Save();
            }*/

            if (MenuState.InMeeting && AnimatedButton("Move in Meeting"))
            {
                if (IsHost()) MenuState.rpcQueue.push(new RpcEndMeeting());
                else MenuState.rpcQueue.push(new EndMeeting());
                MenuState.InMeeting = false;
            }
        }

        if (openUtils) {
            if (ToggleButton("Unlock Vents", &MenuState.UnlockVents)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Move While in Vent & Shapeshifting", &MenuState.MoveInVentAndShapeshift)) {
                if (*Game::pLocalPlayer == NULL) MenuState.Save();
                else if (!MenuState.MoveInVentAndShapeshift && (MenuState.InMeeting || (*Game::pLocalPlayer)->fields.inVent)) {
                    (*Game::pLocalPlayer)->fields.moveable = false;
                    MenuState.Save();
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Always Move", &MenuState.AlwaysMove)) {
                MenuState.Save();
            }

            if (ToggleButton("No Shapeshift Animation", &MenuState.AnimationlessShapeshift)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Copy Lobby Code on Disconnect", &MenuState.AutoCopyLobbyCode)) {
                MenuState.Save();
            }

            if (ToggleButton("NoClip", &MenuState.NoClip)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("No Seeker Animation", &MenuState.NoSeekerAnim)) MenuState.Save();

            if (ToggleButton("Kill Other Impostors", &MenuState.KillImpostors)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Infinite Kill Range", &MenuState.InfiniteKillRange)) {
                MenuState.Save();
            }

            if (ToggleButton("Better Chat Notifications", &MenuState.BetterChatNotifications)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Better Lobby Code Input", &MenuState.BetterLobbyCodeInput)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Better Message Sounds", &MenuState.BetterMessageSounds)) {
                MenuState.Save();
            }

            /*if (ToggleButton("Bypass Guardian Angel Protections", &MenuState.BypassAngelProt)) {
                MenuState.Save();
            }
            ImGui::SameLine();*/
            if (ToggleButton("Autokill", &MenuState.AutoKill)) {
                MenuState.Save();
            }

            if (ToggleButton("Do Tasks as Impostor", &MenuState.DoTasksAsImpostor)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Report Body on Murder", &MenuState.ReportOnMurder)) {
                MenuState.Save();
            }
            if (MenuState.ReportOnMurder) {
                ImGui::SameLine();
                if (ToggleButton("Prevent Self-Report", &MenuState.PreventSelfReport)) {
                    MenuState.Save();
                }
            }
            /*ImGui::SameLine();
            if (ToggleButton("Always Use Kill Exploit", &MenuState.AlwaysUseKillExploit)) {
                MenuState.Save();
            }*/

            if (ToggleButton("Fake Alive", &MenuState.FakeAlive)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (((IsHost() && IsInGame()) || !MenuState.SafeMode) && ToggleButton(IsHost() ? "God Mode" : "Visual Protection", &MenuState.GodMode))
                MenuState.Save();

            if (ToggleButton("(Shift/Ctrl + Right Click) to Teleport", &MenuState.ShiftRightClickTP)) {
                MenuState.Save();
            }
            if (!MenuState.SafeMode) ImGui::SameLine();
            if (!MenuState.SafeMode && ToggleButton("Hold ALT to Teleport Everyone", &MenuState.TeleportEveryone)) {
                MenuState.Save();
            }
            if (ToggleButton((MenuState.SafeMode ? "Rotate Everyone (Client-sided ONLY)" : "Rotate Everyone"), &MenuState.RotateEveryone)) {
                MenuState.Save();
            }
            if (!MenuState.SafeMode) ImGui::SameLine();
            if (!MenuState.SafeMode && MenuState.RotateEveryone && ToggleButton("Server-sided Rotation", &MenuState.RotateServerSide)) {
                MenuState.Save();
            }
            if (ImGui::InputFloat("Rotation Radius", &MenuState.RotateRadius, 0.0f, 0.0f, "%.2f m")) {
                MenuState.Save();
            }

            if (ImGui::InputFloat("X Coordinate", &MenuState.xCoordinate, 0.0f, 0.0f, "%.4f X")) {
                MenuState.Save();
            }

            if (ImGui::InputFloat("Y Coordinate", &MenuState.yCoordinate, 0.0f, 0.0f, "%.4f Y")) {
                MenuState.Save();
            }

            if (ToggleButton("Relative Teleport", &MenuState.RelativeTeleport)) {
                MenuState.Save();
            }
            if (IsInGame() || IsInLobby())
                ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && AnimatedButton("Get Current Position"))
            {
                Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                MenuState.xCoordinate = position.x;
                MenuState.yCoordinate = position.y;
            }
            if (IsInGame() || IsInLobby())
                ImGui::SameLine();

            if ((IsInGame() || IsInLobby()) && AnimatedButton("Teleport To"))
            {
                Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                Vector2 target = { (MenuState.RelativeTeleport ? position.x : 0.f) + MenuState.xCoordinate, (MenuState.RelativeTeleport ? position.y : 0.f) + MenuState.yCoordinate };
                if (IsInGame()) {
                    MenuState.rpcQueue.push(new RpcSnapTo(target));
                }
                else if (IsInLobby()) {
                    MenuState.lobbyRpcQueue.push(new RpcSnapTo(target));
                }
            }
            if (!MenuState.SafeMode && (IsInGame() || IsInLobby())) {
                ImGui::SameLine();
                if (AnimatedButton("Teleport Everyone To"))
                {
                    Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                    Vector2 target = { (MenuState.RelativeTeleport ? position.x : 0.f) + MenuState.xCoordinate, (MenuState.RelativeTeleport ? position.y : 0.f) + MenuState.yCoordinate };
                    std::queue<RPCInterface*>* queue = nullptr;
                    if (IsInGame())
                        queue = &MenuState.rpcQueue;
                    else if (IsInLobby())
                        queue = &MenuState.lobbyRpcQueue;
                    for (auto player : GetAllPlayerControl()) {
                        queue->push(new RpcForceSnapTo(player, target));
                    }
                }
            }

            if (CustomListBoxInt("Select Role", &MenuState.FakeRole, FAKEROLES, 100.0f * MenuState.dpiScale))
                MenuState.Save();
            ImGui::SameLine();
            if ((IsHost() || !MenuState.SafeMode) && (IsInGame() || IsInLobby()) && AnimatedButton("Set Role")) {
                MenuState.FakeRole = std::clamp(MenuState.FakeRole, 0, 10);
                if (IsInGame())
                    MenuState.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum(MenuState.FakeRole)));
                else if (IsInLobby())
                    MenuState.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum(MenuState.FakeRole)));
            }
            if (IsHost() || !MenuState.SafeMode) ImGui::SameLine();
            if ((IsHost() || !MenuState.SafeMode) && (IsInGame() || IsInLobby()) && AnimatedButton("Set for Everyone")) {
                MenuState.FakeRole = std::clamp(MenuState.FakeRole, 0, 10);
                if (IsInGame()) {
                    for (auto player : GetAllPlayerControl())
                        MenuState.rpcQueue.push(new RpcSetRole(player, RoleTypes__Enum(MenuState.FakeRole)));
                }
                else if (IsInLobby()) {
                    for (auto player : GetAllPlayerControl())
                        MenuState.lobbyRpcQueue.push(new RpcSetRole(player, RoleTypes__Enum(MenuState.FakeRole)));
                }
            }
            bool roleAllowed = false;
            switch (MenuState.FakeRole) {
            case (int)RoleTypes__Enum::Crewmate:
            case (int)RoleTypes__Enum::Engineer:
            case (int)RoleTypes__Enum::Scientist:
            case (int)RoleTypes__Enum::Noisemaker:
            case (int)RoleTypes__Enum::Tracker:
            case (int)RoleTypes__Enum::CrewmateGhost:
            case (int)RoleTypes__Enum::ImpostorGhost:
            case (int)RoleTypes__Enum::GuardianAngel:
                roleAllowed = true;
                break;
            case (int)RoleTypes__Enum::Impostor:
                if ((!IsHost() && MenuState.SafeMode) || MenuState.RealRole != RoleTypes__Enum::Impostor || MenuState.RealRole != RoleTypes__Enum::Shapeshifter || MenuState.RealRole != RoleTypes__Enum::Phantom) {
                    roleAllowed = false;
                    break;
                }
                roleAllowed = true;
                break;
            case (int)RoleTypes__Enum::Shapeshifter:
                if (MenuState.RealRole != RoleTypes__Enum::Shapeshifter) {
                    roleAllowed = false;
                    break;
                }
                roleAllowed = true;
                break;
            case (int)RoleTypes__Enum::Phantom:
                if (MenuState.RealRole != RoleTypes__Enum::Phantom) {
                    roleAllowed = false;
                    break;
                }
                roleAllowed = true;
                break;
            default:
                roleAllowed = false;
                break;
            }
            if ((IsInGame() || IsInLobby()) && (roleAllowed || (IsHost() || !MenuState.SafeMode)) && AnimatedButton("Set Fake Role")) {
                if (IsInGame())
                    MenuState.rpcQueue.push(new SetRole(RoleTypes__Enum(MenuState.FakeRole)));
                else if (IsInLobby())
                    MenuState.lobbyRpcQueue.push(new SetRole(RoleTypes__Enum(MenuState.FakeRole)));
            }
            ImGui::SameLine();
            if (ToggleButton("Automatically Set Fake Role", &MenuState.AutoFakeRole)) {
                MenuState.Save();
            }
            if (IsInLobby() || IsInGame()) {
                ImGui::SameLine();
                std::string roleText = FAKEROLES[int(MenuState.RealRole)];
                ImGui::Text(("Real Role: " + roleText).c_str());
            }

            if (!MenuState.SafeMode) {
                if (ToggleButton("Unlock Kill Button", &MenuState.UnlockKillButton)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Kill While Vanished", &MenuState.KillInVanish)) {
                    MenuState.Save();
                }
                /*if (ToggleButton("Bypass Guardian Angel Protections", &MenuState.BypassAngelProt)) {
                    MenuState.Save();
                }*/
            }
        }

        if (openRandomizers) {
            if (ToggleButton("Cycler", &MenuState.Cycler)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Cycle in Meeting", &MenuState.CycleInMeeting)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Cycle Between Players", &MenuState.CycleBetweenPlayers)) {
                MenuState.Save();
            }

            if (SteppedSliderFloat("Cycle Timer", &MenuState.CycleTimer, 0.2f, 1.f, 0.02f, "%.2fs", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                MenuState.PrevCycleTimer = MenuState.CycleTimer;
                MenuState.CycleDuration = MenuState.CycleTimer * 50;
            }

            ImGui::Dummy(ImVec2(4, 4) * MenuState.dpiScale);
            if (ImGui::CollapsingHeader("Cycler Options")) {
                if (ToggleButton("Cycle Name", &MenuState.CycleName)) {
                    MenuState.Save();
                }


                ImGui::SameLine(120.0f * MenuState.dpiScale);
                if (ToggleButton("Cycle Color", &MenuState.RandomColor)) {
                    MenuState.Save();
                }

                ImGui::SameLine(240.0f * MenuState.dpiScale);
                if (ToggleButton("Cycle Hat", &MenuState.RandomHat)) {
                    MenuState.Save();
                }

                if (ToggleButton("Cycle Visor", &MenuState.RandomVisor)) {
                    MenuState.Save();
                }

                ImGui::SameLine(120.0f * MenuState.dpiScale);
                if (ToggleButton("Cycle Skin", &MenuState.RandomSkin)) {
                    MenuState.Save();
                }

                ImGui::SameLine(240.0f * MenuState.dpiScale);
                if (ToggleButton("Cycle Pet", &MenuState.RandomPet)) {
                    MenuState.Save();
                }

                if (ToggleButton("Cycle Nameplate", &MenuState.RandomNamePlate)) {
                    MenuState.Save();
                }

                if (IsHost() || !MenuState.SafeMode) {
                    ImGui::SameLine();
                    if (ToggleButton(IsHost() ? "Cycle for Everyone (name & color only)" : "Cycle for Everyone", &MenuState.CycleForEveryone)) {
                        MenuState.Save();
                    }
                }
            }


            if (ImGui::CollapsingHeader("Cycler Name Options")) {
                if (CustomListBoxInt("Cycler Name Generation", &MenuState.cyclerNameGeneration, NAMEGENERATION, 75 * MenuState.dpiScale)) {
                    MenuState.Save();
                }
                if (MenuState.cyclerNameGeneration == 2) {
                    if (MenuState.cyclerUserNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username generation will fall back to word combo as you have no names in the cycler.");
                    static std::string newName = "";
                    InputString("New Name", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (AnimatedButton("Add Name")) {
                        MenuState.cyclerUserNames.push_back(newName);
                        MenuState.Save();
                        newName = "";
                    }
                    if (!(IsHost() || !MenuState.SafeMode) && !IsNameValid(newName)) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username will be detected by anticheat. This name will be ignored.");
                    }
                    if (!MenuState.cyclerUserNames.empty()) {
                        static int selectedNameIndex = 0;
                        selectedNameIndex = std::clamp(selectedNameIndex, 0, (int)MenuState.cyclerUserNames.size() - 1);
                        std::vector<const char*> nameVector(MenuState.cyclerUserNames.size(), nullptr);
                        for (size_t i = 0; i < MenuState.cyclerUserNames.size(); i++) {
                            nameVector[i] = MenuState.cyclerUserNames[i].c_str();
                        }
                        CustomListBoxInt("Cycler Name to Delete", &selectedNameIndex, nameVector);
                        ImGui::SameLine();
                        if (AnimatedButton("Delete"))
                            MenuState.cyclerUserNames.erase(MenuState.cyclerUserNames.begin() + selectedNameIndex);
                    }
                }
            }

            if (ToggleButton("Confuser (Randomize Appearance at Will)", &MenuState.confuser)) {
                MenuState.Save();
            }

            if (ImGui::CollapsingHeader("Confuser Options")) {
                if ((IsInGame() || IsInLobby()) && AnimatedButton("Confuse Now")) {
                    ControlAppearance(true);
                }
                if (IsInGame() || IsInLobby()) {
                    if (IsHost() || !MenuState.SafeMode)
                        ImGui::SameLine();
                }
                if ((IsInGame() || IsInLobby()) && !MenuState.SafeMode && AnimatedButton("Randomize Everyone")) {
                    std::queue<RPCInterface*>* queue = nullptr;
                    if (IsInGame())
                        queue = &MenuState.rpcQueue;
                    else if (IsInLobby())
                        queue = &MenuState.lobbyRpcQueue;
                    std::vector availableHats = { "hat_NoHat", "hat_AbominalHat", "hat_anchor", "hat_antenna", "hat_Antenna_Black", "hat_arrowhead", "hat_Astronaut-Blue", "hat_Astronaut-Cyan", "hat_Astronaut-Orange", "hat_astronaut", "hat_axe", "hat_babybean", "hat_Baguette", "hat_BananaGreen", "hat_BananaPurple", "hat_bandanaWBY", "hat_Bandana_Blue", "hat_Bandana_Green", "hat_Bandana_Pink", "hat_Bandana_Red", "hat_Bandana_White", "hat_Bandana_Yellow", "hat_baseball_Black", "hat_baseball_Green", "hat_baseball_Lightblue", "hat_baseball_LightGreen", "hat_baseball_Lilac", "hat_baseball_Orange", "hat_baseball_Pink", "hat_baseball_Purple", "hat_baseball_Red", "hat_baseball_White", "hat_baseball_Yellow", "hat_Basketball", "hat_bat_crewcolor", "hat_bat_green", "hat_bat_ice", "hat_beachball", "hat_Beanie_Black", "hat_Beanie_Blue", "hat_Beanie_Green", "hat_Beanie_Lightblue", "hat_Beanie_LightGreen", "hat_Beanie_LightPurple", "hat_Beanie_Pink", "hat_Beanie_Purple", "hat_Beanie_White", "hat_Beanie_Yellow", "hat_bearyCold", "hat_bone", "hat_Bowlingball", "hat_brainslug", "hat_BreadLoaf", "hat_bucket", "hat_bucketHat", "hat_bushhat", "hat_Butter", "hat_caiatl", "hat_caitlin", "hat_candycorn", "hat_captain", "hat_cashHat", "hat_cat_grey", "hat_cat_orange", "hat_cat_pink", "hat_cat_snow", "hat_chalice", "hat_cheeseBleu", "hat_cheeseMoldy", "hat_cheeseSwiss", "hat_ChefWhiteBlue", "hat_cherryOrange", "hat_cherryPink", "hat_Chocolate", "hat_chocolateCandy", "hat_chocolateMatcha", "hat_chocolateVanillaStrawb", "hat_clagger", "hat_clown_purple", "hat_comper", "hat_croissant", "hat_crownBean", "hat_crownDouble", "hat_crownTall", "hat_CuppaJoe", "hat_Deitied", "hat_devilhorns_black", "hat_devilhorns_crewcolor", "hat_devilhorns_green", "hat_devilhorns_murky", "hat_devilhorns_white", "hat_devilhorns_yellow", "hat_Doc_black", "hat_Doc_Orange", "hat_Doc_Purple", "hat_Doc_Red", "hat_Doc_White", "hat_Dodgeball", "hat_Dorag_Black", "hat_Dorag_Desert", "hat_Dorag_Jungle", "hat_Dorag_Purple", "hat_Dorag_Sky", "hat_Dorag_Snow", "hat_Dorag_Yellow", "hat_doubletophat", "hat_DrillMetal", "hat_DrillStone", "hat_DrillWood", "hat_EarmuffGreen", "hat_EarmuffsPink", "hat_EarmuffsYellow", "hat_EarnmuffBlue", "hat_eggGreen", "hat_eggYellow", "hat_enforcer", "hat_erisMorn", "hat_fairywings", "hat_fishCap", "hat_fishhed", "hat_fishingHat", "hat_flowerpot", "hat_frankenbolts", "hat_frankenbride", "hat_fungleFlower", "hat_geoff", "hat_glowstick", "hat_glowstickCyan", "hat_glowstickOrange", "hat_glowstickPink", "hat_glowstickPurple", "hat_glowstickYellow", "hat_goggles", "hat_Goggles_Black", "hat_Goggles_Chrome", "hat_GovtDesert", "hat_GovtHeadset", "hat_halospartan", "hat_hardhat", "hat_Hardhat_black", "hat_Hardhat_Blue", "hat_Hardhat_Green", "hat_Hardhat_Orange", "hat_Hardhat_Pink", "hat_Hardhat_Purple", "hat_Hardhat_Red", "hat_Hardhat_White", "hat_HardtopHat", "hat_headslug_Purple", "hat_headslug_Red", "hat_headslug_White", "hat_headslug_Yellow", "hat_Heart", "hat_heim", "hat_Herohood_Black", "hat_Herohood_Blue", "hat_Herohood_Pink", "hat_Herohood_Purple", "hat_Herohood_Red", "hat_Herohood_Yellow", "hat_hl_fubuki", "hat_hl_gura", "hat_hl_korone", "hat_hl_marine", "hat_hl_mio", "hat_hl_moona", "hat_hl_okayu", "hat_hl_pekora", "hat_hl_risu", "hat_hl_watson", "hat_hunter", "hat_IceCreamMatcha", "hat_IceCreamMint", "hat_IceCreamNeo", "hat_IceCreamStrawberry", "hat_IceCreamUbe", "hat_IceCreamVanilla", "hat_Igloo", "hat_Janitor", "hat_jayce", "hat_jinx", "hat_killerplant", "hat_lilShroom", "hat_maraSov", "hat_mareLwyd", "hat_military", "hat_MilitaryWinter", "hat_MinerBlack", "hat_MinerYellow", "hat_mira_bush", "hat_mira_case", "hat_mira_cloud", "hat_mira_flower", "hat_mira_flower_red", "hat_mira_gem", "hat_mira_headset_blue", "hat_mira_headset_pink", "hat_mira_headset_yellow", "hat_mira_leaf", "hat_mira_milk", "hat_mira_sign_blue", "hat_mohawk_bubblegum", "hat_mohawk_bumblebee", "hat_mohawk_purple_green", "hat_mohawk_rainbow", "hat_mummy", "hat_mushbuns", "hat_mushroomBeret", "hat_mysteryBones", "hat_NewYear2023", "hat_OrangeHat", "hat_osiris", "hat_pack01_Astronaut0001", "hat_pack02_Tengallon0001", "hat_pack02_Tengallon0002", "hat_pack03_Stickynote0004", "hat_pack04_Geoffmask0001", "hat_pack06holiday_candycane0001", "hat_PancakeStack", "hat_paperhat", "hat_Paperhat_Black", "hat_Paperhat_Blue", "hat_Paperhat_Cyan", "hat_Paperhat_Lightblue", "hat_Paperhat_Pink", "hat_Paperhat_Yellow", "hat_papermask", "hat_partyhat", "hat_pickaxe", "hat_Pineapple", "hat_PizzaSliceHat", "hat_pk01_BaseballCap", "hat_pk02_Crown", "hat_pk02_Eyebrows", "hat_pk02_HaloHat", "hat_pk02_HeroCap", "hat_pk02_PipCap", "hat_pk02_PlungerHat", "hat_pk02_ScubaHat", "hat_pk02_StickminHat", "hat_pk02_StrawHat", "hat_pk02_TenGallonHat", "hat_pk02_ThirdEyeHat", "hat_pk02_ToiletPaperHat", "hat_pk02_Toppat", "hat_pk03_Fedora", "hat_pk03_Goggles", "hat_pk03_Headphones", "hat_pk03_Security1", "hat_pk03_StrapHat", "hat_pk03_Traffic", "hat_pk04_Antenna", "hat_pk04_Archae", "hat_pk04_Balloon", "hat_pk04_Banana", "hat_pk04_Bandana", "hat_pk04_Beanie", "hat_pk04_Bear", "hat_pk04_BirdNest", "hat_pk04_CCC", "hat_pk04_Chef", "hat_pk04_DoRag", "hat_pk04_Fez", "hat_pk04_GeneralHat", "hat_pk04_HunterCap", "hat_pk04_JungleHat", "hat_pk04_MinerCap", "hat_pk04_MiniCrewmate", "hat_pk04_Pompadour", "hat_pk04_RamHorns", "hat_pk04_Slippery", "hat_pk04_Snowman", "hat_pk04_Vagabond", "hat_pk04_WinterHat", "hat_pk05_Burthat", "hat_pk05_Cheese", "hat_pk05_cheesetoppat", "hat_pk05_Cherry", "hat_pk05_davehat", "hat_pk05_Egg", "hat_pk05_Ellie", "hat_pk05_EllieToppat", "hat_pk05_Ellryhat", "hat_pk05_Fedora", "hat_pk05_Flamingo", "hat_pk05_FlowerPin", "hat_pk05_GeoffreyToppat", "hat_pk05_Helmet", "hat_pk05_HenryToppat", "hat_pk05_Macbethhat", "hat_pk05_Plant", "hat_pk05_RHM", "hat_pk05_Svenhat", "hat_pk05_Wizardhat", "hat_pk06_Candycanes", "hat_pk06_ElfHat", "hat_pk06_Lights", "hat_pk06_Present", "hat_pk06_Reindeer", "hat_pk06_Santa", "hat_pk06_Snowman", "hat_pk06_tree", "hat_pkHW01_BatWings", "hat_pkHW01_CatEyes", "hat_pkHW01_Horns", "hat_pkHW01_Machete", "hat_pkHW01_Mohawk", "hat_pkHW01_Pirate", "hat_pkHW01_PlagueHat", "hat_pkHW01_Pumpkin", "hat_pkHW01_ScaryBag", "hat_pkHW01_Witch", "hat_pkHW01_Wolf", "hat_Plunger_Blue", "hat_Plunger_Yellow", "hat_police", "hat_Ponytail", "hat_Pot", "hat_Present", "hat_Prototype", "hat_pusheenGreyHat", "hat_PusheenicornHat", "hat_pusheenMintHat", "hat_pusheenPinkHat", "hat_pusheenPurpleHat", "hat_pusheenSitHat", "hat_pusheenSleepHat", "hat_pyramid", "hat_rabbitEars", "hat_Ramhorn_Black", "hat_Ramhorn_Red", "hat_Ramhorn_White", "hat_ratchet", "hat_Records", "hat_RockIce", "hat_RockLava", "hat_Rubberglove", "hat_Rupert", "hat_russian", "hat_saint14", "hat_sausage", "hat_savathun", "hat_schnapp", "hat_screamghostface", "hat_Scrudge", "hat_sharkfin", "hat_shaxx", "hat_shovel", "hat_SlothHat", "hat_SnowbeanieGreen", "hat_SnowbeanieOrange", "hat_SnowBeaniePurple", "hat_SnowbeanieRed", "hat_Snowman", "hat_Soccer", "hat_Sorry", "hat_starBalloon", "hat_starhorse", "hat_Starless", "hat_StarTopper", "hat_stethescope", "hat_StrawberryLeavesHat", "hat_TenGallon_Black", "hat_TenGallon_White", "hat_ThomasC", "hat_tinFoil", "hat_titan", "hat_ToastButterHat", "hat_tombstone", "hat_tophat", "hat_ToppatHair", "hat_towelwizard", "hat_Traffic_Blue", "hat_traffic_purple", "hat_Traffic_Red", "hat_Traffic_Yellow", "hat_Unicorn", "hat_vi", "hat_viking", "hat_Visor", "hat_Voleyball", "hat_w21_candycane_blue", "hat_w21_candycane_bubble", "hat_w21_candycane_chocolate", "hat_w21_candycane_mint", "hat_w21_elf_pink", "hat_w21_elf_swe", "hat_w21_gingerbread", "hat_w21_holly", "hat_w21_krampus", "hat_w21_lights_white", "hat_w21_lights_yellow", "hat_w21_log", "hat_w21_mistletoe", "hat_w21_mittens", "hat_w21_nutcracker", "hat_w21_pinecone", "hat_w21_present_evil", "hat_w21_present_greenyellow", "hat_w21_present_redwhite", "hat_w21_present_whiteblue", "hat_w21_santa_evil", "hat_w21_santa_green", "hat_w21_santa_mint", "hat_w21_santa_pink", "hat_w21_santa_white", "hat_w21_santa_yellow", "hat_w21_snowflake", "hat_w21_snowman", "hat_w21_snowman_evil", "hat_w21_snowman_greenred", "hat_w21_snowman_redgreen", "hat_w21_snowman_swe", "hat_w21_winterpuff", "hat_wallcap", "hat_warlock", "hat_whitetophat", "hat_wigJudge", "hat_wigTall", "hat_WilfordIV", "hat_Winston", "hat_WinterGreen", "hat_WinterHelmet", "hat_WinterRed", "hat_WinterYellow", "hat_witch_green", "hat_witch_murky", "hat_witch_pink", "hat_witch_white", "hat_wolf_grey", "hat_wolf_murky", "hat_Zipper" };
                    std::vector availableSkins = { "skin_None", "skin_Abominalskin", "skin_ApronGreen", "skin_Archae", "skin_Astro", "skin_Astronaut-Blueskin", "skin_Astronaut-Cyanskin", "skin_Astronaut-Orangeskin", "skin_Bananaskin", "skin_benoit", "skin_Bling", "skin_BlueApronskin", "skin_BlueSuspskin", "skin_Box1skin", "skin_BubbleWrapskin", "skin_Burlapskin", "skin_BushSign1skin", "skin_Bushskin", "skin_BusinessFem-Aquaskin", "skin_BusinessFem-Tanskin", "skin_BusinessFemskin", "skin_caitlin", "skin_Capt", "skin_CCC", "skin_ChefBlackskin", "skin_ChefBlue", "skin_ChefRed", "skin_clown", "skin_D2Cskin", "skin_D2Hunter", "skin_D2Osiris", "skin_D2Saint14", "skin_D2Shaxx", "skin_D2Titan", "skin_D2Warlock", "skin_enforcer", "skin_fairy", "skin_FishingSkinskin", "skin_fishmonger", "skin_FishSkinskin", "skin_General", "skin_greedygrampaskin", "skin_halospartan", "skin_Hazmat-Blackskin", "skin_Hazmat-Blueskin", "skin_Hazmat-Greenskin", "skin_Hazmat-Pinkskin", "skin_Hazmat-Redskin", "skin_Hazmat-Whiteskin", "skin_Hazmat", "skin_heim", "skin_hl_fubuki", "skin_hl_gura", "skin_hl_korone", "skin_hl_marine", "skin_hl_mio", "skin_hl_moona", "skin_hl_okayu", "skin_hl_pekora", "skin_hl_risu", "skin_hl_watson", "skin_Horse1skin", "skin_Hotdogskin", "skin_InnerTubeSkinskin", "skin_JacketGreenskin", "skin_JacketPurpleskin", "skin_JacketYellowskin", "skin_Janitorskin", "skin_jayce", "skin_jinx", "skin_LifeVestSkinskin", "skin_Mech", "skin_MechanicRed", "skin_Military", "skin_MilitaryDesert", "skin_MilitarySnowskin", "skin_Miner", "skin_MinerBlackskin", "skin_mummy", "skin_OrangeSuspskin", "skin_PinkApronskin", "skin_PinkSuspskin", "skin_Police", "skin_presentskin", "skin_prisoner", "skin_PrisonerBlue", "skin_PrisonerTanskin", "skin_pumpkin", "skin_PusheenGreyskin", "skin_Pusheenicornskin", "skin_PusheenMintskin", "skin_PusheenPinkskin", "skin_PusheenPurpleskin", "skin_ratchet", "skin_rhm", "skin_RockIceskin", "skin_RockLavaskin", "skin_Sack1skin", "skin_scarfskin", "skin_Science", "skin_Scientist-Blueskin", "skin_Scientist-Darkskin", "skin_screamghostface", "skin_Security", "skin_Skin_SuitRedskin", "skin_Slothskin", "skin_SportsBlueskin", "skin_SportsRedskin", "skin_SuitB", "skin_SuitW", "skin_SweaterBlueskin", "skin_SweaterPinkskin", "skin_Sweaterskin", "skin_SweaterYellowskin", "skin_Tarmac", "skin_ToppatSuitFem", "skin_ToppatVest", "skin_uglysweaterskin", "skin_vampire", "skin_vi", "skin_w21_deer", "skin_w21_elf", "skin_w21_msclaus", "skin_w21_nutcracker", "skin_w21_santa", "skin_w21_snowmate", "skin_w21_tree", "skin_Wall", "skin_Winter", "skin_witch", "skin_YellowApronskin", "skin_YellowSuspskin" };
                    std::vector availableVisors = { "visor_EmptyVisor", "visor_anime", "visor_BaconVisor", "visor_BananaVisor", "visor_beautyMark", "visor_BillyG", "visor_Blush", "visor_Bomba", "visor_BubbleBumVisor", "visor_Candycane", "visor_Carrot", "visor_chimkin", "visor_clownnose", "visor_Crack", "visor_CucumberVisor", "visor_D2CGoggles", "visor_Dirty", "visor_Dotdot", "visor_doubleeyepatch", "visor_eliksni", "visor_erisBandage", "visor_eyeball", "visor_EyepatchL", "visor_EyepatchR", "visor_fishhook", "visor_Galeforce", "visor_heim", "visor_hl_ah", "visor_hl_bored", "visor_hl_hmph", "visor_hl_marine", "visor_hl_nothoughts", "visor_hl_nudge", "visor_hl_smug", "visor_hl_sweepy", "visor_hl_teehee", "visor_hl_wrong", "visor_IceBeard", "visor_IceCreamChocolateVisor", "visor_IceCreamMintVisor", "visor_IceCreamStrawberryVisor", "visor_IceCreamUbeVisor", "visor_is_beard", "visor_JanitorStache", "visor_jinx", "visor_Krieghaus", "visor_Lava", "visor_LolliBlue", "visor_LolliBrown", "visor_LolliOrange", "visor_lollipopCrew", "visor_lollipopLemon", "visor_lollipopLime", "visor_LolliRed", "visor_marshmallow", "visor_masque_blue", "visor_masque_green", "visor_masque_red", "visor_masque_white", "visor_mira_card_blue", "visor_mira_card_red", "visor_mira_glasses", "visor_mira_mask_black", "visor_mira_mask_blue", "visor_mira_mask_green", "visor_mira_mask_purple", "visor_mira_mask_red", "visor_mira_mask_white", "visor_Mouth", "visor_mummy", "visor_PiercingL", "visor_PiercingR", "visor_PizzaVisor", "visor_pk01_AngeryVisor", "visor_pk01_DumStickerVisor", "visor_pk01_FredVisor", "visor_pk01_HazmatVisor", "visor_pk01_MonoclesVisor", "visor_pk01_PaperMaskVisor", "visor_pk01_PlagueVisor", "visor_pk01_RHMVisor", "visor_pk01_Security1Visor", "visor_Plsno", "visor_polus_ice", "visor_pusheenGorgeousVisor", "visor_pusheenKissyVisor", "visor_pusheenKoolKatVisor", "visor_pusheenOmNomNomVisor", "visor_pusheenSmileVisor", "visor_pusheenYaaaaaayVisor", "visor_Reginald", "visor_Rudolph", "visor_savathun", "visor_Scar", "visor_SciGoggles", "visor_shopglasses", "visor_shuttershadesBlue", "visor_shuttershadesLime", "visor_shuttershadesPink", "visor_shuttershadesPurple", "visor_shuttershadesWhite", "visor_shuttershadesYellow", "visor_SkiGoggleBlack", "visor_SKiGogglesOrange", "visor_SkiGogglesWhite", "visor_SmallGlasses", "visor_SmallGlassesBlue", "visor_SmallGlassesRed", "visor_starfish", "visor_Stealthgoggles", "visor_Stickynote_Cyan", "visor_Stickynote_Green", "visor_Stickynote_Orange", "visor_Stickynote_Pink", "visor_Stickynote_Purple", "visor_Straw", "visor_sunscreenv", "visor_teary", "visor_ToastVisor", "visor_tvColorTest", "visor_vr_Vr-Black", "visor_vr_Vr-White", "visor_w21_carrot", "visor_w21_nutstache", "visor_w21_nye", "visor_w21_santabeard", "visor_wash", "visor_WinstonStache" };
                    std::vector availablePets = { "pet_EmptyPet", "pet_Alien", "pet_Bedcrab", "pet_BredPet", "pet_Bush", "pet_Charles", "pet_Charles_Red", "pet_ChewiePet", "pet_clank", "pet_coaltonpet", "pet_Creb", "pet_Crewmate", "pet_Cube", "pet_D2GhostPet", "pet_D2PoukaPet", "pet_D2WormPet", "pet_Doggy", "pet_Ellie", "pet_frankendog", "pet_GuiltySpark", "pet_HamPet", "pet_Hamster", "pet_HolidayHamPet", "pet_Lava", "pet_nuggetPet", "pet_Pip", "pet_poro", "pet_Pusheen", "pet_Robot", "pet_Snow", "pet_Squig", "pet_Stickmin", "pet_Stormy", "pet_test", "pet_UFO", "pet_YuleGoatPet" };
                    std::vector availableNamePlates = { "nameplate_NoPlate", "nameplate_cliffs", "nameplate_grill", "nameplate_plant", "nameplate_sandcastle", "nameplate_zipline", "nameplate_pusheen_01", "nameplate_pusheen_02", "nameplate_pusheen_03", "nameplate_pusheen_04", "nameplate_flagAro", "nameplate_flagMlm", "nameplate_hunter", "nameplate_Polus_DVD", "nameplate_Polus_Ground", "nameplate_Polus_Lava", "nameplate_Polus_Planet", "nameplate_Polus_Snow", "nameplate_Polus_SpecimenBlue", "nameplate_Polus_SpecimenGreen", "nameplate_Polus_SpecimenPurple", "nameplate_is_yard", "nameplate_is_dig", "nameplate_is_game", "nameplate_is_ghost", "nameplate_is_green", "nameplate_is_sand", "nameplate_is_trees", "nameplate_Mira_Cafeteria", "nameplate_Mira_Glass", "nameplate_Mira_Tiles", "nameplate_Mira_Vines", "nameplate_Mira_Wood", "nameplate_hw_candy", "nameplate_hw_woods", "nameplate_hw_pumpkin" };
                    //help me out with the nameplates, couldn't find them in the game assets
                    for (auto player : GetAllPlayerControl()) {
                        std::string name = "";
                        if (MenuState.confuserNameGeneration == 0 || (MenuState.confuserNameGeneration == 2 && MenuState.cyclerUserNames.empty()))
                            name = GenerateRandomString();
                        else if (MenuState.confuserNameGeneration == 1)
                            name = GenerateRandomString(true);
                        else if (MenuState.confuserNameGeneration == 2) {
                            if (!MenuState.cyclerUserNames.empty())
                                name = MenuState.cyclerUserNames[randi(0, MenuState.cyclerUserNames.size() - 1)] + "<size=0>" + std::to_string(player->fields.PlayerId) + "</size>";
                        }
                        else
                            name = GenerateRandomString();
                        queue->push(new RpcForceName(player, name));
                        queue->push(new RpcForceColor(player, randi(0, 17)));
                        queue->push(new RpcForceHat(player, convert_to_string(availableHats[randi(0, availableHats.size() - 1)])));
                        queue->push(new RpcForceSkin(player, convert_to_string(availableSkins[randi(0, availableSkins.size() - 1)])));
                        queue->push(new RpcForceVisor(player, convert_to_string(availableVisors[randi(0, availableVisors.size() - 1)])));
                        queue->push(new RpcForcePet(player, convert_to_string(availablePets[randi(0, availablePets.size() - 1)])));
                        queue->push(new RpcForceNamePlate(player, convert_to_string(availableNamePlates[randi(0, availableNamePlates.size() - 1)])));
                    }
                }

                ImGui::Text("Confuse when:");
                if (ToggleButton("Joining Lobby", &MenuState.confuseOnJoin)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Game Starts", &MenuState.confuseOnStart)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Killing", &MenuState.confuseOnKill)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Venting", &MenuState.confuseOnVent)) {
                    MenuState.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Meeting", &MenuState.confuseOnMeeting)) {
                    MenuState.Save();
                }
            }
            if (ImGui::CollapsingHeader("Confuser Name Options")) {
                if (CustomListBoxInt("Confuser Name Generation", &MenuState.confuserNameGeneration, NAMEGENERATION, 75 * MenuState.dpiScale)) {
                    MenuState.Save();
                }
                if (MenuState.confuserNameGeneration == 2) {
                    if (MenuState.cyclerUserNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username generation will fall back to word combo as you have no names in the cycler.");
                    static std::string newName = "";
                    InputString("New Name ", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (AnimatedButton("Add Name ")) {
                        MenuState.cyclerUserNames.push_back(newName);
                        MenuState.Save();
                        newName = "";
                    }
                    if (!(IsHost() || !MenuState.SafeMode) && !IsNameValid(newName)) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username will be detected by anticheat. This name will be ignored.");
                    }
                    if (!MenuState.cyclerUserNames.empty()) {
                        static int selectedNameIndex = 0;
                        selectedNameIndex = std::clamp(selectedNameIndex, 0, (int)MenuState.cyclerUserNames.size() - 1);
                        std::vector<const char*> nameVector(MenuState.cyclerUserNames.size(), nullptr);
                        for (size_t i = 0; i < MenuState.cyclerUserNames.size(); i++) {
                            nameVector[i] = MenuState.cyclerUserNames[i].c_str();
                        }
                        CustomListBoxInt("Confuser Name to Delete", &selectedNameIndex, nameVector);
                        ImGui::SameLine();
                        if (AnimatedButton("Delete "))
                            MenuState.cyclerUserNames.erase(MenuState.cyclerUserNames.begin() + selectedNameIndex);
                    }
                }
            }
        }
        if (openTextEditor) {
            InputString("Input", &originalText);
            editedText = GetTextEditorName(originalText);
            InputString("Output", &editedText);
            ImGui::SameLine();
            if (AnimatedButton("Copy")) ClipboardHelper_PutClipboardString(convert_to_string(editedText), NULL);

            if (ToggleButton("Italics", &italicName)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Underline", &underlineName)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Strikethrough", &strikethroughName)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Bold", &boldName)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Nobr", &nobrName)) {
                MenuState.Save();
            }

            if (ImGui::ColorEdit4("Starting Gradient Color", (float*)&nameColor1, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Ending Gradient Color", (float*)&nameColor2, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Colored", &coloredName)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(2, 2) * MenuState.dpiScale);

            if (ToggleButton("Font", &font)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (CustomListBoxInt(" ", &fontType, FONTS, 160.f * MenuState.dpiScale)) {
                MenuState.Save();
            }
            ImGui::Dummy(ImVec2(-5, -5) * MenuState.dpiScale);
            if (MenuState.Font) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Note: The white nickname will not be visible in the chat"));

            ImGui::Dummy(ImVec2(2, 2) * MenuState.dpiScale);

            /*if (ToggleButton("Material", &MenuState.Material)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (CustomListBoxInt(" Some materials are not supported", &MenuState.MaterialType, MATERIALS, 160.f * MenuState.dpiScale)) {
                MenuState.Save();
            }*/

            ImGui::Dummy(ImVec2(10, 10) * MenuState.dpiScale);
            if (ToggleButton("Size", &resizeName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Name Size", &nameSize)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            if (ToggleButton("Indent", &indentName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Name Indent", &indentLevel)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            if (ToggleButton("Cspace", &cspaceName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Name Cspace", &cspaceLevel)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            if (ToggleButton("Mspace", &mspaceName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Name Mspace", &mspaceLevel)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            if (ToggleButton("Voffset", &voffsetName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Name Voffset", &voffsetLevel)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            if (ToggleButton("Rotate", &rotateName)) {
                MenuState.Save();
            }

            ImGui::SameLine();
            if (ImGui::InputFloat("Rotation Angle", &rotateAngle)) {
                MenuState.Save();
            }
        }
        ImGui::EndChild();
    }
}


