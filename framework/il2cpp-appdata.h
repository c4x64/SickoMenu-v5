#pragma once
#include <cstdint>
#include "il2cpp-types.h"
#include "il2cpp-translations.h"

// We DO NOT include il2cpp-api-functions.h or il2cpp-functions.h here anymore.
// Those are now handled exclusively by il2cpp-helpers.h to prevent C2365 errors.

// Compatibility aliases for renamed IL2CPP functions
#include "il2cpp-compat.h"

// We keep the Class/TypeInfo pointers here as they are metadata, not functions.
#define DO_APP_CLASS(n, s) extern n ## __Class* n ## __TypeInfo
namespace app {
#include "../appdata/il2cpp-classes.h"
}
#undef DO_APP_CLASS