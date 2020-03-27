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

#ifndef _FFTL_MATH_VEC8_DEFAULT_INL
#define _FFTL_MATH_VEC8_DEFAULT_INL


namespace FFTL
{


FFTL_FORCEINLINE Vec8f V8fZero()
{
	Vec8f r;
	r.a = V4fZero();
	r.b = V4fZero();
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoadA(const f32* pf)
{
	Vec8f r;
	r.a = V4fLoadA(pf+0);
	r.b = V4fLoadA(pf+4);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoadU(const f32* pf)
{
	Vec8f r;
	r.a = V4fLoadU(pf+0);
	r.b = V4fLoadU(pf+4);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoadAR(const f32* pf)
{
	Vec8f r;
	r.a = V4fReverse( V4fLoadA(pf+4) );
	r.b = V4fReverse( V4fLoadA(pf+0) );
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoad1(const f32* pf)
{
	Vec8f r;
	r.a = V4fLoad1(pf);
	r.b = V4fZero();
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoad2(const f32* pf)
{
	Vec8f r;
	r.a = V4fLoad2(pf);
	r.b = V4fZero();
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoad3(const f32* pf)
{
	Vec8f r;
	r.a = V4fLoad3(pf);
	r.b = V4fZero();
	return r;
}
FFTL_FORCEINLINE Vec8f V8fLoad6(const f32* pf)
{
	return V8fSet( V4fLoadU(pf+0), V4fLoad2(pf+4) );
}


FFTL_FORCEINLINE Vec8f V8fLoadUR(const f32* pf)
{
	Vec8f r;
	r.a = V4fReverse( V4fLoadU(pf+4) );
	r.b = V4fReverse( V4fLoadU(pf+0) );
	return r;
}
FFTL_FORCEINLINE void V8fStoreA(f32* pf, Vec8f_In v)
{
	V4fStoreA(pf+0, v.a);
	V4fStoreA(pf+4, v.b);
}
FFTL_FORCEINLINE void V8fStoreU(f32* pf, Vec8f_In v)
{
	V4fStoreU(pf+0, v.a);
	V4fStoreU(pf+4, v.b);
}
FFTL_FORCEINLINE void V8fStore1(f32* pf, Vec8f_In v)
{
	V4fStore1(pf, v.a);
}
FFTL_FORCEINLINE void V8fStore2(f32* pf, Vec8f_In v)
{
	V4fStore2(pf, v.a);
}
FFTL_FORCEINLINE void V8fStore3(f32* pf, Vec8f_In v)
{
	V4fStore3(pf, v.a);
}
FFTL_FORCEINLINE void V8fStore6(f32* pf, Vec8f_In v)
{
	V4fStoreU(pf+0, v.a);
	V4fStore2(pf+4, v.b);
}
FFTL_FORCEINLINE Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d)
{
	Vec8f r;
	r.a = V4fSet(x, y, z, w);
	r.b = V4fSet(a, b, c, d);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSet(Vec4f_In a, Vec4f_In b)
{
	const Vec8f r = { a, b };
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSet0123(Vec8f_In a, Vec4f_In b)
{
	const Vec8f r = { b, a.b };
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSet4567(Vec8f_In a, Vec4f_In b)
{
	const Vec8f r = { a.a, b };
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSplat8(f32 f)
{
	Vec8f r;
	r.a = V4fSplat4(f);
	r.b = V4fSplat4(f);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSplat8(const f32* pf)
{
	Vec8f r;
	r.a = V4fSplat4(pf);
	r.b = V4fSplat4(pf);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSplat(Vec4f_In v)
{
	Vec8f r;
	r.a = v;
	r.b = v;
	return r;
}
FFTL_FORCEINLINE Vec8f V8fAnd(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fAnd(a.a, b.a);
	r.b = V4fAnd(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fAndNot(a.a, b.a);
	r.b = V4fAndNot(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fOr(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fOr(a.a, b.a);
	r.b = V4fOr(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fXOr(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fXOr(a.a, b.a);
	r.b = V4fXOr(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fAdd(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fAdd(a.a, b.a);
	r.b = V4fAdd(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSub(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fSub(a.a, b.a);
	r.b = V4fSub(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fMul(a.a, b.a);
	r.b = V4fMul(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec4f_In b)
{
	Vec8f r;
	r.a = V4fMul(a.a, b);
	r.b = V4fMul(a.b, b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fDiv(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fDiv(a.a, b.a);
	r.b = V4fDiv(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fAddMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
	Vec8f r;
	r.a = V4fAddMul(a.a, b.a, c.a);
	r.b = V4fAddMul(a.b, b.b, c.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSubMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
	Vec8f r;
	r.a = V4fSubMul(a.a, b.a, c.a);
	r.b = V4fSubMul(a.b, b.b, c.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fSqrt(Vec8f_In v)
{
	Vec8f r;
	r.a = V4fSqrt(v.a);
	r.b = V4fSqrt(v.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareEq(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareEq(a.a, b.a);
	r.b = V4fCompareEq(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareNq(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareNq(a.a, b.a);
	r.b = V4fCompareNq(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareGt(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareGt(a.a, b.a);
	r.b = V4fCompareGt(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareLt(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareLt(a.a, b.a);
	r.b = V4fCompareLt(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareGe(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareGe(a.a, b.a);
	r.b = V4fCompareGe(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE Vec8f V8fCompareLe(Vec8f_In a, Vec8f_In b)
{
	Vec8f r;
	r.a = V4fCompareLe(a.a, b.a);
	r.b = V4fCompareLe(a.b, b.b);
	return r;
}
FFTL_FORCEINLINE int V8fToIntMask(Vec8f_In v)
{
	const int ra = V4fToIntMask(v.a);
	const int rb = V4fToIntMask(v.b);
	return ra | (rb << 4);
}
FFTL_FORCEINLINE bool V8fIsEqual(Vec8f_In a, Vec8f_In b)
{
	Vec4f cmp1 = V4fCompareEq(a.a, b.a);
	Vec4f cmp2 = V4fCompareEq(a.b, b.b);
	Vec4f vAnd = V4fAnd(cmp1, cmp2);
	return V4fToIntMask(vAnd) == 15;
}
FFTL_FORCEINLINE bool V8fIsAllZero(Vec8f_In v)
{
	Vec4f cmp1 = V4fCompareEq(v.a, V4fZero());
	Vec4f cmp2 = V4fCompareEq(v.b, V4fZero());
	Vec4f vAnd = V4fAnd(cmp1, cmp2);
	return V4fToIntMask(vAnd) == 15;
}
FFTL_FORCEINLINE Vec8f V8fReverse(Vec8f_In v)
{
	Vec8f r;
	r.a = V4fReverse(v.b);
	r.b = V4fReverse(v.a);
	return r;
}
FFTL_FORCEINLINE Vec4f V8fAsV4f(Vec8f_In v)
{
	return v.a;
}
FFTL_FORCEINLINE Vec4f V8fGet4567(Vec8f_In v)
{
	return v.b;
}


} // namespace FFTL

#endif //_FFTL_MATH_VEC8_DEFAULT_INL
