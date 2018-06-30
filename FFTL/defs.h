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

#ifndef FFTL_DEFS_H
#define FFTL_DEFS_H

#include <type_traits>
#include <stddef.h>
#if defined(__GNUC__) || defined(__clang__)
#	include <unistd.h>
#endif


#if !defined(__cplusplus)
#	error C compilers not supported.
#endif

#if (__cplusplus >= 201703L) || (defined(_MSC_VER) && _MSC_VER >= 1911)
#	define FFTL_CPP_VERSION 17
#else
#	define FFTL_CPP_VERSION 14
#endif


#if (FFTL_CPP_VERSION >= 17)
#	define FFTL_IF_CONSTEXPR(__expr__) if constexpr (__expr__)
#else
#	define FFTL_IF_CONSTEXPR(__expr__) if (__expr__)
#endif

#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
#	define FFTL_64BIT 1
#endif

//	For vector intrinsics on non-x86 platforms (PowerPC/ARM), this can easily be modified to use them.
// Define your symbol for it here, and create your own .inl file that defines all the necessary math
// functions. Don't forget to include that .inl file at the bottom of MathCommon.h.
#if defined(__AVX2__) || defined(FFTL_USE_AVX2)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#	define FFTL_AVX 1
#	define FFTL_AVX2 1
#	define FFTL_FMA3 1
#elif defined(__AVX__) || defined(FFTL_USE_AVX)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#	define FFTL_AVX 1
#elif defined(__SSE4_2__) || defined(FFTL_USE_SSE4)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#	define FFTL_SSE4 1
#elif defined(__SSE3__) || defined(FFTL_USE_SSE3)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#	define FFTL_SSE3 1
#elif defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP > 1) || defined(_M_X64) || defined(FFTL_USE_SSE2)
#	define FFTL_SSE 1
#	define FFTL_SSE2 1
#elif defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP > 0) || defined(_M_X64) || defined(FFTL_USE_SSE)
#	define FFTL_SSE 1
#endif

#if !defined(FFTL_FMA3) && (defined(FFTL_USE_FMA3) || defined(__FMA__))
#	define FFTL_FMA3 1
#endif
#if !defined(FFTL_FMA4) && defined(FFTL_USE_FMA4)
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

#if defined(__ORBIS__) || defined(_DURANGO)
#	define FFTL_CPU_INFO_KNOWN_TO_COMPILER 1
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#	define FFTL_ARM_NEON 1
#endif

#if defined(FFTL_SSE) && defined(_MSC_VER) && _MSC_VER >= 1700
#	define FFTL_VECTORCALL __vectorcall
#elif defined(FFTL_SSE) && defined(__GNUC__)
#	define FFTL_VECTORCALL __attribute__((sseregparm))
#else
#	define FFTL_VECTORCALL
#endif

#if defined(FFTL_SSE) || defined(FFTL_ARM_NEON)
#	define FFTL_SIMD4 1
#endif
#if defined(FFTL_AVX)
#	define FFTL_SIMD8 1
#endif

#if defined(FFTL_SIMD8)
#	define FFTL_SIMD_WIDTH 8
#elif defined(FFTL_SIMD4)
#	define FFTL_SIMD_WIDTH 4
#else
#	define FFTL_SIMD_WIDTH 1
#endif

#define FFTL_StaticAssert(e) static_assert(e, "Compile-time assert triggered!")


#if defined(FFTL_ENABLE_ASSERT)
#	define FFTL_BUILD_QA 1
#	if defined(_MSC_VER) || defined(__ORBIS__)
#		define FFTL_ASSERT(___expr) (void)( (!!(___expr)) || ( __debugbreak(), 1) )
#	elif defined(_POSIX_VERSION)
#		define FFTL_ASSERT(___expr) (void)( (!!(___expr)) || ( raise(SIGTRAP), 1) )
#	else
#		define FFTL_ASSERT(___expr) (void)( (!!(___expr)) || ( *((int*)0) = 0xdeadbeef ) )
#	endif

#	define FFTL_VERIFY(___expr) FFTL_ASSERT(___expr)
#	define FFTL_VERIFY_EQ(___cmp, ___expr) FFTL_ASSERT(___cmp == ___expr)
#	define FFTL_VERIFY_NEQ(___cmp, ___expr) FFTL_ASSERT(___cmp != ___expr)

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


namespace FFTL
{


typedef unsigned char		u8;
typedef char				s8;
typedef unsigned short		u16;
typedef short				s16;
typedef unsigned int		u32;
typedef unsigned int		uint;
typedef int					s32;
typedef unsigned long long	u64;
typedef long long			s64;
typedef float				f32;
typedef double				f64;

//	Aliases and shorthands
typedef u8					byte;





} // namespace FFTL





#define FFTL_INVALID_SAMPLE_CLOCK 0xffffffffffffffff
#define FFTL_MAX_FILTER_CUTOFF_HZ (24000)
#define FFTL_MIN_FILTER_CUTOFF_HZ (0)

#if /*defined(__ORBIS__) ||*/ defined(__ANDROID__)
#	define FFTL_THREAD_USE_POSIX 1
#endif




#endif
