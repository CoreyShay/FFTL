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

#if !defined(__cplusplus)
#	error "C compilers not supported."
#endif

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#	define FFTL_CPP_VERSION 17
#else
#	error "C++17 compiler or greater required."
#endif


#if (FFTL_CPP_VERSION >= 17)
#	define FFTL_IF_CONSTEXPR if constexpr
#	define FFTL_SWITCH_FALLTHROUGH [[fallthrough]]
#	define FFTL_NODISCARD [[nodiscard]]
#else
#	define FFTL_IF_CONSTEXPR if
#	define FFTL_SWITCH_FALLTHROUGH
#	define FFTL_NODISCARD
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

#if !defined(FFTL_FMA3) && (defined(FFTL_USE_FMA3) || defined(__FMA__) || defined(__FMA3__) || defined(__PROSPERO__))
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


#if defined(__ORBIS__) || defined(__PROSPERO__) || defined(_DURANGO) || defined(FFTL_ARM_NEON) // TODO: ARM NEON
#	define FFTL_CPU_INFO_KNOWN_TO_COMPILER 1
#endif

#if defined(FFTL_SSE) && ((defined(_MSC_VER) && _MSC_VER >= 1700) || defined(__clang__) && !defined(__ANDROID__) && !defined(__PROSPERO__))
#	define FFTL_VECTORCALL __vectorcall
#elif 0//defined(FFTL_SSE) && defined(__GNUC__)   __attribute__((sseregparm)) doesn't seem to be recognized
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
#	define FFTL_SIMD_F32x4_ONLY(__stuff__) __stuff__
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

#if defined(FFTL_BUILD_DEBUG)
#	define FFTL_ENABLE_ASSERT 1
#	if !defined(FFTL_ENABLE_PROFILING)
#		define FFTL_ENABLE_PROFILING 1
#	endif
#elif defined(FFTL_BUILD_RELEASE)
#	if !defined(FFTL_ENABLE_PROFILING)
#		define FFTL_ENABLE_PROFILING 1 // To be removed
#	endif
#endif


#if defined(_MSC_VER) || defined(__ORBIS__) || defined(__PROSPERO__)
#	define FFTL_ASSERT_ALWAYS(___expr) (void)( (!!(___expr)) || ( __debugbreak(), 1) )
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
#else
#	define FFTL_ASSERT(___expr) ((void)0)
#	define FFTL_VERIFY(___expr) ___expr
#	define FFTL_VERIFY_EQ(___cmp, ___expr) ___expr
#	define FFTL_VERIFY_NEQ(___cmp, ___expr) ___expr

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


#define FFTL_INLINE inline

#if defined(_MSC_VER)
#	define FFTL_FORCEINLINE __forceinline
#	define FFTL_NOINLINE __declspec(noinline)
#	if defined(FFTL_DLL)
#		define FFTL_DLLEXPORT __declspec(dllexport)
#	else
#		define FFTL_DLLEXPORT __declspec(dllimport)
#	endif
#elif defined(__ANDROID__)
#	define FFTL_FORCEINLINE inline __attribute__((always_inline))
#	define FFTL_NOINLINE __attribute((noinline))
#	ifdef FFTL_DLL
#		define FFTL_DLLEXPORT __attribute__((visibility("default")))
#	else
#		define FFTL_DLLEXPORT
#	endif
#else
#	define FFTL_FORCEINLINE inline __attribute__((always_inline))
#	define FFTL_NOINLINE __noinline
#	ifdef FFTL_DLL
#		define FFTL_DLLEXPORT __declspec(dllexport)
#	else
#		define FFTL_DLLEXPORT __declspec(dllimport)
#	endif
#endif

#if defined(FFTL_FORCE_COND_INLINE)
#	define FFTL_COND_INLINE FFTL_FORCEINLINE
#else
#	define FFTL_COND_INLINE FFTL_NOINLINE
#endif

#define FFTL_UNLIKELY(___expr) ___expr


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


#if /*defined(__ORBIS__) ||*/ defined(__ANDROID__)
#	define FFTL_THREAD_USE_POSIX 1
#endif

