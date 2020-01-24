#ifndef __VECTOR3_NEON_INL__
#define __VECTOR3_NEON_INL__
#else
#error "Already included"
#endif // !__VECTOR3_NEON_INL__

#if defined(FFTL_ARM_NEON)


#include "Utils_NEON.h"

namespace FFTL
{

//
//	vec3 specialization
//

template<>
FFTL_FORCEINLINE void vecT<3>::Set(const vecT<2>& xy, f32 z)
{
	m_v = vsetq_lane_f32(z, xy, 2);
	m_v = vsetq_lane_f32(0, m_v, 3);
}

//
//	Functions unique to vec3
//

FFTL_FORCEINLINE vec3 Cross(const vec3& a, const vec3& b)
{
	const float32x4_t rA = vmulq_f32( V4fPermute<1, 2, 0, 3>(a), V4fPermute<2, 0, 1, 3>(b) );
	const float32x4_t rB = vmulq_f32( V4fPermute<2, 0, 1, 3>(a), V4fPermute<1, 2, 0, 3>(b) );

	return vsubq_f32(rA, rB);
}

FFTL_FORCEINLINE void Transpose3x3(vec3& a, vec3& b, vec3& c)
{
	const float32x4_t t0 = V4fPermute<0, 4, 1, 5>(a, b);
	const float32x4_t t2 = V4fPermute<2, 6, 3, 7>(a, b);
	const float32x4_t t1 = V4fPermute<6, 7, 2, 3>(a, t0);

	a = V4fPermute<0, 1, 4, 5>(t0, c);
	b = V4fPermute<0, 1, 5, 5>(t1, c);
	c = V4fPermute<0, 1, 6, 7>(t2, c);
}

FFTL_FORCEINLINE void Transpose3x3SaveW(vec3& a, vec3& b, vec3& c)
{
	float32x4_t r0 = a;
	float32x4_t r1 = b;
	float32x4_t r2 = c;

	const float32x4_t t0 = V4fPermute<0, 4, 1, 5>(r0, r1);
	const float32x4_t t2 = V4fPermute<2, 6, 3, 7>(r0, r1);
	const float32x4_t t1 = V4fPermute<6, 7, 2, 3>(r0, t0);

	r0 = V4fPermute<0, 1, 4, 5>(t0, r2);
	r1 = V4fPermute<0, 1, 5, 5>(t1, r2);
	r2 = V4fPermute<0, 1, 6, 7>(t2, r2);

	a = neon_blend<1, 1, 1, 0>(a, r0);
	b = neon_blend<1, 1, 1, 0>(b, r1);
//	c = neon_blend<1, 1, 1, 0>(c, r2);
	c = r2; // c.w value is already preserved in r2.
}


} // namespace FFTL


#endif // defined(FFTL_ARM_NEON)
