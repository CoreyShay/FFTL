/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Some useful ARM NEON specific inline functions to be shared.

#ifndef __UTILS_NEON_INL__
#define __UTILS_NEON_INL__
#else
#error "Already included"
#endif


#include <arm_neon.h>


namespace FFTL
{



template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE float32x4_t neon_blend(const float32x4_t& a, const float32x4_t& b)
{
	constexpr int i0 = bX ? 0 : 4;
	constexpr int i1 = bY ? 1 : 5;
	constexpr int i2 = bZ ? 2 : 6;
	constexpr int i3 = bW ? 3 : 7;
	return V4fPermute<i0, i1, i2, i3>(a, b);
}

template<>
FFTL_FORCEINLINE float32x4_t neon_blend<0, 0, 0, 0>(const float32x4_t& a, const float32x4_t& b)
{
	(void)a;
	return b;
}
template<>
FFTL_FORCEINLINE float32x4_t neon_blend<1, 1, 1, 1>(const float32x4_t& a, const float32x4_t& b)
{
	(void)b;
	return a;
}

// r = mask ? a : b;
FFTL_FORCEINLINE float32x4_t neon_blend(const uint32x4_t& mask, const float32x4_t& a, const float32x4_t& b)
{
#if 1
	return vreinterpretq_f32_u32( vbslq_u32(mask, vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
#else
	return vreinterpretq_f32_u32( veorq_u32(vreinterpretq_u32_f32(b), vandq_u32(veorq_u32(vreinterpretq_u32_f32(b), vreinterpretq_u32_f32(a)), mask)) );
#endif
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE float32x4_t neon_zero_elements(const float32x4_t& v)
{
	return neon_blend<bX, bY, bZ, bW>(vdupq_n_f32(0), v);
}
template<>
FFTL_FORCEINLINE float32x4_t neon_zero_elements<1, 1, 1, 1>(const float32x4_t&)
{
	return vdupq_n_f32(0);
}

FFTL_FORCEINLINE float32x4_t neon_negate_ps(const float32x4_t& v)
{
	return vnegq_f32(v);
}

FFTL_FORCEINLINE float32x4_t neon_divide_ps(float32x4_t a, float32x4_t b)
{
#if defined(FFTL_64BIT)
	return vdivq_f32(a, b);
#else
	// get an initial estimate of 1/b.
	float32x4_t r = vrecpeq_f32(b);

	// use a couple Newton-Raphson steps to refine the estimate. Depending on your
	// application's accuracy requirements, you may be able to get away with only
	// one refinement (instead of the two used here). Be sure to test!
	r = vmulq_f32(vrecpsq_f32(b, r), r);
	r = vmulq_f32(vrecpsq_f32(b, r), r);

	// and finally, compute a/b = a*(1/b)
	return vmulq_f32(a, r);
#endif
}

FFTL_FORCEINLINE float32x4_t neon_truncate_ps(const float32x4_t& v)
{
	const int32x4_t vi = vcvtq_s32_f32(v);							// Convert to integer with truncation
	const float32x4_t fi = vcvtq_f32_s32(vi);						// Convert back to f32
	return fi;
}

FFTL_FORCEINLINE int32x4_t neon_iFloor_ps(const float32x4_t& v)
{
	const int32x4_t vi = vcvtq_s32_f32(v);							// Convert to integer with truncation
	const float32x4_t fi = vcvtq_f32_s32(vi);						// Convert back to f32
	const uint32x4_t cmp = vcgtq_f32(fi, v);						// Compare the truncated f32 to the original. If it's greater, this becomes 0xffffffff, else 0
	const int32x4_t r = vaddq_s32(vi, vreinterpretq_s32_u32(cmp));	// Add either 0 or -1 to the truncated value to correct input values below 0
	return r;
}
FFTL_FORCEINLINE float32x4_t neon_fFloor_ps(const float32x4_t& v)
{
	return vcvtq_f32_s32(neon_iFloor_ps(v));
}
inline int32x4_t neon_iCeil_ps(const float32x4_t& v)
{
	const int32x4_t vi = vcvtq_s32_f32(v);							// Convert to integer with truncation
	const float32x4_t fi = vcvtq_f32_s32(vi);						// Convert back to f32
	const uint32x4_t cmp = vcltq_f32(fi, v);						// Compare the truncated f32 to the original. If it's less, this becomes 0xffffffff, else 0
	const int32x4_t r = vsubq_s32(vi, vreinterpretq_s32_u32(cmp));	// Subtract either 0 or -1 from the truncated value to correct input values above 0
	return r;
}
inline float32x4_t neon_fCeil_ps(const float32x4_t& v)
{
	return vcvtq_f32_s32(neon_iCeil_ps(v));
}

inline void neon_transpose4x4(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, float32x4_t& io_d)
{
	const float32x4_t tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const float32x4_t tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const float32x4_t tmp1 = V4fPermute<0, 1, 4, 5>(io_c, io_d);
	const float32x4_t tmp3 = V4fPermute<2, 3, 6, 7>(io_c, io_d);

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
	io_d = V4fPermute<1, 3, 5, 7>(tmp2, tmp3);
}

inline void neon_transpose4x3(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, const float32x4_t& in_d)
{
	const float32x4_t tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const float32x4_t tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const float32x4_t tmp1 = V4fPermute<0, 1, 4, 5>(io_c, in_d);
	const float32x4_t tmp3 = V4fPermute<2, 3, 6, 7>(io_c, in_d);

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
}

inline void neon_transpose3x4(float32x4_t& io_a, float32x4_t& io_b, float32x4_t& io_c, float32x4_t& ot_d)
{
	const float32x4_t tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const float32x4_t tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const float32x4_t tmp1 = V4fPermute<0, 1, 0, 1>(io_c);//2nd half will be garbage but not needed
	const float32x4_t tmp3 = V4fPermute<2, 3, 2, 3>(io_c);//2nd half will be garbage but not needed

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
	ot_d = V4fPermute<1, 3, 5, 7>(tmp2, tmp3);
}

inline void neon_SinCos(const float32x4_t& rads, float32x4_t& s, float32x4_t& c)
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

	float32x4_t x = rads;

	int32x4_t sinSignBit, cosSignBit, polyMask;
	{
		int32x4_t quarters = vcvtq_s32_f32(V4fAddMul(F1111, rads, f4OverPi));
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

		x = vsubq_s32(x, vmulq_f32(vcvtq_f32_s32(quarters), fPiOverTwo));
	}

	float32x4_t xx = vmulq_f32(x, x);

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

	{
		// Use the poly mask to merge the polynomial results
		float32x4_t sin = neon_blend(vreinterpretq_u32_s32(polyMask), y1, y2);
		float32x4_t cos = neon_blend(vreinterpretq_u32_s32(polyMask), y2, y1);
	
		// Set the sign bit and store the result
		s = veorq_s32(vreinterpretq_s32_f32(sin), sinSignBit);
		c = veorq_s32(vreinterpretq_s32_f32(cos), cosSignBit);
	}
}


} // namespace FFTL
