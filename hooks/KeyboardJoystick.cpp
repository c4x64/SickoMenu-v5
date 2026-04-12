#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
using namespace app;

void dKeyboardJoystick_Update(KeyboardJoystick* __this, MethodInfo* method) {
    if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dKeyboardJoystick_Update executed");
    if ((!MenuState.FreeCam && !MenuState.playerToAttach.has_value()) || MenuState.PanicMode) {
        app::KeyboardJoystick_Update(__this, method);
    }
    else
        __this->fields.del = app::Vector2(); //Reset to zero
}

void dScreenJoystick_FixedUpdate(ScreenJoystick* __this, MethodInfo* method)
{
    if (MenuState.ShowHookLogs) LOG_DEBUG("Hook dScreenJoystick_FixedUpdate executed");
    static int countdown;
    if ((!MenuState.FreeCam && !MenuState.playerToAttach.has_value()) || MenuState.PanicMode) {
        app::ScreenJoystick_FixedUpdate(__this, method);
        countdown = 3; //This is necessary for mouseup to zero out the delta movement
    }
    else if (countdown > 0) {
        app::ScreenJoystick_FixedUpdate(__this, method);
        countdown--;
    }
    
}


