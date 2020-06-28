
#ifndef __MATRIX44_NEON_INL__
#define __MATRIX44_NEON_INL__
#else
#error "Already included"
#endif // !__MATRIX44_NEON_INL__

namespace FFTL
{


inline void mat44::Identity()
{
	const float32x4_t v = vsetq_lane_f32(1.f, vdupq_n_f32(0), 0);
	Row<0>() = v;
	Row<1>() = V4fPermute<1, 0, 1, 1>(v);
	Row<2>() = V4fPermute<1, 1, 0, 1>(v);
	Row<3>() = V4fPermute<1, 1, 1, 0>(v);
}

inline bool mat44::operator==(const mat44& rhs) const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), rhs.Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), rhs.Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), rhs.Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), rhs.Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
inline bool mat44::operator!=(const mat44& rhs) const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), rhs.Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), rhs.Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), rhs.Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), rhs.Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}

inline bool mat44::IsNearEqual(const mat44& rhs, f32 tol) const
{
	const float32x4_t vTol = vdupq_n_f32(tol);

	const uint32x4_t a = vcltq_f32(vabdq_f32(Row<0>(), rhs.Row<0>()), vTol);
	const uint32x4_t b = vcltq_f32(vabdq_f32(Row<1>(), rhs.Row<1>()), vTol);
	const uint32x4_t c = vcltq_f32(vabdq_f32(Row<2>(), rhs.Row<2>()), vTol);
	const uint32x4_t d = vcltq_f32(vabdq_f32(Row<3>(), rhs.Row<3>()), vTol);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}

inline bool mat44::IsNan() const
{
	const uint32x4_t a = vceqq_f32(Row<0>(), Row<0>());
	const uint32x4_t b = vceqq_f32(Row<1>(), Row<1>());
	const uint32x4_t c = vceqq_f32(Row<2>(), Row<2>());
	const uint32x4_t d = vceqq_f32(Row<3>(), Row<3>());
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}

inline bool mat44::IsInf() const
{
	const uint32x4_t vInf = vreinterpretq_u32_f32( vrecpeq_f32(vdupq_n_f32(0)) );
	const float32x4_t vZero = vdupq_n_f32(0);
	
	const uint32x4_t a = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<0>()), vInf)), vZero);
	const uint32x4_t b = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<1>()), vInf)), vZero);
	const uint32x4_t c = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<2>()), vInf)), vZero);
	const uint32x4_t d = vceqq_f32(vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(Row<3>()), vInf)), vZero);
	const uint32x4_t r0 = vorrq_u32(a, b);
	const uint32x4_t r1 = vorrq_u32(c, d);
	const uint32x4_t r = vorrq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 | c1) != 0;
}
inline bool mat44::IsFinite() const
{
	const float32x4_t vZero = vdupq_n_f32(0);

	const float32x4_t a = vceqq_f32(vsubq_f32(Row<0>(), Row<0>()), vZero);
	const float32x4_t b = vceqq_f32(vsubq_f32(Row<1>(), Row<1>()), vZero);
	const float32x4_t c = vceqq_f32(vsubq_f32(Row<2>(), Row<2>()), vZero);
	const float32x4_t d = vceqq_f32(vsubq_f32(Row<3>(), Row<3>()), vZero);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
inline bool mat44::IsOutrageous() const
{
	const float32x4_t vZero = vdupq_n_f32(0);

	const float32x4_t a = vceqq_f32(vsubq_f32(Row<0>(), Row<0>()), vZero);
	const float32x4_t b = vceqq_f32(vsubq_f32(Row<1>(), Row<1>()), vZero);
	const float32x4_t c = vceqq_f32(vsubq_f32(Row<2>(), Row<2>()), vZero);
	const float32x4_t d = vceqq_f32(vsubq_f32(Row<3>(), Row<3>()), vZero);
	const uint32x4_t r0 = vandq_u32(a, b);
	const uint32x4_t r1 = vandq_u32(c, d);
	const uint32x4_t r = vandq_u32(r0, r1);
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(r), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}

inline void mat44::Invert()
{
	float32x4_t row0, row1, row2, row3;
	float32x4_t col0, col1, col2, col3;
	float32x4_t det, tmp1;

	/* Load matrix: */

	col0 = m_v[0];
	col1 = m_v[1];
	col2 = m_v[2];
	col3 = m_v[3];

	/* Transpose: */

	tmp1 = __builtin_shufflevector(col0, col2, 0, 4, 1, 5);
	row1 = __builtin_shufflevector(col1, col3, 0, 4, 1, 5);

	row0 = __builtin_shufflevector(tmp1, row1, 0, 4, 1, 5);
	row1 = __builtin_shufflevector(tmp1, row1, 2, 6, 3, 7);

	tmp1 = __builtin_shufflevector(col0, col2, 2, 6, 3, 7);
	row3 = __builtin_shufflevector(col1, col3, 2, 6, 3, 7);

	row2 = __builtin_shufflevector(tmp1, row3, 0, 4, 1, 5);
	row3 = __builtin_shufflevector(tmp1, row3, 2, 6, 3, 7);

	/* Compute adjoint: */

	row1 = __builtin_shufflevector(row1, row1, 2, 3, 0, 1);
	row3 = __builtin_shufflevector(row3, row3, 2, 3, 0, 1);

	tmp1 = row2 * row3;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);

	col0 = row1 * tmp1;
	col1 = row0 * tmp1;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col0 = row1 * tmp1 - col0;
	col1 = row0 * tmp1 - col1;
	col1 = __builtin_shufflevector(col1, col1, 2, 3, 4, 5);

	tmp1 = row1 * row2;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);

	col0 = row3 * tmp1 + col0;
	col3 = row0 * tmp1;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col0 = col0 - row3 * tmp1;
	col3 = row0 * tmp1 - col3;
	col3 = __builtin_shufflevector(col3, col3, 2, 3, 4, 5);

	tmp1 = __builtin_shufflevector(row1, row1, 2, 3, 4, 5) * row3;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);
	row2 = __builtin_shufflevector(row2, row2, 2, 3, 4, 5);

	col0 = row2 * tmp1 + col0;
	col2 = row0 * tmp1;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col0 = col0 - row2 * tmp1;
	col2 = row0 * tmp1 - col2;
	col2 = __builtin_shufflevector(col2, col2, 2, 3, 4, 5);

	tmp1 = row0 * row1;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);

	col2 = row3 * tmp1 + col2;
	col3 = row2 * tmp1 - col3;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col2 = row3 * tmp1 - col2;
	col3 = col3 - row2 * tmp1;

	tmp1 = row0 * row3;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);

	col1 = col1 - row2 * tmp1;
	col2 = row1 * tmp1 + col2;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col1 = row2 * tmp1 + col1;
	col2 = col2 - row1 * tmp1;

	tmp1 = row0 * row2;
	tmp1 = __builtin_shufflevector(tmp1, tmp1, 1, 0, 7, 6);

	col1 = row3 * tmp1 + col1;
	col3 = col3 - row1 * tmp1;

	tmp1 = __builtin_shufflevector(tmp1, tmp1, 2, 3, 4, 5);

	col1 = col1 - row3 * tmp1;
	col3 = row1 * tmp1 + col3;

	/* Compute determinant: */

	det = row0 * col0;
	det = __builtin_shufflevector(det, det, 2, 3, 4, 5) + det;
	det = __builtin_shufflevector(det, det, 1, 0, 7, 6) + det;

	/* Compute reciprocal of determinant: */

	det = vdupq_n_f32(1.0f / vgetq_lane_f32(det, 0));

	/* Multiply matrix of cofactors with reciprocal of determinant: */

	col0 = col0 * det;
	col1 = col1 * det;
	col2 = col2 * det;
	col3 = col3 * det;

	/* Store inverted matrix: */

	m_v[0] = col0;
	m_v[1] = col1;
	m_v[2] = col2;
	m_v[3] = col3;
}


} // namespace FFTL

