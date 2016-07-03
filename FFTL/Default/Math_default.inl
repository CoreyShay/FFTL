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

#ifndef _FFTL_MATH_DEFAULT_INL
#define _FFTL_MATH_DEFAULT_INL


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
	const uint32* pA = reinterpret_cast<const uint32*>(&a);
	const uint32* pB = reinterpret_cast<const uint32*>(&b);
	const uint32 r[4] = { pA[0]&pB[0], pA[1]&pB[1], pA[2]&pB[2], pA[3]&pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	const uint32* pA = reinterpret_cast<const uint32*>(&a);
	const uint32* pB = reinterpret_cast<const uint32*>(&b);
	const uint32 r[4] = { ~pA[0]&pB[0], ~pA[1]&pB[1], ~pA[2]&pB[2], ~pA[3]&pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	const uint32* pA = reinterpret_cast<const uint32*>(&a);
	const uint32* pB = reinterpret_cast<const uint32*>(&b);
	const uint32 r[4] = { pA[0]|pB[0], pA[1]|pB[1], pA[2]|pB[2], pA[3]|pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	const uint32* pA = reinterpret_cast<const uint32*>(&a);
	const uint32* pB = reinterpret_cast<const uint32*>(&b);
	const uint32 r[4] = { pA[0]^pB[0], pA[1]^pB[1], pA[2]^pB[2], pA[3]^pB[3] };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
	const Vec4f r = { a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	const Vec4f r = { a.x+b.x*c.x, a.y+b.y*c.y, a.z+b.z*c.z, a.w+b.w*c.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	const Vec4f r = { a.x-b.x*c.x, a.y-b.y*c.y, a.z-b.z*c.z, a.w-b.w*c.w };
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	const Vec4f r = { Sqrt(v.x), Sqrt(v.y), Sqrt(v.z), Sqrt(v.w) };
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
	const uint32 r[4] = { a.x==b.x ? 0xffffffff : 0, a.y==b.y ? 0xffffffff : 0, a.z==b.z ? 0xffffffff : 0, a.w==b.w ? 0xffffffff : 0 };
	return *reinterpret_cast<const Vec4f*>(&r);
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	const uint32* u = reinterpret_cast<const uint32*>(&v);
	const int r = (u[0]>>31) | ((u[1]>>30)&2) | ((u[1]>>29)&4) | ((u[1]>>28)&8);
	return r;
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w;
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



template <enShuf T_X, enShuf T_Y, enShuf T_Z, enShuf T_W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In v )
{
	FFTL_StaticAssert(	(T_X==sh_X || T_X==sh_Y || T_X==sh_Z || T_X==sh_W) &&
		(T_Y==sh_X || T_Y==sh_Y || T_Y==sh_Z || T_Y==sh_W) &&
		(T_Z==sh_X || T_Z==sh_Y || T_Z==sh_Z || T_Z==sh_W) &&
		(T_W==sh_X || T_W==sh_Y || T_W==sh_Z || T_W==sh_W)  );

	FFTL_StaticAssert( !(T_X==sh_X && T_Y==sh_Y && T_Z==sh_Z && T_W==sh_W) ); // This permute does nothing meaningful!

	const f32* p = reinterpret_cast<const f32*>(&v);
	const Vec4f r = { p[T_X], p[T_Y], p[T_Z], p[T_W] };
	return r;
}

template <enShuf2 T_X, enShuf2 T_Y, enShuf2 T_Z, enShuf2 T_W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In a, Vec4f_In b )
{
	//	Template inputs need to be shuffle enumerations sh_(X/Y/Z/W)(1/2)
	FFTL_StaticAssert(
		(T_X==sh_X1 || T_X==sh_Y1 || T_X==sh_Z1 || T_X==sh_W1	||
		T_X==sh_X2 || T_X==sh_Y2 || T_X==sh_Z2 || T_X==sh_W2 )	&&
		(T_Y==sh_X1 || T_Y==sh_Y1 || T_Y==sh_Z1 || T_Y==sh_W1	||
		T_Y==sh_X2 || T_Y==sh_Y2 || T_Y==sh_Z2 || T_Y==sh_W2 )	&&
		(T_Z==sh_X1 || T_Z==sh_Y1 || T_Z==sh_Z1 || T_Z==sh_W1	||
		T_Z==sh_X2 || T_Z==sh_Y2 || T_Z==sh_Z2 || T_Z==sh_W2 )	&&
		(T_W==sh_X1 || T_W==sh_Y1 || T_W==sh_Z1 || T_W==sh_W1	||
		T_W==sh_X2 || T_W==sh_Y2 || T_W==sh_Z2 || T_W==sh_W2 )	 );

	//	If you try to shuffle this way, you'll just end up with one of the 2 input arguments. Don't do that.
	FFTL_StaticAssert(
		!(T_X==sh_X1 && T_Y==sh_Y1 && T_Z==sh_Z1 && T_W==sh_W1) &&
		!(T_X==sh_X2 && T_Y==sh_Y2 && T_Z==sh_Z2 && T_W==sh_W2) );

	//	If this error fires, it means you should use the single parameter V4fShuffle function.
	FFTL_StaticAssert(
		!( (T_X < 4) && (T_Y < 4) && (T_Z < 4) && (T_W < 4) ) &&
		!( (T_X > 3) && (T_Y > 3) && (T_Z > 3) && (T_W > 3) ) );


	const uint shufX = (T_X==sh_X1||T_X==sh_Y1||T_X==sh_Z1||T_X==sh_W1) ? 0 : 1;
	const uint shufY = (T_Y==sh_X1||T_Y==sh_Y1||T_Y==sh_Z1||T_Y==sh_W1) ? 0 : 1;
	const uint shufZ = (T_Z==sh_X1||T_Z==sh_Y1||T_Z==sh_Z1||T_Z==sh_W1) ? 0 : 1;
	const uint shufW = (T_W==sh_X1||T_W==sh_Y1||T_W==sh_Z1||T_W==sh_W1) ? 0 : 1;

	const f32* arr[2] = { reinterpret_cast<const f32*>(&a), reinterpret_cast<const f32*>(&b) };
	Vec4f r;

	r.x = arr[shufX][FFTL_PERMUTEMASK(T_X)];
	r.y = arr[shufY][FFTL_PERMUTEMASK(T_Y)];
	r.z = arr[shufZ][FFTL_PERMUTEMASK(T_Z)];
	r.w = arr[shufW][FFTL_PERMUTEMASK(T_W)];

	return r;
}









#if 0
FFTL_FORCEINLINE Vec2d V2dZero()
{
	return _mm_setzero_pd();
}
FFTL_FORCEINLINE Vec2d V2dLoadA(const f64* pf)
{
	return _mm_load_pd(pf);
}
FFTL_FORCEINLINE Vec2d V2dLoadU(const f64* pf)
{
	return _mm_loadu_pd(pf);
}
FFTL_FORCEINLINE void V2dStoreA(f64* pf, Vec2d v)
{
	_mm_store_pd(pf, v);
}
FFTL_FORCEINLINE void V2dStoreU(f64* pf, Vec2d_In v)
{
	_mm_storeu_pd(pf, v);
}
FFTL_FORCEINLINE Vec2d V2dSet(f64 x, f64 y)
{
	return _mm_setr_pd(x, y);
}
FFTL_FORCEINLINE Vec2d V2dAnd(Vec2d_In a, Vec2d_In b)
{
	return _mm_and_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b)
{
	return _mm_andnot_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_or_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dXOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_xor_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAdd(Vec2d_In a, Vec2d_In b)
{
	return _mm_add_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSub(Vec2d_In a, Vec2d_In b)
{
	return _mm_sub_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dMul(Vec2d_In a, Vec2d_In b)
{
	return _mm_mul_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dDiv(Vec2d_In a, Vec2d_In b)
{
	return _mm_div_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSqrt(Vec2d_In v)
{
	return _mm_sqrt_pd(v);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec2d_In a, Vec2d_In b)
{
	return _mm_movemask_pd( _mm_cmpeq_pd(a, b) ) == 3;
}
#endif


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

#endif //_FFTL_MATH_DEFAULT_INL