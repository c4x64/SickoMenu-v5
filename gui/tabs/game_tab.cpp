#include "pch-il2cpp.h"
#include "game_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "utility.h"
#include "state.hpp"
#include "logger.h"
/*#include <hunspell/hunspell.hxx>
#include <sstream>
#include <string>
#include <vector>
#include "imgui.h"
using namespace app;

class SpellChecker {
public:
    SpellChecker(const std::string& affPath, const std::string& dicPath) {
        if (!Hunspell::isAvailable()) {
            throw std::runtime_error("Hunspell is not available.");
        }
        spell = new Hunspell(affPath.c_str(), dicPath.c_str());
        if (!spell->load()) {
            delete spell;
            throw std::runtime_error("Failed to load Hunspell dictionary.");
        }
    }

    ~SpellChecker() {
        delete spell;
    }

    bool isCorrect(const std::string& word) const {
        return spell->spell(word.c_str());
    }

private:
    Hunspell* spell;
};

void HighlightMisspelledWords(SpellChecker& checker, const std::string& text) {
    std::istringstream iss(text);
    std::string word;

    while (iss >> word) {

        bool isCorrect = checker.isCorrect(word);

        if (!isCorrect) {

            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", word.c_str());
        } else {

            ImGui::Text("%s ", word.c_str());
        }
    }
}

void RenderMenu() {
    try {
        SpellChecker spellChecker("en_US.aff", "en_US.dic");

        std::string chatMessage = "Ths is a smaple text with sme misspelled wrds.";

        if (ToggleButton("Blocked Words", &MenuState.SMAC_CheckBadWords)) MenuState.Save();
        if (MenuState.SMAC_CheckBadWords) {
            HighlightMisspelledWords(spellChecker, chatMessage);

            static std::string newWord = "";
            InputString("New Word", &newWord, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (AnimatedButton("Add Word")) {
                MenuState.SMAC_BadWords.push_back(newWord);
                MenuState.Save();
                newWord = "";
            }


        }
    } catch (const std::exception& e) {

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
    }
}


bool ToggleButton(const char* label, bool* p_value) {
    return ImGui::Checkbox(label, p_value);
}

void InputString(const char* label, std::string* str, int flags = 0) {
    ImGui::InputText(label, &(*str)[0], str->capacity() + 1, flags);
}


struct MenuState {
    bool SMAC_CheckBadWords;
    void Save() {}
    static std::vector<std::string> SMAC_BadWords;
};

std::vector<std::string> MenuState::SMAC_BadWords;

int main() {


    while (true) {
        RenderMenu();


    }

    return 0;
}

*/

static std::string strToLower(std::string str) {
    std::string new_str = "";
    for (auto i : str) {
        new_str += char(std::tolower(i));
    }
    return new_str;
}

namespace GameTab {
    enum Groups {
        General,
        Chat,
        Anticheat,
        Utils,
        History,
        Options
    };

    static bool openGeneral = true;
    static bool openChat = false;
    static bool openAnticheat = false;
    static bool openUtils = false;
	static bool openHistory = false;
    static bool openOptions = false;

    void CloseOtherGroups(Groups group) {
        openGeneral = group == Groups::General;
        openChat = group == Groups::Chat;
        openAnticheat = group == Groups::Anticheat;
        openUtils = group == Groups::Utils;
		openHistory = group == Groups::History;
        openOptions = group == Groups::Options;
    }

    void Render() {
        ImGui::SameLine(100 * MenuState.dpiScale);
        ImGui::BeginChild("###Game", ImVec2(500 * MenuState.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (SickoTabGroup("General", openGeneral)) {
            CloseOtherGroups(Groups::General);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Chat", openChat)) {
            CloseOtherGroups(Groups::Chat);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Anticheat", openAnticheat)) {
            CloseOtherGroups(Groups::Anticheat);
        }
        ImGui::SameLine();
        if (SickoTabGroup("Utils", openUtils)) {
            CloseOtherGroups(Groups::Utils);
        }
		ImGui::SameLine();
        if (SickoTabGroup("History", openHistory)) {
            CloseOtherGroups(Groups::History);
        }

        if (GameOptions().HasOptions() && (IsInGame() || IsInLobby())) {
            ImGui::SameLine();
            if (SickoTabGroup("Options", openOptions)) {
                CloseOtherGroups(Groups::Options);
            }
        }

        enum WarnViewType {
            WarnView_List = 0,
            WarnView_Manual,
            WarnView_COUNT
        };

        static int selectedWarnView = 0;
        const char* warnViewModes[WarnView_COUNT] = {
            "List View",
            "Manual Warn"
        };

        if (openGeneral) {
            ImGui::Dummy(ImVec2(2, 2) * MenuState.dpiScale);
            if (SteppedSliderFloat("Player Speed Multiplier", &MenuState.PlayerSpeed, 0.f, 10.f, 0.05f, "%.2fx", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                MenuState.PrevPlayerSpeed = MenuState.PlayerSpeed;
            }
            if (SteppedSliderFloat("Kill Distance", &MenuState.KillDistance, 0.f, 20.f, 0.1f, "%.1f m", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                MenuState.PrevKillDistance = MenuState.KillDistance;
            }
            /*if (GameOptions().GetGameMode() == GameModes__Enum::Normal) {
                if (CustomListBoxInt("Task Bar Updates", &MenuState.TaskBarUpdates, TASKBARUPDATES, 225 * MenuState.dpiScale))
                    MenuState.PrevTaskBarUpdates = MenuState.TaskBarUpdates;
            }*/
            if (ToggleButton("No Ability Cooldown", &MenuState.NoAbilityCD)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Multiply Speed", &MenuState.MultiplySpeed)) {
                MenuState.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Modify Kill Distance", &MenuState.ModifyKillDistance)) {
                MenuState.Save();
            }

            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);

            if (IsHost() || !MenuState.SafeMode) {
                CustomListBoxInt(" ", &MenuState.SelectedColorId, HOSTCOLORS, 85.0f * MenuState.dpiScale);
            }
            else {
                if (MenuState.SelectedColorId >= (int)COLORS.size()) MenuState.SelectedColorId = 0;
                CustomListBoxInt(" ", &MenuState.SelectedColorId, COLORS, 85.0f * MenuState.dpiScale);
            }
            ImGui::SameLine();
            if (AnimatedButton("Random Color"))
            {
                MenuState.SelectedColorId = GetRandomColorId();
            }

            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (AnimatedButton("Set Color"))
                {
                    if (IsHost() || !MenuState.SafeMode) {
                        if (IsInGame())
                            MenuState.rpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, MenuState.SelectedColorId));
                        else if (IsInLobby())
                            MenuState.lobbyRpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, MenuState.SelectedColorId));
                    }
                    else if (IsColorAvailable(MenuState.SelectedColorId)) {
                        if (IsInGame())
                            MenuState.rpcQueue.push(new RpcSetColor(MenuState.SelectedColorId));
                        else if (IsInLobby())
                            MenuState.lobbyRpcQueue.push(new RpcSetColor(MenuState.SelectedColorId));
                    }
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Snipe Color", &MenuState.SnipeColor)) {
                MenuState.Save();
            }

            if (ToggleButton("Console", &MenuState.ShowConsole)) {
                MenuState.Save();
            }

            /*if (ToggleButton("Auto-Join", &MenuState.AutoJoinLobby))
                MenuState.Save();
            ImGui::SameLine();
            if (InputString("Lobby Code", &MenuState.AutoJoinLobbyCode))
                MenuState.Save();

            if (AnimatedButton("Join Lobby")) {
                AmongUsClient_CoJoinOnlineGameFromCode(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(MenuState.AutoJoinLobbyCode), NULL),
                    NULL);
            }*/

            if (IsInGame() || IsInLobby()) ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && AnimatedButton("Reset Appearance"))
            {
                ControlAppearance(false);
            }


            if (IsInGame() && (IsHost() || !MenuState.SafeMode) && AnimatedButton("Kill Everyone")) {
                for (auto player : GetAllPlayerControl()) {
                    if (IsInGame() && (IsHost() || !MenuState.SafeMode)) {
                        if (IsInGame())
                            MenuState.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                        else if (IsInLobby())
                            MenuState.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                    }
                    else {
                        if (IsInGame())
                            MenuState.rpcQueue.push(new FakeMurderPlayer(*Game::pLocalPlayer, player));
                        else if (IsInLobby())
                            MenuState.lobbyRpcQueue.push(new FakeMurderPlayer(*Game::pLocalPlayer, player));
                    }
                }
            }
            if (IsInLobby() && !MenuState.SafeMode) ImGui::SameLine();
            if (IsInLobby() && !MenuState.SafeMode && AnimatedButton("Allow Everyone to NoClip")) {
                for (auto p : GetAllPlayerControl()) {
                    if (p != *Game::pLocalPlayer) MenuState.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p, 1, true));
                }
                MenuState.NoClip = true;
                ShowHudNotification("Allowed everyone to NoClip!");
            }
            /*if (IsHost() && (IsInGame() || IsInLobby()) && AnimatedButton("Spawn Dummy")) {
                auto outfit = GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer));
                if (IsInGame()) MenuState.rpcQueue.push(new RpcSpawnDummy(outfit->fields.ColorId, convert_from_string(outfit->fields.PlayerName)));
                if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcSpawnDummy(outfit->fields.ColorId, convert_from_string(outfit->fields.PlayerName)));
            }*/
            if ((IsInGame() || IsInLobby()) && ((IsHost() && IsInGame()) || !MenuState.SafeMode)) {
                ImGui::SameLine();
                if (AnimatedButton(IsHost() ? "Protect Everyone" : "Visual Protect Everyone")) {
                    for (auto player : GetAllPlayerControl()) {
                        uint8_t colorId = GetPlayerOutfit(GetPlayerData(player))->fields.ColorId;
                        if (IsInGame())
                            MenuState.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
                        else if (IsInLobby())
                            MenuState.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
                    }
                }
            }

            if (IsInGame() && ToggleButton("Disable Venting", &MenuState.DisableVents)) {
                MenuState.Save();
            }
            if (IsInGame() && (IsHost() || !MenuState.SafeMode)) ImGui::SameLine();
            if (IsInGame() && (IsHost() || !MenuState.SafeMode) && ToggleButton("Spam Report", &MenuState.SpamReport)) {
                MenuState.Save();
            }

            if ((IsInGame() || (IsInLobby() && MenuState.KillInLobbies)) && (IsHost() || !MenuState.SafeMode)) {
                if (AnimatedButton("Kill All Crewmates")) {
                    for (auto player : GetAllPlayerControl()) {
                        if (!PlayerIsImpostor(GetPlayerData(player))) {
                            if (IsInGame())
                                MenuState.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                            else if (IsInLobby())
                                MenuState.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                        }
                    }
                }
                ImGui::SameLine();
                if (AnimatedButton("Kill All Impostors")) {
                    for (auto player : GetAllPlayerControl()) {
                        if (PlayerIsImpostor(GetPlayerData(player))) {
                            if (IsInGame())
                                MenuState.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
                                    player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                            else if (IsInLobby())
                                MenuState.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
                                    player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                        }
                    }
                }
                if (!MenuState.SafeMode) {
                    ImGui::SameLine();
                    if (AnimatedButton("Suicide Crewmates")) {
                        for (auto player : GetAllPlayerControl()) {
                            if (!PlayerIsImpostor(GetPlayerData(player))) {
                                if (IsInGame())
                                    MenuState.rpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                                else if (IsInLobby())
                                    MenuState.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (AnimatedButton("Suicide Impostors")) {
                        for (auto player : GetAllPlayerControl()) {
                            if (PlayerIsImpostor(GetPlayerData(player))) {
                                if (IsInGame())
                                    MenuState.rpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                                else if (IsInLobby())
                                    MenuState.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || MenuState.BypassAngelProt));
                            }
                        }
                    }
                }

                static int ventId = 0;
                if (IsInGame() && (IsHost() || !MenuState.SafeMode)) {
                    std::vector<const char*> allVents;
                    switch (MenuState.mapType) {
                    case Settings::MapType::Ship:
                        allVents = SHIPVENTS;
                        break;
                    case Settings::MapType::Hq:
                        allVents = HQVENTS;
                        break;
                    case Settings::MapType::Pb:
                        allVents = PBVENTS;
                        break;
                    case Settings::MapType::Airship:
                        allVents = AIRSHIPVENTS;
                        break;
                    case Settings::MapType::Fungle:
                        allVents = FUNGLEVENTS;
                        break;
                    }
                    ventId = std::clamp(ventId, 0, (int)allVents.size() - 1);

                    ImGui::SetNextItemWidth(100 * MenuState.dpiScale);
                    CustomListBoxInt("Vent", &ventId, allVents);
                    ImGui::SameLine();
                    if (AnimatedButton("Teleport All to Vent")) {
                        for (auto p : GetAllPlayerControl()) {
                            MenuState.rpcQueue.push(new RpcBootFromVent(p, (MenuState.mapType == Settings::MapType::Hq) ? ventId + 1 : ventId)); //MiraHQ vents start from 1 instead of 0
                        }
                    }
                }
            }

            if (IsInGame() || IsInLobby()) {
                if (!MenuState.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && AnimatedButton("Scan Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) MenuState.rpcQueue.push(new RpcForceScanner(p, true));
                        else MenuState.lobbyRpcQueue.push(new RpcForceScanner(p, true));
                    }
                }
                if (!MenuState.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (!MenuState.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && AnimatedButton("Stop Scanning Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) MenuState.rpcQueue.push(new RpcForceScanner(p, false));
                        else MenuState.lobbyRpcQueue.push(new RpcForceScanner(p, false));
                    }
                }
                if (IsInGame() && !MenuState.InMeeting && !MenuState.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (IsInGame() && !MenuState.InMeeting && AnimatedButton("Kick Everyone From Vents")) {
                    MenuState.rpcQueue.push(new RpcBootAllVents());
                }
                if ((IsHost() || !MenuState.SafeMode) && MenuState.InMeeting) ImGui::SameLine();
                if ((IsHost() || !MenuState.SafeMode) && MenuState.InMeeting && AnimatedButton("End Meeting")) {
                    MenuState.rpcQueue.push(new RpcEndMeeting());
                    MenuState.InMeeting = false;
                }

                if (!MenuState.SafeMode && !IsHost()) {
                    if (AnimatedButton("Set Name for Everyone")) {
                        for (auto p : GetAllPlayerControl()) {
                            if (IsInGame()) MenuState.rpcQueue.push(new RpcForceName(p, std::format("{}<size=0><{}></size>", MenuState.hostUserName, p->fields.PlayerId)));
                            if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcForceName(p, std::format("{}<size=0><{}></size>", MenuState.hostUserName, p->fields.PlayerId)));
                        }
                    }
                    ImGui::SameLine();
                    if (ToggleButton("Force Name for Everyone", &MenuState.ForceNameForEveryone)) {
                        MenuState.Save();
                    }

                    if (InputString("Username", &MenuState.hostUserName)) {
                        MenuState.Save();
                    }

                    if (AnimatedButton("Set Color for Everyone")) {
                        for (auto p : GetAllPlayerControl()) {
                            if (IsInGame()) MenuState.rpcQueue.push(new RpcForceColor(p, MenuState.HostSelectedColorId));
                            if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcForceColor(p, MenuState.HostSelectedColorId));
                        }
                    }
                    ImGui::SameLine();
                    if (ToggleButton("Force Color for Everyone", &MenuState.ForceColorForEveryone)) {
                        MenuState.Save();
                    }

                    if (CustomListBoxInt(" ­", &MenuState.HostSelectedColorId, HOSTCOLORS, 85.0f * MenuState.dpiScale)) MenuState.Save();
                }
            }
        }

        if (openChat) {
            bool msgAllowed = IsChatValid(MenuState.chatMessage);
            if (!msgAllowed) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.f, 0.f, MenuState.MenuThemeColor.w));
                if (InputStringMultiline("\n\n\n\n\nChat Message", &MenuState.chatMessage)) MenuState.Save();
                ImGui::PopStyleColor();
            }
            else if (InputStringMultiline("\n\n\n\n\nChat Message", &MenuState.chatMessage)) MenuState.Save();
            if ((IsInGame() || IsInLobby()) && MenuState.ChatCooldown >= 3.f && IsChatValid(MenuState.chatMessage)) {
                ImGui::SameLine();
                if (AnimatedButton("Send"))
                {
                    auto player = (!MenuState.SafeMode && MenuState.playerToChatAs.has_value()) ?
                        MenuState.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                    if (IsInGame()) MenuState.rpcQueue.push(new RpcSendChat(player, MenuState.chatMessage));
                    else if (IsInLobby()) MenuState.lobbyRpcQueue.push(new RpcSendChat(player, MenuState.chatMessage));
                    MenuState.MessageSent = true;
                }
            }
            if ((IsInGame() || IsInLobby()) && MenuState.ReadAndSendSickoChat) ImGui::SameLine();
            if (MenuState.ReadAndSendSickoChat && (IsInGame() || IsInLobby()) && AnimatedButton("Send to AUM"))
            {
                auto player = (!MenuState.SafeMode && MenuState.playerToChatAs.has_value()) ?
                    MenuState.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                if (IsInGame()) {
                    MenuState.rpcQueue.push(new RpcForceSickoChat(PlayerSelection(player), MenuState.chatMessage, true));
                }
                else if (IsInLobby()) {
                    MenuState.lobbyRpcQueue.push(new RpcForceSickoChat(PlayerSelection(player), MenuState.chatMessage, true));
                }
            }

            if (ToggleButton("Spam", &MenuState.ChatSpam))
            {
                if (MenuState.BrainrotEveryone) MenuState.BrainrotEveryone = false;
                if (MenuState.RizzUpEveryone) MenuState.RizzUpEveryone = false;
                MenuState.Save();
            }
            if (((IsHost() && IsInGame()) || !MenuState.SafeMode) && MenuState.ChatSpamMode) ImGui::SameLine();
            if ((IsHost() || !MenuState.SafeMode) && MenuState.ChatSpamMode && ToggleButton("Spam by Everyone", &MenuState.ChatSpamEveryone))
            {
                MenuState.Save();
            }
            if (IsHost() || !MenuState.SafeMode) {
                if (CustomListBoxInt("Chat Spam Mode", &MenuState.ChatSpamMode,
                    { MenuState.SafeMode ? "With Message (Self-Spam ONLY)" : "With Message", "Blank Chat", MenuState.SafeMode ? "Self Message + Blank Chat" : "Message + Blank Chat" })) MenuState.Save();
            }

            if (std::find(MenuState.ChatPresets.begin(), MenuState.ChatPresets.end(), MenuState.chatMessage) == MenuState.ChatPresets.end() && AnimatedButton("Add Message as Preset")) {
                MenuState.ChatPresets.push_back(MenuState.chatMessage);
                MenuState.Save();
            }
            if (!(IsHost() || !MenuState.SafeMode) && MenuState.chatMessage.size() > 120) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Message will be detected by anticheat.");
            }
            if (!MenuState.ChatPresets.empty()) {
                static int selectedPresetIndex = 0;
                selectedPresetIndex = std::clamp(selectedPresetIndex, 0, (int)MenuState.ChatPresets.size() - 1);
                std::vector<const char*> presetVector(MenuState.ChatPresets.size(), nullptr);
                for (size_t i = 0; i < MenuState.ChatPresets.size(); i++) {
                    presetVector[i] = MenuState.ChatPresets[i].c_str();
                }
                CustomListBoxInt("Message to Send/Remove", &selectedPresetIndex, presetVector);
                auto msg = MenuState.ChatPresets[selectedPresetIndex];
                if (AnimatedButton("Set as Chat Message"))
                {
                    MenuState.chatMessage = msg;
                }
                ImGui::SameLine();
                if (AnimatedButton("Remove"))
                    MenuState.ChatPresets.erase(MenuState.ChatPresets.begin() + selectedPresetIndex);
            }
        }

        if (openAnticheat) {
            if (ToggleButton("Enable Anticheat (SMAC)", &MenuState.Enable_SMAC)) MenuState.Save();
            if (IsHost()) CustomListBoxInt("Host Punishment ", &MenuState.SMAC_HostPunishment, SMAC_HOST_PUNISHMENTS, 85.0f * MenuState.dpiScale);
            else CustomListBoxInt("Regular Punishment", &MenuState.SMAC_Punishment, SMAC_PUNISHMENTS, 85.0f * MenuState.dpiScale);

            if (ToggleButton("Add Cheaters to Blacklist", &MenuState.SMAC_AddToBlacklist)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Punish Blacklist", &MenuState.SMAC_PunishBlacklist)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Ignore Whitelist", &MenuState.SMAC_IgnoreWhitelist)) MenuState.Save();
            if (MenuState.SMAC_PunishBlacklist) {
                ImGui::Text("Blacklist");
                if (MenuState.BlacklistFriendCodes.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in blacklist!");
                else {
                    ImGui::SameLine(0.f, 0.f);
                    ImGui::Text(" (%d Users Blacklisted)", MenuState.BlacklistFriendCodes.size());
                }
                static std::string newBFriendCode = "";
				bool isInBlacklistAlready = std::find(MenuState.BlacklistFriendCodes.begin(), MenuState.BlacklistFriendCodes.end(), newBFriendCode) != MenuState.BlacklistFriendCodes.end();
                InputString("New Friend Code", &newBFriendCode, ImGuiInputTextFlags_EnterReturnsTrue);
                if (isInBlacklistAlready)
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This user is already blacklisted!");
                if (newBFriendCode != "" && !isInBlacklistAlready) ImGui::SameLine();
                if (newBFriendCode != "" && !isInBlacklistAlready && AnimatedButton("Add")) {
                    MenuState.BlacklistFriendCodes.push_back(newBFriendCode);
                    MenuState.Save();
                    newBFriendCode = "";
                }

                if (!MenuState.BlacklistFriendCodes.empty()) {
                    static int selectedBCodeIndex = 0;
                    selectedBCodeIndex = std::clamp(selectedBCodeIndex, 0, (int)MenuState.BlacklistFriendCodes.size() - 1);
                    std::vector<const char*> bCodeVector(MenuState.BlacklistFriendCodes.size(), nullptr);
                    for (size_t i = 0; i < MenuState.BlacklistFriendCodes.size(); i++) {
                        bCodeVector[i] = MenuState.BlacklistFriendCodes[i].c_str();
                    }
                    CustomListBoxInt("Player to Delete", &selectedBCodeIndex, bCodeVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Delete"))
                        MenuState.BlacklistFriendCodes.erase(MenuState.BlacklistFriendCodes.begin() + selectedBCodeIndex);
                }
            }
            if (MenuState.SMAC_IgnoreWhitelist) {
                ImGui::Text("Whitelist");
                if (MenuState.WhitelistFriendCodes.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in whitelist!");
                else {
                    ImGui::SameLine(0.f, 0.f);
                    ImGui::Text(" (%d Users Whitelisted)", MenuState.WhitelistFriendCodes.size());
                }
                static std::string newWFriendCode = "";
                static bool isInWhitelistAlready = std::find(MenuState.WhitelistFriendCodes.begin(), MenuState.WhitelistFriendCodes.end(), newWFriendCode) != MenuState.WhitelistFriendCodes.end();
                InputString("New Friend Code\n", &newWFriendCode, ImGuiInputTextFlags_EnterReturnsTrue);
                if (isInWhitelistAlready)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This user is already whitelisted!");
                if (newWFriendCode != "" && !isInWhitelistAlready) ImGui::SameLine();
                if (newWFriendCode != "" && !isInWhitelistAlready && AnimatedButton("Add\n")) {
                    MenuState.WhitelistFriendCodes.push_back(newWFriendCode);
                    MenuState.Save();
                    newWFriendCode = "";
                }

                if (!MenuState.WhitelistFriendCodes.empty()) {
                    static int selectedWCodeIndex = 0;
                    selectedWCodeIndex = std::clamp(selectedWCodeIndex, 0, (int)MenuState.WhitelistFriendCodes.size() - 1);
                    std::vector<const char*> wCodeVector(MenuState.WhitelistFriendCodes.size(), nullptr);
                    for (size_t i = 0; i < MenuState.WhitelistFriendCodes.size(); i++) {
                        wCodeVector[i] = MenuState.WhitelistFriendCodes[i].c_str();
                    }
                    CustomListBoxInt("Player to Delete\n", &selectedWCodeIndex, wCodeVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Delete\n"))
                        MenuState.WhitelistFriendCodes.erase(MenuState.WhitelistFriendCodes.begin() + selectedWCodeIndex);
                }
            }
            ImGui::Text("Detect Actions:");
            if (ToggleButton("AUM/KillNetwork Usage", &MenuState.SMAC_CheckAUM)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("SickoMenu Usage", &MenuState.SMAC_CheckSicko)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Names", &MenuState.SMAC_CheckBadNames)) MenuState.Save();

            if (ToggleButton("Abnormal Set Color", &MenuState.SMAC_CheckColor)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Set Cosmetics", &MenuState.SMAC_CheckCosmetics)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Chat Note", &MenuState.SMAC_CheckChatNote)) MenuState.Save();

            if (ToggleButton("Abnormal Scanner", &MenuState.SMAC_CheckScanner)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Animation", &MenuState.SMAC_CheckAnimation)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Setting Tasks", &MenuState.SMAC_CheckTasks)) MenuState.Save();

            if (ToggleButton("Abnormal Murders", &MenuState.SMAC_CheckMurder)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Shapeshift", &MenuState.SMAC_CheckShapeshift)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Vanish", &MenuState.SMAC_CheckVanish)) MenuState.Save();


            if (ToggleButton("Abnormal Meetings/Body Reports", &MenuState.SMAC_CheckReport)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Venting", &MenuState.SMAC_CheckVent)) MenuState.Save();
            ImGui::SameLine();
           
            if (ToggleButton("Abnormal Chat", &MenuState.SMAC_CheckChat)) MenuState.Save();

            if (ToggleButton("Abnormal Task Completion", &MenuState.SMAC_CheckTaskCompletion)) MenuState.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Sabotages", &MenuState.SMAC_CheckSabotage)) MenuState.Save();
            if (ToggleButton("Abnormal Player Levels (0 to ignore)", &MenuState.SMAC_CheckLevel)) MenuState.Save();
            if (MenuState.SMAC_CheckLevel && ImGui::InputInt("Level >=", &MenuState.SMAC_HighLevel)) {
                MenuState.Save();
            }
            if (MenuState.SMAC_CheckLevel && ImGui::InputInt("Level <=", &MenuState.SMAC_LowLevel)) {
                MenuState.Save();
            }
            if (ToggleButton("Blocked Words", &MenuState.SMAC_CheckBadWords)) MenuState.Save();
            if (MenuState.SMAC_CheckBadWords) {
                if (MenuState.SMAC_BadWords.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No bad words added!");
                static std::string newWord = "";
                InputString("New Word", &newWord, ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::SameLine();
                if (AnimatedButton("Add Word")) {
                    MenuState.SMAC_BadWords.push_back(newWord);
                    MenuState.Save();
                    newWord = "";
                }
                if (!MenuState.SMAC_BadWords.empty()) {
                    static int selectedWordIndex = 0;
                    selectedWordIndex = std::clamp(selectedWordIndex, 0, (int)MenuState.SMAC_BadWords.size() - 1);
                    std::vector<const char*> wordVector(MenuState.SMAC_BadWords.size(), nullptr);
                    for (size_t i = 0; i < MenuState.SMAC_BadWords.size(); i++) {
                        wordVector[i] = MenuState.SMAC_BadWords[i].c_str();
                    }
                    CustomListBoxInt("Word to Remove", &selectedWordIndex, wordVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Remove"))
                        MenuState.SMAC_BadWords.erase(MenuState.SMAC_BadWords.begin() + selectedWordIndex);
                }
            }
        }

        if (openUtils) {
            /*if (ToggleButton("Ignore Whitelisted Players [Exploits]", &MenuState.Destruct_IgnoreWhitelist)) {
                MenuState.Save();
            }*/
            if (ToggleButton("Ignore Whitelisted Players [Ban/Kick]", &MenuState.Ban_IgnoreWhitelist)) {
                MenuState.Save();
            }
            if (IsInLobby() && ToggleButton("Attempt to Crash Lobby", &MenuState.CrashSpamReport)) {
                MenuState.Save();
            }
            if (MenuState.CrashSpamReport) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("When the game starts, the lobby is destroyed"));
            if (MenuState.AprilFoolsMode) {
                ImGui::TextColored(ImVec4(0.79f, 0.03f, 1.f, 1.f), MenuState.DiddyPartyMode ? "Diddy Party Mode" : (IsChatCensored() || IsStreamerMode() ? "F***son Mode" : "Fuckson Mode"));
                if (ToggleButton("Mog Everyone [Sigma]", &MenuState.BrainrotEveryone)) {
                    if (MenuState.ChatSpam) MenuState.ChatSpam = false;
                    if (MenuState.RizzUpEveryone) MenuState.RizzUpEveryone = false;
                    MenuState.Save();
                }
                if (/*MenuState.DiddyPartyMode && */ToggleButton("Rizz Up Everyone [Skibidi]", &MenuState.RizzUpEveryone)) {
                    if (MenuState.ChatSpam) MenuState.ChatSpam = false;
                    if (MenuState.BrainrotEveryone) MenuState.BrainrotEveryone = false;
                    MenuState.Save();
                }
            }
            if (IsHost()) {
                ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
                if (((IsInGame() && Object_1_IsNotNull((Object_1*)*Game::pShipStatus)) || (IsInLobby() && Object_1_IsNotNull((Object_1*)*Game::pLobbyBehaviour)))
                    && AnimatedButton(IsInLobby() ? "Remove Lobby" : "Remove Map")) {
                    MenuState.taskRpcQueue.push(new DestroyMap());
                }
                ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
                if (ToggleButton("Ban Everyone", &MenuState.BanEveryone)) {
                    MenuState.Save();
                }
                if (ToggleButton("Kick Everyone", &MenuState.KickEveryone)) {
                    MenuState.Save();
                }
                if (SteppedSliderFloat("Kick/Ban Delay", &MenuState.AutoPunishDelay, 0.f, 10.f, 0.1f, "%.1f", ImGuiSliderFlags_NoInput)) {
                    MenuState.Save();
                }
                ImGui::Dummy(ImVec2(7, 7) * MenuState.dpiScale);
                const char* buttonLabel = IsInGame() ? "Kick AFK Players" : "Kick AFK Players [GAME ONLY]";
                if (ToggleButton(buttonLabel, &MenuState.KickAFK)) {
                    MenuState.Save();
                }
                if (MenuState.KickAFK) ImGui::SameLine();
                if (MenuState.KickAFK && ToggleButton("Enable AFK Notifications", &MenuState.NotificationsAFK)) {
                    MenuState.Save();
                }
                if (MenuState.KickAFK && ToggleButton("AFK - Second Chance", &MenuState.SecondChance)) {
                    MenuState.Save();
                }
                std::string header = "Anti AFK ~ Advanced Options";
                if (!IsInGame()) {
                    header += " [GAME-MATCH]";
                }
                ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
                if (MenuState.KickAFK && ImGui::CollapsingHeader(header.c_str()))
                {
                    if (SteppedSliderFloat("Time Before Kick", &MenuState.TimerAFK, 40.f, 350.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        MenuState.Save();
                    }
                    if (MenuState.SecondChance && SteppedSliderFloat("Extra Time", &MenuState.AddExtraTime, 15.f, 120.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        MenuState.Save();
                    }
                    if (MenuState.SecondChance && SteppedSliderFloat("Min Time Before Adding", &MenuState.ExtraTimeThreshold, 5.f, 60.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        MenuState.Save();
                    }
                    if (MenuState.NotificationsAFK && SteppedSliderFloat("Warn-AFK Notifications Time", &MenuState.NotificationTimeWarn, 5.f, 60.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        MenuState.Save();
                    }
                }
                ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                if (ToggleButton("Whitelisted Players Only", &MenuState.KickByWhitelist)) {
                    MenuState.Save();
                }
                if (MenuState.KickByWhitelist) ImGui::SameLine();
                if (MenuState.KickByWhitelist && ToggleButton("Enable WL Notifications", &MenuState.WhitelistNotifications)) {
                    MenuState.Save();
                }
                ImGui::Dummy(ImVec2(15, 15) * MenuState.dpiScale);
                if (ToggleButton("Ban Auto-Rejoin Players", &MenuState.BanLeavers)) {
                    MenuState.Save();
                }
                ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
                if (ImGui::CollapsingHeader("BA-RP ~ Advanced Options"))
                {
                    if (SteppedSliderFloat("Maximum Rejoins", &MenuState.LeaveCount, 1.f, 15.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        MenuState.Save();
                    }
                    ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
                    if (ToggleButton("Blacklist Auto-Rejoin Players", &MenuState.BL_AutoLeavers)) {
                        MenuState.Save();
                    }
                }
                ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                if (ToggleButton("Warn/Kick By Name-Checker", &MenuState.KickByLockedName)) {
                    MenuState.Save();
                }
                if (MenuState.KickByLockedName) ImGui::SameLine();
                if (MenuState.KickByLockedName && ToggleButton("Show Player Data Notifications", &MenuState.ShowPDataByNC)) {
                    MenuState.Save();
                }
                if (MenuState.KickByLockedName) {
                    ImGui::Text("Blocked Names");
                    if (MenuState.LockedNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in Name-Checker!");
                    static std::string newName = "";
                    InputString("New Nickname", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    if (newName != "") ImGui::SameLine();
                    if (newName != "" && AnimatedButton("Add")) {
						newName = strToLower(newName);
                        MenuState.LockedNames.push_back(newName);
                        MenuState.Save();
                        newName = "";
                    }

                    if (!MenuState.LockedNames.empty()) {
                        static int selectedName = 0;
                        selectedName = std::clamp(selectedName, 0, (int)MenuState.LockedNames.size() - 1);
                        std::vector<const char*> bNameVector(MenuState.LockedNames.size(), nullptr);
                        for (size_t i = 0; i < MenuState.LockedNames.size(); i++) {
                            bNameVector[i] = MenuState.LockedNames[i].c_str();
                        }
                        CustomListBoxInt("Nickname to Delete", &selectedName, bNameVector);
                        ImGui::SameLine();
                        if (AnimatedButton("Delete"))
                            MenuState.LockedNames.erase(MenuState.LockedNames.begin() + selectedName);
                    }
                }
                ImGui::Dummy(ImVec2(15, 15) * MenuState.dpiScale);
                ImGui::BeginGroup();
                if (ToggleButton("Kick Warned Players", &MenuState.KickWarned)) {
                    MenuState.Save();
                }
                if (ToggleButton("Ban Warned Players", &MenuState.BanWarned)) {
                    MenuState.Save();
                }
                if (ToggleButton("Notify Warned Player", &MenuState.NotifyWarned)) {
                    MenuState.Save();
                }

                ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);

                ImGui::PushItemWidth(80);
                ImGui::InputInt("Max Warns", &MenuState.MaxWarns);
                if (MenuState.MaxWarns < 1)
                    MenuState.MaxWarns = 1;
                ImGui::PopItemWidth();
                ImGui::EndGroup();
            }
            if (IsHost()) ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::PushItemWidth(150);
            if (!IsHost()) ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            ImGui::Combo("Warn View Mode", &selectedWarnView, warnViewModes, WarnView_COUNT);
            ImGui::PopItemWidth();


            if (selectedWarnView == WarnView_List) {
                if (!MenuState.WarnedFriendCodes.empty()) {
                    ImGui::Text("Warned Players");

                    std::string localFC = "";
                    if (Game::pLocalPlayer && *Game::pLocalPlayer) {
                        localFC = convert_from_string((*Game::pLocalPlayer)->fields.FriendCode);
                    }

                    std::vector<std::string> warnedList;
                    std::vector<std::string> fcKeys;

                    for (const auto& [fc, count] : MenuState.WarnedFriendCodes) {
                        if (count <= 0 || fc == localFC)
                            continue;

                        warnedList.push_back(std::format("{} ({} warn{})", fc, count, count == 1 ? "" : "s"));
                        fcKeys.push_back(fc);
                    }

                    if (!warnedList.empty()) {
                        static int selectedWarned = 0;
                        selectedWarned = std::clamp(selectedWarned, 0, (int)warnedList.size() - 1);

                        std::vector<const char*> warnedCStrs;
                        for (const auto& entry : warnedList) warnedCStrs.push_back(entry.c_str());

                        ImGui::PushItemWidth(200);
                        CustomListBoxInt("Warned FriendCodes", &selectedWarned, warnedCStrs);
                        ImGui::PopItemWidth();

                        ImGui::SameLine();
                        if (ImGui::Button("Remove")) {
                            if (selectedWarned >= 0 && selectedWarned < (int)fcKeys.size()) {
                                std::string fc = fcKeys[selectedWarned];
                                MenuState.WarnedFriendCodes.erase(fc);
                                MenuState.WarnReasons.erase(fc);
                                selectedWarned = 0;
                                MenuState.Save();
                            }
                        }

                        std::string selectedFc = fcKeys[selectedWarned];
                        auto& warnReasons = MenuState.WarnReasons[selectedFc];

                        if (!warnReasons.empty()) {
                            ImGui::Text("Warn Reasons:");

                            static int selectedReason = 0;
                            selectedReason = std::clamp(selectedReason, 0, (int)warnReasons.size() - 1);

                            std::vector<std::string> numberedReasons;
                            numberedReasons.reserve(warnReasons.size());
                            for (size_t i = 0; i < warnReasons.size(); ++i) {
                                numberedReasons.push_back(std::format("[{}] {}", i + 1, warnReasons[i]));
                            }

                            std::vector<const char*> reasonCStrs;
                            for (const auto& str : numberedReasons) reasonCStrs.push_back(str.c_str());

                            ImGui::PushItemWidth(200);
                            ImGui::ListBox("##WarnReasonList", &selectedReason, reasonCStrs.data(), (int)reasonCStrs.size());
                            ImGui::PopItemWidth();

                            ImGui::SameLine();
                            if (ImGui::Button("Delete")) {
                                if (selectedReason >= 0 && selectedReason < (int)warnReasons.size()) {
                                    warnReasons.erase(warnReasons.begin() + selectedReason);
                                    selectedReason = 0;

                                    if (--MenuState.WarnedFriendCodes[selectedFc] <= 0) {
                                        MenuState.WarnedFriendCodes.erase(selectedFc);
                                        MenuState.WarnReasons.erase(selectedFc);
                                        selectedWarned = 0;
                                    }

                                    MenuState.Save();
                                }
                            }
                        }
                    }
                    else {
                        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No warned players.");
                    }
                }
            }
            else if (selectedWarnView == WarnView_Manual) {
                static std::string friendCodeToWarn;
                static std::string warnReason;

                ImGui::PushItemWidth(200);
                InputString("FriendCode##warn", &friendCodeToWarn);
                InputString("Reason", &warnReason);
                ImGui::PopItemWidth();

                if (ImGui::Button("Submit Warn") && !friendCodeToWarn.empty() && !warnReason.empty()) {
                    MenuState.WarnedFriendCodes[friendCodeToWarn]++;
                    MenuState.WarnReasons[friendCodeToWarn].push_back(warnReason);
                    MenuState.Save();

                    friendCodeToWarn.clear();
                    warnReason.clear();
                }
            }

            ImGui::EndGroup();

            ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

            if (ToggleButton("Enable Temp-Ban System", &MenuState.TempBanEnabled)) {
                MenuState.Save();
			}
            if (MenuState.TempBanEnabled && ImGui::CollapsingHeader("Temp-Ban System")) {
                static std::string friendCodeToTempBan;
                static int banDays = 0, banHours = 0, banMinutes = 0, banSeconds = 0;

                ImGui::BeginGroup();
                ImGui::PushItemWidth(150);
                InputString("Friend Code", &friendCodeToTempBan);

                ImGui::InputInt("Days", &banDays);     banDays = std::max<int>(0, banDays);
                ImGui::InputInt("Hours", &banHours);   banHours = std::clamp(banHours, 0, 23);
                ImGui::InputInt("Minutes", &banMinutes); banMinutes = std::clamp(banMinutes, 0, 59);
                ImGui::InputInt("Seconds", &banSeconds); banSeconds = std::clamp(banSeconds, 0, 59);
                ImGui::PopItemWidth();

                if (!friendCodeToTempBan.empty() && ImGui::Button("Submit Temp-Ban")) {
                    std::string selfFC;
                    if (Game::pLocalPlayer && *Game::pLocalPlayer) {
                        selfFC = convert_from_string((*Game::pLocalPlayer)->fields.FriendCode);
                    }

                    if (!selfFC.empty() && friendCodeToTempBan == selfFC) { }
                    else {
                        int64_t totalSeconds = 0;
                        totalSeconds += static_cast<int64_t>(banDays) * 86400;
                        totalSeconds += static_cast<int64_t>(banHours) * 3600;
                        totalSeconds += static_cast<int64_t>(banMinutes) * 60;
                        totalSeconds += static_cast<int64_t>(banSeconds);

                        if (totalSeconds > MenuState.MAX_BAN_SECONDS) {
                            totalSeconds = MenuState.MAX_BAN_SECONDS;
                        }

                        if (totalSeconds > 0) {
                            auto now = std::chrono::system_clock::now();
                            auto banEnd = now + std::chrono::seconds(totalSeconds);

                            MenuState.TempBannedFCs[friendCodeToTempBan] = banEnd;
                            MenuState.Save();

                            if (IsInGame() || IsInLobby()) {
                                for (auto p : GetAllPlayerControl()) {
                                    if (!p) continue;
                                    if (convert_from_string(p->fields.FriendCode) == friendCodeToTempBan) {
                                        // Main & first ban (new temp-banned user):
                                        if (IsInGame())
                                            MenuState.rpcQueue.push(new PunishPlayer(p, false));
                                        if (IsInLobby())
                                            MenuState.lobbyRpcQueue.push(new PunishPlayer(p, false));
                                    }
                                }
                            }
                        }
                    }
                }
                ImGui::EndGroup();
                ImGui::SameLine();

                ImGui::BeginGroup();
                ImGui::Text("Temp-Banned Players:");

                auto now = std::chrono::system_clock::now();
                if (MenuState.TempBannedFCs.empty()) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "No players are temporarily banned.");
                }
                else {
                    static int selectedTempBanIndex = 0;
                    std::vector<std::string> displayList, friendCodeList;

                    for (const auto& [fc, until] : MenuState.TempBannedFCs) {
                        auto timeLeft = std::chrono::duration_cast<std::chrono::seconds>(until - now).count();
                        if (timeLeft < 0) timeLeft = 0;

                        int d = (int)(timeLeft / 86400);
                        int h = (int)((timeLeft % 86400) / 3600);
                        int m = (int)((timeLeft % 3600) / 60);
                        int s = (int)(timeLeft % 60);

                        char buffer[128];
                        snprintf(buffer, sizeof(buffer), "%s | %02dd:%02dh:%02dm:%02ds", fc.c_str(), d, h, m, s);

                        displayList.push_back(buffer);
                        friendCodeList.push_back(fc);
                    }

                    std::vector<const char*> displayCStrs;
                    for (auto& s : displayList) displayCStrs.push_back(s.c_str());

                    selectedTempBanIndex = std::clamp(selectedTempBanIndex, 0, (int)displayCStrs.size() - 1);
                    CustomListBoxInt("Select TempBan", &selectedTempBanIndex, displayCStrs);

                    if (ImGui::Button("Unban")) {
                        if (selectedTempBanIndex >= 0 && selectedTempBanIndex < (int)friendCodeList.size()) {
                            std::string targetFC = friendCodeList[selectedTempBanIndex];
                            MenuState.TempBannedFCs.erase(targetFC);
                            MenuState.Save();
                        }
                    }
                }

                ImGui::Dummy(ImVec2(10, 10) * MenuState.dpiScale);
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Note: Temporary Ban Features\nWorks as Host Only!");
                ImGui::EndGroup();
            }
        }

		if (openHistory) {
            ImGui::Text("Last 100 players:");

            static int selectedIndex = -1;

            std::vector<std::string> decoratedStorage;
            decoratedStorage.reserve(MenuState.PlayerHistory.size());
            std::vector<const char*> names;
            std::vector<int> filteredIndices;

            names.reserve(MenuState.PlayerHistory.size());
            filteredIndices.reserve(MenuState.PlayerHistory.size());

            for (int i = 0; i < (int)MenuState.PlayerHistory.size(); ++i)
            {
                auto& p = MenuState.PlayerHistory[i];
                auto itf = MenuState.platformFilters.find(p.Platform);
                bool visible = (itf != MenuState.platformFilters.end()) ? itf->second : true;
                if (!visible) continue;

                std::string decorated = p.Nick;

                if (p.NameCheck) decorated += " [!]";
                bool inWL = std::find(MenuState.WhitelistFriendCodes.begin(), MenuState.WhitelistFriendCodes.end(), p.FriendCode) != MenuState.WhitelistFriendCodes.end();
                bool inBL = std::find(MenuState.BlacklistFriendCodes.begin(), MenuState.BlacklistFriendCodes.end(), p.FriendCode) != MenuState.BlacklistFriendCodes.end();
                if (inWL) decorated += " [+]";
                if (inBL) decorated += " [-]";

                decoratedStorage.push_back(std::move(decorated));
                names.push_back(decoratedStorage.back().c_str());
                filteredIndices.push_back(i);
            }

            if (names.empty())
            {
                selectedIndex = -1;
            }
            else
            {
                if (selectedIndex >= (int)names.size()) selectedIndex = (int)names.size() - 1;

                ImGui::PushItemWidth(200);
                if (ImGui::ListBox("##PlayerList", &selectedIndex, names.data(), (int)names.size(), 10))
                {
                    if (selectedIndex < 0 || selectedIndex >= (int)filteredIndices.size()) selectedIndex = -1;
                }
                ImGui::PopItemWidth();

                if (selectedIndex >= 0)
                {
                    int realIndex = filteredIndices[selectedIndex];
                    auto& p = MenuState.PlayerHistory[realIndex];

                    ImGui::SameLine();
                    ImGui::BeginGroup();

                    ImGui::Text("Is using Modified Client: %s", p.IsModded ? "Yes" : "No");
                    if (p.IsModded && !p.ModClient.empty()) ImGui::Text("Client Name: %s", p.ModClient.c_str());
                    ImGui::NewLine();
                    ImGui::Text("Friend Code: %s", p.FriendCode.c_str());
                    ImGui::Text("PUID: %s", p.Puid.c_str());
                    ImGui::Text("Level: %d", p.Level);
                    ImGui::Text("Platform: %s", p.Platform.c_str());
                    ImGui::Text("Name-Checker: %s", p.NameCheck ? "Yes" : "None");
                    ImGui::NewLine();

                    if (AnimatedButton("Clear Player"))
                    {
                        MenuState.RemovedPlayers.insert(p.FriendCode);
                        MenuState.PlayerHistory.erase(MenuState.PlayerHistory.begin() + realIndex);
                        MenuState.Save();
                        selectedIndex = -1;
                    }

                    ImGui::EndGroup();
                    ImGui::Spacing();

                    bool inWL = std::find(MenuState.WhitelistFriendCodes.begin(), MenuState.WhitelistFriendCodes.end(), p.FriendCode) != MenuState.WhitelistFriendCodes.end();
                    std::string wLabel = inWL ? "Remove from Whitelist" : "Add to Whitelist";

                    if (AnimatedButton(wLabel.c_str()))
                    {
                        if (inWL)
                            RemoveFromWhitelist(p.FriendCode);
                        else
                        {
                            AddToWhitelist(p.FriendCode);
                            RemoveFromBlacklist(p.FriendCode);
                        }
                        MenuState.Save();
                        p;
                    }

                    ImGui::SameLine();

                    bool inBL = std::find(MenuState.BlacklistFriendCodes.begin(), MenuState.BlacklistFriendCodes.end(), p.FriendCode) != MenuState.BlacklistFriendCodes.end();
                    std::string bLabel = inBL ? "Remove from Blacklist" : "Add to Blacklist";

                    if (AnimatedButton(bLabel.c_str()))
                    {
                        if (inBL)
                            RemoveFromBlacklist(p.FriendCode);
                        else
                        {
                            AddToBlacklist(p.FriendCode);
                            RemoveFromWhitelist(p.FriendCode);
                        }
                        MenuState.Save();
                    }

                    ImGui::SameLine();

                    std::string lowName = p.Nick;
                    std::transform(lowName.begin(), lowName.end(), lowName.begin(), ::tolower);
                    std::string ncLabel = p.NameCheck ? "Remove from Name-Checker" : "Add to Name-Checker";

                    if (AnimatedButton(ncLabel.c_str()))
                    {
                        if (p.NameCheck)
                        {
                            MenuState.LockedNames.erase(std::remove(MenuState.LockedNames.begin(), MenuState.LockedNames.end(), lowName), MenuState.LockedNames.end());
                            p.NameCheck = false;
                        }
                        else
                        {
                            MenuState.LockedNames.push_back(lowName);
                            p.NameCheck = true;
                        }
                        for (auto& rp : MenuState.PlayerHistory) {
                            std::string lc = rp.Nick;
                            std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
                            rp.NameCheck = (std::find(MenuState.LockedNames.begin(), MenuState.LockedNames.end(), lc) != MenuState.LockedNames.end());
                        }
                        MenuState.Save();
                    }
                }
            }

            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(5, 5) * MenuState.dpiScale);

            if (ImGui::Button("Clear History"))
            {
                for (auto& pp : MenuState.PlayerHistory) MenuState.RemovedPlayers.insert(pp.FriendCode);
                MenuState.PlayerHistory.clear();
                selectedIndex = -1;
                MenuState.Save();
            }
            ImGui::SameLine(0, 20);
            if (ImGui::Button("Update Player History"))
            {
                bool changed = false;
                for (auto pctrl : GetAllPlayerControl())
                {
                    if (!pctrl || pctrl == *Game::pLocalPlayer) continue;
                    auto data = GetPlayerData(pctrl);
                    if (!data || data->fields.Disconnected) continue;

                    std::string fc = convert_from_string(data->fields.FriendCode);
                    std::string name = strToLower(RemoveHtmlTags(convert_from_string(GetPlayerOutfit(data)->fields.PlayerName)));
                    std::string puid = convert_from_string(data->fields.Puid);
                    int level = data->fields.PlayerLevel + 1;

                    if (fc.empty() || name.empty() || level <= 0) continue;
                    if (MenuState.RemovedPlayers.count(fc)) MenuState.RemovedPlayers.erase(fc);

                    bool exists = false;
                    for (auto& rp : MenuState.PlayerHistory) if (rp.FriendCode == fc) { exists = true; break; }
                    if (exists) continue;

                    std::string platform = "Unknown";
                    auto client = app::InnerNetClient_GetClientFromCharacter((InnerNetClient*)(*Game::pAmongUsClient), pctrl, NULL);
                    if (client != nullptr && client->fields.PlatformData != nullptr && pctrl->fields._.OwnerId == client->fields.Id) {
                        switch (client->fields.PlatformData->fields.Platform) {
                        case Platforms__Enum::StandaloneEpicPC:
                            platform = "Epic Games (PC)";
                            break;
                        case Platforms__Enum::StandaloneSteamPC:
                            platform = "Steam (PC)";
                            break;
                        case Platforms__Enum::StandaloneMac:
                            platform = "Mac";
                            break;
                        case Platforms__Enum::StandaloneWin10:
                            platform = "Microsoft Store (PC)";
                            break;
                        case Platforms__Enum::StandaloneItch:
                            platform = "itch.io (PC)";
                            break;
                        case Platforms__Enum::IPhone:
                            platform = "iOS/iPadOS (Mobile)";
                            break;
                        case Platforms__Enum::Android:
                            platform = "Android (Mobile)";
                            break;
                        case Platforms__Enum::Switch:
                            platform = "Nintendo Switch (Console)";
                            break;
                        case Platforms__Enum::Xbox:
                            platform = "Xbox (Console)";
                            break;
                        case Platforms__Enum::Playstation:
                            platform = "Playstation (Console)";
                            break;
                        default:
                            platform = "Unknown";
                            break;
                        }
                    }

                    std::string lcname = name;
                    std::transform(lcname.begin(), lcname.end(), lcname.begin(), ::tolower);
                    bool nameCheck = (std::find(MenuState.LockedNames.begin(), MenuState.LockedNames.end(), lcname) != MenuState.LockedNames.end());

                    bool isCheater = false;
                    std::string cheatName = "";
                    int pid = data->fields.PlayerId;
                    auto modIt = MenuState.modUsers.find(pid);
                    if (modIt != MenuState.modUsers.end()) {
                        cheatName = RemoveHtmlTags(modIt->second);
                        isCheater = true;
                    }

                    if (MenuState.PlayerHistory.size() >= 100)
                        MenuState.PlayerHistory.pop_front();

                    MenuState.PlayerHistory.push_back({ name, fc, puid, level, platform, nameCheck, isCheater, cheatName });
                    changed = true;
                }
                if (changed) MenuState.Save();
            }

            ImGui::Dummy(ImVec2(5, 5)* MenuState.dpiScale);

            if (ImGui::CollapsingHeader("Platform Filters"))
            {
                ImGui::Columns(2, NULL, false);

                for (size_t i = 0; i < PLATFORM_FILTERS.size(); i++)
                {
                    ToggleButton(PLATFORM_FILTERS[i].c_str(), &MenuState.platformFilters[PLATFORM_FILTERS[i]]);

                    if (i == (PLATFORM_FILTERS.size() + 1) / 2 - 1)
                        ImGui::NextColumn();
                }

                ImGui::Columns(1);
            }
        }

        if (openOptions) {
            if ((IsInGame() || IsInLobby()) && GameOptions().HasOptions()) {
                GameOptions options;
                /*std::string hostText = std::format("Host: {}", RemoveHtmlTags(GetHostUsername()));
                ImGui::Text(const_cast<char*>(hostText.c_str()));*/

                if (options.GetGameMode() == GameModes__Enum::Normal)
                {
                    auto allPlayers = GetAllPlayerControl();
                    RoleRates roleRates = RoleRates(options, (int)allPlayers.size());
                    // this should be all the major ones. if people want more they're simple enough to add.
                    ImGui::Text("Visual Tasks: %s", (options.GetBool(app::BoolOptionNames__Enum::VisualTasks) ? "On" : "Off"));
                    switch (options.GetInt(app::Int32OptionNames__Enum::TaskBarMode)) {
                    case 0:
                        ImGui::Text("Task Bar Updates: Always");
                        break;
                    case 1:
                        ImGui::Text("Task Bar Updates: Meetings");
                        break;
                    case 2:
                        ImGui::Text("Task Bar Updates: Never");
                        break;
                    default:
                        ImGui::Text("Task Bar Updates: Other");
                        break;
                    }
                    ImGui::Text("Confirm Ejects: %s", (options.GetBool(app::BoolOptionNames__Enum::ConfirmImpostor) ? "On" : "Off"));
                    switch (options.GetInt(app::Int32OptionNames__Enum::KillDistance)) {
                    case 0:
                        ImGui::Text("Kill Distance: Short");
                        break;
                    case 1:
                        ImGui::Text("Kill Distance: Medium");
                        break;
                    case 2:
                        ImGui::Text("Kill Distance: Long");
                        break;
                    default:
                        ImGui::Text("Kill Distance: Other");
                        break;
                    }

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Engineers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Engineer));
                    ImGui::Text("Engineer Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Engineer));
                    ImGui::Text("Engineer Vent Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerCooldown, 1.0F));
                    ImGui::Text("Engineer Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Scientists: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Scientist));
                    ImGui::Text("Scientist Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Scientist));
                    ImGui::Text("Scientist Vitals Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistCooldown, 1.0F));
                    ImGui::Text("Scientist Battery Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Guardian Angels: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::GuardianAngel));
                    ImGui::Text("Guardian Angel Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::GuardianAngel));
                    ImGui::Text("Guardian Angel Protect Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::GuardianAngelCooldown, 1.0F));
                    ImGui::Text("Guardian Angel Protection Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Shapeshifters: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Shift Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, 1.0F));
                    ImGui::Text("Shapeshifter Shift Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Noisemakers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Alert Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::NoisemakerAlertDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Trackers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracking Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDuration, 1.0F));
                    ImGui::Text("Tracking Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerCooldown, 1.0F));
                    ImGui::Text("Tracking Delay: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDelay, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Max Phantoms: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Vanish Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomCooldown, 1.0F));
                    ImGui::Text("Phantom Vanish Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomDuration, 1.0F));
                }
                else if (options.GetGameMode() == GameModes__Enum::HideNSeek) {

                    int ImpostorId = options.GetInt(app::Int32OptionNames__Enum::ImpostorPlayerID);
                    if (ImpostorId < 0) {
                        ImGui::Text("Impostor: Round-robin");
                    }
                    else {
                        std::string ImpostorName = std::format("Selected Impostor: {}", convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerDataById(ImpostorId), nullptr)));
                        ImGui::Text(const_cast<char*>(ImpostorName.c_str()));
                    }
                    ImGui::Text("Flashlight Mode: %s", (options.GetBool(app::BoolOptionNames__Enum::UseFlashlight) ? "On" : "Off"));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Vent Uses: %d", options.GetInt(app::Int32OptionNames__Enum::CrewmateVentUses));
                    ImGui::Text("Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::CrewmateTimeInVent, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * MenuState.dpiScale);

                    ImGui::Text("Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EscapeTime, 1.0F));
                    ImGui::Text("Final Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::FinalEscapeTime, 1.0F));
                    ImGui::Text("Final Impostor Speed: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::SeekerFinalSpeed, 1.0F));
                }
            }
            else CloseOtherGroups(Groups::General);
        }
        ImGui::EndChild();
    }
}



