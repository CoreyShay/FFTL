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

#ifndef _FFTL_MATH_DEFAULT_INL
#define _FFTL_MATH_DEFAULT_INL
#else
#error "Already included"
#endif // !_FFTL_MATH_DEFAULT_INL

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4127 ) //conditional expression is constant
#endif


namespace FFTL
{




FFTL_FORCEINLINE Vec4f V4fZero()
{
	const Vec4f r = { 0, 0, 0, 0 };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fLoadA(const f32* pf)
{
	return *reinterpret_cast<const Vec4f*>(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadU(const f32* pf)
{
	return *reinterpret_cast<const Vec4f*>(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadAR(const f32* pf)
{
	const Vec4f r = { pf[3], pf[2], pf[1], pf[0] };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fLoadUR(const f32* pf)
{
	const Vec4f r = { pf[3], pf[2], pf[1], pf[0] };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	const Vec4f r = { pf[0], 0, 0, 0 };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	const Vec4f r = { pf[0], pf[1], 0, 0 };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fLoad3(const f32* pf)
{
	const Vec4f r = { pf[0], pf[1], pf[2], 0 };
	return r;
}
FFTL_FORCEINLINE void V4fStoreA(f32* pf, Vec4f_In v)
{
	*reinterpret_cast<Vec4f*>(pf) = v;
}
FFTL_FORCEINLINE void V4fStoreU(f32* pf, Vec4f_In v)
{
	*reinterpret_cast<Vec4f*>(pf) = v;
}
FFTL_FORCEINLINE void V4fStore1(f32* pf, Vec4f_In v)
{
	pf[0] = v.x;
}
FFTL_FORCEINLINE void V4fStore2(f32* pf, Vec4f_In v)
{
	pf[0] = v.x;
	pf[1] = v.y;
}
FFTL_FORCEINLINE void V4fStore3(f32* pf, Vec4f_In v)
{
	pf[0] = v.x;
	pf[1] = v.y;
	pf[2] = v.z;
}
FFTL_FORCEINLINE void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD)
{
	pf[iA] = v.x;
	pf[iB] = v.y;
	pf[iC] = v.z;
	pf[iD] = v.w;
}
FFTL_FORCEINLINE Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w)
{
	const Vec4f r = { x, y, z, w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSet1(f32 x)
{
	const Vec4f r = { x, 0, 0, 0 };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSplat4(f32 f)
{
	const Vec4f r = { f, f, f, f };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSplat4(const f32* pf)
{
	const f32 f = *pf;
	const Vec4f r = { f, f, f, f };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	const u32* pA = reinterpret_cast<const u32*>(&a);
	const u32* pB = reinterpret_cast<const u32*>(&b);
	const u32 r[4] = { pA[0] & pB[0], pA[1] & pB[1], pA[2] & pB[2], pA[3] & pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	const u32* pA = reinterpret_cast<const u32*>(&a);
	const u32* pB = reinterpret_cast<const u32*>(&b);
	const u32 r[4] = { ~pA[0] & pB[0], ~pA[1] & pB[1], ~pA[2] & pB[2], ~pA[3] & pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	const u32* pA = reinterpret_cast<const u32*>(&a);
	const u32* pB = reinterpret_cast<const u32*>(&b);
	const u32 r[4] = { pA[0] | pB[0], pA[1] | pB[1], pA[2] | pB[2], pA[3] | pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	const u32* pA = reinterpret_cast<const u32*>(&a);
	const u32* pB = reinterpret_cast<const u32*>(&b);
	const u32 r[4] = { pA[0] ^ pB[0], pA[1] ^ pB[1], pA[2] ^ pB[2], pA[3] ^ pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fMin(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { Min(a.x,b.x), Min(a.y,b.y), Min(a.z,b.z), Min(a.w,b.w) };
}
FFTL_FORCEINLINE Vec4f V4fMax(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { Max(a.x,b.x), Max(a.y,b.y), Max(a.z,b.z), Max(a.w,b.w) };
}
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	const Vec4f r = { a.x + b.x*c.x, a.y + b.y*c.y, a.z + b.z*c.z, a.w + b.w*c.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	const Vec4f r = { a.x - b.x*c.x, a.y - b.y*c.y, a.z - b.z*c.z, a.w - b.w*c.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fNegate(Vec4f_In v)
{
	const Vec4f r = { -v.x, -v.y, -v.z, -v.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fAbs(Vec4f_In v)
{
	const Vec4f r = { Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w) };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	const Vec4f r = { Sqrt(v.x), Sqrt(v.y), Sqrt(v.z), Sqrt(v.w) };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fRcpSqrt(Vec4f_In v)
{
	const Vec4f r = { RSqrt(v.x), RSqrt(v.y), RSqrt(v.z), RSqrt(v.w) };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fRcp(Vec4f_In v)
{
	const Vec4f r = { 1 / v.x, 1 / v.y, 1 / v.z, 1 / v.w };
	return r;
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	return v.x + v.y + v.z + v.w;
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	return V4fSplat4(V4fHSumF(v));
}
FFTL_FORCEINLINE Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b)
{
	const u32 r[4] = { a.x == b.x ? 0xffffffff : 0, a.y == b.y ? 0xffffffff : 0, a.z == b.z ? 0xffffffff : 0, a.w == b.w ? 0xffffffff : 0 };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	const u32* u = reinterpret_cast<const u32*>(&v);
	const int r = (u[0] >> 31) | ((u[1] >> 30) & 2) | ((u[1] >> 29) & 4) | ((u[1] >> 28) & 8);
	return r;
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x, b.x, a.y, b.y };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.z, b.z, a.w, b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x, a.z, b.x, b.z };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.y, a.w, b.y, b.w };
	return r;
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	const Vec4f r = { v.w, v.z, v.y, v.x };
	return r;
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return v.x;
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return v.y;
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return v.z;
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return v.w;
}



template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute(Vec4f_In v)
{
	static_assert(
		(T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3),
		"Template parameters need to be in range");

	const f32* p = reinterpret_cast<const f32*>(&v);
	const Vec4f r = { p[T0], p[T1], p[T2], p[T3] };
	return r;
}

template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute(Vec4f_In a, Vec4f_In b)
{
	static_assert(
		(T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3 || T0 == 4 || T0 == 5 || T0 == 6 || T0 == 7) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3 || T1 == 4 || T1 == 5 || T1 == 6 || T1 == 7) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3 || T2 == 4 || T2 == 5 || T2 == 6 || T2 == 7) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3 || T3 == 4 || T3 == 5 || T3 == 6 || T3 == 7),
		"Template parameters need to be in range");

#if !defined(FFTL_ALLOW_SINGLE_COMPONENT_PERMUTE2)
	static_assert(
		!((T0 < 4) && (T1 < 4) && (T2 < 4) && (T3 < 4)) &&
		!((T0 > 3) && (T1 > 3) && (T2 > 3) && (T3 > 3)),
		"Use the single parameter V4fPermute function.");
#endif

	constexpr uint shufX = (T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3) ? 0 : 1;
	constexpr uint shufY = (T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3) ? 0 : 1;
	constexpr uint shufZ = (T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3) ? 0 : 1;
	constexpr uint shufW = (T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3) ? 0 : 1;

	const f32* arr[2] = { reinterpret_cast<const f32*>(&a), reinterpret_cast<const f32*>(&b) };
	Vec4f r;

	r.x = arr[shufX][FFTL_PERMUTEMASK(T0)];
	r.y = arr[shufY][FFTL_PERMUTEMASK(T1)];
	r.z = arr[shufZ][FFTL_PERMUTEMASK(T2)];
	r.w = arr[shufW][FFTL_PERMUTEMASK(T3)];

	return r;
}




FFTL_FORCEINLINE Vec4u V4uAdd(Vec4u_In a, Vec4u_In b)
{
	const Vec4u r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	return r;
}
FFTL_FORCEINLINE Vec4u V4uSub(Vec4u_In a, Vec4u_In b)
{
	const Vec4u r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	return r;
}
FFTL_FORCEINLINE Vec4u V4uMul(Vec4u_In a, Vec4u_In b)
{
	const Vec4u r = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	return r;
}
FFTL_FORCEINLINE Vec4i V4iAdd(Vec4i_In a, Vec4i_In b)
{
	const Vec4i r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	return r;
}
FFTL_FORCEINLINE Vec4i V4iSub(Vec4i_In a, Vec4i_In b)
{
	const Vec4i r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	return r;
}
FFTL_FORCEINLINE Vec4i V4iMul(Vec4i_In a, Vec4i_In b)
{
	const Vec4i r = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	return r;
}

FFTL_FORCEINLINE Vec4i V4fRoundToVfi(Vec4f_In a)
{
	const Vec4i r = { static_cast<s32>(a.x + 0.5f - (a.x < 0)), static_cast<s32>(a.y + 0.5f - (a.y < 0)), static_cast<s32>(a.z + 0.5f - (a.z < 0)), static_cast<s32>(a.w + 0.5f - (a.w < 0)) };
	return r;
}







FFTL_FORCEINLINE Vec2d V2dZero()
{
	const Vec2d r = { 0, 0 };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dLoadA(const f64* pf)
{
	return *reinterpret_cast<const Vec2d*>(pf);
}
FFTL_FORCEINLINE Vec2d V2dLoadU(const f64* pf)
{
	return *reinterpret_cast<const Vec2d*>(pf);
}
FFTL_FORCEINLINE void V2dStoreA(f64* pf, Vec2d v)
{
	*reinterpret_cast<Vec2d*>(pf) = v;
}
FFTL_FORCEINLINE void V2dStoreU(f64* pf, Vec2d_In v)
{
	*reinterpret_cast<Vec2d*>(pf) = v;
}
FFTL_FORCEINLINE Vec2d V2dSet(f64 x, f64 y)
{
	const Vec2d r = { x, y };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dAnd(Vec2d_In a, Vec2d_In b)
{
	const u64* pA = reinterpret_cast<const u64*>(&a);
	const u64* pB = reinterpret_cast<const u64*>(&b);
	const u64 r[2] = { pA[0] & pB[0], pA[1] & pB[1] };
	return *reinterpret_cast<const Vec2d_In*>(&r);
}
FFTL_FORCEINLINE Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b)
{
	const u64* pA = reinterpret_cast<const u64*>(&a);
	const u64* pB = reinterpret_cast<const u64*>(&b);
	const u64 r[2] = { ~pA[0] & pB[0], ~pA[1] & pB[1] };
	return *reinterpret_cast<const Vec2d_In*>(&r);
}
FFTL_FORCEINLINE Vec2d V2dOr(Vec2d_In a, Vec2d_In b)
{
	const u64* pA = reinterpret_cast<const u64*>(&a);
	const u64* pB = reinterpret_cast<const u64*>(&b);
	const u64 r[2] = { pA[0] | pB[0], pA[1] | pB[1] };
	return *reinterpret_cast<const Vec2d_In*>(&r);
}
FFTL_FORCEINLINE Vec2d V2dXOr(Vec2d_In a, Vec2d_In b)
{
	const u64* pA = reinterpret_cast<const u64*>(&a);
	const u64* pB = reinterpret_cast<const u64*>(&b);
	const u64 r[2] = { pA[0] ^ pB[0], pA[1] ^ pB[1] };
	return *reinterpret_cast<const Vec2d_In*>(&r);
}
FFTL_FORCEINLINE Vec2d V2dAdd(Vec2d_In a, Vec2d_In b)
{
	const Vec2d r = { a.x + b.x, a.y + b.y };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dSub(Vec2d_In a, Vec2d_In b)
{
	const Vec2d r = { a.x - b.x, a.y - b.y };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dMul(Vec2d_In a, Vec2d_In b)
{
	const Vec2d r = { a.x * b.x, a.y * b.y };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dDiv(Vec2d_In a, Vec2d_In b)
{
	const Vec2d r = { a.x / b.x, a.y / b.y };
	return r;
}
FFTL_FORCEINLINE Vec2d V2dSqrt(Vec2d_In v)
{
	const Vec2d r = { Sqrt(v.x), Sqrt(v.y) };
	return r;
}
FFTL_FORCEINLINE bool V2dIsEqual(Vec2d_In a, Vec2d_In b)
{
	return a.x == b.x && a.y == b.y;
}


FFTL_FORCEINLINE ScopedFlushDenormals::ScopedFlushDenormals()
{
}
FFTL_FORCEINLINE ScopedFlushDenormals::~ScopedFlushDenormals()
{
}

} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif
