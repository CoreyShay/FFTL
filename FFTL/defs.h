/*

Original author:
Corey Shay
cshay892@gmail.com

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


#if !defined(__cplusplus)
#error C compilers not supported.
#endif


#ifndef NULL
#define NULL 0
#endif

//	For vector intrinsics on non-x86 platforms (PowerPC/ARM), this can easily be modified to use them.
// Define your symbol for it here, and create your own .inl file that defines all the necessary math
// functions. Don't forget to include that .inl file at the bottom of Math.h.
#if (defined(_M_IX86_FP) && _M_IX86_FP > 0) || defined(_M_X64) || defined(__AVX__) || defined(FFTL_USE_SSE)
	#define FFTL_SSE 1
#endif
#if (defined(_M_IX86_FP) && _M_IX86_FP > 1) || defined(_M_X64) || defined(__AVX__) || defined(FFTL_USE_SSE2)
	#define FFTL_SSE2 1
#endif
#if defined(__AVX__) || defined(FFTL_USE_AVX)
	#define FFTL_SSE3 1
	#define FFTL_SSE4 1
	#define FFTL_AVX 1
#endif

#if defined(__ARM_NEON__)
	#define FFTL_ARM_NEON 1
#endif

#if FFTL_SSE || FFTL_ARM_NEON
	#define FFTL_SIMD4 1
#else
	#define FFTL_SIMD4 0
#endif
#if FFTL_AVX
	#define FFTL_SIMD8 1
#else
	#define FFTL_SIMD8 0
#endif

#if defined(_MSC_VER)
	#define FFTL_ALIGN_BEGIN(x) __declspec( align(x) )
	#define FFTL_ALIGN_END(x)
#else
	#define FFTL_ALIGN_BEGIN(x)
	#define FFTL_ALIGN_END(x) __attribute__((aligned(x)))
#endif

#if (__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#define FFTL_StaticAssert(e) static_assert(e, "Compile-time assert triggered!")
#else
#define FFTL_MacroJoin( _X, sh_Y ) FFTL_MacroDoJoin(_X, sh_Y)
#define FFTL_MacroDoJoin( _X, sh_Y ) FFTL_MacroDoJoin2(_X, sh_Y)
#define FFTL_MacroDoJoin2( _X, sh_Y ) _X##sh_Y
#define FFTL_StaticAssert(e) typedef char FFTL_MacroJoin(FFTL_MacroJoin(__COMPILETIME_ASSERT, __LINE__), __)[(e)?1:-1]
#endif


#if defined(_DEBUG) || defined(FFTL_ENABLE_ASSERT)
	#if defined(_MSC_VER)
		#define FFTL_ASSERT(___expr) (void)( (!!(___expr)) || ( __debugbreak(), 1) )
	#else
		#define FFTL_ASSERT(___expr) (void)( (!!(___expr)) || ( *((int*)0) = 0xdeadbeef ) )
	#endif
#else
	#define FFTL_ASSERT(___expr) ((void)0)
#endif


#define FFTL_INLINE inline

#if defined(_MSC_VER)
	#define FFTL_FORCEINLINE __forceinline
	#define FFTL_NOINLINE __declspec(noinline)
#elif defined(__ANDROID__)
	#define FFTL_FORCEINLINE inline __attribute__((always_inline))
	#define FFTL_NOINLINE __attribute((noinline))
#else
	#define FFTL_FORCEINLINE inline __attribute__((always_inline))
	#define FFTL_NOINLINE __noinline
#endif

#if defined(FFTL_FORCE_COND_INLINE)
	#define FFTL_COND_INLINE FFTL_FORCEINLINE
#else
	#define FFTL_COND_INLINE FFTL_NOINLINE
#endif


namespace FFTL
{


typedef unsigned char		uint8;
typedef char				int8;
typedef uint8				byte;
typedef unsigned short		uint16;
typedef short				int16;
typedef unsigned long		uint32;
typedef unsigned long		uint;
typedef long				int32;
typedef unsigned long long	uint64;
typedef long long			int64;
typedef float				f32;
typedef double				f64;



template <typename T, uint T_N>
class FixedArray
{
public:
	T data[T_N];
	FFTL_FORCEINLINE T& operator[](uint32 n) { FFTL_ASSERT(n<T_N); return data[n]; }
	FFTL_FORCEINLINE const T& operator[](uint n) const { FFTL_ASSERT(n<T_N); return data[n]; }
	FFTL_FORCEINLINE T* operator+(uint n) { FFTL_ASSERT(n<T_N); return data+n; }
	FFTL_FORCEINLINE const T* operator+(uint n) const { FFTL_ASSERT(n<T_N); return data+n; }
	FFTL_FORCEINLINE static uint size() { return T_N; }
};

template <typename T, uint32 T_N>
class FFTL_ALIGN_BEGIN(16) FixedArray_Aligned16 : public FixedArray<T,T_N>
{
} FFTL_ALIGN_END(16);

template <typename T, uint32 T_N>
class FFTL_ALIGN_BEGIN(32) FixedArray_Aligned32 : public FixedArray<T,T_N>
{
} FFTL_ALIGN_END(32);

template<uint T_ALIGNMENT, typename T>
inline T AlignForward(const T& offset)
{
	FFTL_StaticAssert( (T_ALIGNMENT & (T_ALIGNMENT-1)) == 0);
	return (offset + (T_ALIGNMENT-1)) & ~(T_ALIGNMENT-1);
}


} // namespace FFTL


#endif
