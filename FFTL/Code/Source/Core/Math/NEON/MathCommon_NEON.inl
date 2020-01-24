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

#include "Utils/Casts.h"

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
FFTL_FORCEINLINE Vec4f V4fSplat4(f32 f)
{
	return vdupq_n_f32(f);
}
FFTL_FORCEINLINE Vec4f V4fSplat4(const f32* pf)
{
	return vdupq_n_f32(*pf);
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vandq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vbicq_u32(vreinterpretq_u32_f32(b), vreinterpretq_u32_f32(a)) );
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
FFTL_FORCEINLINE Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vceqq_f32(a, b));
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










FFTL_FORCEINLINE ScopedFlushDenormals::ScopedFlushDenormals()
{
	size_t x;

	//	Get the floating-point status and control register and store it into x.
	asm(
		"vmrs %[result],FPSCR \r\n"
		:[result] "=r" (x) : :
		);

	//	Store the flush-to-zero bit.
	m_prevMode = safestatic_cast<u32>(x & (1<<24));
	
	//	Enable the flush-to-zero bit.
	x |= (1<<24);
	
	//	Store the new x with the flush-to-zero bit enabled into the floating-point status and control register
	asm(
		"vmsr FPSCR,%[value]"
		:
		: [value] "r" (x)
		:
		);
}
FFTL_FORCEINLINE ScopedFlushDenormals::~ScopedFlushDenormals()
{
	size_t x;

	//	Get the floating-point status and control register and store it into x.
	asm(
		"vmrs %[result],FPSCR \r\n"
		:[result] "=r" (x) : :
		);

	//	If the flush-to-zero bit was enabled before, it will be disabled here.
	x &= ~(1<<24) | m_prevMode;

	//	Restore the previous value of the flush-to-zero bit into the the floating-point status and control register.
	asm(
		"vmsr FPSCR,%[value]"
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
