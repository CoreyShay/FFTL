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

#ifndef _FFTL_MATH_AVX_INL
#define _FFTL_MATH_AVX_INL


namespace FFTL
{






FFTL_FORCEINLINE Vec8f V8fZero()
{
	return _mm256_setzero_ps();
}
FFTL_FORCEINLINE Vec8f V8fLoadA(const f32* pf)
{
	// LoadA / StoreA is explicitly aligned for performance reasons.
	// If the assert fires, be sure to align your data accordingly,
	// or use LoadU / StoreU if necessary.
	FFTL_ASSERT(((size_t)pf & 31) == 0);
	return _mm256_load_ps(pf);
//	return *reinterpret_cast<const Vec8f*>(pf);
}
FFTL_FORCEINLINE Vec8f V8fLoadU(const f32* pf)
{
#if defined(__clang__) // Apparently a compiler bug causes _mm256_loadu_ps to emit 2 SSE loads followed by an insert.
	__m256 ret;
	__asm__("vmovups %1, %0" : "=x" (ret) : "m" (*pf));
	return ret;
#else
	return _mm256_loadu_ps(pf);
#endif
}
FFTL_FORCEINLINE Vec8f V8fLoad1(const f32* pf)
{
	return _mm256_castps128_ps256(_mm_load_ss(pf));
}
FFTL_FORCEINLINE Vec8f V8fLoad2(const f32* pf)
{
	return _mm256_castps128_ps256(V4fLoad2(pf));
}
FFTL_FORCEINLINE Vec8f V8fLoad3(const f32* pf)
{
	return _mm256_castps128_ps256(f32_4::Load3(pf).GetNative());
}
FFTL_FORCEINLINE Vec8f V8fLoad4(const f32* pf)
{
	return _mm256_castps128_ps256(f32_4::LoadA(pf).GetNative());
}
FFTL_FORCEINLINE Vec8f V8fLoad6(const f32* pf)
{
	return V8fSet(V4fLoadU(pf+0), V4fLoad2(pf+4));
}
FFTL_FORCEINLINE Vec8f V8fLoadAR(const f32* pf)
{
	return V8fReverse( V8fLoadA(pf) );
}
FFTL_FORCEINLINE Vec8f V8fLoadUR(const f32* pf)
{
	return V8fReverse( V8fLoadU(pf) );
}
FFTL_FORCEINLINE void V8fStoreA(f32* pf, Vec8f_In v)
{
	// LoadA / StoreA is explicitly aligned for performance reasons.
	// If the assert fires, be sure to align your data accordingly,
	// or use LoadU / StoreU if necessary.
	FFTL_ASSERT(((size_t)pf & 31) == 0);
	_mm256_store_ps(pf, v);
//	*reinterpret_cast<Vec8f*>(pf) = v;

}
FFTL_FORCEINLINE void V8fStoreU(f32* pf, Vec8f_In v)
{
	_mm256_storeu_ps(pf, v);
}
FFTL_FORCEINLINE void V8fStore1(f32* pf, Vec8f_In v)
{
	_mm_store_ss(pf, _mm256_castps256_ps128(v));
}
FFTL_FORCEINLINE void V8fStore2(f32* pf, Vec8f_In v)
{
	V4fStore2(pf, _mm256_castps256_ps128(v));
}
FFTL_FORCEINLINE void V8fStore3(f32* pf, Vec8f_In v)
{
	V4fStore3(pf, _mm256_castps256_ps128(v));
}
FFTL_FORCEINLINE void V8fStore4(f32* pf, Vec8f_In v)
{
	V4fStoreA(pf, _mm256_castps256_ps128(v));
}
FFTL_FORCEINLINE void V8fStore6(f32* pf, Vec8f_In v)
{
	V4fStoreU(pf+0, _mm256_castps256_ps128(v));
	V4fStore2(pf+4, V8fGet4567(v));
}
FFTL_FORCEINLINE Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d)
{
	return _mm256_setr_ps(x, y, z, w, a, b, c, d);
}
FFTL_FORCEINLINE Vec8f V8fSet0123(Vec8f_In a, Vec4f_In b)
{
	return _mm256_insertf128_ps(a, b, 0);
}
FFTL_FORCEINLINE Vec8f V8fSet4567(Vec8f_In a, Vec4f_In b)
{
	return _mm256_insertf128_ps(a, b, 1);
}
FFTL_FORCEINLINE Vec8f V8fSet(Vec4f_In a, Vec4f_In b)
{
	return _mm256_insertf128_ps(_mm256_castps128_ps256(a), b, 1);
}
FFTL_FORCEINLINE Vec8f V8fSplat(f32 f)
{
	return _mm256_set1_ps(f);
}
FFTL_FORCEINLINE Vec8f V8fSplat(const f32* pf)
{
	return _mm256_broadcast_ss(pf);
}
FFTL_FORCEINLINE Vec8f V8fSplat(Vec4f_In v)
{
	return _mm256_permute2f128_ps(_mm256_castps128_ps256(v), _mm256_castps128_ps256(v), 0);
}
FFTL_FORCEINLINE Vec8f V8fAnd(Vec8f_In a, Vec8f_In b)
{
	return _mm256_and_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b)
{
	return _mm256_andnot_ps(b, a);
}
FFTL_FORCEINLINE Vec8f V8fOr(Vec8f_In a, Vec8f_In b)
{
	return _mm256_or_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fXOr(Vec8f_In a, Vec8f_In b)
{
	return _mm256_xor_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fAdd(Vec8f_In a, Vec8f_In b)
{
	return _mm256_add_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fSub(Vec8f_In a, Vec8f_In b)
{
	return _mm256_sub_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec8f_In b)
{
	return _mm256_mul_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec4f_In b)
{
	__m256 b8 = _mm256_castps128_ps256(b);
	b8 = _mm256_permute2f128_ps(b8, b8, 0);
	b8 = _mm256_mul_ps(a, b8);
	return b8;
}
FFTL_FORCEINLINE Vec8f V8fDiv(Vec8f_In a, Vec8f_In b)
{
	return _mm256_div_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fAddMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
#if defined(FFTL_FMA4)
	return _mm256_macc_ps(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm256_fmadd_ps(b, c, a);
#else
	__m256 r = _mm256_mul_ps(b, c);
	r = _mm256_add_ps(a, r);
	return r;
#endif
}
FFTL_FORCEINLINE Vec8f V8fSubMul(Vec8f_In a, Vec8f_In b, Vec8f_In c)
{
#if defined(FFTL_FMA4)
	return _mm256_nmacc_ps(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm256_fnmadd_ps(b, c, a);
#else
	__m256 r = _mm256_mul_ps(b, c);
	r = _mm256_sub_ps(a, r);
	return r;
#endif
}
FFTL_FORCEINLINE Vec8f V8fMin(Vec8f_In a, Vec8f_In b)
{
	return _mm256_min_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fMax(Vec8f_In a, Vec8f_In b)
{
	return _mm256_max_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fSqrt(Vec8f_In v)
{
	return _mm256_sqrt_ps(v);
}
FFTL_FORCEINLINE Vec8f V8fAbs(Vec8f_In v)
{
	return _mm256_and_ps(v, _mm256_sub_ps(_mm256_setzero_ps(), v) );
}
FFTL_FORCEINLINE Vec8f V8fCompareEq(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_EQ_OQ);
}
FFTL_FORCEINLINE Vec8f V8fCompareNq(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_NEQ_OQ);
}
FFTL_FORCEINLINE Vec8f V8fCompareGt(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_GT_OQ);
}
FFTL_FORCEINLINE Vec8f V8fCompareLt(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_LT_OQ);
}
FFTL_FORCEINLINE Vec8f V8fCompareGe(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_GE_OQ);
}
FFTL_FORCEINLINE Vec8f V8fCompareLe(Vec8f_In a, Vec8f_In b)
{
	return _mm256_cmp_ps(a, b, _CMP_LE_OQ);
}
FFTL_FORCEINLINE int V8fToIntMask(Vec8f_In v)
{
	return _mm256_movemask_ps(v);
}
FFTL_FORCEINLINE bool V8fIsEqual(Vec8f_In a, Vec8f_In b)
{
	return _mm256_movemask_ps( _mm256_cmp_ps(a, b, _CMP_EQ_OQ) ) == 255;
}
FFTL_FORCEINLINE bool V8fIsAllZero(Vec8f_In v)
{
	return _mm256_movemask_ps( _mm256_cmp_ps(v, _mm256_setzero_ps(), _CMP_EQ_OQ) ) == 255;
}
FFTL_FORCEINLINE Vec8f V8fReverse(Vec8f_In v)
{
	Vec8f r;
	r = _mm256_shuffle_ps(v, v, _MM_SHUFFLE(0,1,2,3));
	r = _mm256_permute2f128_ps(r, r, 1);
	return r;
}
FFTL_FORCEINLINE Vec4f V8fAsV4f(Vec8f_In v)
{
	return _mm256_castps256_ps128(v);
}
FFTL_FORCEINLINE Vec4f V8fGet4567(Vec8f_In v)
{
	return _mm256_extractf128_ps(v, 1);
}



inline constexpr mask32x8::mask32x8(const __m256& v)
	: m_v(v)
{
}

FFTL_FORCEINLINE mask32x8& mask32x8::operator=(const __m256& v)
{
	m_v = v;
	return *this;
}

inline constexpr mask32x8::operator const __m256&() const
{
	return m_v;
}

FFTL_FORCEINLINE mask32x8::operator __m256&()
{
	return m_v;
}

FFTL_FORCEINLINE int mask32x8::ToIntMask() const
{
	return _mm256_movemask_ps(m_v);
}

template<s32 i0, s32 i1, s32 i2, s32 i3, s32 i4, s32 i5, s32 i6, s32 i7>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromInts()
{
	return mask32x8(_mm256_castsi256_ps(_mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7)));
}

template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromBools()
{
	constexpr int i0 = b0 ? 0xffffffff : 0x00000000;
	constexpr int i1 = b1 ? 0xffffffff : 0x00000000;
	constexpr int i2 = b2 ? 0xffffffff : 0x00000000;
	constexpr int i3 = b3 ? 0xffffffff : 0x00000000;
	constexpr int i4 = b4 ? 0xffffffff : 0x00000000;
	constexpr int i5 = b5 ? 0xffffffff : 0x00000000;
	constexpr int i6 = b6 ? 0xffffffff : 0x00000000;
	constexpr int i7 = b7 ? 0xffffffff : 0x00000000;

	return mask32x8(_mm256_castsi256_ps(_mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7)));
}

template<>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromBools<0, 0, 0, 0, 0, 0, 0, 0>()
{
	return mask32x8(_mm256_setzero_ps());
}

template<>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromBools<1, 1, 1, 1, 1, 1, 1, 1>()
{
	__m256 a = _mm256_setzero_ps();
	a = _mm256_cmp_ps(a, a, _CMP_EQ_OQ);
	return mask32x8(a);
}

template<bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE mask32x8 mask32x8::PropagateInt(int i)
{
	constexpr int i0 = b0 ? i : 0;
	constexpr int i1 = b1 ? i : 0;
	constexpr int i2 = b2 ? i : 0;
	constexpr int i3 = b3 ? i : 0;
	constexpr int i4 = b4 ? i : 0;
	constexpr int i5 = b5 ? i : 0;
	constexpr int i6 = b6 ? i : 0;
	constexpr int i7 = b7 ? i : 0;

	return mask32x8(_mm256_castsi256_ps(_mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7)));
}
template<>
FFTL_FORCEINLINE mask32x8 mask32x8::PropagateInt<1, 0, 0, 0, 0, 0, 0, 0>(int i)
{
	const __m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(i));
	return mask32x8(_mm256_insertf128_ps(_mm256_castps128_ps256(a), _mm_setzero_ps(), 1));
}
template<>
FFTL_FORCEINLINE mask32x8 mask32x8::PropagateInt<0, 0, 0, 0, 0, 0, 0, 0>(int)
{
	return mask32x8(_mm256_setzero_ps());
}

template<s32 i>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromInt()
{
	return mask32x8(_mm256_castsi256_ps(_mm256_set1_epi32(i)));
}
template<>
FFTL_FORCEINLINE mask32x8 mask32x8::GenMaskFromInt<0>()
{
	return mask32x8(_mm256_setzero_ps());
}


FFTL_FORCEINLINE mask32x8 mask32x8::operator|(const mask32x8& b) const
{
	return mask32x8(_mm256_or_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x8 mask32x8::operator&(const mask32x8& b) const
{
	return mask32x8(_mm256_and_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x8 mask32x8::operator^(const mask32x8& b) const
{
	return mask32x8(_mm256_xor_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator|(const T& b) const
{
	return T(_mm256_or_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator&(const T& b) const
{
	return T(_mm256_and_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x8::operator^(const T& b) const
{
	return T(_mm256_xor_ps(m_v, b.m_v));
}

template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const T& b)
{
	return T(_mm256_andnot_ps(b.m_v, a.m_v));
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const mask32x8& a, const T& b)
{
	return T(_mm256_andnot_ps(b.m_v, a.m_v));
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type AndNot(const T& a, const mask32x8& b)
{
	return T(_mm256_andnot_ps(b.m_v, a.m_v));
}
FFTL_FORCEINLINE mask32x8 AndNot(const mask32x8& a, const mask32x8& b)
{
	return mask32x8(_mm256_andnot_ps(b.m_v, a.m_v));
}

FFTL_FORCEINLINE f32_8 f32_8::operator|(const mask32x8& b) const
{
	return f32_8(_mm256_or_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE f32_8 f32_8::operator&(const mask32x8& b) const
{
	return f32_8(_mm256_and_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE f32_8 f32_8::operator^(const mask32x8& b) const
{
	return f32_8(_mm256_xor_ps(m_v, b.m_v));
}

FFTL_FORCEINLINE mask32x8 CmpEq(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_EQ_OQ));
}
FFTL_FORCEINLINE mask32x8 CmpNe(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_NEQ_OQ));
}
FFTL_FORCEINLINE mask32x8 CmpLt(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_LT_OQ));
}
FFTL_FORCEINLINE mask32x8 CmpLe(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_LE_OQ));
}
FFTL_FORCEINLINE mask32x8 CmpGt(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_GT_OQ));
}
FFTL_FORCEINLINE mask32x8 CmpGe(f32_8_In a, f32_8_In b)
{
	return mask32x8(_mm256_cmp_ps(a.m_v, b.m_v, _CMP_GE_OQ));
}

template<typename T, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const T& a, const T& b)
{
	constexpr int i0 = b0 ? 1 : 0;
	constexpr int i1 = b1 ? 1 : 0;
	constexpr int i2 = b2 ? 1 : 0;
	constexpr int i3 = b3 ? 1 : 0;
	constexpr int i4 = b4 ? 1 : 0;
	constexpr int i5 = b5 ? 1 : 0;
	constexpr int i6 = b6 ? 1 : 0;
	constexpr int i7 = b7 ? 1 : 0;
	constexpr int mask = (i0) | (i1<<1) | (i2<<2) | (i3<<3) | (i4<<4) | (i5<<5) | (i6<<6) | (i7<<7);
	return T{ _mm256_blend_ps(a, b, mask) };
}
template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_8, T>::value, T>::type Blend(const mask32x8& msk, const T& a, const T& b)
{
	return T{ _mm256_blendv_ps(b, a, msk) };
}

inline Vec8f V8fSin(Vec8f_In r)
{
#if defined(FFTL_AVX2) && (defined(FFTL_FMA3) || defined(FFTL_FMA4))
	const __m256 F1111 = _mm256_set1_ps(1.f);
	const __m256 f4OverPi = _mm256_set1_ps(4 / PI_32);
	const __m256 fPiOverTwo = _mm256_set1_ps(PI_32 / 2);
	const __m256i I1111 = _mm256_set1_epi32(1);
	const __m256i signMask = _mm256_set1_epi32(0x80000000);

	const __m256 s_sinCosCoeff2 = _mm256_set1_ps(-0.5f);
	const __m256 s_sinCosCoeff3 = _mm256_set1_ps(-1.66666667E-1f);
	const __m256 s_sinCosCoeff4 = _mm256_set1_ps(4.16666667E-2f);
	const __m256 s_sinCosCoeff5 = _mm256_set1_ps(8.33333333E-3f);
	const __m256 s_sinCosCoeff6 = _mm256_set1_ps(-1.38888889E-3f);
	const __m256 s_sinCosCoeff7 = _mm256_set1_ps(-1.98412698E-4f);
	const __m256 s_sinCosCoeff8 = _mm256_set1_ps(2.48015873E-5f);

	__m256i sinSignBit, polyMask, quarters;
	quarters = _mm256_cvttps_epi32(V8fAddMul(F1111, r, f4OverPi));
	quarters = _mm256_srai_epi32(quarters, 1);

	// Get the sign bit for sine, which alternates for every whole multiple of pi
	sinSignBit = _mm256_slli_epi32(quarters, 30);
	sinSignBit = _mm256_and_si256(sinSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = _mm256_and_si256(quarters, I1111);
	polyMask = _mm256_cmpeq_epi32(polyMask, I1111);

	const __m256 x = _mm256_sub_ps(r, _mm256_mul_ps(_mm256_cvtepi32_ps(quarters), fPiOverTwo));
	const __m256 xx = _mm256_mul_ps(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	__m256 y1 = s_sinCosCoeff8;
	y1 = V8fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V8fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	__m256 y2 = s_sinCosCoeff7;
	y2 = V8fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V8fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = _mm256_mul_ps(y2, xx);
	y2 = V8fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	const __m256 vSin = _mm256_blendv_ps(y2, y1, _mm256_castsi256_ps(polyMask));

	// Set the sign bit and store the result
	return _mm256_xor_ps(vSin, _mm256_castsi256_ps(sinSignBit));
#else
	return V8fSet(V4fSin(V8fAsV4f(r)), V4fCos(V8fGet4567(r)));
#endif
}
inline Vec8f V8fCos(Vec8f_In r)
{
#if defined(FFTL_AVX2) && (defined(FFTL_FMA3) || defined(FFTL_FMA4))
	const __m256 F1111 = _mm256_set1_ps(1.f);
	const __m256 f4OverPi = _mm256_set1_ps(4 / PI_32);
	const __m256 fPiOverTwo = _mm256_set1_ps(PI_32 / 2);
	const __m256i I1111 = _mm256_set1_epi32(1);
	const __m256i signMask = _mm256_set1_epi32(0x80000000);

	const __m256 s_sinCosCoeff2 = _mm256_set1_ps(-0.5f);
	const __m256 s_sinCosCoeff3 = _mm256_set1_ps(-1.66666667E-1f);
	const __m256 s_sinCosCoeff4 = _mm256_set1_ps(4.16666667E-2f);
	const __m256 s_sinCosCoeff5 = _mm256_set1_ps(8.33333333E-3f);
	const __m256 s_sinCosCoeff6 = _mm256_set1_ps(-1.38888889E-3f);
	const __m256 s_sinCosCoeff7 = _mm256_set1_ps(-1.98412698E-4f);
	const __m256 s_sinCosCoeff8 = _mm256_set1_ps(2.48015873E-5f);

	__m256i cosSignBit, polyMask, quarters;
	quarters = _mm256_cvttps_epi32(V8fAddMul(F1111, r, f4OverPi));
	quarters = _mm256_srai_epi32(quarters, 1);

	// Cos sign bit (offset by pi/2 from sine)
	cosSignBit = _mm256_add_epi32(quarters, I1111); // pi/2 == 1 quarter circle
	cosSignBit = _mm256_slli_epi32(cosSignBit, 30);
	cosSignBit = _mm256_and_si256(cosSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = _mm256_and_si256(quarters, I1111);
	polyMask = _mm256_cmpeq_epi32(polyMask, I1111);

	const __m256 x = _mm256_sub_ps(r, _mm256_mul_ps(_mm256_cvtepi32_ps(quarters), fPiOverTwo));
	const __m256 xx = _mm256_mul_ps(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	__m256 y1 = s_sinCosCoeff8;
	y1 = V8fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V8fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	__m256 y2 = s_sinCosCoeff7;
	y2 = V8fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V8fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = _mm256_mul_ps(y2, xx);
	y2 = V8fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	const __m256 vCos = _mm256_blendv_ps(y1, y2, _mm256_castsi256_ps(polyMask));

	// Set the sign bit and store the result
	return _mm256_xor_ps(vCos, _mm256_castsi256_ps(cosSignBit));
#else
	return V8fSet(V4fCos(V8fAsV4f(r)), V4fCos(V8fGet4567(r)));
#endif
}
inline void V8fSinCos(Vec8f_In r, Vec8f& s, Vec8f& c)
{
#if defined(FFTL_AVX2) && (defined(FFTL_FMA3) || defined(FFTL_FMA4))
	const __m256 F1111 = _mm256_set1_ps(1.f);
	const __m256 f4OverPi = _mm256_set1_ps(4 / PI_32);
	const __m256 fPiOverTwo = _mm256_set1_ps(PI_32 / 2);
	const __m256i I1111 = _mm256_set1_epi32(1);
	const __m256i signMask = _mm256_set1_epi32(0x80000000);

	const __m256 s_sinCosCoeff2 = _mm256_set1_ps(-0.5f);
	const __m256 s_sinCosCoeff3 = _mm256_set1_ps(-1.66666667E-1f);
	const __m256 s_sinCosCoeff4 = _mm256_set1_ps(4.16666667E-2f);
	const __m256 s_sinCosCoeff5 = _mm256_set1_ps(8.33333333E-3f);
	const __m256 s_sinCosCoeff6 = _mm256_set1_ps(-1.38888889E-3f);
	const __m256 s_sinCosCoeff7 = _mm256_set1_ps(-1.98412698E-4f);
	const __m256 s_sinCosCoeff8 = _mm256_set1_ps(2.48015873E-5f);

	__m256i sinSignBit, cosSignBit, polyMask, quarters;
	quarters = _mm256_cvttps_epi32(V8fAddMul(F1111, r, f4OverPi));
	quarters = _mm256_srai_epi32(quarters, 1);

	// Get the sign bit for sine, which alternates for every whole multiple of pi
	sinSignBit = _mm256_slli_epi32(quarters, 30);
	sinSignBit = _mm256_and_si256(sinSignBit, signMask);

	// Cos sign bit (offset by pi/2 from sine)
	cosSignBit = _mm256_add_epi32(quarters, I1111); // pi/2 == 1 quarter circle
	cosSignBit = _mm256_slli_epi32(cosSignBit, 30);
	cosSignBit = _mm256_and_si256(cosSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = _mm256_and_si256(quarters, I1111);
	polyMask = _mm256_cmpeq_epi32(polyMask, I1111);

	const __m256 x = _mm256_sub_ps(r, _mm256_mul_ps(_mm256_cvtepi32_ps(quarters), fPiOverTwo));
	const __m256 xx = _mm256_mul_ps(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	__m256 y1 = s_sinCosCoeff8;
	y1 = V8fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V8fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V8fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	__m256 y2 = s_sinCosCoeff7;
	y2 = V8fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V8fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = _mm256_mul_ps(y2, xx);
	y2 = V8fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	const __m256 vSin = _mm256_blendv_ps(y2, y1, _mm256_castsi256_ps(polyMask));
	const __m256 vCos = _mm256_blendv_ps(y1, y2, _mm256_castsi256_ps(polyMask));

	// Set the sign bit and store the result
	s = _mm256_xor_ps(vSin, _mm256_castsi256_ps(sinSignBit));
	c = _mm256_xor_ps(vCos, _mm256_castsi256_ps(cosSignBit));
#else
	Vec4f s0, c0, s1, c1;
	V4fSinCos(V8fAsV4f(r), s0, c0);
	V4fSinCos(V8fGet4567(r), s1, c1);
	s = V8fSet(s0, s1);
	c = V8fSet(c0, c1);
#endif
}


} // namespace FFTL

#endif //_FFTL_MATH_AVX_INL
