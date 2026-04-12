#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
using namespace app;

void dSceneManager_Internal_ActiveSceneChanged(Scene previousActiveScene, Scene newActiveScene, MethodInfo* method) {
	//if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dSceneManager_Internal_ActiveSceneChanged executed");
	MenuState.CurrentScene = convert_from_string(app::Scene_GetNameInternal(newActiveScene.m_Handle, NULL));
	LOG_DEBUG(("Scene changed to " + MenuState.CurrentScene).c_str());
	if (MenuState.CurrentScene == "MainMenu") {
		MenuState.MainMenuLoaded = true;
		MenuState.IsFreePlay = false;
	}
	app::SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene, method);
}


