
#ifndef __MATRIX33_SSE_INL__
#define __MATRIX33_SSE_INL__
#else
#error "Already included"
#endif // !__MATRIX33_SSE_INL__

namespace FFTL
{


inline void mat33::Identity()
{
	const __m128 v = _mm_set_ss(1.f);
	Row<0>() = v;
	Row<1>() = V4fPermute<1, 0, 1, 1>(v);
	Row<2>() = V4fPermute<1, 1, 0, 1>(v);
}

inline bool mat33::operator==(const mat33& rhs) const
{
	const __m128 a = _mm_cmpeq_ps(Row<0>(), rhs.Row<0>());
	const __m128 b = _mm_cmpeq_ps(Row<1>(), rhs.Row<1>());
	const __m128 c = _mm_cmpeq_ps(Row<2>(), rhs.Row<2>());
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r = _mm_and_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) == 7;
}
inline bool mat33::operator!=(const mat33& rhs) const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), rhs.Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), rhs.Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), rhs.Row<2>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r = _mm_or_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) != 0;
}

inline bool IsNearEqual(const mat33& a, const mat33& b, f32 tol)
{
	const __m128 vTol = _mm_set1_ps(tol);

	const __m128 c0 = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(a.Row<0>(), b.Row<0>())), vTol);
	const __m128 c1 = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(a.Row<1>(), b.Row<1>())), vTol);
	const __m128 c2 = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(a.Row<2>(), b.Row<2>())), vTol);
	const __m128 r0 = _mm_and_ps(c0, c1);
	const __m128 r = _mm_and_ps(r0, c2);
	return( _mm_movemask_ps(r) & 7) == 7;
}

inline bool mat33::IsNan() const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), Row<2>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r = _mm_or_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) != 0;
}

inline bool mat33::IsInf() const
{
	const __m128 vInf = _mm_rcp_ps(_mm_setzero_ps());
	const __m128 vZero = _mm_setzero_ps();
	
	const __m128 a = _mm_cmpeq_ps(_mm_xor_ps(Row<0>(), vInf), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_xor_ps(Row<1>(), vInf), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_xor_ps(Row<2>(), vInf), vZero);
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r = _mm_or_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) != 0;
}
inline bool mat33::IsFinite() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r = _mm_and_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) == 7;
}
inline bool mat33::IsOutrageous() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r = _mm_and_ps(r0, c);
	return( _mm_movemask_ps(r) & 7) != 7;
}


} // namespace FFTL

