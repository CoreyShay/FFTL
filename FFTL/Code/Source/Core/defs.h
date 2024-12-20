/*

Original author:
Corey Shay
corey@signalflowtechnologies.com

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include <cstdint>
#if defined(__GNUC__)
#	include <unistd.h> // _POSIX_VERSION
#endif

#include <type_traits>

#if !defined(__cplusplus)
#	error "C compilers not supported."
#endif

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#	define FFTL_CPP_VERSION 2020
#elif (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#	define FFTL_CPP_VERSION 2017
#else
#	error "C++17 compiler or greater required."
#endif

//	CPU Architecture
#if defined(_M_IX86) || defined(__i386__)
#	define FFTL_ARCHITECTURE_X86 1
#endif
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)
#	if !defined(FFTL_ARCHITECTURE_X86)
#		define FFTL_ARCHITECTURE_X86 1
#	endif
#	define FFTL_ARCHITECTURE_X64 1
#endif
#if defined(_M_ARM) || defined(__ARM) || defined(__ARM__) || defined(__arm) || defined(__arm__) || defined(__arm64) || defined(__arm64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
#	define FFTL_ARCHITECTURE_ARM 1
#endif

#if !defined(FFTL_ARCHITECTURE_X86) && !defined(FFTL_ARCHITECTURE_ARM)
#	error "FFTL Unsupported CPU architechure."
#endif

#if defined(__powerpc__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(_XENON)
#	define FFTL_BIGENDIAN 1
#else
#	define FFTL_LITTLEENDIAN 1
#endif

#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
#	define FFTL_64BIT 1
#endif

//	For vector intrinsics on non-x86 platforms (PowerPC/ARM), this can easily be modified to use them.
// Define your symbol for it here, and create your own .inl file that defines all the necessary math
// functions. Don't forget to include that .inl file at the bottom of MathCommon.h.
#if defined(__AVX512CD__)
#	define FFTL_AVX512CD 1
#endif
#if defined(__AVX512ER__)
#	define FFTL_AVX512ER 1
#endif
#if defined(__AVX512F__)
#	define FFTL_AVX512F 1
#endif
#if defined(__AVX512PF__)
#	define FFTL_AVX512PF 1
#endif
#if defined(__AVX512BW__)
#	define FFTL_AVX512BW 1
#endif
#if defined(__AVX512CD__)
#	define FFTL_AVX512CD 1
#endif
#if defined(__AVX512DQ__)
#	define FFTL_AVX512DQ 1
#endif
#if defined(__AVX512VL__)
#	define FFTL_AVX512VL 1
#endif

#if defined(__AVX2__) || defined(FFTL_ASSUME_AVX2)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#	define FFTL_AVX 1
#	define FFTL_AVX2 1
#elif defined(__AVX__) || defined(FFTL_ASSUME_AVX)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#	define FFTL_AVX 1
#elif defined(__SSE4_2__) || defined(FFTL_ASSUME_SSE4)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#elif defined(__SSE3__) || defined(FFTL_ASSUME_SSE3)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#elif defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP > 1) || defined(_M_X64) || defined(FFTL_USE_SSE2)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#elif defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP > 0) || defined(_M_X64) || defined(FFTL_USE_SSE)
#	define FFTL_SSE 1
#endif

//	Platforms
#if defined(__ANDROID__)
#	define FFTL_PLATFORM_ANDROID 1
#endif
#if defined(__ORBIS__)
#	define FFTL_PLATFORM_ORBIS 1
#	define FFTL_PLATFORM_PLAYSTATION 1
#elif defined(__PROSPERO__)
#	define FFTL_PLATFORM_PROSPERO 1
#	define FFTL_PLATFORM_PLAYSTATION 1
#elif defined(_DURANGO) || defined(_GAMING_XBOX_XBOXONE)
#	define FFTL_PLATFORM_DURANGO 1
#	define FFTL_PLATFORM_XBOX 1
#elif defined(_SCARLETT) || defined(_GAMING_XBOX_SCARLETT)
#	define FFTL_PLATFORM_SCARLETT 1
#	define FFTL_PLATFORM_XBOX 1
#elif defined(_WINDOWS) || defined(WIN32) || defined(__WIN32) || defined(__WIN32__)
#	define FFTL_PLATFORM_WINDOWS 1
#endif

#if defined(FFTL_PLATFORM_WINDOWS) || defined(FFTL_PLATFORM_PLAYSTATION) || defined(FFTL_PLATFORM_XBOX) || defined(_X86_) || defined(_M_IX86) || defined(_AMD64_) || defined(_M_X64)
#	define FFTL_PLATFORM_ARCHITECTURE_X86 1
#elif defined(__ANDROID__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
#	define FFTL_PLATFORM_ARCHITECTURE_ARM 1
#else
#	error "Missing platform architecture"
#endif

#if !defined(FFTL_FMA3) && (defined(FFTL_USE_FMA3) || defined(__FMA__) || defined(__FMA3__) || defined(__PROSPERO__) || defined(_SCARLETT))
#	define FFTL_FMA3 1
#endif
#if !defined(FFTL_FMA4) && (defined(FFTL_USE_FMA4) || defined(__FMA4__))
#	define FFTL_FMA4 1
#endif

#if defined(FFTL_SSE)
#	define FFTL_SSE_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SSE_ONLY(__stuff__)
#endif
#if defined(FFTL_SSE2)
#	define FFTL_SSE2_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SSE2_ONLY(__stuff__)
#endif
#if defined(FFTL_SSE3)
#	define FFTL_SSE3_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SSE3_ONLY(__stuff__)
#endif
#if defined(FFTL_SSE4)
#	define FFTL_SSE4_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SSE4_ONLY(__stuff__)
#endif
#if defined(FFTL_AVX)
#	define FFTL_AVX_ONLY(__stuff__) __stuff__
#else
#	define FFTL_AVX_ONLY(__stuff__)
#endif
#if defined(FFTL_AVX2)
#	define FFTL_AVX2_ONLY(__stuff__) __stuff__
#else
#	define FFTL_AVX2_ONLY(__stuff__)
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#	define FFTL_ARM_NEON 1
#endif

#if defined(FFTL_FMA3) || defined(FFTL_FMA4) || defined(FFTL_ARM_NEON)
#	define FFTL_HAS_FMA 1
#endif


#if defined(FFTL_PLATFORM_PLAYSTATION) || defined(FFTL_PLATFORM_XBOX) || defined(FFTL_ARM_NEON) || !defined(FFTL_SSE) // TODO: ARM NEON
#	define FFTL_CPU_INFO_KNOWN_TO_COMPILER 1
#endif

#if defined(FFTL_SSE) && (defined(_MSC_VER) && _MSC_VER >= 1700)
#	define FFTL_VECTORCALL __vectorcall
#elif 0 //defined(FFTL_SSE) && defined(__GNUC__)   //__attribute__((sseregparm)) doesn't seem to be recognized
#	define FFTL_VECTORCALL __attribute__((sseregparm))
#else
#	define FFTL_VECTORCALL
#endif

#if defined(FFTL_SSE) || defined(FFTL_ARM_NEON)
#	define FFTL_SIMD_F32x4 1
#endif
#if defined(FFTL_SSE2) || defined(FFTL_ARM_NEON)
#	define FFTL_SIMD_I32x4 1
#endif
#if defined(FFTL_AVX)
#	define FFTL_SIMD_F32x8 1
#endif
#if defined(FFTL_AVX2)
#	define FFTL_SIMD_I32x8 1
#endif

#if defined(FFTL_SIMD_F32x8)
#	define FFTL_SIMD_F32_WIDTH 8
#elif defined(FFTL_SIMD_F32x4)
#	define FFTL_SIMD_F32_WIDTH 4
#else
#	define FFTL_SIMD_F32_WIDTH 1
#endif

#if defined(FFTL_SIMD_I32x8)
#	define FFTL_SIMD_I32_WIDTH 8
#elif defined(FFTL_SIMD_I32x4)
#	define FFTL_SIMD_I32_WIDTH 4
#else
#	define FFTL_SIMD_I32_WIDTH 1
#endif

#if defined(FFTL_SIMD_F32x8)
#	define FFTL_SIMD_F32x8_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SIMD_F32x8_ONLY(__stuff__)
#endif
#if defined(FFTL_SIMD_F32x4)
#	define FFTL_SIMD_F32x4_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SIMD_F32x4_ONLY(__stuff__)
#endif
#if defined(FFTL_SIMD_I32x8)
#	define FFTL_SIMD_I32x8_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SIMD_I32x8_ONLY(__stuff__)
#endif
#if defined(FFTL_SIMD_I32x4)
#	define FFTL_SIMD_I32x4_ONLY(__stuff__) __stuff__
#else
#	define FFTL_SIMD_I32x4_ONLY(__stuff__)
#endif

#if !defined(FFTL_ENABLE_ASSERT) && (defined(FFTL_BUILD_DEBUG) || defined(DEBUG) || defined(_DEBUG))
#	define FFTL_ENABLE_ASSERT 1
#endif

#if !defined(FFTL_ENABLE_PROFILING) && (defined(FFTL_BUILD_DEBUG) || defined(DEBUG) || defined(_DEBUG) || defined(FFTL_BUILD_PROFILE) || defined(PROFILE) || defined(_PROFILE))
#	define FFTL_ENABLE_PROFILING 1
#endif

//	Make sure to include Platform/Timer.h
#if defined(FFTL_ENABLE_PROFILING)
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__) TimerScope<typename std::remove_pointer<decltype(__pTimer__)>::type> __name__(__pTimer__)
#	define FFTL_PROFILE_TIMERSCOPE_PAUSE(__name__, __pTimer__) TimerPauseScope<typename std::remove_pointer<decltype(__pTimer__)>::type> __name__(__pTimer__)
#else
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__)
#	define FFTL_PROFILE_TIMERSCOPE_PAUSE(__name__, __pTimer__)
#endif


#if defined(_MSC_VER) || defined(__ORBIS__) || defined(__PROSPERO__)
#	define FFTL_ASSERT_ALWAYS(___expr) (void)( (!!(___expr)) || ( __debugbreak(), 1) )
#elif defined(__GNUC__)
#	define FFTL_ASSERT_ALWAYS(___expr) (void)( (!!(___expr)) || ( __builtin_trap(), 1) )
#elif defined(_POSIX_VERSION)
#	define FFTL_ASSERT_ALWAYS(___expr) (void)( (!!(___expr)) || ( raise(SIGTRAP), 1) )
#else
#	define FFTL_ASSERT_ALWAYS(___expr) (void)( (!!(___expr)) || ( *((int*)0) = 0xdeadbeef ) )
#endif

#define FFTL_VERIFY_ALWAYS(___expr) FFTL_ASSERT_ALWAYS(___expr)
#define FFTL_VERIFY_EQ_ALWAYS(___cmp, ___expr) FFTL_ASSERT_ALWAYS(___cmp == ___expr)
#define FFTL_VERIFY_NEQ_ALWAYS(___cmp, ___expr) FFTL_ASSERT_ALWAYS(___cmp != ___expr)

#define FFTL_ASSERT_MSG_ALWAYS(___expr, msg, ...) FFTL_ASSERT_ALWAYS(___expr)
#define FFTL_VERIFY_MSG_ALWAYS(___expr, msg, ...) FFTL_VERIFY_ALWAYS(___expr)
#define FFTL_VERIFY_EQ_MSG_ALWAYS(___cmp, ___expr, msg, ...) FFTL_VERIFY_EQ_ALWAYS(___cmp, ___expr)
#define FFTL_VERIFY_NEQ_MSG_ALWAYS(___cmp, ___expr, msg, ...) FFTL_VERIFY_NEQ_ALWAYS(___cmp, ___expr)


#if defined(FFTL_ENABLE_ASSERT)
#	define FFTL_BUILD_QA 1
#	define FFTL_ASSERT(___expr) FFTL_ASSERT_ALWAYS(___expr)
#	define FFTL_VERIFY(___expr) FFTL_VERIFY_ALWAYS(___expr)
#	define FFTL_VERIFY_EQ(___cmp, ___expr) FFTL_VERIFY_EQ_ALWAYS(___cmp, ___expr)
#	define FFTL_VERIFY_NEQ(___cmp, ___expr) FFTL_VERIFY_NEQ_ALWAYS(___cmp, ___expr)
#	define FFTL_USING_ASSERTS 1
#else
#	define FFTL_ASSERT(___expr) ((void)0)
#	define FFTL_VERIFY(___expr) ___expr
#	define FFTL_VERIFY_EQ(___cmp, ___expr) ___expr
#	define FFTL_VERIFY_NEQ(___cmp, ___expr) ___expr
#	define FFTL_USING_ASSERTS 0

#	if defined(_MSC_VER)
#		undef FFTL_VERIFY
#		define FFTL_VERIFY(___expr)			\
		__pragma(warning(push))				\
		__pragma(warning(disable : 4552))	\
		__pragma(warning(disable : 4553))	\
		___expr								\
		__pragma(warning(pop))
#	endif
#endif

#define FFTL_ASSERT_MSG(___expr, msg, ...) FFTL_ASSERT(___expr)
#define FFTL_VERIFY_MSG(___expr, msg, ...) FFTL_VERIFY(___expr)
#define FFTL_VERIFY_EQ_MSG(___cmp, ___expr, msg, ...) FFTL_VERIFY_EQ(___cmp, ___expr)
#define FFTL_VERIFY_NEQ_MSG(___cmp, ___expr, msg, ...) FFTL_VERIFY_NEQ(___cmp, ___expr)

#if defined( __SNC__ )
#	define FFTL_DISABLE_OPTIMIZATION _Pragma( "control %push O=0" )
#	define FFTL_ENABLE_OPTIMIZATION	 _Pragma( "control %pop O" )
#elif defined( __clang__ ) || defined( __GNUC__ ) // #if defined( __SNC__ )
#	if defined( _DEBUG )
#		define FFTL_DISABLE_OPTIMIZATION
#		define FFTL_ENABLE_OPTIMIZATION
#	else
#		define FFTL_DISABLE_OPTIMIZATION									\
		_Pragma( "clang optimize off" )								\
		_Pragma( "GCC diagnostic push" )							\
		_Pragma( "GCC diagnostic ignored \"-Wunused-variable\"" )	\
		_Pragma( "GCC diagnostic ignored \"-Wunused-value\"" )		\
		_Pragma( "message \"OPTIMIZATION DISABLED!!( FFTL_DISABLE_OPTIMIZATION )\"" )
#		define FFTL_ENABLE_OPTIMIZATION		\
		_Pragma( "clang optimize on" )	\
		_Pragma( "GCC diagnostic pop" )	\
		_Pragma( "message \"OPTIMIZATION ENABLED!!( FFTL_ENABLE_OPTIMIZATION )\"" )
#	endif
#elif defined( _MSC_VER )
#	if defined( TL_BUILD_DEBUG ) || defined( _DEBUG )
#		define MacroStr( x )		#x
#		define MacroStr2( x )		MacroStr( x )
#		define PragmaError( desc ) __pragma( message( __FILE__ "(" MacroStr2( __LINE__ ) "): error:" #desc ) )
#		define FFTL_DISABLE_OPTIMIZATION
#		define FFTL_ENABLE_OPTIMIZATION
#	else // #if defined( TL_BUILD_DEBUG ) || defined( _DEBUG )
#		define FFTL_DISABLE_OPTIMIZATION				\
		__pragma( optimize( "", off ) )			\
		__pragma( warning( disable : 4748 ) )	\
		__pragma( warning( disable : 4189 ) )	\
		__pragma( message( "OPTIMIZATION DISABLED!!( FFTL_DISABLE_OPTIMIZATION )" ) )
#		define FFTL_ENABLE_OPTIMIZATION		\
		__pragma( optimize( "", on ) )	\
		__pragma( message( "OPTIMIZATION ENABLED!!( FFTL_ENABLE_OPTIMIZATION )" ) )
#	endif
#else
#	define FFTL_ENABLE_OPTIMIZATION
#	define FFTL_DISABLE_OPTIMIZATION
#endif

//	inline defs
#define FFTL_INLINE inline
#if defined(_MSC_VER)
#	define FFTL_FORCEINLINE __forceinline
#	define FFTL_NOINLINE __declspec(noinline)
#elif defined(FFTL_PLATFORM_ANDROID)
#	define FFTL_FORCEINLINE inline __attribute__((always_inline))
#	define FFTL_NOINLINE __attribute((noinline))
#else
#	define FFTL_FORCEINLINE inline __attribute__((always_inline))
#	define FFTL_NOINLINE __noinline
#endif

//	DLL defs
#if defined(_MSC_VER)
#	if defined(FFTL_DLL)
#		define FFTL_DLLEXPORT __declspec(dllexport)
#	else
#		define FFTL_DLLEXPORT __declspec(dllimport)
#	endif
#elif defined(FFTL_PLATFORM_ANDROID)
#	if defined(FFTL_DLL)
#		define FFTL_DLLEXPORT __attribute__((visibility("default")))
#	else
#		define FFTL_DLLEXPORT
#	endif
#else
#	if defined(FFTL_DLL)
#		define FFTL_DLLEXPORT __declspec(dllexport)
#	else
#		define FFTL_DLLEXPORT __declspec(dllimport)
#	endif
#endif

#if defined(_MSC_VER)
#	define FFTL_LOOP_VECTORIZE_DISABLE __pragma (loop(no_vector))
#elif defined(__clang__)
#	define FFTL_LOOP_VECTORIZE_DISABLE _Pragma("clang loop vectorize(disable) interleave(disable)")
#else
#	define FFTL_LOOP_VECTORIZE_DISABLE
#endif

#if defined(FFTL_FORCE_COND_INLINE)
#	define FFTL_COND_INLINE FFTL_FORCEINLINE
#else
#	define FFTL_COND_INLINE FFTL_NOINLINE
#endif

#if defined(__clang__)
#	define FFTL_HAS_BUILTIN(__x__) (__has_builtin(__x__))
#else
#	define FFTL_HAS_BUILTIN(__x__) (0)
#endif

#if defined(__has_cpp_attribute)
#	define FFTL_HAS_CPP_ATTRIBUTE(__x__) (__has_cpp_attribute(__x__))
#else
#	define FFTL_HAS_CPP_ATTRIBUTE(__x__) (0)
#endif

//	Likely / unlikely attributes introduced Visual Studio 16.6
#if FFTL_CPP_VERSION >= 2020 || (FFTL_HAS_CPP_ATTRIBUTE(likely) && FFTL_HAS_CPP_ATTRIBUTE(unlikely)) || defined(FFTL_PLATFORM_PLAYSTATION)
#	define FFTL_LIKELY [[likely]]
#	define FFTL_UNLIKELY [[unlikely]]
#else
#	define FFTL_LIKELY
#	define FFTL_UNLIKELY
#endif

#define FFTL_NODISCARD [[nodiscard]]
#define FFTL_SWITCH_FALLTHROUGH [[fallthrough]]
#define FFTL_MAYBE_UNUSED [[maybe_unused]]

#if FFTL_HAS_BUILTIN(__builtin_is_constant_evaluated)
#	define FFTL_IS_CONSTANT_EVALUATED __builtin_is_constant_evaluated()
#elif FFTL_CPP_VERSION >= 2020 || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)
#	define FFTL_IS_CONSTANT_EVALUATED (std::is_constant_evaluated())
#endif
#if !defined(FFTL_IS_CONSTANT_EVALUATED)
#	define FFTL_IS_CONSTANT_EVALUATED false
#endif

#if (FFTL_CPP_VERSION >= 2020) || defined(__cpp_consteval) || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)
#	define FFTL_CONSTEVAL consteval
#else
#	define FFTL_CONSTEVAL constexpr
#endif

#if (FFTL_CPP_VERSION >= 2020) || FFTL_HAS_BUILTIN(__builtin_bit_cast)
#	define FFTL_HAS_CONSTEXPR_BIT_CAST 1
#	define FFTL_BIT_CAST_CONSTEXPR constexpr
#else
#	define FFTL_HAS_CONSTEXPR_BIT_CAST 0
#	define FFTL_BIT_CAST_CONSTEXPR
#endif

#if defined(_MSC_VER)
#	pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif

#if defined(_MSC_VER)// || (defined(__WCHAR_TYPE__) && !defined(__ORBIS__))
#	define FFTL_WCHAR 1
#endif

namespace FFTL
{
typedef uint8_t			u8;
typedef int8_t			s8;
typedef uint16_t		u16;
typedef int16_t			s16;
typedef uint32_t		u32;
typedef int32_t			s32;
typedef uint64_t		u64;
typedef int64_t			s64;
typedef float			f32;
typedef double			f64;

//	Aliases and shorthands
typedef u8				byte;
typedef unsigned int	uint;

#if defined(FFTL_WCHAR)
typedef wchar_t			tchar;
#	define FFTL_T(x)	L ## x
#else
typedef char			tchar;
#	define FFTL_T(x)	x
#endif
} // namespace FFTL


#if !defined(_MSC_VER) && !defined(FFTL_PLATFORM_PLAYSTATION)
#	define FFTL_THREAD_USE_POSIX 1
#endif

