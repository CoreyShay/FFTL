
#ifndef __MATRIX43_SSE_INL__
#define __MATRIX43_SSE_INL__
#else
#error "Already included"
#endif // !__MATRIX43_SSE_INL__

#include "Utils_SSE.h"

namespace FFTL
{


inline void mat43::Identity()
{
	const __m128 v = _mm_set_ss(1.f);
	Row<0>() = v;
	Row<1>() = V4fPermute<1, 0, 1, 1>(v);
	Row<2>() = V4fPermute<1, 1, 0, 1>(v);
	Row<3>() = V4fPermute<1, 1, 1, 0>(v);
}

inline bool mat43::operator==(const mat43& in_b) const
{
	const __m128 a = _mm_cmpeq_ps(Row<0>(), in_b.Row<0>());
	const __m128 b = _mm_cmpeq_ps(Row<1>(), in_b.Row<1>());
	const __m128 c = _mm_cmpeq_ps(Row<2>(), in_b.Row<2>());
	const __m128 d = _mm_cmpeq_ps(Row<3>(), in_b.Row<3>());
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) == 7;
}
inline bool mat43::operator!=(const mat43& in_b) const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), in_b.Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), in_b.Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), in_b.Row<2>());
	const __m128 d = _mm_cmpneq_ps(Row<3>(), in_b.Row<3>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r1 = _mm_or_ps(c, d);
	const __m128 r = _mm_or_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) != 0;
}

inline bool mat43::IsNearEqual(const mat43& in_b, f32 tol) const
{
	const __m128 vTol = _mm_set1_ps(tol);

	const __m128 a = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<0>(), in_b.Row<0>())), vTol);
	const __m128 b = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<1>(), in_b.Row<1>())), vTol);
	const __m128 c = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<2>(), in_b.Row<2>())), vTol);
	const __m128 d = _mm_cmplt_ps(sse_abs_ps(_mm_sub_ps(Row<3>(), in_b.Row<3>())), vTol);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) == 7;
}

inline bool mat43::IsNan() const
{
	const __m128 a = _mm_cmpneq_ps(Row<0>(), Row<0>());
	const __m128 b = _mm_cmpneq_ps(Row<1>(), Row<1>());
	const __m128 c = _mm_cmpneq_ps(Row<2>(), Row<2>());
	const __m128 d = _mm_cmpneq_ps(Row<3>(), Row<3>());
	const __m128 r0 = _mm_or_ps(a, b);
	const __m128 r1 = _mm_or_ps(c, d);
	const __m128 r = _mm_or_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) != 0;
}

inline bool mat43::IsInf() const
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
	return (_mm_movemask_ps(r) & 7) != 0;
}
inline bool mat43::IsFinite() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 d = _mm_cmpeq_ps(_mm_sub_ps(Row<3>(), Row<3>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) == 7;
}
inline bool mat43::IsOutrageous() const
{
	const __m128 vZero = _mm_setzero_ps();

	const __m128 a = _mm_cmpeq_ps(_mm_sub_ps(Row<0>(), Row<0>()), vZero);
	const __m128 b = _mm_cmpeq_ps(_mm_sub_ps(Row<1>(), Row<1>()), vZero);
	const __m128 c = _mm_cmpeq_ps(_mm_sub_ps(Row<2>(), Row<2>()), vZero);
	const __m128 d = _mm_cmpeq_ps(_mm_sub_ps(Row<3>(), Row<3>()), vZero);
	const __m128 r0 = _mm_and_ps(a, b);
	const __m128 r1 = _mm_and_ps(c, d);
	const __m128 r = _mm_and_ps(r0, r1);
	return (_mm_movemask_ps(r) & 7) != 7;
}




} // namespace FFTL

