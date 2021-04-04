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



} // namespace FFTL
