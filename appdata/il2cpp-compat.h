#pragma once
// Compatibility aliases for renamed IL2CPP functions in the latest game version.
// This file maps old function names used throughout the codebase to their updated names.

// MessageWriter overloads
#define MessageWriter_WriteBoolean(writer, val, method)  MessageWriter_Write(writer, val, method)
#define MessageWriter_WriteByte(writer, val, method)      MessageWriter_Write_2(writer, val, method)
#define MessageWriter_WriteUShort(writer, val, method)    MessageWriter_Write_3(writer, val, method)
#define MessageWriter_WriteInt32(writer, val, method)     MessageWriter_Write_5(writer, val, method)
#define MessageWriter_WriteString(writer, val, method)    MessageWriter_Write_8(writer, val, method)

// Debug functions renamed from Debug_X to Debug_2_X
#define Debug_Log(msg, method)          Debug_2_Log(msg, method)
#define Debug_LogError(msg, method)     Debug_2_LogError(msg, method)
#define Debug_LogException(ex, method)  Debug_2_LogException(ex, method)
#define Debug_LogWarning(msg, method)   Debug_2_LogWarning(msg, method)

// Constants renamed from Constants_1 to Constants
#define Constants_1_GetBroadcastVersion(method)  Constants_GetBroadcastVersion(method)
#define Constants_1_IsVersionModded(method)      Constants_IsVersionModded(method)

// Time functions renamed from Time_ to Time_1_
#define Time_get_deltaTime(method)        Time_1_get_deltaTime(method)
#define Time_get_fixedDeltaTime(method)   Time_1_get_fixedDeltaTime(method)

// GameObject renamed
#define GameObject_GetActive(go, method)  GameObject_get_active(go, method)

// Time_get_time missing alias
#define Time_get_time(method)  Time_1_get_time(method)

// Object_DestroyImmediate - new version takes 2 args (obj, allowDestroyingAssets)
#define Object_DestroyImmediate(obj, method)  Object_1_DestroyImmediate(obj, false, method)

// InnerNet_GameCode renamed to GameCode
#define InnerNet_GameCode_IntToGameName(id, method)  GameCode_IntToGameName(id, method)

// Camera enum C-style alias for C++ scoped enum
#define Camera_MonoOrStereoscopicEye__Enum_Mono  Camera_MonoOrStereoscopicEye__Enum::Mono
