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

#ifndef _FFTL_MATH_H
#define _FFTL_MATH_H

#include "defs.h"

//#define _USE_MATH_DEFINES
#include <math.h>
//#undef _USE_MATH_DEFINES

#if FFTL_SSE
	#include <xmmintrin.h>
#endif
#if FFTL_SSE2
	#include <emmintrin.h>
#endif
#if FFTL_SSE4
	#include <pmmintrin.h>
#endif
#if FFTL_AVX
	#include <immintrin.h>
#endif
#if FFTL_ARM_NEON
	#include <arm_neon.h>
#endif


namespace FFTL
{


static const f64 pi_64 = 3.14159265358979323846;
static const f32 pi_32 = 3.14159265358979323846f;
static const f64 invLog2_64 = 1.4426950408889634073599246810019;
static const f32 invLog2_32 = 1.4426950408889634073599246810019f;


template <typename T>
FFTL_FORCEINLINE void Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

FFTL_FORCEINLINE f64 LogBase(f64 base, f64 y) { return log(y) / log(base); }
FFTL_FORCEINLINE f32 LogBase(f32 base, f32 y) { return logf(y) / logf(base); }

FFTL_FORCEINLINE f64 Log2(f64 y) { return log(y) * invLog2_64; }
FFTL_FORCEINLINE f32 Log2(f32 y) { return logf(y) * invLog2_32; }

FFTL_FORCEINLINE f64 Cos(f64 y) { return cos(y); }
FFTL_FORCEINLINE f32 Cos(f32 y) { return cosf(y); }
FFTL_FORCEINLINE f64 Sin(f64 y) { return sin(y); }
FFTL_FORCEINLINE f32 Sin(f32 y) { return sinf(y); }
FFTL_FORCEINLINE f64 Tan(f64 y) { return tan(y); }
FFTL_FORCEINLINE f32 Tan(f32 y) { return tanf(y); }

FFTL_FORCEINLINE f64 Abs(f64 y) { return fabs(y); }
FFTL_FORCEINLINE f32 Abs(f32 y) { return fabsf(y); }

FFTL_FORCEINLINE f64 Sqrt(f64 y) { return sqrt(y); }
FFTL_FORCEINLINE f32 Sqrt(f32 y) { return sqrtf(y); }


template <typename T>
FFTL_FORCEINLINE T Square(T value) { return value*value; }

template <typename T>
FFTL_FORCEINLINE T Min(T a, T b) 
{
	return a < b ? a : b;
}
template <typename T>
FFTL_FORCEINLINE T Max(T a, T b) 
{
	return a > b ? a : b;
}

template <typename T>
FFTL_FORCEINLINE T Clamp(T value, T low, T high)
{
	return Min(high, Max(low, value));
}

template <typename T>
FFTL_FORCEINLINE T Lerp(T mu, T from, T to)
{
	return from + (mu * (to - from));
}

template <typename T>
FFTL_FORCEINLINE T CosInterpolate(T mu, T from, T to)
{
	T scale2 = ( (T)1 - (T)Cos(mu * (T)M_PI)) * (T)0.5;
	return Lerp(scale2, from, to);
}

template <typename T>
T CubicInterpolate(T mu, T p0, T p1, T p2, T p3)
{
	const T mu2 = mu * mu;
	const T a0 = p3 - p2 - p0 + p1;
	const T a1 = p0 - p1 - a0;
	const T a2 = p2 - p0;

	return (a0 * mu * mu2) + (a1 * mu2) + (a2 * mu) + p1;
}

template <typename T>
T OctavesToLinear(T linear)
{
	return pow((T)2, linear);
}

template <typename T>
T LinearToOctaves(T linear)
{
	return Log2(linear);
}






#if FFTL_SSE
	typedef __m128 Vec4f;
#elif FFTL_ARM_NEON
	typedef float32x4_t Vec4f;
#else
	struct Vec4f
	{
		float x,y,z,w;
	};
#endif
	
#if FFTL_SSE2
	typedef __m128d Vec2d;
#else
	struct Vec2d
	{
		double x,y;
	};
#endif
	
#if FFTL_AVX
	typedef __m256 Vec8f;
#else
	struct Vec8f
	{
		Vec4f a, b;
	};
#endif

class f32_4;
class f64_2;
class f32_8;


#if 0
	typedef Vec4f Vec4f_In;
	typedef Vec2d Vec2d_In;
	typedef Vec8f Vec8f_In;

	typedef f32_4 f32_4Arg;
	typedef f64_2 f64_2Arg;
	typedef f32_8 f32_8Arg;

#else
	typedef const Vec4f& Vec4f_In;
	typedef const Vec2d& Vec2d_In;
	typedef const Vec8f& Vec8f_In;

	typedef const f32_4& f32_4Arg;
	typedef const f64_2& f64_2Arg;
	typedef const f32_8& f32_8Arg;
#endif

//	Permute constants
enum enShuf
{
	sh_X = 0,
	sh_Y = 1,
	sh_Z = 2,
	sh_W = 3,
};
enum enShuf2
{
	sh_X1 = 0,
	sh_Y1 = 1,
	sh_Z1 = 2,
	sh_W1 = 3,
	sh_X2 = 4,
	sh_Y2 = 5,
	sh_Z2 = 6,
	sh_W2 = 7,
};

// Wraps around integers higher than 3
#define FFTL_PERMUTEMASK(x) (x & 3)


Vec4f V4fZero();
Vec4f V4fLoadA(const f32* pf);
Vec4f V4fLoadU(const f32* pf);
Vec4f V4fLoadAR(const f32* pf); // Aligned Reverse order loading.
Vec4f V4fLoadUR(const f32* pf); // Unaligned Reverse order loading.
Vec4f V4fLoad1(const f32* pf);
Vec4f V4fLoad2(const f32* pf);
Vec4f V4fLoad3(const f32* pf);
void V4fStoreA(f32* pf, Vec4f_In v);
void V4fStoreU(f32* pf, Vec4f_In v);
void V4fStore1(f32* pf, Vec4f_In v);
void V4fStore2(f32* pf, Vec4f_In v);
void V4fStore3(f32* pf, Vec4f_In v);
void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD);
Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w);
Vec4f V4fSplat4(f32 f);
Vec4f V4fSplat4(const f32* pf);
Vec4f V4fAnd(Vec4f_In a, Vec4f_In b);
Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b);
Vec4f V4fOr(Vec4f_In a, Vec4f_In b);
Vec4f V4fXOr(Vec4f_In a, Vec4f_In b);
Vec4f V4fAdd(Vec4f_In a, Vec4f_In b);
Vec4f V4fSub(Vec4f_In a, Vec4f_In b);
Vec4f V4fMul(Vec4f_In a, Vec4f_In b);
Vec4f V4fDiv(Vec4f_In a, Vec4f_In b);
Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a+b*c
Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c); // a-b*c
Vec4f V4fSqrt(Vec4f_In v);
float V4fHSumF(Vec4f_In v);
Vec4f V4fHSumV(Vec4f_In v);
Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b);
int V4fToIntMask(Vec4f_In v);
bool V4fIsEqual(Vec4f_In a, Vec4f_In b);

f32 V4fGetX(Vec4f_In v);
f32 V4fGetY(Vec4f_In v);
f32 V4fGetZ(Vec4f_In v);
f32 V4fGetW(Vec4f_In v);

Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b);
Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b);

Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b);
Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b);

Vec4f V4fReverse(Vec4f_In v);

template <enShuf T_X, enShuf T_Y, enShuf T_Z, enShuf T_W>
Vec4f V4fPermute( Vec4f_In v );

template <enShuf2 T_X, enShuf2 T_Y, enShuf2 T_Z, enShuf2 T_W>
Vec4f V4fPermute( Vec4f_In a, Vec4f_In b );




Vec2d V2dZero();
Vec2d V2dLoadA(const f64* pf);
Vec2d V2dLoadU(const f64* pf);
void V2dStoreA(f64* pf, Vec2d v);
void V2dStoreU(f64* pf, Vec2d_In v);
Vec2d V2dSet(f64 x, f64 y);
Vec2d V2dAnd(Vec2d_In a, Vec2d_In b);
Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b);
Vec2d V2dOr(Vec2d_In a, Vec2d_In b);
Vec2d V2dXOr(Vec2d_In a, Vec2d_In b);
Vec2d V2dAdd(Vec2d_In a, Vec2d_In b);
Vec2d V2dSub(Vec2d_In a, Vec2d_In b);
Vec2d V2dMul(Vec2d_In a, Vec2d_In b);
Vec2d V2dDiv(Vec2d_In a, Vec2d_In b);
Vec2d V2dSqrt(Vec2d_In v);
bool V2dIsEqual(Vec2d_In a, Vec2d_In b);




Vec8f V8fZero();
Vec8f V8fLoadA(const f32* pf);
Vec8f V8fLoadU(const f32* pf);
Vec8f V8fLoadAR(const f32* pf);
Vec8f V8fLoadUR(const f32* pf);
void V8fStoreA(f32* pf, Vec8f_In v);
void V8fStoreU(f32* pf, Vec8f_In v);
void V8fStore1(f32* pf, Vec8f_In v);
Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d);
Vec8f V8fSplat8(f32 f);
Vec8f V8fSplat8(const f32* pf);
Vec8f V8fSplat(Vec4f_In v);
Vec8f V8fAnd(Vec8f_In a, Vec8f_In b);
Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b);
Vec8f V8fOr(Vec8f_In a, Vec8f_In b);
Vec8f V8fXOr(Vec8f_In a, Vec8f_In b);
Vec8f V8fAdd(Vec8f_In a, Vec8f_In b);
Vec8f V8fSub(Vec8f_In a, Vec8f_In b);
Vec8f V8fMul(Vec8f_In a, Vec8f_In b);
Vec8f V8fDiv(Vec8f_In a, Vec8f_In b);
Vec8f V8fSqrt(Vec8f_In v);
bool V8fIsEqual(Vec8f_In a, Vec8f_In b);
Vec8f V8fReverse(Vec8f_In v);





enum enMoveAlignment
{
	kAligned, kUnaligned
};



class f32_4
{
public:
	FFTL_FORCEINLINE f32_4() {}
	FFTL_FORCEINLINE f32_4(Vec4f_In v) : m_v(v) {}
	FFTL_FORCEINLINE f32_4(f32_4Arg v) : m_v(v.m_v) {}
	FFTL_FORCEINLINE f32_4(f32 x, f32 y, f32 z, f32 w) : m_v(V4fSet(x, y, z, w)) {}
	FFTL_FORCEINLINE f32_4& operator=(f32_4Arg v)		{ m_v = v.m_v; return *this; }

	FFTL_FORCEINLINE static f32_4 GenZero()				{ return f32_4(V4fZero()); }

	FFTL_FORCEINLINE static f32_4 LoadA(const f32* pf)	{ return f32_4(V4fLoadA(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadU(const f32* pf)	{ return f32_4(V4fLoadU(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadAR(const f32* pf)	{ return f32_4(V4fLoadAR(pf)); }
	FFTL_FORCEINLINE static f32_4 LoadUR(const f32* pf)	{ return f32_4(V4fLoadUR(pf)); }
	FFTL_FORCEINLINE static f32_4 Load1(const f32* pf)	{ return f32_4(V4fLoad1(pf)); }
	FFTL_FORCEINLINE static f32_4 Load2(const f32* pf)	{ return f32_4(V4fLoad2(pf)); }
	FFTL_FORCEINLINE static f32_4 Load3(const f32* pf)	{ return f32_4(V4fLoad3(pf)); }
	FFTL_FORCEINLINE static f32_4 Splat4(const f32* pf)	{ return f32_4(V4fSplat4(pf)); }
	FFTL_FORCEINLINE static f32_4 Splat4(f32 f)			{ return f32_4(V4fSplat4(f)); }

	FFTL_FORCEINLINE void StoreA(f32* pf) const			{ V4fStoreA(pf, m_v); }
	FFTL_FORCEINLINE void StoreU(f32* pf) const			{ V4fStoreU(pf, m_v); }
	FFTL_FORCEINLINE void Store1(f32* pf) const			{ V4fStore1(pf, m_v); }
	FFTL_FORCEINLINE void Store2(f32* pf) const			{ V4fStore2(pf, m_v); }
	FFTL_FORCEINLINE void Store3(f32* pf) const			{ V4fStore3(pf, m_v); }
	FFTL_FORCEINLINE void Scatter(f32* pf, int iA, int iB, int iC, int iD) const	{ V4fScatter(pf, m_v, iA, iB, iC, iD); }

	template<enMoveAlignment _A>
	FFTL_FORCEINLINE static f32_4 Load(const f32* pf)	{ return f32_4(_A==kAligned ? V4fLoadA(pf) : V4fLoadU(pf)); }
	template<enMoveAlignment _A>
	FFTL_FORCEINLINE void Store(f32* pf) const			{ _A==kAligned ? V4fStoreA(pf, m_v) : V4fStoreU(pf, m_v); }


	FFTL_FORCEINLINE void Set(float x, float y, float z, float w)	{ m_v = V4fSet(x, y, z, w); }

	FFTL_FORCEINLINE f32 GetX() const					{ return V4fGetX(m_v); }
	FFTL_FORCEINLINE f32 GetY() const					{ return V4fGetY(m_v); }
	FFTL_FORCEINLINE f32 GetZ() const					{ return V4fGetZ(m_v); }
	FFTL_FORCEINLINE f32 GetW() const					{ return V4fGetW(m_v); }

	FFTL_FORCEINLINE f32_4 operator+(f32_4Arg b) const	{ return f32_4(V4fAdd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator-(f32_4Arg b) const	{ return f32_4(V4fSub(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator*(f32_4Arg b) const	{ return f32_4(V4fMul(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator/(f32_4Arg b) const	{ return f32_4(V4fDiv(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator&(f32_4Arg b) const	{ return f32_4(V4fAnd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator|(f32_4Arg b) const	{ return f32_4(V4fOr(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_4 operator^(f32_4Arg b) const	{ return f32_4(V4fXOr(m_v, b.m_v)); }

	FFTL_FORCEINLINE f32_4& operator+=(f32_4Arg b)		{ m_v = V4fAdd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator-=(f32_4Arg b)		{ m_v = V4fSub(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator*=(f32_4Arg b)		{ m_v = V4fMul(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator/=(f32_4Arg b)		{ m_v = V4fDiv(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator&=(f32_4Arg b)		{ m_v = V4fAnd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator|=(f32_4Arg b)		{ m_v = V4fOr(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_4& operator^=(f32_4Arg b)		{ m_v = V4fXOr(m_v, b.m_v);	return *this; }

	//	Unary operators
	FFTL_FORCEINLINE f32_4 operator+() const			{ return *this; }
	FFTL_FORCEINLINE f32_4 operator-() const			{ return GenZero() - *this; }

	FFTL_FORCEINLINE bool operator==(f32_4Arg b) const	{ return V4fIsEqual(m_v, b.m_v); }

	FFTL_FORCEINLINE const Vec4f& GetNative() const		{ return m_v; }

private:
	Vec4f m_v;
};

FFTL_FORCEINLINE void StoreA(f32* pf, f32_4Arg v)			{ v.StoreA(pf); }
FFTL_FORCEINLINE void StoreU(f32* pf, f32_4Arg v)			{ v.StoreU(pf); }

FFTL_FORCEINLINE f32_4 Sqrt(f32_4Arg v)						{ return f32_4(V4fSqrt(v.GetNative())); }
FFTL_FORCEINLINE f32_4 HSumV(f32_4Arg v)					{ return V4fHSumV(v.GetNative()); }
FFTL_FORCEINLINE f32 HSumF(f32_4Arg v)						{ return V4fHSumF(v.GetNative()); }

FFTL_FORCEINLINE f32_4 MergeXY(f32_4Arg a, f32_4Arg b)		{ return V4fMergeXY(a.GetNative(), b.GetNative()); }
FFTL_FORCEINLINE f32_4 MergeZW(f32_4Arg a, f32_4Arg b)		{ return V4fMergeZW(a.GetNative(), b.GetNative()); }
FFTL_FORCEINLINE f32_4 SplitXZ(f32_4Arg a, f32_4Arg b)		{ return V4fSplitXZ(a.GetNative(), b.GetNative()); }
FFTL_FORCEINLINE f32_4 SplitYW(f32_4Arg a, f32_4Arg b)		{ return V4fSplitYW(a.GetNative(), b.GetNative()); }

FFTL_FORCEINLINE f32_4 AddMul(f32_4Arg a, f32_4Arg b, f32_4Arg c)	{ return V4fAddMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a+b*c
FFTL_FORCEINLINE f32_4 SubMul(f32_4Arg a, f32_4Arg b, f32_4Arg c)	{ return V4fSubMul(a.GetNative(), b.GetNative(), c.GetNative()); } // a-b*c


template <enShuf T_X, enShuf T_Y, enShuf T_Z, enShuf T_W>
FFTL_FORCEINLINE f32_4 Permute( f32_4Arg v )				{ return f32_4(V4fPermute<T_X,T_Y,T_Z,T_W>(v.GetNative())); }

template <enShuf2 T_X, enShuf2 T_Y, enShuf2 T_Z, enShuf2 T_W>
FFTL_FORCEINLINE f32_4 Permute( f32_4Arg a, f32_4Arg b )	{ return f32_4(V4fPermute<T_X,T_Y,T_Z,T_W>(a.GetNative(), b.GetNative())); }

FFTL_FORCEINLINE f32_4 XXXX(f32_4Arg v)						{ return Permute<sh_X,sh_X,sh_X,sh_X>(v); }
FFTL_FORCEINLINE f32_4 YYYY(f32_4Arg v)						{ return Permute<sh_Y,sh_Y,sh_Y,sh_Y>(v); }
FFTL_FORCEINLINE f32_4 ZZZZ(f32_4Arg v)						{ return Permute<sh_Z,sh_Z,sh_Z,sh_Z>(v); }
FFTL_FORCEINLINE f32_4 WWWW(f32_4Arg v)						{ return Permute<sh_W,sh_W,sh_W,sh_W>(v); }
FFTL_FORCEINLINE f32_4 WZYX(f32_4Arg v)						{ return Permute<sh_W,sh_Z,sh_Y,sh_X>(v); }
FFTL_FORCEINLINE f32_4 ZYXX(f32_4Arg v)						{ return Permute<sh_Z,sh_Y,sh_X,sh_X>(v); }
FFTL_FORCEINLINE f32_4 Reverse(f32_4Arg v)					{ return f32_4(V4fReverse(v.GetNative())); }




class f32_8
{
public:
	FFTL_FORCEINLINE f32_8() {}
	FFTL_FORCEINLINE f32_8(Vec8f_In v) : m_v(v) {}
	FFTL_FORCEINLINE f32_8(f32_8Arg v) : m_v(v.m_v) {}
	FFTL_FORCEINLINE f32_8(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d) : m_v(V8fSet(x,y,z,w,a,b,c,d)) {}
	FFTL_FORCEINLINE f32_8& operator=(f32_8Arg v)		{ m_v = v.m_v; return *this; }

	FFTL_FORCEINLINE static f32_8 GenZero()				{ return f32_8(V8fZero()); }

	FFTL_FORCEINLINE static f32_8 LoadA(const f32* pf)	{ return f32_8(V8fLoadA(pf)); }
	FFTL_FORCEINLINE static f32_8 LoadU(const f32* pf)	{ return f32_8(V8fLoadU(pf)); }
	FFTL_FORCEINLINE static f32_8 LoadAR(const f32* pf)	{ return f32_8(V8fLoadAR(pf)); }
	FFTL_FORCEINLINE static f32_8 LoadUR(const f32* pf)	{ return f32_8(V8fLoadUR(pf)); }
	FFTL_FORCEINLINE static f32_8 Splat8(const f32* pf)	{ return f32_8(V8fSplat8(pf)); }
	FFTL_FORCEINLINE static f32_8 Splat(f32_4Arg v)		{ return f32_8(V8fSplat(v.GetNative())); }

	FFTL_FORCEINLINE void StoreA(f32* pf) const			{ V8fStoreA(pf, m_v); }
	FFTL_FORCEINLINE void StoreU(f32* pf) const			{ V8fStoreU(pf, m_v); }
	FFTL_FORCEINLINE void Store1(f32* pf) const			{ V8fStore1(pf, m_v); }

	template<enMoveAlignment _A>
	FFTL_FORCEINLINE static f32_8 Load(const f32* pf)	{ return f32_8(_A==kAligned ? V8fLoadA(pf) : V8fLoadU(pf)); }
	template<enMoveAlignment _A>
	FFTL_FORCEINLINE void Store(f32* pf) const			{ _A==kAligned ? V8fStoreA(pf, m_v) : V8fStoreU(pf, m_v); }

	FFTL_FORCEINLINE f32_8 operator+(f32_8Arg b) const	{ return f32_8(V8fAdd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator-(f32_8Arg b) const	{ return f32_8(V8fSub(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator*(f32_8Arg b) const	{ return f32_8(V8fMul(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator/(f32_8Arg b) const	{ return f32_8(V8fDiv(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator&(f32_8Arg b) const	{ return f32_8(V8fAnd(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator|(f32_8Arg b) const	{ return f32_8(V8fOr(m_v, b.m_v)); }
	FFTL_FORCEINLINE f32_8 operator^(f32_8Arg b) const	{ return f32_8(V8fXOr(m_v, b.m_v)); }

	FFTL_FORCEINLINE f32_8& operator+=(f32_8Arg b)		{ m_v = V8fAdd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator-=(f32_8Arg b)		{ m_v = V8fSub(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator*=(f32_8Arg b)		{ m_v = V8fMul(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator/=(f32_8Arg b)		{ m_v = V8fDiv(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator&=(f32_8Arg b)		{ m_v = V8fAnd(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator|=(f32_8Arg b)		{ m_v = V8fOr(m_v, b.m_v);	return *this; }
	FFTL_FORCEINLINE f32_8& operator^=(f32_8Arg b)		{ m_v = V8fXOr(m_v, b.m_v);	return *this; }

	//	Unary operators
	FFTL_FORCEINLINE f32_8 operator+() const			{ return *this; }
	FFTL_FORCEINLINE f32_8 operator-() const			{ return GenZero() - *this; }

	FFTL_FORCEINLINE bool operator==(f32_8Arg b) const	{ return V8fIsEqual(m_v, b.m_v); }

	FFTL_FORCEINLINE const Vec8f& GetNative() const		{ return m_v; }

private:
	Vec8f m_v;
};

FFTL_FORCEINLINE void StoreA(f32* pf, f32_8Arg v)		{ v.StoreA(pf); }
FFTL_FORCEINLINE void StoreU(f32* pf, f32_8Arg v)		{ v.StoreU(pf); }

FFTL_FORCEINLINE f32_8 Sqrt(f32_8Arg v)					{ return f32_8(V8fSqrt(v.GetNative())); }

FFTL_FORCEINLINE f32_8 Reverse(f32_8Arg v)				{ return f32_8(V8fReverse(v.GetNative())); }



class ScopedFlushDenormals
{
public:
	ScopedFlushDenormals();
	~ScopedFlushDenormals();
private:
	uint32 m_prevMode;
};


} // namespace FFTL

#if FFTL_SSE
	#include "SSE/Math_sse2.inl"
#elif FFTL_ARM_NEON
	#include "NEON/Math_NEON.inl"
#else
	#include "Default/Math_default.inl"
#endif

#if FFTL_AVX
	#include "SSE/Math_avx.inl"
#else
	#include "Default/Math_vec8_default.inl"
#endif





#endif // _FFTL_MATH_H
