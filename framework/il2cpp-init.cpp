#include "pch-il2cpp.h"
#include "il2cpp-init.h"
#include "il2cpp-helpers.h"

using namespace app;

// We only define the Class/TypeInfo pointers here as they are unique to this file
#define DO_APP_CLASS(n, s) n ## __Class* n ## __TypeInfo
namespace app {
#include "../appdata/il2cpp-classes.h"
}
#undef DO_APP_CLASS

void init_il2cpp()
{
    HMODULE moduleHandle = GetModuleHandleW(L"GameAssembly.dll");
    if (!moduleHandle) {
        // Optional: Add a check or log if the DLL isn't found
        return;
    }

    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleHandle);

    // 1. Initialize API Functions
    // We use the existing symbols from il2cpp-helpers.h and assign their addresses
    #define DO_API(r, n, p) n = reinterpret_cast<decltype(n)>(GetProcAddress(moduleHandle, #n))
    #include "../appdata/il2cpp-api-functions.h"
    #undef DO_API

    using namespace app;

    // 2. Initialize App Functions (Game specific functions)
    #define DO_APP_FUNC(a, r, n, p) n = reinterpret_cast<decltype(n)>(baseAddress + a)
    #define DO_APP_FUNC_METHODINFO(a, n) n = reinterpret_cast<MethodInfo*>(baseAddress + a)
    #include "../appdata/il2cpp-functions.h"
    #undef DO_APP_FUNC
    #undef DO_APP_FUNC_METHODINFO

    // 3. Initialize Class TypeInfos
 
#define DO_API(r, n, p) n = reinterpret_cast<r(*)p>(GetProcAddress(moduleHandle, #n))
    #include "../appdata/il2cpp-classes.h"
    #undef DO_APP_CLASS
}