
#ifndef __MATRIX43_NEON_INL__
#define __MATRIX43_NEON_INL__
#else
#error "Already included"
#endif // !__MATRIX43_NEON_INL__

#include "Utils_NEON.h"

namespace FFTL
{


inline void mat43::Identity()
{
	const float32x4_t v = vsetq_lane_f32(1.f, vdupq_n_f32(0), 0);
	Row<0>() = v;
	Row<1>() = V4fPermute<1, 0, 1, 1>(v);
	Row<2>() = V4fPermute<1, 1, 0, 1>(v);
	Row<3>() = V4fPermute<1, 1, 1, 0>(v);
}

inline bool mat43::operator==(const mat43& in_b) const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), in_b.Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), in_b.Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), in_b.Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), in_b.Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
inline bool mat43::operator!=(const mat43& in_b) const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), in_b.Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), in_b.Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), in_b.Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), in_b.Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 != 0xffffffffffffffff) | (c1 != 0xffffffff);
}

inline bool mat43::IsNearEqual(const mat43& in_b, f32 tol) const
{
	const float32x4_t vTol = vdupq_n_f32(tol);

	const uint32x4_t a = vcltq_f32(vabdq_f32(Row<0>(), in_b.Row<0>()), vTol);
	const uint32x4_t b = vcltq_f32(vabdq_f32(Row<1>(), in_b.Row<1>()), vTol);
	const uint32x4_t c = vcltq_f32(vabdq_f32(Row<2>(), in_b.Row<2>()), vTol);
	const uint32x4_t d = vcltq_f32(vabdq_f32(Row<3>(), in_b.Row<3>()), vTol);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}

inline bool mat43::IsNan() const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}

inline bool mat43::IsInf() const
{
	const uint32x4_t vInf = vreinterpretq_u32_f32(vrecpeq_f32(vdupq_n_f32(0)));
	const float32x4_t vZero = vdupq_n_f32(0);
	
	const uint32x4_t a = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<0>()), vInf)), vZero);
	const uint32x4_t b = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<1>()), vInf)), vZero);
	const uint32x4_t c = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<2>()), vInf)), vZero);
	const uint32x4_t d = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<3>()), vInf)), vZero);
	const uint32x4_t r0 = vorrq_u32(a, b);
	const uint32x4_t r1 = vorrq_u32(c, d);
	const uint32x4_t r = vorrq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 != 0) | (c1 != 0);
}
inline bool mat43::IsFinite() const
{
	const float32x4_t vZero = vdupq_n_f32(0);

	const uint32x4_t a = vceqq_f32(vsubq_f32(Row<0>(), Row<0>()), vZero);
	const uint32x4_t b = vceqq_f32(vsubq_f32(Row<1>(), Row<1>()), vZero);
	const uint32x4_t c = vceqq_f32(vsubq_f32(Row<2>(), Row<2>()), vZero);
	const uint32x4_t d = vceqq_f32(vsubq_f32(Row<3>(), Row<3>()), vZero);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
inline bool mat43::IsOutrageous() const
{
	const float32x4_t vZero = vdupq_n_f32(0);

	const uint32x4_t a = vceqq_f32(vsubq_f32(Row<0>(), Row<0>()), vZero);
	const uint32x4_t b = vceqq_f32(vsubq_f32(Row<1>(), Row<1>()), vZero);
	const uint32x4_t c = vceqq_f32(vsubq_f32(Row<2>(), Row<2>()), vZero);
	const uint32x4_t d = vceqq_f32(vsubq_f32(Row<3>(), Row<3>()), vZero);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint32_t c1 = vgetq_lane_u32(r, 2);
	return (c0 != 0xffffffffffffffff) | (c1 != 0xffffffff);
}



} // namespace FFTL

