
#ifndef VECTOR_SSE_INL
#define VECTOR_SSE_INL
#else
#error "Already included"
#endif // !VECTOR_SSE_INL

#if defined(FFTL_SSE)


#include "Utils_SSE.h"

namespace FFTL
{




template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT()
#if defined(FFTL_ENABLE_ASSERT)
//	Fill with invalid stuff that will be more obvious if used accidentally
	: f32_4( _mm_castsi128_ps( _mm_set1_epi32(0xffffffff) ) )
#endif
{
}

template<uint N>
inline constexpr vecT<N>::vecT(const vecT<N>& v)
	: f32_4(v)
{
}

template<uint N>
inline constexpr vecT<N>::vecT(const f32_4& v)
	: f32_4(v)
{
}

template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator=(const vecT<N>& v)
{
	m_v = v.m_v;
	return *this;
}

template<uint N>
inline constexpr vecT<N>::vecT(const __m128& v)
	: f32_4(v)
{
}

template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator=(const __m128& v)
{
	m_v = v;
	return *this;
}

template<uint N>
inline constexpr vecT<N>::operator const __m128&() const
{
	return m_v;
}

template<uint N>
FFTL_FORCEINLINE vecT<N>::operator __m128&()
{
	return m_v;
}

template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x, f32 y, f32 z)
{
	const __m128 vx = _mm_set_ss(x);
	const __m128 vy = _mm_set_ss(y);
	const __m128 vz = _mm_set_ss(z);
	m_v = _mm_unpacklo_ps(vx, vy);
	m_v = _mm_movelh_ps(m_v, vz);
}
template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x, f32 y)
	: f32_4( _mm_unpacklo_ps(_mm_set_ss(x), _mm_set_ss(y)))
{
}
template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x)
	: f32_4(_mm_set_ss(x))
{
}

///	Use these template sets when we know the input values at compile time to eliminate unnecessary code generation.
/// Input integer bits will be reinterpreted as floats.
///	If less than 4 parameters, remaining components will be zeroed.
/// { x = *(f32*)(&x); y = *(f32*)(&y); z = *(f32*)(&z); w = *(f32*)(&w);
template<uint N>
template<s32 x, s32 y, s32 z, s32 w>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	m_v = sse_constRtp<x, y, z, w>();
}
template<uint N>
template<s32 x, s32 y, s32 z>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	m_v = sse_constRtp<x, y, z, 0>();
}
template<uint N>
template<s32 x, s32 y>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	m_v = sse_constRtp<x, y, 0, 0>();
}
template<uint N>
template<s32 x>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	m_v = sse_constRtp<x, 0, 0, 0>();
}

template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y, f32 z, f32 w)
{
	m_v = _mm_setr_ps(x, y, z, w);
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y, f32 z)
{
	__m128 vx = _mm_set_ss(x);
	__m128 vy = _mm_set_ss(y);
	__m128 vz = _mm_setzero_ps(); vz = _mm_move_ss(vz, _mm_set_ss(z)); // MSVC 12 compiler bug in optimized builds sometimes fails to set y,z,w to 0 with _mm_set_ss
	m_v = _mm_unpacklo_ps(vx, vy);
	m_v = _mm_movelh_ps(m_v, vz);
	
	//	Make sure the w component has been set to 0! Note, FFTL_MATH_ASSERT(getW() == 0.f) didn't work, because it scared the compiler into producing the correct output. Sneaky bastard.
//	FFTL_MATH_ASSERT((_mm_movemask_ps(_mm_cmpeq_ps(m_v, _mm_setzero_ps())) & 8) == 8);
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y)
{
	const __m128 vx = _mm_set_ss(x);
	const __m128 vy = _mm_set_ss(y);
	m_v = _mm_unpacklo_ps(vx, vy);
	FFTL_MATH_ASSERT((_mm_movemask_ps(_mm_cmpeq_ps(m_v, _mm_setzero_ps())) & 12) == 12);
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x)
{
	m_v = _mm_set_ss(x);
}

//
// Getters
//

template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetX() const
{
	return _mm_cvtss_f32(m_v);
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetY() const
{
	return _mm_cvtss_f32(_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE_XYZW(1, 1, 1, 1)));
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetZ() const
{
	return _mm_cvtss_f32(_mm_movehl_ps(m_v, m_v)); //	Sometimes higher throughput instruction which permutes 2,3,2,3
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetW() const
{
	return _mm_cvtss_f32(_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE_XYZW(3, 3, 3, 3)));
}

template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntX() const
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_extract_ps(m_v, 0);
#else
	return _mm_cvtsi128_si32(_mm_castps_si128(m_v));
#endif
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntY() const
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_extract_ps(m_v, 1);
#else
	return _mm_cvtsi128_si32(_mm_shuffle_epi32(_mm_castps_si128(m_v), _MM_SHUFFLE_XYZW(1, 1, 1, 1)));
#endif
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntZ() const
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_extract_ps(m_v, 2);
#else
	return _mm_cvtsi128_si32(_mm_shuffle_epi32(_mm_castps_si128(m_v), _MM_SHUFFLE_XYZW(2, 2, 2, 2)));
#endif
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntW() const
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_extract_ps(m_v, 3);
#else
	return _mm_cvtsi128_si32(_mm_shuffle_epi32(_mm_castps_si128(m_v), _MM_SHUFFLE_XYZW(3, 3, 3, 3)));
#endif
}

//
// Setters
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::InsertX(f32 f) const
{
	return V4fPermute<B0, A1, A2, A3>(m_v, _mm_set_ss(f));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::InsertY(f32 f) const
{
	return V4fPermute<A0, B0, A2, A3>(m_v, _mm_set_ss(f));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::InsertZ(f32 f) const
{
	return V4fPermute<A0, A1, B0, A3>(m_v, _mm_set_ss(f));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::InsertW(f32 f) const
{
	return V4fPermute<A0, A1, A2, B0>(m_v, _mm_set_ss(f));
}

template<uint N>
FFTL_FORCEINLINE void vecT<N>::SetAll(f32 f)
{
	m_v = _mm_set1_ps(f);
}

template<bool bX, bool bY, bool bZ, bool bW, uint M>
FFTL_FORCEINLINE vecT<M> ZeroElements(const vecT<M>& v)
{
	return sse_zero_elements<bX, bY, bZ, bW>(v.m_v);
}

template<uint N>
template<u32 x, u32 y, u32 z, u32 w>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	return sse_constRtp<x, y, z, w>();
}

template<uint N>
template<u32 val>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	return sse_constRtp<val>();
}


//
// Operators
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator-() const
{
	return vecT<N>(sse_negate_ps(m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator+(const vecT<N>& b) const
{
	return vecT<N>(_mm_add_ps(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator+=(const vecT<N>& b)
{
	return *this = *this + b;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator-(const vecT<N>& b) const
{
	return vecT<N>(_mm_sub_ps(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator-=(const vecT<N>& b)
{
	return *this = *this - b;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator*(const vecT<N>& b) const
{
	return vecT<N>(_mm_mul_ps(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator*=(const vecT<N>& b)
{
	m_v = _mm_mul_ps(m_v, b.m_v); return (*this);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator/(const vecT<N>& b) const
{
	return vecT<N>(_mm_div_ps(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator/=(const vecT& b)
{
	m_v = _mm_div_ps(m_v, b.m_v); return (*this);
}

template<uint N>
FFTL_FORCEINLINE bool vecT<N>::operator==(const vecT<N>& b) const
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(_mm_cmpeq_ps(m_v, b.m_v)) & uMask) == uMask;
}
template<uint N>
FFTL_FORCEINLINE bool vecT<N>::operator!=(const vecT<N>& b) const
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(_mm_cmpneq_ps(m_v, b.m_v)) & uMask) != 0;
}
template<>
FFTL_FORCEINLINE bool vecT<4>::operator==(const vecT<4>& b) const
{
	return _mm_movemask_ps(_mm_cmpeq_ps(m_v, b.m_v)) == 15;
}
template<>
FFTL_FORCEINLINE bool vecT<4>::operator!=(const vecT<4>& b) const
{
	return _mm_movemask_ps(_mm_cmpneq_ps(m_v, b.m_v)) != 0;
}
template<>
FFTL_FORCEINLINE bool vecT<1>::operator==(const vecT<1>& b) const
{
	return _mm_comieq_ss(m_v, b.m_v) != 0;
}
template<>
FFTL_FORCEINLINE bool vecT<1>::operator!=(const vecT<1>& b) const
{
	return _mm_comineq_ss(m_v, b.m_v) != 0;
}

//
// Surgical arithmetic affecting only the selected elements
//

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Add(f32 f) const
{
	FFTL_IF_CONSTEXPR (bX | bY | bZ | bW)	// NOP if none are set
	{
		__m128 v = _mm_set_ss(f);
		FFTL_IF_CONSTEXPR (bY | bZ | bW)	// Permute only needed if we need to affect anything aside from X
			v = V4fPermute<!bX, !bY, !bZ, !bW>(v);
		return _mm_add_ps(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Sub(f32 f) const
{
	FFTL_IF_CONSTEXPR (bX | bY | bZ | bW)	// NOP if none are set
	{
		__m128 v = _mm_set_ss(f);
		FFTL_IF_CONSTEXPR (bY | bZ | bW)	// Permute only needed if we need to affect anything aside from X
			v = V4fPermute<!bX, !bY, !bZ, !bW>(v);
		return _mm_sub_ps(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Mul(f32 f) const
{
	FFTL_IF_CONSTEXPR (bX | bY | bZ | bW)	// NOP if none are set
	{
		__m128 v = _mm_set_ss(f);
		FFTL_IF_CONSTEXPR (bY | bZ | bW)	// Permute only needed if we need to affect anything aside from X
			v = V4fPermute<!bX, !bY, !bZ, !bW>(v);
		v = _mm_mul_ps(m_v, v);
		return V4fPermute<bX?B0:A0, bY?B1:A1, bZ?B2:A2, bW?B3:A3>(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Div(f32 f) const
{
	FFTL_IF_CONSTEXPR (bX | bY | bZ | bW)	// NOP if none are set
	{
		__m128 v = _mm_set_ss(f);
		FFTL_IF_CONSTEXPR (bY | bZ | bW)	// Permute only needed if we need to affect anything aside from X
			v = V4fPermute<!bX, !bY, !bZ, !bW>(v);
		v = _mm_div_ps(m_v, v);
		return V4fPermute<bX?B0:A0, bY?B1:A1, bZ?B2:A2, bW?B3:A3>(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Negate() const
{
	FFTL_IF_CONSTEXPR (bX | bY | bZ | bW)	// NOP if none are set
	{
		__m128 v = sse_negate_ps(m_v);
		return V4fPermute<bX?B0:A0, bY?B1:A1, bZ?B2:A2, bW?B3:A3>(m_v, v);
	}
	return m_v;
}


//
//	Requiring no template arguments.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Negate() const
{
	return sse_negate_ps(m_v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::ZeroNonFinite() const
{
	const __m128 vZero = _mm_setzero_ps();
	const __m128 vShouldBeZero = _mm_sub_ps(m_v, m_v);
	const __m128 vMask = _mm_cmpeq_ps(vShouldBeZero, vZero);
	m_v = _mm_and_ps(m_v, vMask);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::ReplaceNonFinite(const vecT& b) const
{
	const __m128 vZero = _mm_setzero_ps();
	const __m128 vShouldBeZero = _mm_sub_ps(m_v, m_v);
	const __m128 vMask = _mm_cmpeq_ps(vShouldBeZero, vZero);
	m_v = sse_blend(b.m_v, m_v, vMask);
}




//
// 2, 3, or 4 goes in as template argument.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::LoadA(const f32* p)
{
	(void)p;
	static_assert(N <= 4 && N >= 1, "Not implemented");
}
template<>
FFTL_FORCEINLINE vecT<4> vecT<4>::LoadA(const f32* p)
{
	return _mm_load_ps(p);
}
template<>
FFTL_FORCEINLINE vecT<3> vecT<3>::LoadA(const f32* p)
{
	return sse_load3(p);
}
template<>
FFTL_FORCEINLINE vecT<2> vecT<2>::LoadA(const f32* p)
{
	return _mm_castpd_ps(_mm_load_sd((double*)p));
}
template<>
FFTL_FORCEINLINE vecT<1> vecT<1>::LoadA(const f32* p)
{
	return _mm_load_ss(p);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::LoadU(const f32* p)
{
	return LoadA(p);
}
template<>
FFTL_FORCEINLINE vecT<4> vecT<4>::LoadU(const f32* p)
{
	return _mm_loadu_ps(p);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Splat(f32 f)
{
	return _mm_set1_ps(f);
}

template<uint N>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<N>& v)
{
	(void)p; (void)v;
	static_assert(N <= 4 && N >= 1, "Not implemented");
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<4>& v)
{
	_mm_store_ps(p, v.m_v);
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<3>& v)
{
	sse_store3(p, v.m_v);
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<2>& v)
{
	_mm_store_sd((double*)p, _mm_castps_pd(v.m_v));
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<1>& v)
{
	_mm_store_ss(p, v.m_v);
}

template<uint N>
FFTL_FORCEINLINE void StoreU(f32* p, const vecT<N>& v)
{
	StoreA(p, v);
}
template<>
FFTL_FORCEINLINE void StoreU(f32* p, const vecT<4>& v)
{
	_mm_storeu_ps(p, v.m_v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::X000(f32 f)
{
	return _mm_set_ss(f);
}

template<uint N>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const __m128 vMask = _mm_cmple_ps(sse_abs_ps(_mm_sub_ps(a.m_v, b.m_v)), tol);
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(vMask) & uMask) == uMask;
}

template<>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<4>& a, const vecT<4>& b, const vecT<4>& tol)
{
	const __m128 vMask = _mm_cmple_ps(sse_abs_ps(_mm_sub_ps(a.m_v, b.m_v)), tol);
	return _mm_movemask_ps(vMask) == 15;
}

template<uint N>
FFTL_FORCEINLINE vecmask IsNearEqualV(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	return _mm_cmple_ps(sse_abs_ps(_mm_sub_ps(a.m_v, b.m_v)), tol);
}

template<uint N>
FFTL_FORCEINLINE bool IsNearZero(const vecT<N>& v, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const __m128 vAbs = sse_abs_ps(v.m_v);
	FFTL_IF_CONSTEXPR (N == 4)
		return _mm_movemask_ps(_mm_cmple_ps(vAbs, tol.m_v)) == 15;
	else
	{
		const uint uMask = (1 << N) - 1;
		return (_mm_movemask_ps(_mm_cmple_ps(vAbs, tol)) & uMask) == uMask;
	}
}

template<uint N>
FFTL_FORCEINLINE vecmask IsNearZeroV(const vecT<N>& v, const vecT<N>& tol)
{
	const __m128 vAbs = sse_abs_ps(v.m_v);
	return _mm_cmple_ps(vAbs, tol);
}

template<uint N>
FFTL_FORCEINLINE bool IsNan(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(_mm_cmpneq_ps(v.m_v, v.m_v)) & uMask) != 0;
}
template<>
FFTL_FORCEINLINE bool IsNan(const vecT<4>& v)
{
	return _mm_movemask_ps(_mm_cmpneq_ps(v.m_v, v.m_v)) != 0;
}
template<>
FFTL_FORCEINLINE bool IsNan(const vecT<1>& v)
{
#if 1//defined(FPFAST)
	return (_mm_movemask_ps(_mm_cmpneq_ss(v.m_v, v.m_v)) & 1) != 0;
#else
	const f32 f = _mm_cvtss_f32(m_v);
	return f != f;
#endif
}
template<uint N>
FFTL_FORCEINLINE vecmask IsNanV(const vecT<N>& v)
{
	return _mm_cmpneq_ps(v.m_v, v.m_v);
}

template<uint N>
FFTL_FORCEINLINE bool IsInf(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const __m128 vZero = _mm_setzero_ps();
	const __m128 vInf = _mm_rcp_ps(vZero);
	const __m128 vMask = _mm_cmpeq_ps(vZero, _mm_xor_ps(v.m_v, vInf));
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(vMask) & uMask) != 0;
}
template<>
FFTL_FORCEINLINE bool IsInf(const vecT<4>& v)
{
	const __m128 vZero = _mm_setzero_ps();
	const __m128 vInf = _mm_rcp_ps(vZero);
	const __m128 vMask = _mm_cmpeq_ps(vZero, _mm_xor_ps(v.m_v, vInf));
	return _mm_movemask_ps(vMask) != 0;
}
template<uint N>
FFTL_FORCEINLINE vecmask IsInfV(const vecT<N>& v)
{
	const __m128 vZero = _mm_setzero_ps();
	const __m128 vInf = _mm_rcp_ps(vZero);
	const __m128 vMask = _mm_cmpeq_ps(vZero, _mm_xor_ps(v.m_v, vInf));
	return vMask;
}
template<>
FFTL_FORCEINLINE bool IsInf(const vecT<1>& v)
{
	const int il = _mm_cvtsi128_si32(_mm_castps_si128(v.m_v));
	return (il & 0x7fffffff) == 0x7f800000;
//	const __m128 vZero = _mm_setzero_ps();
//	const __m128 vInf = _mm_rcp_ps(vZero);
//	const __m128 vMask = _mm_cmpeq_ss(vZero, _mm_xor_ps(m_v, vInf));
//	return (_mm_movemask_ps(vMask) & 1) != 0;
}

template<uint N>
FFTL_FORCEINLINE bool IsFinite(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(_mm_cmpeq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps())) & uMask) == uMask;
}
template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<4>& v)
{
	return _mm_movemask_ps(_mm_cmpeq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps())) == 15;
}
template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<1>& v)
{
//	return (_mm_movemask_ps(_mm_cmpeq_ss(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps())) & 1) != 0;
	const int il = _mm_cvtsi128_si32(_mm_castps_si128(v.m_v));
	return (il & 0x7f800000) != 0x7f800000;
}

template<uint N>
FFTL_FORCEINLINE vecmask IsFiniteV(const vecT<N>& v)
{
	return _mm_cmpeq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps());
}

template<uint N>
FFTL_FORCEINLINE bool IsOutrageous(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const uint uMask = (1 << N) - 1;
	return (_mm_movemask_ps(_mm_cmpneq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps())) & uMask) != 0;
}
template<>
FFTL_FORCEINLINE bool IsOutrageous(const vecT<4>& v)
{
	return _mm_movemask_ps(_mm_cmpneq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps())) != 0;
}
template<uint N>
FFTL_FORCEINLINE vecmask IsOutrageousV(const vecT<N>& v)
{
	return _mm_cmpneq_ps(_mm_sub_ps(v.m_v, v.m_v), _mm_setzero_ps());
}

template<uint N>
FFTL_FORCEINLINE vecT<N> ZeroNegative(const vecT<N>& v)
{
	const __m128 vMask = _mm_cmpge_ps(v.m_v, _mm_setzero_ps());
	return _mm_and_ps(v.m_v, vMask);
}













//
//	vecT<N> functions outside the class.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> Min(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>(_mm_min_ps(a.AsIntrinsic(), b.AsIntrinsic()));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Max(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>(_mm_max_ps(a.AsIntrinsic(), b.AsIntrinsic()));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Sqrt(const vecT<N>& v)
{
	return vecT<N>(_mm_sqrt_ps(v.AsIntrinsic()));
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Abs(const vecT<N>& a)
{
	return sse_abs_ps(a);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> AbsDiff(const vecT<N>& a, const vecT<N>& b)
{
	return sse_abs_ps(_mm_sub_ps(a, b));
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Pow4(const vecT<N>& a, f32 exp)
{
	//	TODO!
	return _mm_setr_ps(powf(a.GetX(), exp), powf(a.GetY(), exp), powf(a.GetZ(), exp), powf(a.GetW(), exp));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Sign(const vecT<N>& a)
{
	__m128 r = a.AsIntrinsic();
	const __m128 vOne = _mm_set1_ps(+1.f);
	const __m128 vMsk = _mm_set1_ps(-0.f);
	const __m128 vCmpNZ = _mm_cmpneq_ps(r, _mm_setzero_ps());	// Get a full mask if anything but zero
	const __m128 vSignBit = _mm_and_ps(r, vMsk);				// Extract the sign bit
	r = _mm_or_ps(vOne, vSignBit);								// Set the sign bit in 1.0 to negate if val was negative
	r = _mm_and_ps(r, vCmpNZ);									// If val was 0, r becomes 0
	return r;
}

template<uint N>
FFTL_FORCEINLINE vecT<N> XXXX(const vecT<N>& a)
{
	return V4fPermute<0,0,0,0>(a);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> YYYY(const vecT<N>& a)
{
	return V4fPermute<1,1,1,1>(a);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> ZZZZ(const vecT<N>& a)
{
	return V4fPermute<2,2,2,2>(a);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> WWWW(const vecT<N>& a)
{
	return V4fPermute<3,3,3,3>(a);
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& v)
{
	return V4fPermute<T0, T1, T2, T3>(v);
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& a, const vecT<N>& b)
{
	return V4fPermute<T0, T1, T2, T3>(a, b);
}





//
//	vecT<N> functions outside the class with template params
//

template<uint N>
FFTL_FORCEINLINE f32 Min(const vecT<N>& v)
{
	(void)v;
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
	r = _mm_min_ps(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_min_ps(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<3>& v)
{
	__m128 r;
	r = _mm_min_ps(v, V4fPermute<2, 3, 2, 3>(v));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_min_ps(r, V4fPermute<1, 0, 0, 0>(v));	//	x+z+y, ....
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<2>& v)
{
	const __m128 r = _mm_min_ss(v, V4fPermute<1, 0, 0, 0>(v));
	return _mm_cvtss_f32(r);
}

template<uint N>
FFTL_FORCEINLINE f32 Max(const vecT<N>& v)
{
	(void)v;
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
	r = _mm_max_ps(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_max_ps(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<3>& v)
{
	__m128 r;
	r = _mm_max_ps(v, V4fPermute<2, 3, 2, 3>(v));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_max_ps(r, V4fPermute<1, 0, 0, 0>(v));	//	x+z+y, ....
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<2>& v)
{
	const __m128 r = _mm_max_ps(v, V4fPermute<1, 0, 0, 0>(v));
	return _mm_cvtss_f32(r);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> MinV(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return v;
}
template<>
FFTL_FORCEINLINE vecT<4> MinV(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
	r = _mm_min_ps(r, V4fPermute<1, 0, 3, 2>(r));
	r = _mm_min_ps(r, V4fPermute<2, 2, 0, 0>(r));
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> MinV(const vecT<3>& v)
{
	__m128 a = v.AsIntrinsic();
	__m128 r;
	r = _mm_min_ps(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_min_ps(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return _mm_shuffle_ps(r, r, 0);
}
template<>
FFTL_FORCEINLINE vecT<2> MinV<2>(const vecT<2>& v)
{
	const __m128 a = v.AsIntrinsic();
	const __m128 x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const __m128 y = V4fPermute<1, 1, 1, 1>(a);
	const __m128 r = _mm_min_ps(x, y);
	return r;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> MaxV(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return v;
}
template<>
FFTL_FORCEINLINE vecT<4> MaxV(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
	r = _mm_max_ps(r, V4fPermute<1, 0, 3, 2>(r));
	r = _mm_max_ps(r, V4fPermute<2, 2, 0, 0>(r));
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> MaxV(const vecT<3>& v)
{
	__m128 a = v.AsIntrinsic();
	__m128 r;
	r = _mm_max_ps(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_max_ps(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return _mm_shuffle_ps(r, r, 0);
}
template<>
FFTL_FORCEINLINE vecT<2> MaxV<2>(const vecT<2>& v)
{
	const __m128 a = v.AsIntrinsic();
	const __m128 x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const __m128 y = V4fPermute<1, 1, 1, 1>(a);
	const __m128 r = _mm_max_ps(x, y);
	return r;
}

template<uint N>
FFTL_FORCEINLINE f32 HSum(const vecT<N>& v)
{
	(void)v;
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
#if defined(FFTL_VECTOR_USE_SSE4)
	r = _mm_hadd_ps(r, r);
	r = _mm_hadd_ps(r, r);
#else
	r = _mm_add_ps(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_add_ps(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
#endif
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<3>& v)
{
	__m128 r;
	r = _mm_add_ps(v, V4fPermute<2, 3, 2, 3>(v));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_add_ps(r, V4fPermute<1, 0, 0, 0>(v));	//	x+z+y, ....
	return _mm_cvtss_f32(r);
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<2>& v)
{
	const __m128 r = _mm_add_ps(v, V4fPermute<1, 0, 0, 0>(v));
	return _mm_cvtss_f32(r);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> HSumV(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return v;
}
template<>
FFTL_FORCEINLINE vecT<4> HSumV(const vecT<4>& v)
{
	__m128 r = v.AsIntrinsic();
#if defined(FFTL_VECTOR_USE_SSE4)
	r = _mm_hadd_ps(r, r);
	r = _mm_hadd_ps(r, r);
#else
	r = _mm_add_ps(r, V4fPermute<1, 0, 3, 2>(r));
	r = _mm_add_ps(r, V4fPermute<2, 2, 0, 0>(r));
#endif
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> HSumV(const vecT<3>& v)
{
	__m128 a = v.AsIntrinsic();
	__m128 r;
	r = _mm_add_ps(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = _mm_add_ps(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return _mm_shuffle_ps(r, r, 0);
}
template<>
FFTL_FORCEINLINE vecT<2> HSumV<2>(const vecT<2>& v)
{
	const __m128 a = v.AsIntrinsic();
	const __m128 x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const __m128 y = V4fPermute<1, 1, 1, 1>(a);
	const __m128 r = _mm_add_ps(x, y);
	return r;
}

template<uint N>
FFTL_FORCEINLINE f32 Dot(const vecT<N>& a, const vecT<N>& b)
{
#if defined(FFTL_VECTOR_USE_SSE4)
	const __m128 r = _mm_dp_ps(a, b, _MM_DPPS_MASK_HELPER((1<<N)-1, 1));
	return _mm_cvtss_f32(r);
#else
	return HSum(a * b);
#endif
}

template<uint N>
FFTL_FORCEINLINE vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_dp_ps(a, b, _MM_DPPS_MASK_HELPER((1<<N)-1, 15));
#else
	return HSumV(a * b);
#endif
}

template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
#if defined(FFTL_VECTOR_USE_SSE4)
	return _mm_dp_ps(a, b, _MM_DPPS_MASK_HELPER((1 << N) - 1, (1 << R) - 1));
#else
	FFTL_IF_CONSTEXPR (R == 4)
		return HSumV(a * b);
	else
	{
		const int ix = R <= 0;
		const int iy = R <= 1;
		const int iz = R <= 2;
		const int iw = R <= 3;
		return V4fPermute<ix, iy, iz, iw>( _mm_set_ss(HSum(a * b)) );
	}
#endif
}


//	Utility functions
template<bool bX, bool bY, bool bZ, bool bW, uint N>
FFTL_FORCEINLINE vecT<N> Blend(const vecT<N>& a, const vecT<N>& b)
{
	return sse_blend<bX, bY, bZ, bW>(a, b);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Blend(const vecT<N>& a, const vecT<N>& b, const vecmask& msk)
{
	return sse_blend(a, b, msk);
}







inline constexpr vecmask::vecmask(const vecmask& v)
	: m_v(v.m_v)
{
}

FFTL_FORCEINLINE vecmask& vecmask::operator=(const vecmask& v)
{
	m_v = v.m_v;
	return *this;
}

inline constexpr vecmask::vecmask(const __m128& v)
	: m_v(v)
{
}

FFTL_FORCEINLINE vecmask& vecmask::operator=(const __m128& v)
{
	m_v = v;
	return *this;
}

inline constexpr vecmask::operator const __m128&() const
{
	return m_v;
}

FFTL_FORCEINLINE vecmask::operator __m128&()
{
	return m_v;
}

FFTL_FORCEINLINE vecmask vecmask::operator|(const vecmask& b) const
{
	return vecmask(_mm_or_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE vecmask vecmask::operator&(const vecmask& b) const
{
	return vecmask(_mm_and_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE vecmask vecmask::operator^(const vecmask& b) const
{
	return vecmask(_mm_xor_ps(m_v, b.m_v));
}

template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator|(const vecT<N>& b) const
{
	return vecT<N>(_mm_or_ps(m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator&(const vecT<N>& b) const
{
	return vecT<N>(_mm_and_ps(m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator^(const vecT<N>& b) const
{
	return vecT<N>(_mm_xor_ps(m_v, b.m_v));
}

template<uint N> FFTL_FORCEINLINE vecT<N> AndNot(const vecmask& a, const vecT<N>& b)
{
	return vecT<N>(_mm_andnot_ps(b.m_v, a.m_v));
}
template<uint N> FFTL_FORCEINLINE vecT<N> AndNot(const vecT<N>& a, const vecmask& b)
{
	return vecT<N>(_mm_andnot_ps(b.m_v, a.m_v));
}
FFTL_FORCEINLINE vecmask AndNot(const vecmask& a, const vecmask& b)
{
	return vecmask(_mm_andnot_ps(b.m_v, a.m_v));
}

template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator|(const vecmask& b) const
{
	return vecT<N>(_mm_or_ps(m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator&(const vecmask& b) const
{
	return vecT<N>(_mm_and_ps(m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator^(const vecmask& b) const
{
	return vecT<N>(_mm_xor_ps(m_v, b.m_v));
}

template<uint N> FFTL_FORCEINLINE vecmask CmpEq(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmpeq_ps(a.m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecmask CmpNe(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmpneq_ps(a.m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecmask CmpLt(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmplt_ps(a.m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecmask CmpLe(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmple_ps(a.m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecmask CmpGt(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmpgt_ps(a.m_v, b.m_v));
}
template<uint N> FFTL_FORCEINLINE vecmask CmpGe(const vecT<N>& a, const vecT<N>& b)
{
	return vecmask(_mm_cmpge_ps(a.m_v, b.m_v));
}

FFTL_FORCEINLINE int vecmask::ToIntMask() const
{
	return _mm_movemask_ps(m_v);
}

template<s32 x, s32 y, s32 z, s32 w>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromInts()
{
	return _mm_castsi128_ps( _mm_setr_epi32(x, y, z, w) );
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromBools()
{
	const int ix = bX ? 0 : 1;
	const int iy = bY ? 0 : 1;
	const int iz = bZ ? 0 : 2;
	const int iw = bW ? 0 : 3;

	__m128 a = _mm_setzero_ps();
	a = _mm_cmpeq_ss(a, a);
	a = V4fPermute<ix, iy, iz, iw>(a);
	return a;
}

template<>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromBools<0, 0, 0, 0>()
{
	return _mm_setzero_ps();
}

template<>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromBools<1, 1, 1, 1>()
{
	__m128 a = _mm_setzero_ps();
	a = _mm_cmpeq_ps(a, a);
	return a;
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecmask vecmask::PropagateInt(int i)
{
	const int ix = bX ? 0 : 2;
	const int iy = bY ? 0 : 2;
	const int iz = bZ ? 0 : 2;
	const int iw = bW ? 0 : 2;

	__m128i a = _mm_cvtsi32_si128(i);
	a = _mm_shuffle_epi32(a, _MM_SHUFFLE_XYZW(ix, iy, iz, iw));
	return _mm_castsi128_ps(a);
}
template<>
FFTL_FORCEINLINE vecmask vecmask::PropagateInt<1, 0, 0, 0>(int i)
{
	__m128i a = _mm_cvtsi32_si128(i);
	return _mm_castsi128_ps(a);
}
template<>
FFTL_FORCEINLINE vecmask vecmask::PropagateInt<0, 0, 0, 0>(int)
{
	return _mm_setzero_ps();
}






} // namespace FFTL




#endif // defined(FFTL_SSE)
