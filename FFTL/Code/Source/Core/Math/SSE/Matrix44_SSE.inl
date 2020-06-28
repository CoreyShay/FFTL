
#ifndef __MATRIX44_SSE_INL__
#define __MATRIX44_SSE_INL__
#else
#error "Already included"
#endif // !__MATRIX44_SSE_INL__

namespace FFTL
{


inline void mat44::Identity()
{
	const __m128 v = _mm_set_ss(1.f);
	Row<0>() = v;
	Row<1>() = V4fPermute<1, 0, 1, 1>(v);
	Row<2>() = V4fPermute<1, 1, 0, 1>(v);
	Row<3>() = V4fPermute<1, 1, 1, 0>(v);
}

inline bool mat44::operator==(const mat44& rhs) const
{
	const __m128 a = _mm_cmpeq_ps(Row<0>(), rhs.Row<0>());
	const __m128 b = _mm_cmpeq_ps(Row<1>(), rhs.Row<1>());
	const __m128 c = _mm_cmpeq_ps(Row<2>(), rhs.Row<2>());
	const __m128 d = _mm_cmpeq_ps(Row<3>(), rhs.Row<3>());
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return _mm_movemask_ps(r) == 15;
}
inline bool mat44::operator!=(const mat44& rhs) const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), rhs.Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), rhs.Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), rhs.Row<2>());
	const __m128 d = _mm_cmpneq_ps(Row<3>(), rhs.Row<3>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r1 = _mm_or_ps(c, d);
	const __m128 r = _mm_or_ps(r0, r1);
	return _mm_movemask_ps(r) != 0;
}

inline bool mat44::IsNearEqual(const mat44& rhs, f32 tol) const
{
	const __m128 vTol = _mm_set1_ps(tol);

	const __m128 a = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<0>(), rhs.Row<0>())), vTol);
	const __m128 b = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<1>(), rhs.Row<1>())), vTol);
	const __m128 c = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<2>(), rhs.Row<2>())), vTol);
	const __m128 d = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<3>(), rhs.Row<3>())), vTol);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return _mm_movemask_ps(r) == 15;
}

inline bool mat44::IsNan() const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), Row<2>());
	const __m128 d = _mm_cmpneq_ps(Row<3>(), Row<3>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r1 = _mm_or_ps(c, d);
	const __m128 r = _mm_or_ps(r0, r1);
	return _mm_movemask_ps(r) != 0;
}

inline bool mat44::IsInf() const
{
	const __m128 vInf = _mm_rcp_ps(_mm_setzero_ps());
	const __m128 vZero = _mm_setzero_ps();
	
	const __m128 a = _mm_cmpeq_ps(_mm_xor_ps(Row<0>(), vInf), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_xor_ps(Row<1>(), vInf), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_xor_ps(Row<2>(), vInf), vZero);
	const __m128 d = _mm_cmpeq_ps(_mm_xor_ps(Row<3>(), vInf), vZero);
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r1 = _mm_or_ps(c, d);
	const __m128 r = _mm_or_ps(r0, r1);
	return _mm_movemask_ps(r) != 0;
}
inline bool mat44::IsFinite() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 d = _mm_cmpeq_ps(_mm_sub_ps(Row<3>(), Row<3>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return _mm_movemask_ps(r) == 15;
}
inline bool mat44::IsOutrageous() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 d = _mm_cmpeq_ps(_mm_sub_ps(Row<3>(), Row<3>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return _mm_movemask_ps(r) != 15;
}

inline void mat44::Invert()
{
	//	Reference: ftp://download.intel.com/design/pentiumiii/sml/24504301.pdf

	const __m128 vOne = _mm_set1_ps(1.f);
	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;
	tmp1 = _mm_movelh_ps(m_v[0], m_v[1]);
	row1 = _mm_movelh_ps(m_v[2], m_v[3]);
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_movehl_ps(m_v[1], m_v[0]);
	row3 = _mm_movehl_ps(m_v[3], m_v[2]);
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
	// -----------------------------------------------
	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_cmpeq_ps( _mm_add_ps(det, vOne), vOne ); // if (abs(det) < FLT_EPSILON) det = 1.f;
	det = sse_blend(det, vOne, tmp1);
//	tmp1 = _mm_rcp_ps(det);
//	det = _mm_sub_ps(_mm_add_ps(tmp1, tmp1), _mm_mul_ps(det, _mm_mul_ps(tmp1, tmp1)));
	det = _mm_div_ps(vOne, det);
	m_v[0] = _mm_mul_ps(det, minor0);
	m_v[1] = _mm_mul_ps(det, minor1);
	m_v[2] = _mm_mul_ps(det, minor2);
	m_v[3] = _mm_mul_ps(det, minor3);
}


} // namespace FFTL

