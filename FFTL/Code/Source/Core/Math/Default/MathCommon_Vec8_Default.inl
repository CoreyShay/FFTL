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
	return V8fSet(V4fLoad1(pf), V4fZero());
}
FFTL_FORCEINLINE Vec8f V8fLoad2(const f32* pf)
{
	return V8fSet(V4fLoad2(pf), V4fZero());
}
FFTL_FORCEINLINE Vec8f V8fLoad3(const f32* pf)
{
	return V8fSet(V4fLoad3(pf), V4fZero());
}
FFTL_FORCEINLINE Vec8f V8fLoad4(const f32* pf)
{
	return V8fSet(V4fLoadU(pf), V4fZero());
}
FFTL_FORCEINLINE Vec8f V8fLoad6(const f32* pf)
{
	return V8fSet(V4fLoadU(pf + 0), V4fLoad2(pf + 4));
}


FFTL_FORCEINLINE Vec8f V8fLoadUR(const f32* pf)
{
	return Vec8f{ V4fReverse(V4fLoadU(pf + 4)), V4fReverse(V4fLoadU(pf + 0)) };
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
FFTL_FORCEINLINE void V8fStore4(f32* pf, Vec8f_In v)
{
	V4fStoreA(pf, v.a);
}
FFTL_FORCEINLINE void V8fStore6(f32* pf, Vec8f_In v)
{
	V4fStoreU(pf+0, v.a);
	V4fStore2(pf+4, v.b);
}
FFTL_FORCEINLINE Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d)
{
	return Vec8f
	{
		V4fSet(x, y, z, w),
		V4fSet(a, b, c, d),
	};
}
FFTL_FORCEINLINE Vec8f V8fSet(Vec4f_In a, Vec4f_In b)
{
	return Vec8f{ a, b };
}
FFTL_FORCEINLINE Vec8f V8fSet0123(Vec8f_In a, Vec4f_In b)
{
	return Vec8f{ b, a.b };
}
FFTL_FORCEINLINE Vec8f V8fSet4567(Vec8f_In a, Vec4f_In b)
{
	return Vec8f{ a.a, b };
}
FFTL_FORCEINLINE Vec8f V8fSplat(f32 f)
{
	return Vec8f
	{
		V4fSplat(f),
		V4fSplat(f),
	};
}
FFTL_FORCEINLINE Vec8f V8fSplat(const f32* pf)
{
	return Vec8f
	{
		V4fSplat(pf),
		V4fSplat(pf),
	};
}
FFTL_FORCEINLINE Vec8f V8fSplat(Vec4f_In v)
{
	return Vec8f{v, v};
}
FFTL_FORCEINLINE Vec8f V8fAnd(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fAnd(a.a, b.a),
		V4fAnd(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fAndNot(a.a, b.a),
		V4fAndNot(a.b, b.b)
	};
}
FFTL_FORCEINLINE Vec8f V8fOr(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fOr(a.a, b.a),
		V4fOr(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fXOr(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fXOr(a.a, b.a),
		V4fXOr(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fAdd(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fAdd(a.a, b.a),
		V4fAdd(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fSub(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fSub(a.a, b.a),
		V4fSub(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fMul(a.a, b.a),
		V4fMul(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec4f_In b)
{
	return Vec8f
	{
		V4fMul(a.a, b),
		V4fMul(a.b, b),
	};
}
FFTL_FORCEINLINE Vec8f V8fDiv(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fDiv(a.a, b.a),
		V4fDiv(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fAddMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
	return Vec8f
	{
		V4fAddMul(a.a, b.a, c.a),
		V4fAddMul(a.b, b.b, c.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fSubMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
	return Vec8f
	{
		V4fSubMul(a.a, b.a, c.a),
		V4fSubMul(a.b, b.b, c.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fMin(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fMin(a.a, b.a),
		V4fMin(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fMax(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fMax(a.a, b.a),
		V4fMax(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fSqrt(Vec8f_In v)
{
	return Vec8f
	{
		V4fSqrt(v.a),
		V4fSqrt(v.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fAbs(Vec8f_In v)
{
	return Vec8f
	{
		V4fAbs(v.a),
		V4fAbs(v.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareEq(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareEq(a.a, b.a),
		V4fCompareEq(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareNq(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareNq(a.a, b.a),
		V4fCompareNq(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareGt(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareGt(a.a, b.a),
		V4fCompareGt(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareLt(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareLt(a.a, b.a),
		V4fCompareLt(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareGe(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareGe(a.a, b.a),
		V4fCompareGe(a.b, b.b),
	};
}
FFTL_FORCEINLINE Vec8f V8fCompareLe(Vec8f_In a, Vec8f_In b)
{
	return Vec8f
	{
		V4fCompareLe(a.a, b.a),
		V4fCompareLe(a.b, b.b),
	};
}
FFTL_FORCEINLINE int V8fToIntMask(Vec8f_In v)
{
	const int ra = V4fToIntMask(v.a);
	const int rb = V4fToIntMask(v.b);
	return ra | (rb << 4);
}
FFTL_FORCEINLINE bool V8fIsEqual(Vec8f_In a, Vec8f_In b)
{
	const Vec4f cmp1 = V4fCompareEq(a.a, b.a);
	const Vec4f cmp2 = V4fCompareEq(a.b, b.b);
	const Vec4f vAnd = V4fAnd(cmp1, cmp2);
	return V4fToIntMask(vAnd) == 15;
}
FFTL_FORCEINLINE bool V8fIsAllZero(Vec8f_In v)
{
	const Vec4f cmp1 = V4fCompareEq(v.a, V4fZero());
	const Vec4f cmp2 = V4fCompareEq(v.b, V4fZero());
	const Vec4f vAnd = V4fAnd(cmp1, cmp2);
	return V4fToIntMask(vAnd) == 15;
}
FFTL_FORCEINLINE Vec8f V8fReverse(Vec8f_In v)
{
	return Vec8f
	{
		V4fReverse(v.b),
		V4fReverse(v.a),
	};
}
FFTL_FORCEINLINE Vec4f V8fAsV4f(Vec8f_In v)
{
	return v.a;
}
FFTL_FORCEINLINE Vec4f V8fGet4567(Vec8f_In v)
{
	return v.b;
}



constexpr mask32x8::mask32x8(const mask32x4& a, const mask32x4& b)
	: m_v{ { a, b } }
{
}

FFTL_FORCEINLINE int mask32x8::ToIntMask() const
{
	return (m_v[0].ToIntMask()) | (m_v[1].ToIntMask() << 4);
}

template<s32 i0, s32 i1, s32 i2, s32 i3, s32 i4, s32 i5, s32 i6, s32 i7>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromInts()
{
	return mask32x8{ mask32x4::GenMaskFromInts<i0, i1, i2, i3>(), mask32x4::GenMaskFromInts<i4, i5, i6, i7>() };
}

template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromBools()
{
	return mask32x8{ mask32x4::GenMaskFromBools<b0, b1, b2, b3>(), mask32x4::GenMaskFromBools<b4, b5, b6, b7>() };
}

template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE mask32x8 mask32x8::PropagateInt(int i)
{
	return mask32x8{ mask32x4::PropagateInt<b0, b1, b2, b3>(i), mask32x4::PropagateInt<b4, b5, b6, b7>(i) };
}

template<s32 i>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromInt()
{
	const mask32x4 r = mask32x4::GenMaskFromInt<i>();
	return mask32x8{ r, r };
}

FFTL_FORCEINLINE mask32x8 mask32x8::operator|(const mask32x8& b) const
{
	return mask32x8{ mask32x4(m_v[0]) | mask32x4(b.m_v[0]), mask32x4(m_v[1]) | mask32x4(b.m_v[1]) };
}
FFTL_FORCEINLINE mask32x8 mask32x8::operator&(const mask32x8& b) const
{
	return mask32x8{ mask32x4(m_v[0]) & mask32x4(b.m_v[0]), mask32x4(m_v[1]) & mask32x4(b.m_v[1]) };
}
FFTL_FORCEINLINE mask32x8 mask32x8::operator^(const mask32x8& b) const
{
	return mask32x8{ mask32x4(m_v[0]) ^ mask32x4(b.m_v[0]), mask32x4(m_v[1]) ^ mask32x4(b.m_v[1]) };
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator|(const T& b) const
{
	return T{ mask32x4(m_v[0]) | f32_4(b.m_v.a), mask32x4(m_v[1]) | f32_4(b.m_v.b) };
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator&(const T& b) const
{
	return T{ mask32x4(m_v[0]) & f32_4(b.m_v.a), mask32x4(m_v[1]) & f32_4(b.m_v.b) };
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator^(const T& b) const
{
	return T{ mask32x4(m_v[0]) ^ f32_4(b.m_v.a), mask32x4(m_v[1]) ^ f32_4(b.m_v.b) };
}

template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const T& b)
{
	return T{ AndNot(f32_4(a.m_v.a), f32_4(b.m_v.a)), AndNot(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const mask32x8& a, const T& b)
{
	return T{ AndNot(mask32x4(a.m_v[0]), f32_4(b.m_v.a)), AndNot(mask32x4(a.m_v[1]), f32_4(b.m_v.b)) };
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const mask32x8& b)
{
	return T{ AndNot(f32_4(a.m_v.a), mask32x4(b.m_v[0])), AndNot(f32_4(a.m_v.b), mask32x4(b.m_v[1])) };
}
FFTL_FORCEINLINE mask32x8 AndNot(const mask32x8& a, const mask32x8& b)
{
	return mask32x8{ AndNot(a.m_v[0], b.m_v[0]), AndNot(a.m_v[1], b.m_v[1]) };
}

FFTL_FORCEINLINE f32_8 f32_8::operator|(const mask32x8& b) const
{
	return f32_8{ f32_4(m_v.a) | mask32x4(b.m_v[0]), f32_4(m_v.b) | mask32x4(b.m_v[1]) };
}
FFTL_FORCEINLINE f32_8 f32_8::operator&(const mask32x8& b) const
{
	return f32_8{ f32_4(m_v.a) & mask32x4(b.m_v[0]), f32_4(m_v.b) & mask32x4(b.m_v[1]) };
}
FFTL_FORCEINLINE f32_8 f32_8::operator^(const mask32x8& b) const
{
	return f32_8{ f32_4(m_v.a) ^ mask32x4(b.m_v[0]), f32_4(m_v.b) ^ mask32x4(b.m_v[1]) };
}

FFTL_FORCEINLINE mask32x8 CmpEq(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpEq(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpEq(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
FFTL_FORCEINLINE mask32x8 CmpNe(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpNe(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpNe(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
FFTL_FORCEINLINE mask32x8 CmpLt(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpLt(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpLt(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
FFTL_FORCEINLINE mask32x8 CmpLe(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpLe(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpLe(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
FFTL_FORCEINLINE mask32x8 CmpGt(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpGt(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpGt(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}
FFTL_FORCEINLINE mask32x8 CmpGe(f32_8_In a, f32_8_In b)
{
	return mask32x8{ CmpGe(f32_4(a.m_v.a), f32_4(b.m_v.a)), CmpGe(f32_4(a.m_v.b), f32_4(b.m_v.b)) };
}

template<typename T, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const T& a, const T& b)
{
	return T{ Blend<b0, b1, b2, b3>(f32_4(a.GetNative().a), f32_4(b.GetNative().a)), Blend<b4, b5, b6, b7>(f32_4(a.GetNative().b), f32_4(b.GetNative().b)) };
}
template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const mask32x8& msk, const T& a, const T& b)
{
	return T{ Blend(msk.GetArray()[0], f32_4(a.GetNative().a), f32_4(b.GetNative().a)), Blend(msk.GetArray()[1], f32_4(a.GetNative().b), f32_4(b.GetNative().b)) };
}

inline Vec8f V8fSin(Vec8f_In r)
{
	return Vec8f{ V4fSin(V8fAsV4f(r)), V4fCos(V8fGet4567(r)) };
}
inline Vec8f V8fCos(Vec8f_In r)
{
	return Vec8f{ V4fCos(V8fAsV4f(r)), V4fSin(V8fGet4567(r)) };
}
inline void V8fSinCos(Vec8f_In r, Vec8f& s, Vec8f& c)
{
	Vec4f s0, c0, s1, c1;
	V4fSinCos(V8fAsV4f(r), s0, c0);
	V4fSinCos(V8fGet4567(r), s1, c1);
	s = V8fSet(s0, s1);
	c = V8fSet(c0, c1);
}


} // namespace FFTL

#endif //_FFTL_MATH_VEC8_DEFAULT_INL
