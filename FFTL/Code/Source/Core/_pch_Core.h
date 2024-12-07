#pragma once


#if defined(_MSC_VER)
struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-
#	ifndef NOMINMAX
#		define NOMINMAX //	Kills the stupid min / max macros in windef.h
#	endif
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <SDKDDKVer.h>
#	include <windows.h>
#endif

#if defined(_DURANGO)
#	include <xdk.h>
#	include <wrl.h>
#endif

#include <atomic>
#include <cmath>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "defs.h"
#include "ReturnCodes.h"
#if __has_include("Math/MathCommon.h")
#	include "Math/MathCommon.h"
#endif
#if __has_include("Math/Vector2.h")
#	include "Math/Vector2.h"
#endif
#if __has_include("Math/Vector3.h")
#	include "Math/Vector3.h"
#endif
#if __has_include("Math/Vector4.h")
#	include "Math/Vector4.h"
#endif
#if __has_include("Math/Matrix44.h")
#	include "Math/Matrix44.h"
#endif
#if __has_include("Math/Matrix43.h")
#	include "Math/Matrix43.h"
#endif
#if __has_include("Math/Matrix33.h")
#	include "Math/Matrix33.h"
#endif
#if __has_include("Platform/Log.h")
#	include "Platform/Log.h"
#endif
#if __has_include("Utils/MetaProgramming.h")
#	include "Utils/MetaProgramming.h"
#endif
#if __has_include("Utils/StdFunctions.h")
#	include "Utils/StdFunctions.h"
#endif
#if __has_include("Utils/S24.h")
#	include "Utils/S24.h"
#endif
