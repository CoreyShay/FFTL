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

#ifndef _FFTL_MATH_NEON_INL
#define _FFTL_MATH_NEON_INL
#else
#error "Already included"
#endif // !_FFTL_MATH_NEON_INL

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning( disable : 4127 ) //conditional expression is constant
#endif

#ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wuninitialized"
#endif

#define FFTL_SIMD_DIVIDE_APPROXIMATE 1


namespace FFTL
{




FFTL_FORCEINLINE Vec4f V4fZero()
{
	return vdupq_n_f32(0);
}
FFTL_FORCEINLINE Vec4f V4fLoadA(const f32* pf)
{
	return vld1q_f32(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadU(const f32* pf)
{
	return vld1q_f32(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadAR(const f32* pf)
{
	const Vec4f v = vld1q_f32(pf);
	return V4fReverse(v);
}
FFTL_FORCEINLINE Vec4f V4fLoadUR(const f32* pf)
{
	const Vec4f v = vld1q_f32(pf);
	return V4fReverse(v);
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	const Vec4f v = vdupq_n_f32(0);
	return vld1q_lane_f32(pf, v, 0);
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	return vcombine_f32( vld1_f32(pf), vdup_n_f32(0) );
}
FFTL_FORCEINLINE Vec4f V4fLoad3(const f32* pf)
{
	float32x2_t v0 = vld1_f32(pf);
	float32x2_t v1 = vdup_n_f32(0);
	v1 = vld1_lane_f32(pf+2, v1, 0);
	return vcombine_f32(v0, v1);
}
FFTL_FORCEINLINE void V4fStoreA(f32* pf, Vec4f_In v)
{
	vst1q_f32(pf, v);
}
FFTL_FORCEINLINE void V4fStoreU(f32* pf, Vec4f_In v)
{
	vst1q_f32(pf, v);
}
FFTL_FORCEINLINE void V4fStore1(f32* pf, Vec4f_In v)
{
	vst1q_lane_f32(pf+0, v, 0);
}
FFTL_FORCEINLINE void V4fStore2(f32* pf, Vec4f_In v)
{
	vst1_f32(pf, vget_low_f32(v));
}
FFTL_FORCEINLINE void V4fStore3(f32* pf, Vec4f_In v)
{
	vst1_f32(pf, vget_low_f32(v));
	vst1q_lane_f32(pf+2, v, 2);
}
FFTL_FORCEINLINE void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD)
{
	vst1q_lane_f32(pf+iA, v, 0);
	vst1q_lane_f32(pf+iB, v, 1);
	vst1q_lane_f32(pf+iC, v, 2);
	vst1q_lane_f32(pf+iD, v, 3);
}
FFTL_FORCEINLINE Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w)
{
	Vec4f v;
	v = vsetq_lane_f32(x, v, 0);
	v = vsetq_lane_f32(y, v, 1);
	v = vsetq_lane_f32(z, v, 2);
	v = vsetq_lane_f32(w, v, 3);
	return v;
}
FFTL_FORCEINLINE Vec4f V4fSet1(f32 x)
{
	return vsetq_lane_f32(x, vdupq_n_f32(0), 0);
}
FFTL_FORCEINLINE Vec4f V4fSplat(f32 f)
{
	return vdupq_n_f32(f);
}
FFTL_FORCEINLINE Vec4f V4fSplat(const f32* pf)
{
	return vdupq_n_f32(*pf);
}
FFTL_FORCEINLINE Vec4f V4fSplatXY(const f32* pf)
{
	const auto v2 = vld1_f32(pf);
	return vcombine_f32(v2, v2);
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vandq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vbicq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vorrq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( veorq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	return vaddq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	return vsubq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	return vmulq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
#if defined(FFTL_64BIT)
	return vdivq_f32(a, b);
#else
	// get an initial estimate of 1/b.
	Vec4f r = vrecpeq_f32(b);

	// use a couple Newton-Raphson steps to refine the estimate. Depending on your
	// application's accuracy requirements, you may be able to get away with only
	// one refinement (instead of the two used here). Be sure to test!
	r = vmulq_f32(vrecpsq_f32(b, r), r);
	r = vmulq_f32(vrecpsq_f32(b, r), r);

	// and finally, compute a/b = a*(1/b)
	return vmulq_f32(a, r);
#endif
}
FFTL_FORCEINLINE Vec4f V4fMin(Vec4f_In a, Vec4f_In b)
{
	return vminq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMax(Vec4f_In a, Vec4f_In b)
{
	return vmaxq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a*b+c;
#if defined(__clang__) && !defined(FFTL_64BIT)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	Vec4f ret = c;
	__asm__("vmla.f32 %0, %1, %2" : "+w" (ret) : "w" (a), "w" (b));
	return ret;
#else
	return vmlaq_f32(c, a, b); // Fused-multiply-add
#endif
}
FFTL_FORCEINLINE Vec4f V4fNMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return -a*b+c;
	return V4fSubMul(c, a, b);
}
FFTL_FORCEINLINE Vec4f V4fMulSub(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a*b-c;
	FFTL_ASSERT_MSG(0, "MulSub is sub-optimal on ARM, due to forced negation. Use SubMul instead.");
	return V4fMulAdd(a, b, V4fNegate(c));
}
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a+b*c;
#if defined(__clang__) && !defined(FFTL_64BIT)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	Vec4f ret = a;
	__asm__("vmla.f32 %0, %1, %2" : "+w" (ret) : "w" (b), "w" (c));
	return ret;
#else
	return vmlaq_f32(a, b, c); // Fused-multiply-add
#endif
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a-b*c;
#if defined(__clang__) && !defined(FFTL_64BIT)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	Vec4f ret = a;
	__asm__("vmls.f32 %0, %1, %2" : "+w" (ret) : "w" (b), "w" (c));
	return ret;
#else
	return vmlsq_f32(a, b, c); // Fused-multiply-subtract
#endif
}
FFTL_FORCEINLINE Vec4f V4fNegate(Vec4f_In v)
{
	return vnegq_f32(v);
}
FFTL_FORCEINLINE Vec4f V4fAbs(Vec4f_In v)
{
	return vabsq_f32(v);
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
#if defined(FFTL_64BIT)
	return vsqrtq_f32(v);
#else
	Vec4f r = vrsqrteq_f32(v);
	return vmulq_f32(r, v);
#endif
}
FFTL_FORCEINLINE Vec4f V4fRcpSqrt(Vec4f_In v)
{
	return vrsqrteq_f32(v);
}
FFTL_FORCEINLINE Vec4f V4fRcp(Vec4f_In v)
{
	return vrecpeq_f32(v);
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	float32x2_t r;
	float32x2_t v0 = vget_low_f32(v);
	float32x2_t v1 = vget_high_f32(v);
	r = vpadd_f32(v0, v1);
	r = vpadd_f32(r, r);
	return vget_lane_f32(r, 0);
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	float32x2_t r;
	float32x2_t v0 = vget_low_f32(v);
	float32x2_t v1 = vget_high_f32(v);
	r = vpadd_f32(v0, v1);
	r = vpadd_f32(r, r);
	return vcombine_f32(r, r);
}
FFTL_FORCEINLINE Vec4f V4fCompareEq(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vceqq_f32(a, b));
}
FFTL_FORCEINLINE Vec4f V4fCompareNq(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vmvnq_u32(vceqq_f32(a, b)));
}
FFTL_FORCEINLINE Vec4f V4fCompareGt(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vcgtq_f32(a, b));
}
FFTL_FORCEINLINE Vec4f V4fCompareLt(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vcltq_f32(a, b));
}
FFTL_FORCEINLINE Vec4f V4fCompareGe(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vcgeq_f32(a, b));
}
FFTL_FORCEINLINE Vec4f V4fCompareLe(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vcleq_f32(a, b));
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	const uint32x4_t n = vreinterpretq_u32_f32(v);
	const uint32_t n0 = vgetq_lane_u32(n, 0) >> 31;
	const uint32_t n1 = vgetq_lane_u32(n, 1) >> 31;
	const uint32_t n2 = vgetq_lane_u32(n, 2) >> 31;
	const uint32_t n3 = vgetq_lane_u32(n, 3) >> 31;
	const uint32_t r = n0 | (n1 << 1) | (n2 << 2) | (n3 << 3);
	return static_cast<int>(r);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	const uint32x4_t n = vceqq_f32(a, b);
	const uint32_t n0 = vgetq_lane_u32(n, 0);
	const uint32_t n1 = vgetq_lane_u32(n, 1);
	const uint32_t n2 = vgetq_lane_u32(n, 2);
	const uint32_t n3 = vgetq_lane_u32(n, 3);
	return (n0 & n1 & n2 & n3) != 0;
}
FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	const float32x2x2_t r = vzip_f32(vget_low_f32(a), vget_low_f32(b) );
	return vcombine_f32(r.val[0], r.val[1]);
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	const float32x2x2_t r = vzip_f32(vget_high_f32(a), vget_high_f32(b));
	return vcombine_f32(r.val[0], r.val[1]);
}
FFTL_FORCEINLINE Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b)
{
	float32x2_t a0 = vget_low_f32(a);
	float32x2_t a1 = vget_high_f32(a);
	float32x2_t b0 = vget_low_f32(b);
	float32x2_t b1 = vget_high_f32(b);
	float32x2x2_t f0213 = vuzp_f32(a0, a1);
	float32x2x2_t f4657 = vuzp_f32(b0, b1);
	return vcombine_f32( f0213.val[0], f4657.val[0] );
}
FFTL_FORCEINLINE Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b)
{
	float32x2_t a0 = vget_low_f32(a);
	float32x2_t a1 = vget_high_f32(a);
	float32x2_t b0 = vget_low_f32(b);
	float32x2_t b1 = vget_high_f32(b);
	float32x2x2_t f0213 = vuzp_f32(a0, a1);
	float32x2x2_t f4657 = vuzp_f32(b0, b1);
	return vcombine_f32( f0213.val[1], f4657.val[1] );
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	const Vec4f r = vrev64q_f32(v);
	return vcombine_f32( vget_high_f32(r), vget_low_f32(r) );
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return vgetq_lane_f32(v, 0);
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return vgetq_lane_f32(v, 1);
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return vgetq_lane_f32(v, 2);
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return vgetq_lane_f32(v, 3);
}


template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute(Vec4f_In v)
{
	static_assert(
		(T0==0 || T0==1 || T0==2 || T0==3) &&
		(T1==0 || T1==1 || T1==2 || T1==3) &&
		(T2==0 || T2==1 || T2==2 || T2==3) &&
		(T3==0 || T3==1 || T3==2 || T3==3),
		"Template parameters need to be in range");

#if defined(__clang__)
	return __builtin_shufflevector(v, v, T0, T1, T2, T3);
#elif 0
	Vec4f r;
	r = vsetq_lane_f32(vgetq_lane_f32(v, T0), r, 0);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T1), r, 1);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T2), r, 2);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T3), r, 3);
	return r;
#else
	const f32* p = reinterpret_cast<const f32*>(&v);
	const Vec4f r = { p[T0], p[T1], p[T2], p[T3] };
	return r;
#endif
}

template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 1, 2, 3>(Vec4f_In v)
{
	return v;
}

template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In a, Vec4f_In b )
{
	static_assert(
		(T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3 || T0 == 4 || T0 == 5 || T0 == 6 || T0 == 7) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3 || T1 == 4 || T1 == 5 || T1 == 6 || T1 == 7) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3 || T2 == 4 || T2 == 5 || T2 == 6 || T2 == 7) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3 || T3 == 4 || T3 == 5 || T3 == 6 || T3 == 7),
		"Template parameters need to be in range");

#if !defined(FFTL_ALLOW_SINGLE_COMPONENT_PERMUTE2)
	static_assert(
		!( (T0 < 4) && (T1 < 4) && (T2 < 4) && (T3 < 4) ) &&
		!( (T0 > 3) && (T1 > 3) && (T2 > 3) && (T3 > 3) ),
		"Use the single parameter V4fPermute function.");
#endif

#if defined(__clang__)
	return __builtin_shufflevector(a, b, T0, T1, T2, T3);
#else
	constexpr uint shufX = (T0==0||T0==1||T0==2||T0==3) ? 0 : 1;
	constexpr uint shufY = (T1==0||T1==1||T1==2||T1==3) ? 0 : 1;
	constexpr uint shufZ = (T2==0||T2==1||T2==2||T2==3) ? 0 : 1;
	constexpr uint shufW = (T3==0||T3==1||T3==2||T3==3) ? 0 : 1;

	const f32* arr[2] = { reinterpret_cast<const f32*>(&a), reinterpret_cast<const f32*>(&b) };
	const Vec4f r = { arr[shufX][FFTL_PERMUTEMASK(T0)], arr[shufY][FFTL_PERMUTEMASK(T1)], arr[shufZ][FFTL_PERMUTEMASK(T2)], arr[shufW][FFTL_PERMUTEMASK(T3)] };
	return r;
#endif
}

#if defined(FFTL_ALLOW_SINGLE_COMPONENT_PERMUTE2)
//	Compiler catch naive permutes
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 1, 2, 3>(Vec4f_In a, Vec4f_In b)
{
	(void)b;
	return a;
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 5, 6, 7>(Vec4f_In a, Vec4f_In b)
{
	(void)a;
	return b;
}
#endif

//	We can have the compiler catch some special cases where we can minimize instructions
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 4, 1, 5>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeXY(a, b);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 0, 5, 1>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeXY(b, a);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<2, 6, 3, 7>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeZW(a, b);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<6, 2, 7, 3>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeZW(b, a);
}

FFTL_FORCEINLINE Vec4f V4fSin(Vec4f_In r)
{
	const float32x4_t F1111 = vdupq_n_f32(1.f);
	const float32x4_t f4OverPi = vdupq_n_f32(4 / PI_32);
	const float32x4_t fPiOverTwo = vdupq_n_f32(PI_32 / 2);
	const int32x4_t I1111 = vdupq_n_s32(1);
	const int32x4_t signMask = vdupq_n_s32(0x80000000);

	const float32x4_t s_sinCosCoeff2 = vdupq_n_f32(-0.5f);
	const float32x4_t s_sinCosCoeff3 = vdupq_n_f32(-1.66666667E-1f);
	const float32x4_t s_sinCosCoeff4 = vdupq_n_f32(4.16666667E-2f);
	const float32x4_t s_sinCosCoeff5 = vdupq_n_f32(8.33333333E-3f);
	const float32x4_t s_sinCosCoeff6 = vdupq_n_f32(-1.38888889E-3f);
	const float32x4_t s_sinCosCoeff7 = vdupq_n_f32(-1.98412698E-4f);
	const float32x4_t s_sinCosCoeff8 = vdupq_n_f32(2.48015873E-5f);

	int32x4_t sinSignBit, polyMask, quarters;
	quarters = vcvtq_s32_f32(V4fAddMul(F1111, r, f4OverPi));
	quarters = vshrq_n_s32(quarters, 1);

	// Get the sign bit for sine, which alternates for every whole multiple of pi
	sinSignBit = vshlq_n_s32(quarters, 30);
	sinSignBit = vandq_s32(sinSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = vandq_s32(quarters, I1111);
	polyMask = vceqq_s32(polyMask, I1111);

	const float32x4_t x = vsubq_s32(r, vmulq_f32(vcvtq_f32_s32(quarters), fPiOverTwo));
	const float32x4_t xx = vmulq_f32(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	float32x4_t y1 = s_sinCosCoeff8;
	y1 = V4fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V4fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	float32x4_t y2 = s_sinCosCoeff7;
	y2 = V4fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V4fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = vmulq_f32(y2, xx);
	y2 = V4fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	float32x4_t vSin = neon_blend(vreinterpretq_u32_s32(polyMask), y1, y2);

	// Set the sign bit and store the result
	return veorq_s32(vreinterpretq_s32_f32(vSin), sinSignBit);
}
FFTL_FORCEINLINE Vec4f V4fCos(Vec4f_In r)
{
	const float32x4_t F1111 = vdupq_n_f32(1.f);
	const float32x4_t f4OverPi = vdupq_n_f32(4 / PI_32);
	const float32x4_t fPiOverTwo = vdupq_n_f32(PI_32 / 2);
	const int32x4_t I1111 = vdupq_n_s32(1);
	const int32x4_t signMask = vdupq_n_s32(0x80000000);

	const float32x4_t s_sinCosCoeff2 = vdupq_n_f32(-0.5f);
	const float32x4_t s_sinCosCoeff3 = vdupq_n_f32(-1.66666667E-1f);
	const float32x4_t s_sinCosCoeff4 = vdupq_n_f32(4.16666667E-2f);
	const float32x4_t s_sinCosCoeff5 = vdupq_n_f32(8.33333333E-3f);
	const float32x4_t s_sinCosCoeff6 = vdupq_n_f32(-1.38888889E-3f);
	const float32x4_t s_sinCosCoeff7 = vdupq_n_f32(-1.98412698E-4f);
	const float32x4_t s_sinCosCoeff8 = vdupq_n_f32(2.48015873E-5f);

	int32x4_t cosSignBit, polyMask, quarters;
	quarters = vcvtq_s32_f32(V4fAddMul(F1111, r, f4OverPi));
	quarters = vshrq_n_s32(quarters, 1);

	// Cos sign bit (offset by pi/2 from sine)
	cosSignBit = vaddq_s32(quarters, I1111); // pi/2 == 1 quarter circle
	cosSignBit = vshlq_n_s32(cosSignBit, 30);
	cosSignBit = vandq_s32(cosSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = vandq_s32(quarters, I1111);
	polyMask = vceqq_s32(polyMask, I1111);

	const float32x4_t x = vsubq_s32(r, vmulq_f32(vcvtq_f32_s32(quarters), fPiOverTwo));
	const float32x4_t xx = vmulq_f32(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	float32x4_t y1 = s_sinCosCoeff8;
	y1 = V4fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V4fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	float32x4_t y2 = s_sinCosCoeff7;
	y2 = V4fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V4fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = vmulq_f32(y2, xx);
	y2 = V4fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	float32x4_t vCos = neon_blend(vreinterpretq_u32_s32(polyMask), y2, y1);

	// Set the sign bit and store the result
	return veorq_s32(vreinterpretq_s32_f32(vCos), cosSignBit);
}
FFTL_FORCEINLINE void V4fSinCos(Vec4f_In r, Vec4f& s, Vec4f& c)
{
	const float32x4_t F1111 = vdupq_n_f32(1.f);
	const float32x4_t f4OverPi = vdupq_n_f32(4 / PI_32);
	const float32x4_t fPiOverTwo = vdupq_n_f32(PI_32 / 2);
	const int32x4_t I1111 = vdupq_n_s32(1);
	const int32x4_t signMask = vdupq_n_s32(0x80000000);

	const float32x4_t s_sinCosCoeff2 = vdupq_n_f32(-0.5f);
	const float32x4_t s_sinCosCoeff3 = vdupq_n_f32(-1.66666667E-1f);
	const float32x4_t s_sinCosCoeff4 = vdupq_n_f32(4.16666667E-2f);
	const float32x4_t s_sinCosCoeff5 = vdupq_n_f32(8.33333333E-3f);
	const float32x4_t s_sinCosCoeff6 = vdupq_n_f32(-1.38888889E-3f);
	const float32x4_t s_sinCosCoeff7 = vdupq_n_f32(-1.98412698E-4f);
	const float32x4_t s_sinCosCoeff8 = vdupq_n_f32(2.48015873E-5f);

	int32x4_t sinSignBit, cosSignBit, polyMask, quarters;
	quarters = vcvtq_s32_f32(V4fAddMul(F1111, r, f4OverPi));
	quarters = vshrq_n_s32(quarters, 1);

	// Get the sign bit for sine, which alternates for every whole multiple of pi
	sinSignBit = vshlq_n_s32(quarters, 30);
	sinSignBit = vandq_s32(sinSignBit, signMask);

	// Cos sign bit (offset by pi/2 from sine)
	cosSignBit = vaddq_s32(quarters, I1111); // pi/2 == 1 quarter circle
	cosSignBit = vshlq_n_s32(cosSignBit, 30);
	cosSignBit = vandq_s32(cosSignBit, signMask);

	// The poly mask is used to evaluate each polynomial only over its intended domain
	polyMask = vandq_s32(quarters, I1111);
	polyMask = vceqq_s32(polyMask, I1111);

	const float32x4_t x = vsubq_s32(r, vmulq_f32(vcvtq_f32_s32(quarters), fPiOverTwo));
	const float32x4_t xx = vmulq_f32(x, x);

	// Evaluate the even polynomial for the upper part of the curve (((c8 x^2 + c6) x^2 + c4) x^2 + c2) x^2 + 1
	float32x4_t y1 = s_sinCosCoeff8;
	y1 = V4fAddMul(s_sinCosCoeff6, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff4, y1, xx);
	y1 = V4fAddMul(s_sinCosCoeff2, y1, xx);
	y1 = V4fAddMul(F1111, y1, xx);

	// Evaluate the odd polynomial for the lower part of the curve ((c7 x^2 + c5) x^2 + c3) x^3 + x
	float32x4_t y2 = s_sinCosCoeff7;
	y2 = V4fAddMul(s_sinCosCoeff5, y2, xx);
	y2 = V4fAddMul(s_sinCosCoeff3, y2, xx);
	y2 = vmulq_f32(y2, xx);
	y2 = V4fAddMul(x, x, y2);

	// Use the poly mask to merge the polynomial results
	float32x4_t vSin = neon_blend(vreinterpretq_u32_s32(polyMask), y1, y2);
	float32x4_t vCos = neon_blend(vreinterpretq_u32_s32(polyMask), y2, y1);

	// Set the sign bit and store the result
	s = veorq_s32(vreinterpretq_s32_f32(vSin), sinSignBit);
	c = veorq_s32(vreinterpretq_s32_f32(vCos), cosSignBit);
}




FFTL_FORCEINLINE Vec4u V4uAdd(Vec4u_In a, Vec4u_In b)
{
	return vaddq_u32(a, b);
}
FFTL_FORCEINLINE Vec4u V4uSub(Vec4u_In a, Vec4u_In b)
{
	return vsubq_u32(a, b);
}
FFTL_FORCEINLINE Vec4u V4uMul(Vec4u_In a, Vec4u_In b)
{
	return vmulq_u32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iAdd(Vec4i_In a, Vec4i_In b)
{
	return vaddq_s32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iSub(Vec4i_In a, Vec4i_In b)
{
	return vsubq_s32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iMul(Vec4i_In a, Vec4i_In b)
{
	return vmulq_s32(a, b);
}

FFTL_FORCEINLINE Vec4i V4fRoundToVfi(Vec4f_In a)
{
#if defined(FFTL_64BIT)
	return vcvtnq_s32_f32(a);
#else
	const auto vZero = vdupq_n_f32(0);
	const auto v0p5 = vdupq_n_f32(0.5f);
	const auto vMask = vcltq_f32(a, vZero);
	const auto vRounded = neon_blend(vsubq_f32(a, v0p5), vaddq_f32(a, v0p5), vMask);
	return vcvtq_s32_f32(vRounded);
#endif
}



inline constexpr mask32x4::mask32x4(const uint32x4_t& v)
	: m_v(v)
{
}

FFTL_FORCEINLINE mask32x4& mask32x4::operator=(const uint32x4_t& v)
{
	m_v = v;
	return *this;
}

inline constexpr mask32x4::operator const uint32x4_t&() const
{
	return m_v;
}

FFTL_FORCEINLINE mask32x4::operator uint32x4_t&()
{
	return m_v;
}

FFTL_FORCEINLINE mask32x4 mask32x4::operator|(const mask32x4& b) const
{
	return mask32x4(vorrq_u32(m_v, b));
}
FFTL_FORCEINLINE mask32x4 mask32x4::operator&(const mask32x4& b) const
{
	return mask32x4(vandq_s32(m_v, b));
}
FFTL_FORCEINLINE mask32x4 mask32x4::operator^(const mask32x4& b) const
{
	return mask32x4( veorq_u32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)) );
}

FFTL_FORCEINLINE int mask32x4::ToIntMask() const
{
	const uint32_t n0 = vgetq_lane_u32(m_v, 0) & 1;
	const uint32_t n1 = vgetq_lane_u32(m_v, 1) & 2;
	const uint32_t n2 = vgetq_lane_u32(m_v, 2) & 4;
	const uint32_t n3 = vgetq_lane_u32(m_v, 3) & 8;
	return static_cast<int>(n0 | n1 | n2 | n3);
}

template<s32 x, s32 y, s32 z, s32 w>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromInts()
{
	uint32x2_t a = vcreate_u32(static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32));
	uint32x2_t b = vcreate_u32(static_cast<uint64_t>(z) | (static_cast<uint64_t>(w) << 32));
	return vcombine_u32(a, b);
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools()
{
	constexpr int ix = bX ? 0 : 0xffffffff;
	constexpr int iy = bY ? 0 : 0xffffffff;
	constexpr int iz = bZ ? 0 : 0xffffffff;
	constexpr int iw = bW ? 0 : 0xffffffff;
	return GenMaskFromInts<ix, iy, iz, iw>();
}

template<>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools<0, 0, 0, 0>()
{
	return vdupq_n_u32(0);
}

template<>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools<1, 1, 1, 1>()
{
	return vdupq_n_u32(0xffffffff);
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt(int i)
{
	constexpr int ix = bX ? 0 : 2;
	constexpr int iy = bY ? 0 : 2;
	constexpr int iz = bZ ? 0 : 2;
	constexpr int iw = bW ? 0 : 2;

	uint32x4_t a = vreinterpretq_u32_u64( vsetq_lane_u64(i, vdupq_n_u64(0), 0) );
	return __builtin_shufflevector(a, a, ix, iy, iz, iw);
}
template<>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt<1, 0, 0, 0>(int i)
{
	uint32x4_t a = vsetq_lane_u32(i, vdupq_n_u32(0), 0);
	return a;
}
template<>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt<0, 0, 0, 0>(int)
{
	return vdupq_n_u32(0);
}

template<s32 i>
FFTL_FORCEINLINE mask32x4 GenMaskFromInt()
{
	return vdupq_n_u32(static_cast<u32>(i));
}


template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator|(const T& b) const
{
	return T( vorrq_u32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)) );
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator&(const T& b) const
{
	return T( vandq_s32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)) );
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator^(const T& b) const
{
	return T(veorq_u32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)));
}

template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const T& a, const T& b)
{
	return T(vbicq_u32(vreinterpretq_u32_f32(a.m_v), vreinterpretq_u32_f32(b.m_v)));
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const mask32x4& a, const T& b)
{
	return T(vbicq_u32(vreinterpretq_u32_f32(a.m_v), vreinterpretq_u32_f32(b.m_v)));
}
template<typename T> FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type AndNot(const T& a, const mask32x4& b)
{
	return T(vbicq_u32(vreinterpretq_u32_f32(a.m_v), vreinterpretq_u32_f32(b.m_v)));
}
FFTL_FORCEINLINE mask32x4 AndNot(const mask32x4& a, const mask32x4& b)
{
	return mask32x4(vbicq_u32(vreinterpretq_u32_f32(a.m_v), vreinterpretq_u32_f32(b.m_v)));
}

FFTL_FORCEINLINE f32_4 f32_4::operator|(const mask32x4& b) const
{
	return f32_4(vorrq_u32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)));
}
FFTL_FORCEINLINE f32_4 f32_4::operator&(const mask32x4& b) const
{
	return f32_4(vandq_s32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)));
}
FFTL_FORCEINLINE f32_4 f32_4::operator^(const mask32x4& b) const
{
	return f32_4(veorq_u32(vreinterpretq_u32_f32(m_v), vreinterpretq_u32_f32(b.m_v)));
}

FFTL_FORCEINLINE mask32x4 CmpEq(f32_4_In a, f32_4_In b)
{
	return mask32x4(vceqq_f32(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpNe(f32_4_In a, f32_4_In b)
{
	return mask32x4(vmvnq_u32(vceqq_f32(a.m_v, b.m_v)));
}
FFTL_FORCEINLINE mask32x4 CmpLt(f32_4_In a, f32_4_In b)
{
	return mask32x4(vcltq_f32(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpLe(f32_4_In a, f32_4_In b)
{
	return mask32x4(vcleq_f32(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpGt(f32_4_In a, f32_4_In b)
{
	return mask32x4(vcgtq_f32(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpGe(f32_4_In a, f32_4_In b)
{
	return mask32x4(vcgeq_f32(a.m_v, b.m_v));
}

template<typename T, bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const T& a, const T& b)
{
	return T{ neon_blend<bX, bY, bZ, bW>(a.GetNative(), b.GetNative()) };
}
template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const mask32x4& msk, const T& a, const T& b)
{
	return T{ neon_blend(msk, a.GetNative(), b.GetNative()) };
}




#if defined(FFTL_64BIT)
#	define FFTL_ASM_READ_FPCR "mrs"
#	define FFTL_ASM_WRITE_FPCR "msr"
#	define FFTL_ASM_FPCR "FPCR"
#else
#	define FFTL_ASM_READ_FPCR "vmrs"
#	define FFTL_ASM_WRITE_FPCR "vmsr"
#	define FFTL_ASM_FPCR "FPSCR"
#endif




FFTL_FORCEINLINE bool GetCpuFlushDenormalMode()
{
	size_t x;

	//	Get the floating-point status and control register and store it into x.
	__asm(
	FFTL_ASM_READ_FPCR " %[result], " FFTL_ASM_FPCR " \r\n"
		: [result] "=r" (x) : :
		);

	//	Return the flush-to-zero bit.
	return (x & (1 << 24)) != 0;
}
FFTL_FORCEINLINE void SetCpuFlushDenormalMode(bool bEnable)
{
	size_t x;

	//	Get the floating-point status and control register and store it into x.
	__asm(
		FFTL_ASM_READ_FPCR " %[result], " FFTL_ASM_FPCR " \r\n"
		: [result] "=r" (x) : :
		);

	//	Enable or disable the flush to zero bit
	x = bEnable ? x | (1 << 24) : x & ~(1 << 24);

	//	Store the new x into the floating-point status and control register
	__asm(
		FFTL_ASM_WRITE_FPCR " " FFTL_ASM_FPCR " ,%[value]"
		:
		: [value] "r" (x)
		:
		);
}



} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif
