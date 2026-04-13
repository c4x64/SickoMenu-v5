#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
using namespace app;

void dPlayerStorageManager_OnReadPlayerPrefsComplete(PlayerStorageManager* __this, ReadFileCallbackInfo* data, MethodInfo* method) {
    if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dPlayerStorageManager_OnReadPlayerPrefsComplete executed");
    app::PlayerStorageManager_OnReadPlayerPrefsComplete(__this, data, method);
}
