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

#include "../defs.h"
#include "../Containers/Array.h"
#include "../Utils/MetaProgramming.h"

#include <cmath>
#include <limits>
//#undef _USE_MATH_DEFINES

#if defined(FFTL_SSE)
#	if __has_include(<x86intrin.h>)
#		include <x86intrin.h>
#	elif __has_include(<intrin.h>)
#		include <intrin.h>
#	endif
#	include <xmmintrin.h>
#endif
#if defined(FFTL_SSE2)
#	include <emmintrin.h>
#endif
#if defined(FFTL_SSE3)
#	include <pmmintrin.h>
#endif
#if defined(FFTL_SSE4)
#	include <smmintrin.h>
#endif
#if defined(FFTL_AVX)
#	include <immintrin.h>
#endif
#if defined(FFTL_FMA3)
#	include <immintrin.h>
#endif
#if defined(FFTL_FMA4)
#	include <ammintrin.h>
#endif

#if defined(FFTL_ARM_NEON)
#	include <arm_neon.h>
#endif

#define FFTL_MATH_ASSERT(___expr, ...) FFTL_ASSERT(___expr)


#if defined(FFTL_SSE)
#	include "SSE/Utils_SSE.h"
#elif FFTL_ARM_NEON
#	include "NEON/Utils_NEON.h"
#endif

//	Fires off in clang tidy here for some reason when not appropriate
#if defined(__clang__)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmissing-braces"
#endif


namespace FFTL
{

#if defined(FFTL_SSE)
typedef __m128 Vec4f;
typedef __m128i Vec4i;
typedef __m128i Vec4u;
#elif FFTL_ARM_NEON
typedef float32x4_t Vec4f;
typedef int32x4_t Vec4i;
typedef uint32x4_t Vec4u;
#else
struct alignas(16) Vec4f { f32 x, y, z, w; };
struct alignas(16) Vec4i { s32 x, y, z, w; };
struct alignas(16) Vec4u { u32 x, y, z, w; };

#endif
	
#if defined(FFTL_SSE2)
typedef __m128d Vec2d;
#elif FFTL_ARM_NEON && defined(__aarch64__)
typedef float64x2_t Vec2d;
#else
struct alignas(16) Vec2d { f64 x, y; };
#endif
	
#if defined(FFTL_AVX)
typedef __m256 Vec8f;
#else
struct alignas(32) Vec8f { Vec4f a, b; };
#endif


class f32_4;
class f64_2;
class f32_8;
class mask32x4;
class mask32x8;


#if 0
typedef Vec4f Vec4f_In;
typedef Vec2d Vec2d_In;
typedef Vec8f Vec8f_In;
typedef Vec4u Vec4u_In;
typedef Vec4i Vec4i_In;
typedef f32_4 f32_4Arg;
typedef f64_2 f64_2Arg;
typedef f32_8 f32_8Arg;

#else
typedef const Vec4f& Vec4f_In;
typedef const Vec2d& Vec2d_In;
typedef const Vec8f& Vec8f_In;
typedef const Vec4u& Vec4u_In;
typedef const Vec4i& Vec4i_In;
typedef const f32_4& f32_4_In;
typedef const f64_2& f64_2_In;
typedef const f32_8& f32_8_In;
typedef const mask32x4& mask32x4_In;
#endif

// Wraps around integers higher than 3
#define FFTL_PERMUTEMASK(x) (x & 3)

template <typename T>
inline constexpr T PI_ = static_cast<T>(3.14159265358979323846264338327L); // Trailing underscore to prevent conflicts with #define PI

inline constexpr f32 PI_32 = PI_<f32>;
inline constexpr f64 PI_64 = PI_<f64>;

inline constexpr f64 invLog2_64 = 1.4426950408889634073599246810019;
inline constexpr f32 invLog2_32 = 1.4426950408889634073599246810019f;

inline constexpr f32 INV_SQRT2 = 0.707106781186547524401f;
inline constexpr f32 PI_4 = 0.78539816339744830961566084581988f;
inline constexpr f32 SPEED_OF_SOUND = 343.2f;
inline constexpr f32 NEAR_SPEED_OF_SOUND = SPEED_OF_SOUND * 0.95f;

}// namespace FFTL





namespace FFTL
{



namespace math_constexpr
{

template<std::size_t... Is> struct seq {};

template<std::size_t N, std::size_t... Is>
struct gen_seq : gen_seq<N - 1, N, Is...> {};

template<std::size_t... Is>
struct gen_seq<0, Is...> : seq<Is...> {};

constexpr long double pi = 3.14159265358979323846264338327L;
constexpr long double two_pi = 6.28318530717958647692528676656L;
constexpr long double half_pi = pi * 0.5L;

constexpr long double pi_v = pi;
constexpr long double two_pi_v = two_pi;
constexpr long double half_pi_v = half_pi;

FFTL_NODISCARD constexpr inline long double inverse(long double value)
{
	return (value == 0) ? 0.0 : 1.0L / value;
}
FFTL_NODISCARD constexpr inline long double factorial(intmax_t n)
{
	if (n == 0)
		return 1;
	auto result = static_cast<long double>(n);
	for (intmax_t i = n - 1; i > 0; --i)
	{
		result *= i;
	}
	return result;
}
FFTL_NODISCARD constexpr inline std::size_t max_factorial()
{
	std::size_t i = 0;
	long double d = 0;
	while ((d = factorial(i)) < (std::numeric_limits<long double>::max)())
	{
		++i;
	}
	return i;
}

template<class base, std::size_t N>
class FFTL_NODISCARD trig_coeffs
{
	using T = typename base::value_type;
	using array_type = FFTL::FixedArray<T, N>;

	template<std::size_t ... NS>
	constexpr static inline array_type _coeffs(seq<NS ...>)
	{
		return {base::coeff(NS) ...};
	}
public:
	constexpr static array_type coeffs = _coeffs(gen_seq<N>{});
};


template<class base, std::size_t N, class dcy = std::decay_t<typename base::value_type>>
FFTL_NODISCARD constexpr std::enable_if_t<std::is_floating_point<dcy>::value, dcy>
_sincos(typename base::value_type x) noexcept
{
	using c = trig_coeffs<base, N>;

	if (x != x && std::numeric_limits<dcy>::has_quiet_NaN)
	{
		return static_cast<dcy>(std::numeric_limits<dcy>::quiet_NaN());
	}
	else if (x - x != 0.0L && std::numeric_limits<dcy>::has_infinity)
	{
		return static_cast<dcy>(std::numeric_limits<dcy>::infinity());
	}
	else
	{
		dcy result = 0.0;//result accumulator
						 //do input range mapping
		dcy _x = base::range_reduce(x);
		//taylor series
		{
			const dcy x_2 = _x * _x; //store x^2
			dcy pow = base::initial_condition(_x);
			for (size_t i = 0; i < c::coeffs.size(); ++i)
			{
				auto cf = c::coeffs[i];
				result += cf * pow;
				pow *= x_2;
			}
		}
		return result;
	}
}
namespace detail
{
struct FFTL_NODISCARD _sin
{
	using value_type = long double;
	constexpr static inline long double coeff(std::size_t n) noexcept
	{
		return (n % 2 != 0 ? 1 : -1) * inverse(factorial((2 * n) - 1));
	}
	constexpr static inline long double range_reduce(long double x) noexcept
	{
		long double _x = x;
		_x += math_constexpr::pi;
		_x -= static_cast<std::size_t>(_x / math_constexpr::two_pi) * math_constexpr::two_pi;
		_x -= math_constexpr::pi;
		return _x;
	}
	constexpr static inline long double initial_condition(long double x) noexcept
	{
		return x;
	}
	constexpr static inline std::size_t default_N() noexcept
	{
		return 16;
	}
};
}// namespace math_constexpr

template<class T, std::size_t N = math_constexpr::detail::_sin::default_N()>
FFTL_NODISCARD constexpr inline std::decay_t<T> Sin(T x) noexcept
{
	return static_cast<T>(math_constexpr::_sincos<math_constexpr::detail::_sin, N>(x));
}
template<class T, std::size_t N = math_constexpr::detail::_sin::default_N()>
FFTL_NODISCARD constexpr inline std::decay_t<T> Cos(T x) noexcept
{
	return static_cast<T>(math_constexpr::_sincos<math_constexpr::detail::_sin, N>(x + math_constexpr::half_pi));
}

}



/// Returns the bit index position of the first set bit found starting from the MSB. Returns 0 if val is 0.
FFTL_NODISCARD inline u32			MS1Bit(u32 val);
FFTL_NODISCARD inline u32			MS1Bit(s32 val) { return MS1Bit(static_cast<u32>(val)); }
FFTL_NODISCARD inline u32			MS1Bit(u64 val);
FFTL_NODISCARD inline u32			MS1Bit(s64 val) { return MS1Bit(static_cast<u64>(val)); }
/// Returns the bit index position of the first set bit found starting from the LSB. Returns 0 if val is 0.
FFTL_NODISCARD inline u32			LS1Bit(u32 val);
FFTL_NODISCARD inline u32			LS1Bit(s32 val) { return LS1Bit(static_cast<u32>(val)); }
FFTL_NODISCARD inline u32			LS1Bit(u64 val);
FFTL_NODISCARD inline u32			LS1Bit(s64 val) { return LS1Bit(static_cast<u64>(val)); }


template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Log(T y)
{
	return std::log(y);
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T LogBase(T base, T y)
{
	return Log(y) / Log(base);
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Log2(T y)
{
#if !defined(__GNUC__) || defined(__LP64__)
	return std::log2(y);
#else
	return LogBase((T)2, y);
#endif
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Log10(T y)
{
	return std::log10(y);
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Pow(T x, T y)
{
	return std::pow(x, y);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Pow2(T b)
{
	return Pow(static_cast<T>(2), b);
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T>::is_integer, T>::type Pow2(T b)
{
	return static_cast<T>(1) << b;
}
template <uint b>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr uint Pow2()
{
	return 1u << (b);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Cos(T x)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return math_constexpr::Cos(x);
	else
		return std::cos(x);
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Sin(T x)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return math_constexpr::Sin(x);
	else
		return std::sin(x);
}

template <typename T>
inline void SinCos(T r, T &s, T &c)
{
	s = Sin(r);
	c = Cos(r);
}

#if defined(_MSC_VER) && !defined(_M_X64)
template<>
inline void SinCos(f32 r, f32 &s, f32 &c)
{
	_asm
	{
		fld r
		fsincos
		mov ecx, c
		fstp dword ptr[ecx]
		mov ecx, s
		fstp dword ptr[ecx]
	}
}
#endif

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Tan(T r)
{
	return std::tan(r);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Exp(T r)
{
	return std::exp(r);
}

template <typename FLOAT>
FFTL_NODISCARD constexpr FLOAT FastNormalizedSin( const FLOAT x )
{
	// Approximation of f(x) = sin(sqrt(x))/sqrt(x)
	// with weight function g(x) = 1/sqrt(x)
	// on interval [ 1e-50, pi*pi/4 ]
	// with a polynomial of degree 3.
	// p(x)=((-1.8363653976946785e-4*x+8.306325227159894e-3)*x-1.6664828381895057e-1)*x+9.9999661590800277e-1
	const FLOAT x2 = x * x;

	FLOAT u = static_cast<FLOAT>( -2.7826745268663658e-3 );
	u = u * x2 + static_cast<FLOAT>( 5.0611685929833776e-2 );
	u = u * x2 + static_cast<FLOAT>( -4.1120923903538607e-1 );
	u = u * x2 + static_cast<FLOAT>( 9.9999924560571454e-1 );

	return x * u;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Abs(T y) { return static_cast<T>(std::abs(y)); }


template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Square(T y) { return y * y; }

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Cube(T y) { return y * y * y; }

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T Sqrt(T f) { return std::sqrt(f); }

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T RSqrt(T f) { static_assert(std::is_same<decltype(f), f32>::value || std::is_same<decltype(f), f64>::value, "Must be a float"); }

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Min(T a, T b)
{
	return a < b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T1>::is_integer && std::numeric_limits<T2>::is_integer && (sizeof(T1) > sizeof(T2)), T1>::type Min(T1 a, T2 b)
{
	return a < b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T1>::is_integer && std::numeric_limits<T2>::is_integer && (sizeof(T1) < sizeof(T2)), T2>::type Min(T1 a, T2 b)
{
	return a < b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value && (sizeof(T1) > sizeof(T2)), T1>::type Min(T1 a, T2 b)
{
	return a < b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value && (sizeof(T1) < sizeof(T2)), T2>::type Min(T1 a, T2 b)
{
	return a < b ? a : b;
}
template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Max(T a, T b)
{
	return a > b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T1>::is_integer && std::numeric_limits<T2>::is_integer && (sizeof(T1) > sizeof(T2)), T1>::type Max(T1 a, T2 b)
{
	return a > b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T1>::is_integer && std::numeric_limits<T2>::is_integer && (sizeof(T1) < sizeof(T2)), T2>::type Max(T1 a, T2 b)
{
	return a > b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value && (sizeof(T1) > sizeof(T2)), T1>::type Max(T1 a, T2 b)
{
	return a > b ? a : b;
}
template <typename T1, typename T2>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value && (sizeof(T1) < sizeof(T2)), T2>::type Max(T1 a, T2 b)
{
	return a > b ? a : b;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE bool IsNan(T y)
{
	return std::isnan(y);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T NanCheck(T y)
{
	FFTL_ASSERT(!IsNan(y));
	return y;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE bool IsInfinite(T y)
{
	return std::isinf(y);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE bool IsFinite(T y)		// true only if not infinite and not NaN
{
	return std::isfinite(y);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE bool IsNonFinite(T y)	// Returns true if not finite, NaN included
{
	return !IsFinite(y);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE bool IsNearEqual(T a, T b, T tol = (T)0.001)
{
	return Abs(a - b) <= tol;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Clamp(T value, T low, T high)
{
	return Min(high, Max(low, value));
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Lerp(T mu, T from, T to)
{
	return from + (mu * (to - from));
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T FSel(T test, T retGE, T retLT)
{
	return test >= 0 ? retGE : retLT;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T Approach(T rate, T from, T to)
{
	const T diff = to - from;
	const T fMin =  Min(diff, +rate);
	const T delta = Max(fMin, -rate);
	return from + delta;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T CosInterpolate(T mu, T from, T to)
{
	T scale2 = ( (T)1 - Cos(mu * PI_<T>)) * (T)0.5;
	return Lerp(scale2, from, to);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T CubicInterpolate(T mu, T p0, T p1, T p2, T p3)
{
	const T mu2 = mu * mu;
	const T a0 = p3 - p2 - p0 + p1;
	const T a1 = p0 - p1 - a0;
	const T a2 = p2 - p0;

	return (a0 * mu * mu2) + (a1 * mu2) + (a2 * mu) + p1;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T DecibelsToLinear(T dB)
{
	return Pow((T)10, dB/(T)20);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T LinearToDecibels(T linear)
{
	return 20 * Log10(linear);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T OctavesToLinear(T oct)
{
	return Pow((T)2, oct);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T SemitonesToLinear(T semitones)
{
	return Pow((T)2, semitones / (T)12);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T CentsToLinear(T cents)
{
	return SemitonesToLinear(cents / (T)100);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T LinearToOctaves(T linear)
{
	return Log2(linear);
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T LinearToSemitones(T linear)
{
	return LinearToOctaves(linear) * (T)12;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T LinearToCents(T linear)
{
	return LinearToOctaves(linear) * (T)1200;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T DegreesToRadians(T deg)
{
	return (PI_<T> / (T)180) * deg;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE T RadiansToDegrees(T rad)
{
	return ((T)180 / PI_<T>) * rad;
}

#if defined(FFTL_SSE)
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR s32 bit_cast(const f32& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
	{
		return bit_cast_constexpr<s32>(v);
	}
	else
	{
#if defined(FFTL_VECTOR_USE_SSE4)
		return _mm_extract_ps(_mm_set_ss(v), 0);
#else
		return _mm_cvtsi128_si32(_mm_castps_si128(_mm_set_ss(v)));
#endif
	}
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR u32 bit_cast(const f32& v)
{
	return static_cast<u32>(bit_cast<s32>(v));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR __m128 bit_cast(const f32& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
	{
		FixedArray<f32, 4> arr = { v, 0.0f, 0.0f, 0.0f };
		return bit_cast<const __m128>(arr);
	}
	else
		return _mm_set_ss(v);
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR f32 bit_cast(const s32& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<f32>(v);
	else
		return _mm_cvtss_f32(_mm_castsi128_ps(_mm_cvtsi32_si128(v)));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR f32 bit_cast(const u32& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<f32>(v);
	else
		return bit_cast<f32>(static_cast<s32>(v));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR f32 bit_cast(const __m128& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
	{
		const auto arr = bit_cast<FixedArray<f32, 4>>(v);
		return arr[0];
	}
	else
		return _mm_cvtss_f32(v);
}

#if defined(FFTL_64BIT)
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR s64 bit_cast(const f64& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<s64>(v);
	else
		return _mm_cvtsi128_si64(_mm_castpd_si128(_mm_set_sd(v)));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR f64 bit_cast(const s64& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<f64>(v);
	else
		return _mm_cvtsd_f64(_mm_castsi128_pd(_mm_cvtsi64_si128(v)));
}
#endif
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR u64 bit_cast(const f64& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<u64>(v);
	else
		return static_cast<u64>(bit_cast<s64>(v));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR f64 bit_cast(const u64& v)
{
	if (FFTL_IS_CONSTANT_EVALUATED)
		return bit_cast_constexpr<f64>(v);
	else
		return bit_cast<f64>(static_cast<s64>(v));
}
#endif

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T>::is_integer && std::numeric_limits<T>::is_signed, T>::type Wrap(T val, T range)
{
	if (val < 0)
		val += range * (-val / range + 1);

	return val % range;
}

template <typename T>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr typename std::enable_if<std::numeric_limits<T>::is_integer && std::numeric_limits<T>::is_signed, T>::type Wrap(T val, T lowBnd, T end)
{
	const T range = end - lowBnd;

	if (val < lowBnd)
		val += range * ((lowBnd - val) / range + 1);

	return lowBnd + (val - lowBnd) % range;
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator|(T a, T b)
{
	return static_cast<T>(underlying_cast(a) | underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator|=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) |= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator&(T a, T b)
{
	return static_cast<T>(underlying_cast(a) & underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator&=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) &= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator^(T a, T b)
{
	return static_cast<T>(underlying_cast(a) ^ underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator^=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) ^= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator~(T a)
{
	return static_cast<T>(~underlying_cast(a));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator+(T a, typename std::underlying_type<T>::type b)
{
	return static_cast<T>(underlying_cast(a) + b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator+=(T& a, typename std::underlying_type<T>::type b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) += b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator-(T a, typename std::underlying_type<T>::type b)
{
	return static_cast<T>(underlying_cast(a) - b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T& operator-=(T& a, typename std::underlying_type<T>::type b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) -= b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T operator%(T a, T b)
{
	return static_cast<T>(underlying_cast(a) % underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr T Wrap(T val, T lowBnd, T end)
{
	static_assert(sizeof(T) <= sizeof(ptrdiff_t), "Only enums of reasonable size allowed");
	return static_cast<T>(Wrap(static_cast<ptrdiff_t>(val), static_cast<ptrdiff_t>(lowBnd), static_cast<ptrdiff_t>(end)));
}


#if defined(_MSC_VER)
FFTL_NODISCARD FFTL_FORCEINLINE u32 MS1Bit(u32 val)
{
	unsigned long r;
	const int b0 = _BitScanReverse(&r, val); // Emits the bsr x86 instruction
	return b0 != 0 ? r : 32; // b0 will be 0 if val is zero, else, 1.
}

FFTL_NODISCARD FFTL_FORCEINLINE u32 MS1Bit(u64 val)
{
#if defined(_M_X64)
	unsigned long r;
	const int b0 = _BitScanReverse64(&r, static_cast<unsigned __int64>(val)); // Emits the bsr x86 instruction
	return b0 != 0 ? r : 64; // b0 will be 0 if val is zero, else, 1.
#else
	unsigned long r0, r1;
	const int b0 = _BitScanReverse(&r0, static_cast<u32>(val)); // Emits the bsr x86 instruction
	const int b1 = _BitScanReverse(&r1, static_cast<u32>(val>>32)); // Emits the bsr x86 instruction
	return b1 != 0 ? r1 + 32 : b0 != 0 ? r0 : 64; // b0 will be 0 if val is zero, else, 1.
#endif
}
FFTL_NODISCARD FFTL_FORCEINLINE u32 LS1Bit(u32 val)
{
	unsigned long r;
	const int b0 = _BitScanForward(&r, val); // Emits the bsf x86 instruction
	return b0 != 0 ? r : 32; // b0 will be 0 if val is zero, else, 1.
}

FFTL_NODISCARD FFTL_FORCEINLINE u32 LS1Bit(u64 val)
{
#if defined(_M_X64)
	unsigned long r;
	const int b0 = _BitScanForward64(&r, static_cast<unsigned __int64>(val)); // Emits the bsf x86 instruction
	return b0 != 0 ? r : 64;
#else
	unsigned long r0, r1 = 0;
	const int b0 = _BitScanForward(&r0, static_cast<u32>(val)); // Emits the bsf x86 instruction
	const int b1 = _BitScanForward(&r1, static_cast<u32>(val >> 32)); // Emits the bsf x86 instruction
	return b1 != 0 ? r1 + 32 : b0 != 0 ? r0 : 64; // b1 will be 0 if val is zero, else, 1.
#endif
}
#elif defined(__GNUC__)
FFTL_NODISCARD FFTL_FORCEINLINE u32 MS1Bit(u32 val)
{
	const auto r = 31 - __builtin_clz(val); // Emits the bsf x86 instruction
	return val != 0 ? r : 32;
}

FFTL_NODISCARD FFTL_FORCEINLINE u32 MS1Bit(u64 val)
{
	const auto r = 63 - __builtin_clzll(val); // Emits the bsf x86 instruction
	return val != 0 ? r : 64;
}
FFTL_NODISCARD FFTL_FORCEINLINE u32 LS1Bit(u32 val)
{
	const auto r = __builtin_ctz(val); // Emits the bsr x86 instruction
	return val != 0 ? r : 32;
}

FFTL_NODISCARD FFTL_FORCEINLINE u32 LS1Bit(u64 val)
{
	const auto r = __builtin_ctzll(val); // Emits the bsr x86 instruction
	return val != 0 ? r : 64;
}
#else
FFTL_NODISCARD inline u32 MS1Bit(u32 val)
{
#error "Make sure there is no intrinsic instruction that does this on your platform before eliminating this error message."
	static constexpr u8 sLogTable[256] =
	{
		0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
	};

	if (val == 0)
		return 32;

	unsigned int temp1;
	unsigned int temp2;

	// if greater than 2^16
	temp1 = val >> 16;
	if (temp1)
	{
		temp2 = temp1 >> 8;
		return temp2 ? 24 + sLogTable[temp2] : 16 + sLogTable[temp1];
	}
	// else less than 2^16
	else
	{
		temp2 = val >> 8;
		return temp2 ? 8 + sLogTable[temp2] : sLogTable[val];
	}
}
FFTL_NODISCARD inline u32 MS1Bit(u64 val)
{
#error "Make sure there is no intrinsic instruction that does this on your platform before eliminating this error message."

	if (val == 0)
		return 64;

	u64 temp;

	// if greater than 2^32
	temp = val >> 32;
	if (temp)
	{
		return MS1Bit(safestatic_cast<u32>(temp)) + 32;
	}
	// else less than 2^32
	else
	{
		temp = val & 0xffffffffULL;
		return MS1Bit(safestatic_cast<u32>(u32>(temp));
	}
}
FFTL_NODISCARD inline u32 LS1Bit(u32 val)
{
#error "Make sure there is no intrinsic instruction that does this on your platform before eliminating this error message."

	if (val == 0)
		return 32;

	for (u32 i = 1, j = 0; i != 0; i <<= 1, ++j)
	{
		if (val & i)
			return j;
	}
	return 0;
}
FFTL_NODISCARD inline u32 LS1Bit(u64 val)
{
#error "Make sure there is no intrinsic instruction that does this on your platform before eliminating this error message."

	if (val == 0)
		return 64;

	for (u64 i = 1, u32 j = 0; i != 0; i <<= 1, ++j)
	{
		if (val & i)
			return j;
	}
	return 0;
}
#endif


#if defined(FFTL_SSE)
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f32 RSqrt(f32 f)
{
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(f)));
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f64 RSqrt(f64 f)
{
	//	_mm_rsqrt_sd does not exist, unfortunately.

	//	Taken from Quake 3 source code referenced here: https://en.wikipedia.org/wiki/Fast_inverse_square_root
	s64 i;
	f64 x2, y;
	constexpr f32 threehalfs = 1.5;

	x2 = f * 0.5;
	y  = f;
	i  = bit_cast<s64>(y);					// evil floating point bit level hacking
	i  = 0x5FE6EB50C7B537A9 - (i >> 1);		// what the fuck?
	y  = bit_cast<f64>(i);
	y  = y * (threehalfs - (x2 * y * y));	// 1st iteration
//	y  = y * (threehalfs - (x2 * y * y));	// 2nd iteration, this can be removed

	return y;

}
#elif defined(FFTL_ARM_NEON)
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f32 RSqrt(f32 f)
{
	return vget_lane_f32(vrsqrte_f32(vdup_n_f32(f)), 0);
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f64 RSqrt(f64 f)
{
#if defined(FFTL_64BIT)
	return vget_lane_f64(vrsqrte_f64(vdup_n_f64(f)), 0);
#else
	//	Taken from Quake 3 source code referenced here: https://en.wikipedia.org/wiki/Fast_inverse_square_root
	s64 i;
	f64 x2, y;
	constexpr f32 threehalfs = 1.5;

	x2 = f * 0.5;
	y  = f;
	i  = bit_cast<s64>(y);						// evil floating point bit level hacking
	i  = 0x5FE6EB50C7B537A9 - (i >> 1);			// what the fuck? 
	y  = bit_cast<f64>(i);
	y  = y * (threehalfs - (x2 * y * y));		// 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );	// 2nd iteration, this can be removed

	return y;
#endif
}
#else
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f32 RSqrt(f32 f)
{
	//	Taken from Quake 3 source code referenced here: https://en.wikipedia.org/wiki/Fast_inverse_square_root
	s32 i;
	f32 x2, y;
	constexpr f32 threehalfs = 1.5;

	x2 = f * 0.5f;
	y  = f;
	i  = bit_cast<s32>(y);						// evil floating point bit level hacking
	i  = 0x5f3759df - (i >> 1);					// what the fuck? 
	y  = bit_cast<f32>(i);
	y  = y * (threehalfs - (x2 * y * y));		// 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );	// 2nd iteration, this can be removed

	return y;
}
template <>
FFTL_NODISCARD FFTL_FORCEINLINE f64 RSqrt(f64 f)
{
	//	Taken from Quake 3 source code referenced here: https://en.wikipedia.org/wiki/Fast_inverse_square_root
	s64 i;
	f64 x2, y;
	constexpr f32 threehalfs = 1.5;

	x2 = f * 0.5;
	y  = f;
	i  = bit_cast<s64>(y);						// evil floating point bit level hacking
	i  = 0x5FE6EB50C7B537A9 - (i >> 1);			// what the fuck? 
	y  = bit_cast<f64>(i);
	y  = y * (threehalfs - (x2 * y * y));		// 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );	// 2nd iteration, this can be removed

	return y;
}
#endif

FFTL_NODISCARD FFTL_FORCEINLINE f32 Floor(f32 val)
{
#if defined(FFTL_SSE) // Branchless implementation
	const __m128 v = _mm_set_ss(val);
	return _mm_cvtss_f32( sse_fFloor_ss(v) );
#else
	const s64 ival = safestatic_cast<s64>(val);
	return safestatic_cast<f32>(val < ival ? ival - 1 : ival);
#endif
}
FFTL_NODISCARD FFTL_FORCEINLINE f32 Ceil(f32 val)
{
#if defined(FFTL_SSE) // Branchless implementation
	const __m128 v = _mm_set_ss(val);
	return _mm_cvtss_f32( sse_fCeil_ss(v) );
#else
	const s64 ival = safestatic_cast<s64>(val);
	return safestatic_cast<f32>((val - ival) > 0.0f ? ival + 1 : ival);
#endif
}

FFTL_NODISCARD FFTL_FORCEINLINE f32 AddMul(f32 a, f32 b, f32 c)
{
#if defined(FFTL_SSE)
	return _mm_cvtss_f32(sse_AddMul_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c)));
#elif defined(FFTL_ARM_NEON)
	return fmaf(b, c, a);
#else
	return std::fma(b, c, a);
#endif
} // a+b*c
FFTL_NODISCARD FFTL_FORCEINLINE f32 SubMul(f32 a, f32 b, f32 c)
{
#if defined(FFTL_SSE)
	return _mm_cvtss_f32(sse_SubMul_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c)));
#elif defined(FFTL_ARM_NEON)
#if defined(__clang__)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	float32x2_t ret = vdup_n_f32(a);
	__asm__("vmls.f32 %0, %1, %2" : "+w" (ret) : "w" (b), "w" (c));
	return vget_lane_f32(ret, 0);
#else
	return vmls_f32(a, b, c); // Fused-multiply-subtract
#endif
#else
	return a - b * c;
#endif
} // a-b*c
FFTL_NODISCARD FFTL_FORCEINLINE f32 MulAdd(f32 a, f32 b, f32 c)
{
#if defined(FFTL_SSE)
	return _mm_cvtss_f32(sse_MulAdd_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c)));
#elif defined(FFTL_ARM_NEON)
	return fmaf(a, b, c);
#else
	return std::fma(a, b, c);
#endif
} // a*b+c
FFTL_NODISCARD FFTL_FORCEINLINE f32 NMulAdd(f32 a, f32 b, f32 c)
{
#if defined(FFTL_SSE)
	return _mm_cvtss_f32(sse_NMulAdd_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c)));
#else
	return SubMul(c, a, b);
#endif
} // -a*b+c
FFTL_NODISCARD FFTL_FORCEINLINE f32 MulSub(f32 a, f32 b, f32 c)
{
#if defined(FFTL_SSE)
	return _mm_cvtss_f32(sse_SubMul_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c)));
#elif defined(FFTL_ARM_NEON)
	FFTL_ASSERT_MSG(0, "MulSub is sub-optimal on ARM, due to forced negation. Use SubMul instead.");
	return fmaf(a, b, -c); // Fused-multiply-subtract
#else
	return a * b - c;
#endif
} // a*b-c


FFTL_NODISCARD inline const f32* FindFirstValueNotEqualTo(f32 val, const f32* pBuffer, size_t nSize)
{
#if defined(FFTL_SSE)
	constexpr size_t step = 4;
	const size_t vecSampleCount = (nSize / step) * step;
	const auto vVal = _mm_set1_ps(val);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* p = pBuffer + i;
		const auto f = _mm_loadu_ps(p);
		const auto c = _mm_cmpneq_ps(f, vVal);
		const auto m = _mm_movemask_ps(c);
		if (m != 0)
		{
			const auto r = LS1Bit(m);
			return p + (r >> 5);
		}
	}
#else
	const size_t vecSampleCount = 0;
#endif

	for (size_t i = vecSampleCount; i < nSize; i += 1)
	{
		const auto* p = pBuffer + i;
		const auto f = *p;
		if (f != val)
		{
			return p;
		}
	}

	return nullptr;
}

FFTL_NODISCARD inline const byte* FindFirstValueNotEqualTo(byte val, const byte* pBuffer, size_t nSize)
{
#if defined(FFTL_SSE)
	constexpr size_t step = 16;
	const size_t vecSampleCount = (nSize / step) * step;
	const auto vVal = _mm_set1_epi8(val);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* p = pBuffer + i;
		const auto f = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const auto c = _mm_cmpeq_epi8(f, vVal);
		const auto m = static_cast<u16>(_mm_movemask_epi8(c));
		if (m != 0xffff)
		{
			const auto r = LS1Bit(~m);
			return p + r;
		}
	}
#else
	const size_t vecSampleCount = 0;
#endif

	for (size_t i = vecSampleCount; i < nSize; i += 1)
	{
		const auto* p = pBuffer + i;
		const auto f = *p;
		if (f != val)
		{
			return p;
		}
	}

	return nullptr;
}

FFTL_NODISCARD inline const f32* FindLastValueNotEqualTo(f32 val, const f32* pBuffer, size_t nSize)
{
	const auto* pEnd = pBuffer + nSize;
#if defined(FFTL_SSE)
	constexpr size_t step = 4;
	const size_t vecSampleCount = (nSize / step) * step;
	const auto vVal = _mm_set1_ps(val);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* p = pEnd - i - step;
		const auto f = _mm_loadu_ps(p);
		const auto c = _mm_cmpneq_ps(f, vVal);
		const auto m = _mm_movemask_ps(c);
		if (m != 0)
		{
			const auto r = MS1Bit(m);
			return p + r;
		}
	}
#else
	const size_t vecSampleCount = 0;
#endif

	for (size_t i = vecSampleCount; i < nSize; i += 1)
	{
		const auto* p = pEnd - i - 1;
		const auto f = *p;
		if (f != val)
		{
			return p;
		}
	}

	return nullptr;
}

FFTL_NODISCARD inline const byte* FindLastValueNotEqualTo(byte val, const byte* pBuffer, size_t nSize)
{
	const auto* pEnd = pBuffer + nSize;
#if defined(FFTL_SSE)
	constexpr size_t step = 16;
	const size_t vecSampleCount = (nSize / step) * step;
	const auto vVal = _mm_set1_epi8(val);

	for (size_t i = 0; i < vecSampleCount; i += step)
	{
		const auto* p = pEnd - i - step;
		const auto f = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
		const auto c = _mm_cmpeq_epi8(f, vVal);
		const auto m = _mm_movemask_epi8(c);
		if (m != 0xffff)
		{
			const auto r = MS1Bit(static_cast<u16>(~m));
			return p + r;
		}
	}
#else
	const size_t vecSampleCount = 0;
#endif

	for (size_t i = vecSampleCount; i < nSize; i += 1)
	{
		const auto* p = pEnd - i - 1;
		const auto f = *p;
		if (f != val)
		{
			return p;
		}
	}

	return nullptr;
}











FFTL_NODISCARD Vec4f V4fZero();
FFTL_NODISCARD Vec4f V4fLoadA(const f32* pf);
FFTL_NODISCARD Vec4f V4fLoadU(const f32* pf);
FFTL_NODISCARD Vec4f V4fLoadAR(const f32* pf); // Aligned Reverse order loading.
FFTL_NODISCARD Vec4f V4fLoadUR(const f32* pf); // Unaligned Reverse order loading.
FFTL_NODISCARD Vec4f V4fLoad1(const f32* pf);
FFTL_NODISCARD Vec4f V4fLoad2(const f32* pf);
FFTL_NODISCARD Vec4f V4fLoad3(const f32* pf);
void V4fStoreA(f32* pf, Vec4f_In v);
void V4fStoreU(f32* pf, Vec4f_In v);
void V4fStore1(f32* pf, Vec4f_In v);
void V4fStore2(f32* pf, Vec4f_In v);
void V4fStore3(f32* pf, Vec4f_In v);
void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD);
FFTL_NODISCARD Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w);
FFTL_NODISCARD Vec4f V4fSet1(f32 x);
FFTL_NODISCARD Vec4f V4fSplat(f32 f);
FFTL_NODISCARD Vec4f V4fSplat(const f32* pf);
FFTL_NODISCARD Vec4f V4fSplatXY(const f32* pf);
FFTL_NODISCARD Vec4f V4fAnd(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fOr(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fXOr(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fAdd(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fSub(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fMul(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fDiv(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fMin(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fMax(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a*b+c
FFTL_NODISCARD Vec4f V4fNMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c); // -a*b+c
FFTL_NODISCARD Vec4f V4fMulSub(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a*b-c
FFTL_NODISCARD Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a+b*c
FFTL_NODISCARD Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a-b*c
FFTL_NODISCARD Vec4f V4fNegate(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fAbs(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fSqrt(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fRcpSqrt(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fRcp(Vec4f_In v);
FFTL_NODISCARD f32 V4fHSumF(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fHSumV(Vec4f_In v);
FFTL_NODISCARD Vec4f V4fCompareEq(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fCompareNq(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fCompareGt(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fCompareLt(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fCompareGe(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fCompareLe(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD int V4fToIntMask(Vec4f_In v);
FFTL_NODISCARD bool V4fIsEqual(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD bool V4fIsAllZero(Vec4f_In v);

FFTL_NODISCARD f32 V4fGetX(Vec4f_In v);
FFTL_NODISCARD f32 V4fGetY(Vec4f_In v);
FFTL_NODISCARD f32 V4fGetZ(Vec4f_In v);
FFTL_NODISCARD f32 V4fGetW(Vec4f_In v);

FFTL_NODISCARD Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b);

FFTL_NODISCARD Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b);

FFTL_NODISCARD Vec4f V4fReverse(Vec4f_In v);

template <int T0, int T1, int T2, int T3>
FFTL_NODISCARD Vec4f V4fPermute(Vec4f_In v);

template <int T0, int T1, int T2, int T3>
FFTL_NODISCARD Vec4f V4fPermute(Vec4f_In a, Vec4f_In b);

FFTL_NODISCARD Vec4f V4fSin(Vec4f_In r);
FFTL_NODISCARD Vec4f V4fCos(Vec4f_In r);
void V4fSinCos(Vec4f_In r, Vec4f& s, Vec4f& c);



FFTL_NODISCARD Vec2d V2dZero();
FFTL_NODISCARD Vec2d V2dLoadA(const f64* pf);
FFTL_NODISCARD Vec2d V2dLoadU(const f64* pf);
void V2dStoreA(f64* pf, Vec2d v);
void V2dStoreU(f64* pf, Vec2d_In v);
FFTL_NODISCARD Vec2d V2dSet(f64 x, f64 y);
FFTL_NODISCARD Vec2d V2dAnd(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dOr(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dXOr(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dAdd(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dSub(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dMul(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dDiv(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD Vec2d V2dSqrt(Vec2d_In v);
FFTL_NODISCARD bool V2dIsEqual(Vec2d_In a, Vec2d_In b);
FFTL_NODISCARD bool V2dIsAllZero(Vec2d_In v);




FFTL_NODISCARD Vec8f V8fZero();
FFTL_NODISCARD Vec8f V8fLoadA(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoadU(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoadAR(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoadUR(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoad1(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoad2(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoad3(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoad4(const f32* pf);
FFTL_NODISCARD Vec8f V8fLoad6(const f32* pf);
void V8fStoreA(f32* pf, Vec8f_In v);
void V8fStoreU(f32* pf, Vec8f_In v);
void V8fStore1(f32* pf, Vec8f_In v);
void V8fStore2(f32* pf, Vec8f_In v);
void V8fStore3(f32* pf, Vec8f_In v);
void V8fStore4(f32* pf, Vec8f_In v);
void V8fStore6(f32* pf, Vec8f_In v);
FFTL_NODISCARD Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d);
FFTL_NODISCARD Vec8f V8fSet(Vec4f_In a, Vec4f_In b);
FFTL_NODISCARD Vec8f V8fSet0123(Vec8f_In a, Vec4f_In b);
FFTL_NODISCARD Vec8f V8fSet4567(Vec8f_In a, Vec4f_In b);
FFTL_NODISCARD Vec8f V8fSplat(f32 f);
FFTL_NODISCARD Vec8f V8fSplat(const f32* pf);
FFTL_NODISCARD Vec8f V8fSplat(Vec4f_In v);
FFTL_NODISCARD Vec8f V8fAnd(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fOr(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fXOr(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fAdd(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fSub(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fMul(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fMul(Vec8f_In a, Vec4f_In b);
FFTL_NODISCARD Vec8f V8fDiv(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fAddMul(Vec8f_In a, Vec8f_In b, Vec8f_In c);
FFTL_NODISCARD Vec8f V8fSubMul(Vec8f_In a, Vec8f_In b, Vec8f_In c);
FFTL_NODISCARD Vec8f V8fMin(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fMax(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fSqrt(Vec8f_In v);
FFTL_NODISCARD Vec8f V8fAbs(Vec8f_In v);
FFTL_NODISCARD Vec8f V8fCompareEq(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fCompareNq(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fCompareGt(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fCompareLt(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fCompareGe(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD Vec8f V8fCompareLe(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD int V8fToIntMask(Vec8f_In v);
FFTL_NODISCARD bool V8fIsEqual(Vec8f_In a, Vec8f_In b);
FFTL_NODISCARD bool V8fIsAllZero(Vec8f_In v);
FFTL_NODISCARD Vec8f V8fReverse(Vec8f_In v);
FFTL_NODISCARD Vec4f V8fAsV4f(Vec8f_In v);
FFTL_NODISCARD Vec4f V8fGet4567(Vec8f_In v);
FFTL_NODISCARD Vec8f V8fSin(Vec8f_In r);
FFTL_NODISCARD Vec8f V8fCos(Vec8f_In r);
void V8fSinCos(Vec8f_In r, Vec8f& s, Vec8f& c);


FFTL_NODISCARD Vec4u V4uAdd(Vec4u_In a, Vec4u_In b);
FFTL_NODISCARD Vec4u V4uSub(Vec4u_In a, Vec4u_In b);
FFTL_NODISCARD Vec4u V4uMul(Vec4u_In a, Vec4u_In b);
FFTL_NODISCARD Vec4i V4iAdd(Vec4i_In a, Vec4i_In b);
FFTL_NODISCARD Vec4i V4iSub(Vec4i_In a, Vec4i_In b);
FFTL_NODISCARD Vec4i V4iMul(Vec4i_In a, Vec4i_In b);

FFTL_NODISCARD Vec4i V4fRoundToVfi(Vec4f_In a);




enum enMoveAlignment
{
	kAligned, kUnaligned
};



class FFTL_NODISCARD f32_4
{
public:
	using InType = f32_4_In;

	enum enZeroType { ZERO };

	FFTL_FORCEINLINE static constexpr size_t GetSize() { return 4; }

	FFTL_FORCEINLINE f32_4() = default;
	FFTL_FORCEINLINE f32_4(enZeroType) : m_v(V4fZero()) {}
	constexpr FFTL_FORCEINLINE f32_4(Vec4f_In v) : m_v(v) {}
	constexpr FFTL_FORCEINLINE f32_4(f32_4_In v) = default;
	constexpr FFTL_FORCEINLINE f32_4(f32 x, f32 y, f32 z, f32 w) : m_v{ x, y, z, w } {}
	FFTL_FORCEINLINE f32_4& operator=(f32_4_In v) = default;
	FFTL_FORCEINLINE f32_4& operator=(Vec4f_In v)		{ m_v = v; return *this; }
	FFTL_NODISCARD FFTL_FORCEINLINE operator const Vec4f&() const { return GetNative(); }
	FFTL_NODISCARD FFTL_FORCEINLINE operator Vec4f&()	{ return GetNative(); }

	FFTL_FORCEINLINE static f32_4 Zero()				{ return f32_4(V4fZero()); }

	FFTL_FORCEINLINE static f32_4 LoadA(const f32* pf)	{ return f32_4(V4fLoadA(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadU(const f32* pf)	{ return f32_4(V4fLoadU(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadAR(const f32* pf)	{ return f32_4(V4fLoadAR(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadUR(const f32* pf)	{ return f32_4(V4fLoadUR(pf)); }
	FFTL_FORCEINLINE static f32_4 Load1(const f32* pf)	{ return f32_4(V4fLoad1(pf)); }
	FFTL_FORCEINLINE static f32_4 Load2(const f32* pf)	{ return f32_4(V4fLoad2(pf)); }
	FFTL_FORCEINLINE static f32_4 Load3(const f32* pf)	{ return f32_4(V4fLoad3(pf)); }
	FFTL_FORCEINLINE static f32_4 Set1(f32 f)			{ return f32_4(V4fSet1(f)); }
	FFTL_FORCEINLINE static f32_4 Set2(f32 x, f32 y);
	FFTL_FORCEINLINE static f32_4 Splat(const f32* pf)	{ return f32_4(V4fSplat(pf)); }
	FFTL_FORCEINLINE static f32_4 Splat(f32 f)			{ return f32_4(V4fSplat(f)); }
	FFTL_FORCEINLINE static f32_4 SplatXY(const f32* pf) { return f32_4(V4fSplatXY(pf)); }

	FFTL_FORCEINLINE void StoreA(f32* pf) const			{ V4fStoreA(pf, m_v); }
	FFTL_FORCEINLINE void StoreU(f32* pf) const			{ V4fStoreU(pf, m_v); }
	FFTL_FORCEINLINE void Store1(f32* pf) const			{ V4fStore1(pf, m_v); }
	FFTL_FORCEINLINE void Store2(f32* pf) const			{ V4fStore2(pf, m_v); }
	FFTL_FORCEINLINE void Store3(f32* pf) const			{ V4fStore3(pf, m_v); }
	FFTL_FORCEINLINE void Scatter(f32* pf, int iA, int iB, int iC, int iD) const	{ V4fScatter(pf, m_v, iA, iB, iC, iD); }

	template<enMoveAlignment _A> FFTL_FORCEINLINE static f32_4 Load(const f32* pf)	{ return _A == kAligned ? LoadA(pf) : LoadU(pf); }
	template<enMoveAlignment _A> FFTL_FORCEINLINE void Store(f32* pf) const			{ _A == kAligned ? StoreA(pf) : StoreA(pf); }

	template<uint N> FFTL_FORCEINLINE static f32_4 Load(const f32* pf)
	{
		static_assert(N >= 1 && N <= 4, "Only loads between 1 and 4 allowed");
		if constexpr (N == 1) return Load1(pf);
		if constexpr (N == 2) return Load2(pf);
		if constexpr (N == 3) return Load3(pf);
		if constexpr (N == 4) return LoadA(pf);
	}

	template<uint N> FFTL_FORCEINLINE void Store(f32* pf) const
	{
		static_assert(N >= 1 && N <= 4, "Only stores between 1 and 4 allowed");
		if		constexpr (N == 1)	Store1(pf);
		else if constexpr (N == 2)	Store2(pf);
		else if constexpr (N == 3)	Store3(pf);
		else						StoreA(pf);
	}

	FFTL_FORCEINLINE void Set(f32 x, f32 y, f32 z, f32 w)	{ m_v = V4fSet(x, y, z, w); }

	FFTL_NODISCARD FFTL_FORCEINLINE f32 GetX() const	{ return V4fGetX(m_v); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32 GetY() const	{ return V4fGetY(m_v); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32 GetZ() const	{ return V4fGetZ(m_v); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32 GetW() const	{ return V4fGetW(m_v); }

	FFTL_FORCEINLINE f32_4 operator+(f32_4_In b) const	{ return f32_4(V4fAdd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator-(f32_4_In b) const	{ return f32_4(V4fSub(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator*(f32_4_In b) const	{ return f32_4(V4fMul(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator/(f32_4_In b) const	{ return f32_4(V4fDiv(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator&(f32_4_In b) const	{ return f32_4(V4fAnd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator|(f32_4_In b) const	{ return f32_4(V4fOr(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator^(f32_4_In b) const	{ return f32_4(V4fXOr(m_v, b.m_v)); }

	FFTL_FORCEINLINE f32_4& operator+=(f32_4_In b)		{ m_v = V4fAdd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator-=(f32_4_In b)		{ m_v = V4fSub(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator*=(f32_4_In b)		{ m_v = V4fMul(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator/=(f32_4_In b)		{ m_v = V4fDiv(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator&=(f32_4_In b)		{ m_v = V4fAnd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator|=(f32_4_In b)		{ m_v = V4fOr(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator^=(f32_4_In b)		{ m_v = V4fXOr(m_v, b.m_v);	return *this; }

	//	Scalar methods
	FFTL_FORCEINLINE f32_4 operator*(f32 b) const		{ return f32_4(V4fMul(m_v, V4fSplat(b))); }
	FFTL_FORCEINLINE f32_4 operator/(f32 b) const		{ return f32_4(V4fDiv(m_v, V4fSplat(b))); }
	FFTL_FORCEINLINE f32_4& operator*=(f32 b)			{ m_v = V4fMul(m_v, V4fSplat(b));	return *this; }
	FFTL_FORCEINLINE f32_4& operator/=(f32 b)			{ m_v = V4fDiv(m_v, V4fSplat(b));	return *this; }

	//	Unary operators
	FFTL_FORCEINLINE f32_4 operator+() const			{ return *this; }
	FFTL_FORCEINLINE f32_4 operator-() const			{ return Zero() - *this; }

	FFTL_FORCEINLINE bool operator==(f32_4_In b) const		{ return V4fIsEqual(m_v, b.m_v); }

	//	Returns a vector mask
	FFTL_NODISCARD FFTL_FORCEINLINE mask32x4 IsZero() const;
	//	Returns a vector mask
	FFTL_NODISCARD FFTL_FORCEINLINE mask32x4 IsNonZero() const;
	FFTL_NODISCARD FFTL_FORCEINLINE bool IsAllZero() const	{ return V4fIsAllZero(m_v); }

	FFTL_NODISCARD f32_4 operator|(const mask32x4& b) const;
	FFTL_NODISCARD f32_4 operator&(const mask32x4& b) const;
	FFTL_NODISCARD f32_4 operator^(const mask32x4& b) const;
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const T& a, const T& b);
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const T& a, const mask32x4& b);
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const mask32x4& a, const T& b);

	f32_4& operator|=(const mask32x4& b);
	f32_4& operator&=(const mask32x4& b);
	f32_4& operator^=(const mask32x4& b);

	//	Comparison friends
	friend mask32x4		CmpEq(f32_4_In a, f32_4_In b);
	friend mask32x4		CmpNe(f32_4_In a, f32_4_In b);
	friend mask32x4		CmpLt(f32_4_In a, f32_4_In b);
	friend mask32x4		CmpLe(f32_4_In a, f32_4_In b);
	friend mask32x4		CmpGt(f32_4_In a, f32_4_In b);
	friend mask32x4		CmpGe(f32_4_In a, f32_4_In b);

	FFTL_NODISCARD FFTL_FORCEINLINE const Vec4f& GetNative() const		{ return m_v; }
	FFTL_NODISCARD FFTL_FORCEINLINE Vec4f& GetNative()					{ return m_v; }

protected:
	Vec4f m_v;
};

FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Min(f32_4_In a, f32_4_In b)					{ return f32_4(V4fMin(a.GetNative(), b.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Max(f32_4_In a, f32_4_In b)					{ return f32_4(V4fMax(a.GetNative(), b.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Sqrt(f32_4_In v)								{ return f32_4(V4fSqrt(v.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Abs(f32_4_In v)								{ return f32_4(V4fAbs(v.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 HSumV(f32_4_In v)								{ return V4fHSumV(v.GetNative()); }
FFTL_NODISCARD FFTL_FORCEINLINE f32 HSumF(f32_4_In v)								{ return V4fHSumF(v.GetNative()); }

FFTL_NODISCARD FFTL_FORCEINLINE f32_4 MergeXY(f32_4_In a, f32_4_In b)				{ return V4fMergeXY(a.GetNative(), b.GetNative()); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 MergeZW(f32_4_In a, f32_4_In b)				{ return V4fMergeZW(a.GetNative(), b.GetNative()); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 SplitXZ(f32_4_In a, f32_4_In b)				{ return V4fSplitXZ(a.GetNative(), b.GetNative()); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 SplitYW(f32_4_In a, f32_4_In b)				{ return V4fSplitYW(a.GetNative(), b.GetNative()); }

FFTL_NODISCARD FFTL_FORCEINLINE f32_4 MulAdd(f32_4_In a, f32_4_In b, f32_4_In c)		{ return V4fMulAdd(a.GetNative(), b.GetNative(), c.GetNative()); } // a*b+c
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 NMulAdd(f32_4_In a, f32_4_In b, f32_4_In c)	{ return V4fNMulAdd(a.GetNative(), b.GetNative(), c.GetNative()); } // -a*b+c
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 MulSub(f32_4_In a, f32_4_In b, f32_4_In c)		{ return V4fMulSub(a.GetNative(), b.GetNative(), c.GetNative()); } // a*b-c
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 AddMul(f32_4_In a, f32_4_In b, f32_4_In c)		{ return V4fAddMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a+b*c
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 SubMul(f32_4_In a, f32_4_In b, f32_4_In c)		{ return V4fSubMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a-b*c


template <int T0, int T1, int T2, int T3>
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Permute( f32_4_In v )							{ return f32_4(V4fPermute<T0,T1,T2,T3>(v.GetNative())); }

template <int T0, int T1, int T2, int T3>
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Permute( f32_4_In a, f32_4_In b )				{ return f32_4(V4fPermute<T0,T1,T2,T3>(a.GetNative(), b.GetNative())); }

FFTL_NODISCARD FFTL_FORCEINLINE f32_4 XXXX(f32_4_In v)								{ return Permute<0,0,0,0>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 YYYY(f32_4_In v)								{ return Permute<1,1,1,1>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 ZZZZ(f32_4_In v)								{ return Permute<2,2,2,2>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 WWWW(f32_4_In v)								{ return Permute<3,3,3,3>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 WZYX(f32_4_In v)								{ return Permute<3,2,1,0>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 ZYXX(f32_4_In v)								{ return Permute<2,1,0,0>(v); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Reverse(f32_4_In v)							{ return f32_4(V4fReverse(v.GetNative())); }

FFTL_FORCEINLINE f32_4 f32_4::Set2(f32 x, f32 y)									{ return Permute<0, 4, 1, 5>(Set1(x), Set1(y)); }

template<> FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Sin(f32_4 r)						{ return V4fSin(r.GetNative()); }
template<> FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Cos(f32_4 r)						{ return V4fCos(r.GetNative()); }
template<> FFTL_FORCEINLINE void SinCos(const f32_4 r, f32_4& s, f32_4& c)			{ V4fSinCos(r.GetNative(), s.GetNative(), c.GetNative()); }





class f32_8
{
public:
	using InType = f32_8_In;

	FFTL_NODISCARD FFTL_FORCEINLINE static constexpr size_t GetSize() { return 8; }

	FFTL_FORCEINLINE f32_8() = default;
	constexpr FFTL_FORCEINLINE f32_8(f32_8_In v) = default;
	constexpr FFTL_FORCEINLINE f32_8(Vec8f_In v) : m_v(v) {}
	FFTL_FORCEINLINE f32_8(f32_4_In a, f32_4_In b) : m_v(V8fSet(a.GetNative(), b.GetNative())) {}
	constexpr FFTL_FORCEINLINE f32_8(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d) : m_v{ x, y, z, w, a, b, c, d } {}
	FFTL_FORCEINLINE f32_8& operator=(f32_8_In v)		{ m_v = v.m_v; return *this; }
	FFTL_NODISCARD FFTL_FORCEINLINE operator const Vec8f&() const	{ return GetNative(); }
	FFTL_NODISCARD FFTL_FORCEINLINE operator Vec8f&()				{ return GetNative(); }

	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Zero()					{ return f32_8(V8fZero()); }

	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 LoadA(const f32* pf)	{ return f32_8(V8fLoadA(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 LoadU(const f32* pf)	{ return f32_8(V8fLoadU(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 LoadAR(const f32* pf)	{ return f32_8(V8fLoadAR(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 LoadUR(const f32* pf)	{ return f32_8(V8fLoadUR(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load1(const f32* pf)	{ return f32_8(V8fLoad1(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load2(const f32* pf)	{ return f32_8(V8fLoad2(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load3(const f32* pf)	{ return f32_8(V8fLoad3(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load4(const f32* pf)	{ return f32_8(V8fLoad4(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load6(const f32* pf)	{ return f32_8(V8fLoad6(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Splat(const f32* pf)	{ return f32_8(V8fSplat(pf)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Splat(f32 f)			{ return f32_8(V8fSplat(f)); }
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Splat(f32_4_In v)		{ return f32_8(V8fSplat(v.GetNative())); }

	FFTL_FORCEINLINE void StoreA(f32* pf) const			{ V8fStoreA(pf, m_v); }
	FFTL_FORCEINLINE void StoreU(f32* pf) const			{ V8fStoreU(pf, m_v); }
	FFTL_FORCEINLINE void Store1(f32* pf) const			{ V8fStore1(pf, m_v); }
	FFTL_FORCEINLINE void Store2(f32* pf) const			{ V8fStore2(pf, m_v); }
	FFTL_FORCEINLINE void Store3(f32* pf) const			{ V8fStore3(pf, m_v); }
	FFTL_FORCEINLINE void Store4(f32* pf) const			{ V8fStore4(pf, m_v); }
	FFTL_FORCEINLINE void Store6(f32* pf) const			{ V8fStore6(pf, m_v); }

	template<enMoveAlignment _A>
	FFTL_NODISCARD FFTL_FORCEINLINE static f32_8 Load(const f32* pf)	{ return f32_8(_A==kAligned ? V8fLoadA(pf) : V8fLoadU(pf)); }
	template<enMoveAlignment _A>
	FFTL_FORCEINLINE void Store(f32* pf) const			{ _A==kAligned ? V8fStoreA(pf, m_v) : V8fStoreU(pf, m_v); }

	template<uint N> FFTL_FORCEINLINE static f32_8 Load(const f32* pf)
	{
		static_assert((N >= 1 && N <= 4) || N == 6 || N == 8, "Only loads of some sizes allowed");
		if constexpr (N == 1) return Load1(pf);
		if constexpr (N == 2) return Load2(pf);
		if constexpr (N == 3) return Load3(pf);
		if constexpr (N == 4) return Load4(pf);
		if constexpr (N == 6) return Load6(pf);
		if constexpr (N == 8) return LoadA(pf);
	}

	template<uint N> FFTL_FORCEINLINE void Store(f32* pf) const
	{
		static_assert((N >= 1 && N <= 4) || N == 6 || N == 8, "Only stores between of some sizes allowed");
		if		constexpr (N == 1)	Store1(pf);
		else if constexpr (N == 2)	Store2(pf);
		else if constexpr (N == 3)	Store3(pf);
		else if constexpr (N == 4)	Store4(pf);
		else if constexpr (N == 6)	Store6(pf);
		else if constexpr (N == 8)	StoreA(pf);
	}

	FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Asf32_4() const				{ return V8fAsV4f(m_v); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Get0123() const				{ return V8fAsV4f(m_v); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_4 Get4567() const				{ return V8fGet4567(m_v); }

	FFTL_FORCEINLINE void Set0123(f32_4_In v)			{ m_v = V8fSet0123(m_v, v.GetNative()); }
	FFTL_FORCEINLINE void Set4567(f32_4_In v)			{ m_v = V8fSet4567(m_v, v.GetNative()); }

	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator+(f32_8_In b) const	{ return f32_8(V8fAdd(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator-(f32_8_In b) const	{ return f32_8(V8fSub(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator*(f32_8_In b) const	{ return f32_8(V8fMul(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator*(f32_4_In b) const	{ return f32_8(V8fMul(m_v, b.GetNative())); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator/(f32_8_In b) const	{ return f32_8(V8fDiv(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator&(f32_8_In b) const	{ return f32_8(V8fAnd(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator|(f32_8_In b) const	{ return f32_8(V8fOr(m_v, b.m_v)); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator^(f32_8_In b) const	{ return f32_8(V8fXOr(m_v, b.m_v)); }

	FFTL_FORCEINLINE f32_8& operator+=(f32_8_In b)		{ m_v = V8fAdd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator-=(f32_8_In b)		{ m_v = V8fSub(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator*=(f32_8_In b)		{ m_v = V8fMul(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator/=(f32_8_In b)		{ m_v = V8fDiv(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator&=(f32_8_In b)		{ m_v = V8fAnd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator|=(f32_8_In b)		{ m_v = V8fOr(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator^=(f32_8_In b)		{ m_v = V8fXOr(m_v, b.m_v);	return *this; }

	//	Scalar methods
#if FFTL_SIMD_F32x8
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator*(f32 b) const		{ return f32_8(V8fMul(m_v, V8fSplat(b))); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator/(f32 b) const		{ return f32_8(V8fDiv(m_v, V8fSplat(b))); }
	FFTL_FORCEINLINE f32_8& operator*=(f32 b)			{ m_v = V8fMul(m_v, V8fSplat(b));	return *this; }
	FFTL_FORCEINLINE f32_8& operator/=(f32 b)			{ m_v = V8fDiv(m_v, V8fSplat(b));	return *this; }
#else
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator*(f32 b) const		{ Vec4f vB = V4fSplat(b); return f32_8( V8fSet( V4fMul(m_v.a, vB), V4fMul(m_v.b, vB) ) ); }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator/(f32 b) const		{ Vec4f vB = V4fSplat(b); return f32_8( V8fSet( V4fDiv(m_v.a, vB), V4fDiv(m_v.b, vB) ) ); }
	FFTL_FORCEINLINE f32_8& operator*=(f32 b)			{ Vec4f vB = V4fSplat(b); m_v.a = V4fMul(m_v.a, vB); m_v.b = V4fMul(m_v.b, vB); return *this; }
	FFTL_FORCEINLINE f32_8& operator/=(f32 b)			{ Vec4f vB = V4fSplat(b); m_v.a = V4fDiv(m_v.a, vB); m_v.b = V4fDiv(m_v.b, vB); return *this; }
#endif

	//	Unary operators
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator+() const			{ return *this; }
	FFTL_NODISCARD FFTL_FORCEINLINE f32_8 operator-() const			{ return Zero() - *this; }

	FFTL_NODISCARD FFTL_FORCEINLINE bool operator==(f32_8_In b) const	{ return V8fIsEqual(m_v, b.m_v); }

	//	Returns a vector mask
	FFTL_NODISCARD mask32x8 IsZero() const;
	//	Returns a vector mask
	FFTL_NODISCARD mask32x8 IsNonZero() const;
	FFTL_NODISCARD FFTL_FORCEINLINE bool IsAllZero() const				{ return V8fIsAllZero(m_v); }


	FFTL_NODISCARD f32_8 operator|(const mask32x8& b) const;
	FFTL_NODISCARD f32_8 operator&(const mask32x8& b) const;
	FFTL_NODISCARD f32_8 operator^(const mask32x8& b) const;
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const T& b);
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const mask32x8& b);
	template<typename T> friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const mask32x8& a, const T& b);

	f32_8& operator|=(const mask32x8& b);
	f32_8& operator&=(const mask32x8& b);
	f32_8& operator^=(const mask32x8& b);

	//	Comparison friends
	friend mask32x8		CmpEq(f32_8_In a, f32_8_In b);
	friend mask32x8		CmpNe(f32_8_In a, f32_8_In b);
	friend mask32x8		CmpLt(f32_8_In a, f32_8_In b);
	friend mask32x8		CmpLe(f32_8_In a, f32_8_In b);
	friend mask32x8		CmpGt(f32_8_In a, f32_8_In b);
	friend mask32x8		CmpGe(f32_8_In a, f32_8_In b);

	FFTL_NODISCARD FFTL_FORCEINLINE const Vec8f& GetNative() const		{ return m_v; }
	FFTL_NODISCARD FFTL_FORCEINLINE Vec8f& GetNative()					{ return m_v; }
private:
	Vec8f m_v;
};

template<typename T> FFTL_FORCEINLINE T LoadA(const f32* pf) { return T::LoadA(pf); }
template<typename T> FFTL_FORCEINLINE T LoadU(const f32* pf) { return T::LoadU(pf); }
template<uint N, typename T> FFTL_FORCEINLINE T Load(const f32* pf) { return T::template Load<N>(pf); }
template<typename T> FFTL_FORCEINLINE T Load1(const f32* pf) { return T::Load<1>(pf); }
template<typename T> FFTL_FORCEINLINE T Load2(const f32* pf) { return T::Load<2>(pf); }
template<typename T> FFTL_FORCEINLINE T Load3(const f32* pf) { return T::Load<3>(pf); }

template<typename T> FFTL_FORCEINLINE T Splat(const f32* pf) { return T::Splat(pf); }
template<typename T> FFTL_FORCEINLINE T Splat(f32 f) { return T::Splat(f); }

template<typename T> FFTL_FORCEINLINE void StoreA(f32* pf, const T& v) { v.StoreA(pf); }
template<typename T> FFTL_FORCEINLINE void StoreU(f32* pf, const T& v) { v.StoreU(pf); }
template<uint N, typename T> FFTL_FORCEINLINE void Store(f32* pf, const T& v) { v.template Store<N>(pf); }
template<typename T> FFTL_FORCEINLINE void Store1(f32* pf, const T& v) { Store<1>(pf, v); }
template<typename T> FFTL_FORCEINLINE void Store2(f32* pf, const T& v) { Store<2>(pf, v); }
template<typename T> FFTL_FORCEINLINE void Store3(f32* pf, const T& v) { Store<3>(pf, v); }

//	f32 specializations
template<> FFTL_FORCEINLINE f32 Load<1, f32>(const f32* pf) { return *pf; }
template<> FFTL_FORCEINLINE f32 Splat<f32>(const f32* pf) { return *pf; }
template<> FFTL_FORCEINLINE f32 Splat<f32>(f32 f) { return f; }
template<> FFTL_FORCEINLINE void Store<1, f32>(f32* pf, const f32& v) { *pf = v; }

FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Min(f32_8_In a, f32_8_In b)		{ return f32_8(V8fMin(a.GetNative(), b.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Max(f32_8_In a, f32_8_In b)		{ return f32_8(V8fMax(a.GetNative(), b.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Sqrt(f32_8_In v)					{ return f32_8(V8fSqrt(v.GetNative())); }
FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Abs(f32_8_In v)					{ return f32_8(V8fAbs(v.GetNative())); }

FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Reverse(f32_8_In v)				{ return f32_8(V8fReverse(v.GetNative())); }

FFTL_NODISCARD FFTL_FORCEINLINE f32_8 AddMul(f32_8_In a, f32_8_In b, f32_8_In c) { return V8fAddMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a+b*c
FFTL_NODISCARD FFTL_FORCEINLINE f32_8 SubMul(f32_8_In a, f32_8_In b, f32_8_In c) { return V8fSubMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a-b*c

FFTL_NODISCARD FFTL_FORCEINLINE f32_8 TransformXandY(const f32_4& v, const f32_8& col0, const f32_8& col1)
{
	f32_8 result;
	result = col0 * XXXX(v);
	result += col1 * YYYY(v);
	return result;
}

template<>FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Sin(f32_8 r) { return V8fSin(r.GetNative()); }
template<>FFTL_NODISCARD FFTL_FORCEINLINE f32_8 Cos(f32_8 r) { return V8fCos(r.GetNative()); }
template<> FFTL_FORCEINLINE void SinCos(const f32_8 r, f32_8& s, f32_8& c) { V8fSinCos(r.GetNative(), s.GetNative(), c.GetNative()); }



// Purpose: mask32x4 - Holds a vector comparison result, or used to mask vectors using bitwise logical operators.
class mask32x4
{
	friend class f32_4;

public:
	mask32x4() = default;
	constexpr mask32x4(const mask32x4& v) = default;
	mask32x4& operator=(const mask32x4& v) = default;

#if defined(FFTL_SSE)
	constexpr mask32x4(const __m128& v);
	mask32x4& operator=(const __m128& v);
	FFTL_NODISCARD constexpr operator const __m128&() const;
	operator __m128&();
#elif defined(FFTL_ARM_NEON)
	constexpr mask32x4(const uint32x4_t& v);
	mask32x4& operator=(const uint32x4_t& v);
	FFTL_NODISCARD constexpr operator const uint32x4_t&() const;
	FFTL_NODISCARD operator uint32x4_t&();
#else
	constexpr mask32x4(u32 x, u32 y, u32 z, u32 w);
	u32							Get(uint n) const { return m_v[n]; }
	const u32*					Ptr() const { return m_v; }
	u32*						Ptr() { return m_v; }
#endif

	FFTL_NODISCARD mask32x4		operator|(const mask32x4& b) const;
	FFTL_NODISCARD mask32x4		operator&(const mask32x4& b) const;
	FFTL_NODISCARD mask32x4		operator^(const mask32x4& b) const;
	friend mask32x4				AndNot	 (const mask32x4& a, const mask32x4& b);

	mask32x4&					operator|=(const mask32x4& b) { return *this = *this | b; }
	mask32x4&					operator&=(const mask32x4& b) { return *this = *this & b; }
	mask32x4&					operator^=(const mask32x4& b) { return *this = *this ^ b; }

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type>
	FFTL_NODISCARD T				operator|(const T& b) const;

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type>
	FFTL_NODISCARD T				operator&(const T& b) const;

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type>
	FFTL_NODISCARD T				operator^(const T& b) const;

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	AndNot(const T& a, const T& b);

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	AndNot(const T& a, const mask32x4& b);

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	AndNot(const mask32x4& a, const T& b);


	FFTL_NODISCARD int			ToIntMask() const; // returns a 4 bit int containing the high bit from each element

	template<bool x, bool y, bool z, bool w>	FFTL_NODISCARD static mask32x4	GenMaskFromBools();
	template<s32 x, s32 y, s32 z, s32 w>		FFTL_NODISCARD static mask32x4	GenMaskFromInts();
	template<bool x, bool y, bool z, bool w>	FFTL_NODISCARD static mask32x4	PropagateInt(int i);
	template<s32 i>								FFTL_NODISCARD static mask32x4	GenMaskFromInt();

	FFTL_NODISCARD static mask32x4 Zero();

	//	Comparison friends
	friend mask32x4		CmpEq(mask32x4_In a, mask32x4_In b);
	friend mask32x4		CmpNe(mask32x4_In a, mask32x4_In b);
	friend mask32x4		CmpLt(mask32x4_In a, mask32x4_In b);
	friend mask32x4		CmpLe(mask32x4_In a, mask32x4_In b);
	friend mask32x4		CmpGt(mask32x4_In a, mask32x4_In b);
	friend mask32x4		CmpGe(mask32x4_In a, mask32x4_In b);


private:
#if defined(FFTL_SSE)
	__m128	m_v;
#elif defined(FFTL_ARM_NEON)
	uint32x4_t m_v;
#else
	alignas(16) u32 m_v[4];
#endif
};

//	Or: a | b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	Or(const T& a, const T& b) { return a | b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	Or(const T& a, const mask32x4& b) { return a | b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	Or(const mask32x4& a, const T& b) { return a | b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x4																Or(const mask32x4& a, const mask32x4& b) { return a | b; }

//	And: a & b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	And(const T& a, const T& b) { return a & b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	And(const T& a, const mask32x4& b) { return a & b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	And(const mask32x4& a, const T& b) { return a & b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x4	And(const mask32x4& a, const mask32x4& b) { return a & b; }

//	XOr: a ^ b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	XOr(const T& a, const T& b) { return a ^ b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	XOr(const T& a, const mask32x4& b) { return a ^ b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type	XOr(const mask32x4& a, const T& b) { return a ^ b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x4	XOr(const mask32x4& a, const mask32x4& b) { return a ^ b; }

//	AndNot: a & ~b
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type					AndNot(const T& a, const T& b);
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type					AndNot(const T& a, const mask32x4& b);
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type					AndNot(const mask32x4& a, const T& b);
FFTL_NODISCARD mask32x4																				AndNot(const mask32x4& a, const mask32x4& b);

// Comparisons that return a mask32x4 compare mask
FFTL_NODISCARD mask32x4		CmpEq(f32_4_In a, f32_4_In b);
FFTL_NODISCARD mask32x4		CmpNe(f32_4_In a, f32_4_In b);
FFTL_NODISCARD mask32x4		CmpLt(f32_4_In a, f32_4_In b);
FFTL_NODISCARD mask32x4		CmpLe(f32_4_In a, f32_4_In b);
FFTL_NODISCARD mask32x4		CmpGt(f32_4_In a, f32_4_In b);
FFTL_NODISCARD mask32x4		CmpGe(f32_4_In a, f32_4_In b);

//	Blend
template<typename T, bool bX, bool bY, bool bZ, bool bW>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const T& a, const T& b);
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const mask32x4& msk, const T& a, const T& b);


// Purpose: mask32x8 - Holds a vector comparison result, or used to mask vectors using bitwise logical operators.
class mask32x8
{
	friend class f32_8;

public:
	mask32x8() = default;
	constexpr mask32x8(const mask32x8& v) = default;
	mask32x8& operator=(const mask32x8& v) = default;

#if defined(FFTL_AVX)
	constexpr mask32x8(const __m256& v);
	mask32x8& operator=(const __m256& v);
	FFTL_NODISCARD constexpr operator const __m256&() const;
	operator __m256&();
#else
	constexpr mask32x8(const mask32x4& a, const mask32x4& b);
	u32							Get(uint n) const { return Ptr()[n]; }
	const u32*					Ptr() const { return reinterpret_cast<const u32*>(m_v.data()); }
	u32*						Ptr() { return reinterpret_cast<u32*>(m_v.data()); }
	const FixedArray_Aligned32<mask32x4, 2>& GetArray() const { return m_v; }
#endif

	FFTL_NODISCARD mask32x8		operator|(const mask32x8& b) const;
	FFTL_NODISCARD mask32x8		operator&(const mask32x8& b) const;
	FFTL_NODISCARD mask32x8		operator^(const mask32x8& b) const;
	friend mask32x8				AndNot	 (const mask32x8& a, const mask32x8& b);

	mask32x8&					operator|=(const mask32x8& b) { return *this = *this | b; }
	mask32x8&					operator&=(const mask32x8& b) { return *this = *this & b; }
	mask32x8&					operator^=(const mask32x8& b) { return *this = *this ^ b; }

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type>
	FFTL_NODISCARD T				operator|(const T& b) const;

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type>
	FFTL_NODISCARD T				operator&(const T& b) const;

	template<typename T, typename = typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type>
	FFTL_NODISCARD T				operator^(const T& b) const;

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	AndNot(const T& a, const T& b);

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	AndNot(const T& a, const mask32x8& b);

	template<typename T>
	friend typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	AndNot(const mask32x8& a, const T& b);

	FFTL_NODISCARD int			ToIntMask() const; // returns an 8 bit int containing the high bit from each element

	template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>	FFTL_NODISCARD static mask32x8	GenMaskFromBools();
	template<s32 i0, s32 i1, s32 i2, s32 i3, s32 i4, s32 i5, s32 i6, s32 i7>			FFTL_NODISCARD static mask32x8	GenMaskFromInts();
	template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>	FFTL_NODISCARD static mask32x8	PropagateInt(int i);
	template<s32 i>																		FFTL_NODISCARD static mask32x8	GenMaskFromInt();

	FFTL_NODISCARD static mask32x8 Zero();

private:
#if defined(FFTL_AVX)
	__m256	m_v;
#else
	FixedArray_Aligned32<mask32x4, 2> m_v;
#endif
};

//	Or: a | b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	Or(const T& a, const T& b) { return a | b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	Or(const T& a, const mask32x8& b) { return a | b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	Or(const mask32x8& a, const T& b) { return a | b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x8																Or(const mask32x8& a, const mask32x8& b) { return a | b; }

//	And: a & b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	And(const T& a, const T& b) { return a & b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	And(const T& a, const mask32x8& b) { return a & b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	And(const mask32x8& a, const T& b) { return a & b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x8																And(const mask32x8& a, const mask32x8& b) { return a & b; }

//	XOr: a ^ b
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	XOr(const T& a, const T& b) { return a ^ b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	XOr(const T& a, const mask32x8& b) { return a ^ b; }
template<typename T>
FFTL_NODISCARD FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type	XOr(const mask32x8& a, const T& b) { return a ^ b; }
FFTL_NODISCARD FFTL_FORCEINLINE mask32x8																XOr(const mask32x8& a, const mask32x8& b) { return a ^ b; }

//	AndNot: a & ~b
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type					AndNot(const T& a, const T& b);
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type					AndNot(const T& a, const mask32x8& b);
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type					AndNot(const mask32x8& a, const T& b);
FFTL_NODISCARD mask32x8																				AndNot(const mask32x8& a, const mask32x8& b);


// Comparisons that return a mask32x8 compare mask
FFTL_NODISCARD mask32x8		CmpEq(f32_8_In a, f32_8_In b);
FFTL_NODISCARD mask32x8		CmpNe(f32_8_In a, f32_8_In b);
FFTL_NODISCARD mask32x8		CmpLt(f32_8_In a, f32_8_In b);
FFTL_NODISCARD mask32x8		CmpLe(f32_8_In a, f32_8_In b);
FFTL_NODISCARD mask32x8		CmpGt(f32_8_In a, f32_8_In b);
FFTL_NODISCARD mask32x8		CmpGe(f32_8_In a, f32_8_In b);

//	Blend: return b(N) ? a[N] : b[N]
template<typename T, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const T& a, const T& b);

//	Blend: return msk ? a : b
template<typename T>
FFTL_NODISCARD typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const mask32x8& msk, const T& a, const T& b);


FFTL_FORCEINLINE mask32x4 f32_4::IsZero() const { return CmpEq( *this, f32_4::Zero() ); }
FFTL_FORCEINLINE mask32x4 f32_4::IsNonZero() const { return CmpNe( *this, f32_4::Zero() ); }
FFTL_FORCEINLINE mask32x8 f32_8::IsZero() const { return CmpEq( *this, f32_8::Zero() ); }
FFTL_FORCEINLINE mask32x8 f32_8::IsNonZero() const { return CmpNe( *this, f32_8::Zero() ); }




FFTL_NODISCARD FFTL_FORCEINLINE Vec4f Vec4DitherFloat(Vec4u& inout_nSeeds)
{
	constexpr f32 fRandToFloat = static_cast<f32>(1.0 / (1 << 16));
#if defined(FFTL_SSE)
	const Vec4u A = _mm_setr_epi32(214013, 17405, 214013, 69069);
	const Vec4u C = _mm_setr_epi32(2531011, 10395331, 13737667, 1);
	const Vec4u n65536 = _mm_set1_epi32(1 << 16);
	const Vec4f vRandToFloat = _mm_set1_ps(fRandToFloat);

	auto a = sse_MulInt32(A, inout_nSeeds);
	auto b = _mm_add_epi32(C, a);
	const auto r0 = _mm_srli_epi32(b, 16);
	a = sse_MulInt32(A, b);
	b = _mm_add_epi32(C, a);
	const auto r1 = _mm_srli_epi32(b, 16);
	inout_nSeeds = b;

	const auto nDither = _mm_sub_epi32(_mm_add_epi32(r0, r1), n65536);
	const auto fDither = _mm_mul_ps(_mm_cvtepi32_ps(nDither), vRandToFloat);
#elif defined(FFTL_ARM_NEON)
	const Vec4u A = { 214013, 17405, 214013, 69069 };
	const Vec4u C = { 2531011, 10395331, 13737667, 1 };
	const Vec4i n65536 = vdupq_n_s32(1 << 16);
	const Vec4f vRandToFloat = vdupq_n_f32(fRandToFloat);

	auto a = vmulq_u32(A, inout_nSeeds);
	auto b = vaddq_u32(C, a);
	const auto r0 = vshrq_n_u32(b, 16);
	a = vmulq_u32(A, b);
	b = vaddq_u32(C, a);
	const auto r1 = vshrq_n_u32(b, 16);
	inout_nSeeds = b;

	const auto nDither = vsubq_s32(vreinterpretq_s32_u32(vaddq_u32(r0, r1)), n65536);
	const auto fDither = vmulq_f32(vcvtq_f32_s32(nDither), vRandToFloat);
#else
	const Vec4u A = { 214013, 17405, 214013, 69069 };
	const Vec4u C = { 2531011, 10395331, 13737667, 1 };
	const Vec4i n65536 = { 1 << 16, 1 << 16, 1 << 16, 1 << 16 };
	const Vec4f vRandToFloat = { fRandToFloat, fRandToFloat, fRandToFloat, fRandToFloat };

	Vec4u a = V4uMul(A, inout_nSeeds);
	Vec4u b = V4uAdd(C, a);
	const Vec4u r0 = { b.x >> 16, b.y >> 16, b.z >> 16, b.w >> 16 };
	a = V4uMul(A, b);
	b = V4uAdd(C, a);
	const Vec4u r1 = { b.x >> 16, b.y >> 16, b.z >> 16, b.w >> 16 };
	inout_nSeeds = b;

	const auto r = V4uAdd(r0, r1);
	const auto nDither = V4iSub(bit_cast<const Vec4i&>(r), n65536);
	const Vec4f nDitherf = { static_cast<f32>(nDither.x), static_cast<f32>(nDither.y), static_cast<f32>(nDither.z), static_cast<f32>(nDither.w) };
	const auto fDither = V4fMul(nDitherf, vRandToFloat);
#endif

	return fDither;
}

FFTL_NODISCARD FFTL_FORCEINLINE Vec4i Vec4AddDitherInt32(const Vec4i& in_n32Signal, const Vec4f& fScale, Vec4u& inout_nSeeds)
{
	const auto vDitherf = Vec4DitherFloat(inout_nSeeds);
#if defined(FFTL_SSE)
	const auto vDithered = V4fAddMul(vDitherf, _mm_cvtepi32_ps(in_n32Signal), fScale);
#elif defined(FFTL_ARM_NEON)
	const auto vDithered = V4fAddMul(vDitherf, vcvtq_f32_s32(in_n32Signal), fScale);
#else
	const Vec4f in_f32Signal = { static_cast<f32>(in_n32Signal.x), static_cast<f32>(in_n32Signal.y), static_cast<f32>(in_n32Signal.z), static_cast<f32>(in_n32Signal.w) };
	const auto vDithered = V4fAddMul(vDitherf, in_f32Signal, fScale);
#endif
	return V4fRoundToVfi(vDithered);
}

FFTL_NODISCARD FFTL_FORCEINLINE Vec4i Vec4AddDitherFloat32(const Vec4f& in_f32Signal, const Vec4f& fScale, Vec4u& inout_nSeeds)
{
	const auto vDitherf = Vec4DitherFloat(inout_nSeeds);
	const auto vDithered = V4fAddMul(vDitherf, in_f32Signal, fScale);
	return V4fRoundToVfi(vDithered);
}

FFTL_NODISCARD FFTL_FORCEINLINE f32 DitherFloat(u32& inout_nSeed, size_t n)
{
	alignas(16) static constexpr u32 A[4] = { 214013, 17405, 214013, 69069 };
	alignas(16) static constexpr u32 C[4] = { 2531011, 10395331, 13737667, 1 };
	constexpr s32 n65536 = 1 << 16;
	constexpr f32 fRandToFloat = static_cast<f32>(1.0 / n65536);

	auto a = A[n] * inout_nSeed;
	auto b = C[n] + a;
	const auto r0 = b >> 16;
	a = A[n] * b;
	b = C[n] + a;
	inout_nSeed = b;
	const auto r1 = b >> 16;

	const auto nDither = static_cast<s32>(r0 + r1) - n65536;
	const auto fDither = static_cast<f32>(nDither) * fRandToFloat;
	return fDither;
}

//	Gets the CPU's current setting for flushing denormalized float values to zero
bool GetCpuFlushDenormalMode();

//	Sets the CPU's current setting for flushing denormalized float values to zero
void SetCpuFlushDenormalMode(bool bEnable);


class ScopedFlushDenormals
{
public:
	ScopedFlushDenormals(bool bEnable)
	{
		m_prevMode = GetCpuFlushDenormalMode();
		SetCpuFlushDenormalMode(bEnable);
	}
	~ScopedFlushDenormals()
	{
		SetCpuFlushDenormalMode(m_prevMode);
	}
private:
	bool m_prevMode;
};


} // namespace FFTL



#if defined(FFTL_SSE)
#	include "SSE/MathCommon_SSE.inl"
#	include "SSE/Utils_SSE.inl"
#elif FFTL_ARM_NEON
#	include "NEON/MathCommon_NEON.inl"
#	include "NEON/Utils_NEON.inl"
#else
#	include "Default/MathCommon_Default.inl"
#endif

#if defined(FFTL_AVX)
#	include "SSE/MathCommon_AVX.inl"
#else
#	include "Default/MathCommon_Vec8_Default.inl"
#endif

namespace FFTL
{
	FFTL_FORCEINLINE mask32x4 mask32x4::Zero() { return GenMaskFromInt<0>(); }
	FFTL_FORCEINLINE mask32x8 mask32x8::Zero() { return GenMaskFromInt<0>(); }
}


#if defined(__clang__)
#	pragma clang diagnostic pop
#endif


