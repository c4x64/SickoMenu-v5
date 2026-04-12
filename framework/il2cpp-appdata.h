#pragma once
#include <cstdint>
#include "il2cpp-types.h"
#include "il2cpp-translations.h"
#include "il2cpp-helpers.h"

#define DO_API(r, n, p) extern r (*n) p
#include "il2cpp-api-functions.h"
#undef DO_API

#define DO_APP_FUNC(a, r, n, p) extern r (*n) p
#define DO_APP_FUNC_METHODINFO(a, n) extern MethodInfo* n
namespace app {
#include "il2cpp-functions.h"
}
#undef DO_APP_FUNC
#undef DO_APP_FUNC_METHODINFO

#define DO_APP_CLASS(n, s) extern n ## __Class* n ## __TypeInfo
namespace app {
#include "il2cpp-classes.h"
}
#undef DO_APP_CLASS
