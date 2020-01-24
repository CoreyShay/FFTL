#ifndef __VECTOR3_SSE_INL__
#define __VECTOR3_SSE_INL__
#else
#error "Already included"
#endif // !__VECTOR3_SSE_INL__


#if defined(FFTL_SSE)


#include "Utils_SSE.h"

namespace FFTL
{

//
//	vec3 specialization
//

template<>
FFTL_FORCEINLINE void vecT<3>::Set(const vecT<2>& xy, f32 z)
{
	m_v = V4fPermute<A0, A1, B0, B1>(xy, _mm_set_ss(z));
}


//
//	Functions unique to vec3
//

FFTL_FORCEINLINE vec3 Cross(const vec3& a, const vec3& b)
{
	const __m128 rA = _mm_mul_ps( V4fPermute<1, 2, 0, 3>(a), V4fPermute<2, 0, 1, 3>(b) );
	const __m128 rB = _mm_mul_ps( V4fPermute<2, 0, 1, 3>(a), V4fPermute<1, 2, 0, 3>(b) );

	return _mm_sub_ps(rA, rB);
}

FFTL_FORCEINLINE void Transpose3x3(vec3& a, vec3& b, vec3& c)
{
	const __m128 t0 = _mm_unpacklo_ps(a, b);
	const __m128 t2 = _mm_unpackhi_ps(a, b);
	const __m128 t1 = _mm_movehl_ps(a, t0);

	a = _mm_movelh_ps(t0, c);
	b = _mm_shuffle_ps(t1, c, _MM_SHUFFLE_XYZW(0, 1, 1, 1));
	c = _mm_shuffle_ps(t2, c, _MM_SHUFFLE_XYZW(0, 1, 2, 3));
}

FFTL_FORCEINLINE void Transpose3x3SaveW(vec3& a, vec3& b, vec3& c)
{
	__m128 r0 = a;
	__m128 r1 = b;
	__m128 r2 = c;

	const __m128 t0 = _mm_unpacklo_ps(r0, r1);
	const __m128 t2 = _mm_unpackhi_ps(r0, r1);
	const __m128 t1 = _mm_movehl_ps(r0, t0);

	r0 = _mm_movelh_ps(t0, r2);
	r1 = _mm_shuffle_ps(t1, r2, _MM_SHUFFLE_XYZW(0, 1, 1, 1));
	r2 = _mm_shuffle_ps(t2, r2, _MM_SHUFFLE_XYZW(0, 1, 2, 3));

	a = sse_blend<1, 1, 1, 0>(a, r0);
	b = sse_blend<1, 1, 1, 0>(b, r1);
//	c = sse_blend<1, 1, 1, 0>(c, r2);
	c = r2; // c.w value is already preserved in r2.
}






} // namespace FFTL



#endif // defined(SL_MATH_USE_SSE_VEC4)