#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
using namespace app;

static bool isGuestAccount = false;

void fakeSuccessfulLogin(EOSManager* eosManager)
{
	EOSManager_DeleteDeviceID(eosManager, NULL, NULL);
	/*eosManager->fields.loginFlowFinished = true;
	EOSManager_HasFinishedLoginFlow(eosManager, NULL);*/
	auto player = app::DataManager_get_Player(nullptr);
	static FieldInfo* field = il2cpp_class_get_field_from_name((Il2CppClass*)player->klass, "account");
	LOG_ASSERT(field != nullptr);
	auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, (Il2CppObject*)player);
	//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
	static FieldInfo* field1 = il2cpp_class_get_field_from_name((Il2CppClass*)account->klass, "loginStatus");
	auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
	il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
}

void dEOSManager_StartInitialLoginFlow(EOSManager* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_StartInitialLoginFlow executed");
	if (!MenuState.SpoofGuestAccount) {
		EOSManager_StartInitialLoginFlow(__this, method);
		return;
	}
	EOSManager_StartTempAccountFlow(__this, method);
	//isGuestAccount = true;
	EOSManager_CloseStartupWaitScreen(__this, method);
}

void dEOSManager_LoginFromAccountTab(EOSManager* __this, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_LoginFromAccountTab executed");
	EOSManager_LoginFromAccountTab(__this, method);
	if (MenuState.SpoofGuestAccount) {
		LOG_DEBUG("Faking login");
		fakeSuccessfulLogin(__this);
	}
}

void dEOSManager_InitializePlatformInterface(EOSManager* __this, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_InitializePlatformInterface executed");
	EOSManager_InitializePlatformInterface(__this, method);
	//LOG_DEBUG("Skipping device identification");
	//__this->fields.platformInitialized = true;
}

bool dEOSManager_IsFreechatAllowed(EOSManager* __this, MethodInfo* method)
{
	bool ret = !isGuestAccount || IsInGame() || IsInLobby();
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_IsFreechatAllowed executed");
	return ret;
}

QuickChatModes__Enum dMultiplayerSettingsData_get_ChatMode(MultiplayerSettingsData* __this, MethodInfo* method) {
	if (IsInGame() || IsInLobby()) return QuickChatModes__Enum::FreeChatOrQuickChat;
	return !isGuestAccount || IsInGame() || IsInLobby() ? MultiplayerSettingsData_get_ChatMode(__this, method) : QuickChatModes__Enum::QuickChatOnly;
}

bool dEOSManager_IsFriendsListAllowed(EOSManager* __this, MethodInfo* method)
{
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_IsFriendsListAllowed executed");
	return app::EOSManager_IsFriendsListAllowed(__this, method);
}

void dEOSManager_UpdatePermissionKeys(EOSManager* __this, Action* callback, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_UpdatePermissionKeys executed");
	/*Il2CppClass* klass = get_class("Assembly-CSharp, EOSManager");
	LOG_ASSERT(klass);
	FieldInfo* field = il2cpp_class_get_field_from_name(klass, "isKWSMinor");
	LOG_ASSERT(field);
	bool value = false;
	il2cpp_field_set_value((Il2CppObject*)__this, field, &value);*/

	app::EOSManager_UpdatePermissionKeys(__this, callback, method);
}

void dEOSManager_Update(EOSManager* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_Update executed");
	static bool hasDeletedDeviceId = false;
	//__this->fields.ageOfConsent = 0; //why tf does amogus have an age of consent lmao
	//if (MenuState.SpoofFriendCode) __this->fields.friendCode = convert_to_string(MenuState.FakeFriendCode);
	EOSManager_Update(__this, method);
	//EOSManager_set_FriendCode(__this, __this->fields.friendCode, NULL);
	if (MenuState.SpoofGuestAccount) {
		auto player = app::DataManager_get_Player(nullptr);
		static FieldInfo* field = il2cpp_class_get_field_from_name((Il2CppClass*)player->klass, "account");
		LOG_ASSERT(field != nullptr);
		auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, (Il2CppObject*)player);
		//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
		static FieldInfo* field1 = il2cpp_class_get_field_from_name((Il2CppClass*)account->klass, "loginStatus");
		auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
		auto loggedOut = EOSManager_AccountLoginStatus__Enum::Offline;
		if ((int)il2cpp_field_get_value_object(field1, (Il2CppObject*)account) != (int)loggedOut)
			il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
		/*if (MenuState.UseGuestFriendCode && MenuState.GuestFriendCode != "") {
			auto username = __this->fields.editAccountUsername;
			TMP_Text_set_text((TMP_Text*)username->fields.UsernameText, convert_to_string(MenuState.GuestFriendCode), NULL);
			//EditAccountUsername_SaveUsername(username, NULL);
		}*/
	}

	/*if (__this->fields.hasRunLoginFlow && !hasDeletedDeviceId) {
		EOSManager_DeleteDeviceID(__this, NULL, NULL);
		LOG_DEBUG("Successfully deleted device ID!");
		hasDeletedDeviceId = true;
	}*/

	if (MenuState.ForceLoginAsGuest) {
		auto player = app::DataManager_get_Player(nullptr);
		static FieldInfo* field = il2cpp_class_get_field_from_name((Il2CppClass*)player->klass, "account");
		LOG_ASSERT(field != nullptr);
		auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, (Il2CppObject*)player);
		//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
		static FieldInfo* field1 = il2cpp_class_get_field_from_name((Il2CppClass*)account->klass, "loginStatus");
		auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
		auto loggedOut = EOSManager_AccountLoginStatus__Enum::Offline;
		if ((int)il2cpp_field_get_value_object(field1, (Il2CppObject*)account) != (int)loggedOut)
			il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
		/*if (MenuState.UseGuestFriendCode && MenuState.GuestFriendCode != "") {
			auto username = __this->fields.editAccountUsername;
			TMP_Text_set_text((TMP_Text*)username->fields.UsernameText, convert_to_string(MenuState.GuestFriendCode), NULL);
			//EditAccountUsername_SaveUsername(username, NULL);
		}*/
		MenuState.ForceLoginAsGuest = false; //button behavior
	}

	if (MenuState.SpoofLevel) {
		auto player = DataManager_get_Player(NULL);
		auto stats = PlayerData_get_Stats(player, NULL);
		int fakeLevel = MenuState.SafeMode ? std::clamp(MenuState.FakeLevel, 1, 100001) : MenuState.FakeLevel;
		stats->fields.level = fakeLevel - 1;
		AbstractSaveData_Save((AbstractSaveData*)player, NULL);
	}
}

String* dEOSManager_get_ProductUserId(EOSManager* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_get_ProductUserId executed");
	auto puid = EOSManager_get_ProductUserId(__this, method);
	if (MenuState.UseGuestPuid && MenuState.GuestPuid != "")
		return convert_to_string(MenuState.FakePuid);
	return puid;
}


//Encrypted PUID Spoofing code [Undetectable]

/*static void f1(const char* c1) {
	if (a2.b2) {
		int c2 = (c1[0] != 0) ? 1 : 0;
		LOG_DEBUG("Vtpnq1");
	}
}

static String* f2() {
	if (a2.c3 && !a2.d4.empty()) {
		int c3 = a2.d4.size();
		String* c4 = f3(a2.d4);
		return c4;
	}
	return nullptr;
}*/

void dPlatformSpecificData_Serialize(PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dPlatformSpecificData_Serialize executed");
	if (MenuState.SpoofPlatform) __this->fields.Platform = Platforms__Enum(MenuState.FakePlatform + 1);
	if (MenuState.FakePlatform == 8) __this->fields.XboxPlatformId = MenuState.FakeXboxId;
	if (MenuState.FakePlatform == 9) __this->fields.PsnPlatformId = MenuState.FakePsnId;
	PlatformSpecificData_Serialize(__this, writer, method);
}

void dEditAccountUsername_SaveUsername(EditAccountUsername* __this, MethodInfo* method) {
	if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dEditAccountUsername_SaveUsername executed");
	if (MenuState.UseNewFriendCode && MenuState.NewFriendCode != "") {
		std::string newFriendCode = "";
		for (auto i : MenuState.NewFriendCode) {
			if (newFriendCode.ends_with(" ")) {
				break;
			}
			newFriendCode += tolower(i);
		}
		TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(newFriendCode), NULL);
	}
	else {
		auto textStr = TMP_Text_get_text((TMP_Text*)__this->fields.UsernameText, NULL);
		if (textStr != convert_to_string("")) {
			std::string newFriendCode = "";
			for (auto i : convert_from_string(textStr)) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(newFriendCode), NULL);
		}
		else {
			std::string newFriendCode = "";
			std::string randomString = GenerateRandomString();
			for (auto i : randomString) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(newFriendCode), NULL);
		}
	}
	EditAccountUsername_SaveUsername(__this, method);
}


