#ifndef VECTOR_NEON_INL
#define VECTOR_NEON_INL
#else
#error "Already included"
#endif // !VECTOR_NEON_INL


#if defined(FFTL_ARM_NEON)

namespace FFTL
{




template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT()
#if defined(FFTL_ENABLE_ASSERT)
//	Fill with invalid stuff that will be more obvious if used accidentally
	: f32x4( vreinterpretq_f32_u32( vdupq_n_u32(0xffffffff) ) )
#endif
{
}

template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x, f32 y, f32 z)
{
	float32x4_t v = vdupq_n_f32(0);
	v = vsetq_lane_f32(x, v, 0);
	v = vsetq_lane_f32(y, v, 1);
	v = vsetq_lane_f32(z, v, 2);
	m_v = v;
}
template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x, f32 y)
{
	float32x2_t v;
	v = vset_lane_f32(x, v, 0);
	v = vset_lane_f32(y, v, 1);
	m_v = vcombine_f32(v, vdup_n_f32(0));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(f32 x)
: f32x4(vsetq_lane_f32(x, vdupq_n_f32(0), 0))
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
	float32x2_t a = vcreate_f32( static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32) );
	float32x2_t b = vcreate_f32( static_cast<uint64_t>(z) | (static_cast<uint64_t>(w) << 32) );
	m_v = vcombine_f32(a, b);
}
template<uint N>
template<s32 x, s32 y, s32 z>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	float32x2_t a = vcreate_f32( static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32) );
	float32x2_t b = vcreate_f32( static_cast<uint64_t>(z) );
	m_v = vcombine_f32(a, b);
}
template<uint N>
template<s32 x, s32 y>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	float32x2_t a = vcreate_f32( static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32) );
	m_v = vcombine_f32(a, vdup_n_f32(0));
}
template<uint N>
template<s32 x>
FFTL_FORCEINLINE void vecT<N>::Set()
{
	float32x2_t a = vcreate_f32( static_cast<uint64_t>(x) );
	m_v = vcombine_f32(a, vdup_n_f32(0));
}

template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y, f32 z, f32 w)
{
	const float32x4_t v = { x, y, z, w };
//	v = vsetq_lane_f32(x, v, 0);
//	v = vsetq_lane_f32(y, v, 1);
//	v = vsetq_lane_f32(z, v, 2);
//	v = vsetq_lane_f32(w, v, 3);
	m_v = v;
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y, f32 z)
{
	float32x4_t v = vdupq_n_f32(0);
	v = vsetq_lane_f32(x, v, 0);
	v = vsetq_lane_f32(y, v, 1);
	v = vsetq_lane_f32(z, v, 2);
	m_v = v;
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x, f32 y)
{
	float32x2_t v;
	v = vset_lane_f32(x, v, 0);
	v = vset_lane_f32(y, v, 1);
	m_v = vcombine_f32(v, vdup_n_f32(0));
}
template<uint N>
FFTL_FORCEINLINE void vecT<N>::Set(f32 x)
{
	m_v = vsetq_lane_f32(x, vdupq_n_f32(0), 0);
}


//
// Getters
//

template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetX() const
{
	return vgetq_lane_f32(m_v, 0);
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetY() const
{
	return vgetq_lane_f32(m_v, 1);
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetZ() const
{
	return vgetq_lane_f32(m_v, 2);
}
template<uint N>
FFTL_FORCEINLINE f32 vecT<N>::GetW() const
{
	return vgetq_lane_f32(m_v, 3);
}

template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntX() const
{
	vgetq_lane_u32(vreinterpretq_u32_f32(m_v), 0);
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntY() const
{
	vgetq_lane_u32(vreinterpretq_u32_f32(m_v), 1);
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntZ() const
{
	vgetq_lane_u32(vreinterpretq_u32_f32(m_v), 2);
}
template<uint N>
FFTL_FORCEINLINE int vecT<N>::GetAsIntW() const
{
	vgetq_lane_u32(vreinterpretq_u32_f32(m_v), 3);
}

//
// Setters
//

template<uint N>
FFTL_FORCEINLINE vecT<N> InsertX(const vecT<N>& v, f32 f)
{
	return vsetq_lane_f32(f, v.m_v, 0);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> InsertY(const vecT<N>& v, f32 f)
{
	return vsetq_lane_f32(f, v.m_v, 1);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> InsertZ(const vecT<N>& v, f32 f)
{
	return vsetq_lane_f32(f, v.m_v, 2);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> InsertW(const vecT<N>& v, f32 f)
{
	return vsetq_lane_f32(f, v.m_v, 3);
}

template<uint N>
FFTL_FORCEINLINE void vecT<N>::SetAll(f32 f)
{
	m_v = vdupq_n_f32(f);
}

template<bool bX, bool bY, bool bZ, bool bW, uint M>
FFTL_FORCEINLINE vecT<M> ZeroElements(const vecT<M>& v)
{
#if 0 && defined(__clang__)
	constexpr int i0 = bX ? 4 : 0;
	constexpr int i1 = bY ? 5 : 1;
	constexpr int i2 = bZ ? 6 : 2;
	constexpr int i3 = bW ? 7 : 3;

	return __builtin_shufflevector(v.m_v, vdupq_n_f32(0), i0, i1, i2, i3);
#else
	if constexpr (bX)
		return vsetq_lane_f32(0, v.m_v, 0);
	if constexpr (bY)
		return vsetq_lane_f32(0, v.m_v, 1);
	if constexpr (bZ)
		return vsetq_lane_f32(0, v.m_v, 2);
	if constexpr (bW)
		return vsetq_lane_f32(0, v.m_v, 3);
#endif
}

template<uint N>
template<u32 x, u32 y, u32 z, u32 w>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	float32x2_t a = vcreate_f32(static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32));
	float32x2_t b = vcreate_f32(static_cast<uint64_t>(z) | (static_cast<uint64_t>(w) << 32));
	return vcombine_f32(a, b);
}

template<uint N>
template<u32 val>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	return vreinterpretq_f32_s32(vdupq_n_s32(val));
}


//
// Operators
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator-() const
{
	return vecT<N>(vnegq_f32(m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator+(const vecT<N>& b) const
{
	return vecT<N>(vaddq_f32(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator+=(const vecT<N>& b)
{
	return *this = *this + b;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator-(const vecT<N>& b) const
{
	return vecT<N>(vsubq_f32(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator-=(const vecT<N>& b)
{
	return *this = *this - b;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator*(const vecT<N>& b) const
{
	return vecT<N>(vmulq_f32(m_v, b.m_v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator*=(const vecT<N>& b)
{
	m_v = vmulq_f32(m_v, b.m_v); return (*this);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator/(const vecT<N>& b) const
{
	return vecT<N>(neon_divide_ps(m_v, b));
}
template<uint N>
FFTL_FORCEINLINE vecT<N>& vecT<N>::operator/=(const vecT& b)
{
	m_v = neon_divide_ps(m_v, b);
	return *this;
}

template<>
FFTL_FORCEINLINE bool vecT<4>::operator==(const vecT<4>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool vecT<3>::operator==(const vecT<3>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool vecT<2>::operator==(const vecT<2>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 == 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool vecT<1>::operator==(const vecT<1>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 == 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool vecT<4>::operator!=(const vecT<4>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool vecT<3>::operator!=(const vecT<3>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 != 0xffffffffffffffff) | (c1 != 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool vecT<2>::operator!=(const vecT<2>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 != 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool vecT<1>::operator!=(const vecT<1>& b) const
{
	uint32x4_t cmp = vceqq_f32(m_v, b);
	uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 == 0);
}

//
// Surgical arithmetic affecting only the selected elements
//

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Add(f32 f) const
{
	if constexpr (bX | bY | bZ | bW)	// NOP if none are set
	{
		float32x4_t v = vdupq_n_f32(f);
		if constexpr (!bX || !bY || !bZ || !bW)	// Permute only needed if we don't do the whole row
			v = __builtin_shufflevector(v, vdupq_n_f32(0), !!bX, !!bY, !!bZ, !!bW);
		return vaddq_f32(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Sub(f32 f) const
{
	if constexpr (bX | bY | bZ | bW)	// NOP if none are set
	{
		float32x4_t v = vdupq_n_f32(f);
		if constexpr (!bX || !bY || !bZ || !bW)	// Permute only needed if we don't do the whole row
			v = __builtin_shufflevector(v, vdupq_n_f32(0), !!bX, !!bY, !!bZ, !!bW);
		return vsubq_f32(m_v, v);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Mul(f32 f) const
{
	if constexpr (bX | bY | bZ | bW)	// NOP if none are set
	{
		float32x4_t v = vdupq_n_f32(f);
		if constexpr (!bX || !bY || !bZ || !bW)	// Permute only needed if we don't do the whole row
			v = __builtin_shufflevector(v, vdupq_n_f32(0), !!bX, !!bY, !!bZ, !!bW);
		v = vmulq_f32(m_v, v);
		return __builtin_shufflevector(m_v, v, bX ? B0 : A0, bY ? B1 : A1, bZ ? B2 : A2, bW ? B3 : A3);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Div(f32 f) const
{
	if constexpr (bX | bY | bZ | bW)	// NOP if none are set
	{
		float32x4_t v = vdupq_n_f32(f);
		if constexpr (!bX || !bY || !bZ || !bW)	// Permute only needed if we don't do the whole row
			v = __builtin_shufflevector(v, vdupq_n_f32(0), !!bX, !!bY, !!bZ, !!bW);
		v = neon_divide_ps(m_v, v);
		return __builtin_shufflevector(m_v, v, bX ? B0 : A0, bY ? B1 : A1, bZ ? B2 : A2, bW ? B3 : A3);
	}
	return m_v;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecT<N> vecT<N>::Negate() const
{
	if constexpr (bX | bY | bZ | bW)	// NOP if none are set
	{
		float32x4_t v = vnegq_f32(m_v);
		return __builtin_shufflevector(m_v, v, bX ? B0 : A0, bY ? B1 : A1, bZ ? B2 : A2, bW ? B3 : A3);
	}
	return m_v;
}


//
//	Requiring no template arguments.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Negate() const
{
	return vnegq_f32(m_v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::ZeroNonFinite() const
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vShouldBeZero = vsubq_f32(m_v, m_v);
	const uint32x4_t vMask = vceqq_f32(vShouldBeZero, vZero);
	return vreinterpretq_u32_f32(vandq_s32(vreinterpretq_u32_f32(m_v), vMask));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::ReplaceNonFinite(const vecT& a) const
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vShouldBeZero = vsubq_f32(m_v, m_v);
	const uint32x4_t vMask = vceqq_f32(vShouldBeZero, vZero);
	return neon_blend(vMask, m_v, a.m_v);
}




//
// 2, 3, or 4 goes in as template argument.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::LoadA(const f32* p)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
}
template<>
FFTL_FORCEINLINE vecT<4> vecT<4>::LoadA(const f32* p)
{
	return vld1q_f32(p);
}
template<>
FFTL_FORCEINLINE vecT<3> vecT<3>::LoadA(const f32* p)
{
	float32x2_t v0 = vld1_f32(p);
	float32x2_t v1 = vdup_n_f32(0);
	v1 = vld1_lane_f32(p+2, v1, 0);
	return vcombine_f32(v0, v1);
}
template<>
FFTL_FORCEINLINE vecT<2> vecT<2>::LoadA(const f32* p)
{
	return vcombine_f32(vld1_f32(p), vdup_n_f32(0));
}
template<>
FFTL_FORCEINLINE vecT<1> vecT<1>::LoadA(const f32* p)
{
	const Vec4f v = vdupq_n_f32(0);
	return vld1q_lane_f32(p, v, 0);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::LoadU(const f32* p)
{
	return LoadA(p);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Splat(f32 f)
{
	return vdupq_n_f32(f);
}

template<uint N>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<4>& v)
{
	vst1q_f32(p, v.m_v);
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<3>& v)
{
	vst1_f32(p, vget_low_f32(v.m_v));
	vst1q_lane_f32(p+2, v.m_v, 2);
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<2>& v)
{
	vst1_f32(p, vget_low_f32(v.m_v));
}
template<>
FFTL_FORCEINLINE void StoreA(f32* p, const vecT<1>& v)
{
	vst1q_lane_f32(p+0, v.m_v, 0);
}

template<uint N>
FFTL_FORCEINLINE void StoreU(f32* p, const vecT<N>& v)
{
	StoreA(p, v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::X000(f32 f)
{
	return vsetq_lane_f32(f, vdupq_n_f32(0), 0);
}

template<>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<4>& a, const vecT<4>& b, const vecT<4>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(vsubq_f32(a.m_v, b.m_v)), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<3>& a, const vecT<3>& b, const vecT<3>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(vsubq_f32(a.m_v, b.m_v)), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<2>& a, const vecT<2>& b, const vecT<2>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(vsubq_f32(a.m_v, b.m_v)), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 == 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<1>& a, const vecT<1>& b, const vecT<1>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(vsubq_f32(a.m_v, b.m_v)), tol.m_v);
	uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 == 0xffffffff);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsNearEqualV(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	return vcleq_f32(vabsq_f32(vsubq_f32(a.m_v, b.m_v)), tol);
}

template<>
FFTL_FORCEINLINE bool IsNearZero(const vecT<4>& a, const vecT<4>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(a.m_v), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool IsNearZero(const vecT<3>& a, const vecT<3>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(a.m_v), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNearZero(const vecT<2>& a, const vecT<2>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(a.m_v), tol.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 == 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNearZero(const vecT<1>& a, const vecT<1>& tol)
{
	uint32x4_t cmp = vcleq_f32(vabsq_f32(a.m_v), tol.m_v);
	uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 == 0xffffffff);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsNearZeroV(const vecT<N>& a, const vecT<N>& tol)
{
	const float32x4_t vAbs = vabsq_f32(a.m_v);
	return vcleq_f32(vAbs, tol);
}

template<>
FFTL_FORCEINLINE bool IsNan(const vecT<4>& v)
{
	uint32x4_t cmp = vceqq_f32(v.m_v, v.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool IsNan(const vecT<3>& v)
{
	uint32x4_t cmp = vceqq_f32(v.m_v, v.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 != 0xffffffffffffffff) | (c1 != 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNan(const vecT<1>& v)
{
	uint32x4_t cmp = vceqq_f32(v.m_v, v.m_v);
	uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 != 0xffffffffffffffff);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsNanV(const vecT<N>& v)
{
	return vmvnq_u32( vceqq_f32(v.m_v, v.m_v) );
}

template<>
FFTL_FORCEINLINE bool IsInf(const vecT<4>& v)
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vInf = vrecpeq_f32(vZero);
	const uint32x4_t cmp = vceqq_f32(vZero, veorq_s32(vreinterpretq_u32_f32(v.m_v), vreinterpretq_u32_f32(vInf)));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 | c1) != 0;
}
template<>
FFTL_FORCEINLINE bool IsInf(const vecT<3>& v)
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vInf = vrecpeq_f32(vZero);
	const uint32x4_t cmp = vceqq_f32(vZero, veorq_s32(vreinterpretq_u32_f32(v.m_v), vreinterpretq_u32_f32(vInf)));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 != 0) | (c1 != 0);
}
template<>
FFTL_FORCEINLINE bool IsInf(const vecT<2>& v)
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vInf = vrecpeq_f32(vZero);
	const uint32x4_t cmp = vceqq_f32(vZero, veorq_s32(vreinterpretq_u32_f32(v.m_v), vreinterpretq_u32_f32(vInf)));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 != 0);
}
template<>
FFTL_FORCEINLINE bool IsInf(const vecT<1>& v)
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vInf = vrecpeq_f32(vZero);
	const uint32x4_t cmp = vceqq_f32(vZero, veorq_s32(vreinterpretq_u32_f32(v.m_v), vreinterpretq_u32_f32(vInf)));
	const uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 != 0);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsInfV(const vecT<N>& v)
{
	const float32x4_t vZero = vdupq_n_f32(0);
	const float32x4_t vInf = vrecpeq_f32(vZero);
	const uint32x4_t vMask = vceqq_f32(vZero, veorq_s32(vreinterpretq_u32_f32(v.m_v), vreinterpretq_u32_f32(vInf)));
	return vMask;
}

template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<4>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) == 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<3>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 == 0xffffffffffffffff) & (c1 == 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<2>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 == 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool IsFinite(const vecT<1>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 == 0xffffffff);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsFiniteV(const vecT<N>& v)
{
	return vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
}

template<>
FFTL_FORCEINLINE bool IsNonFinite(const vecT<4>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint64_t c1 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 1);
	return (c0 & c1) != 0xffffffffffffffff;
}
template<>
FFTL_FORCEINLINE bool IsNonFinite(const vecT<3>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	const uint32_t c1 = vgetq_lane_u32(cmp, 2);
	return (c0 != 0xffffffffffffffff) | (c1 != 0xffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNonFinite(const vecT<2>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint64_t c0 = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
	return (c0 != 0xffffffffffffffff);
}
template<>
FFTL_FORCEINLINE bool IsNonFinite(const vecT<1>& v)
{
	const uint32x4_t cmp = vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0));
	const uint32_t c0 = vgetq_lane_u32(cmp, 0);
	return (c0 != 0xffffffff);
}

template<uint N>
FFTL_FORCEINLINE mask32x4 IsNonFiniteV(const vecT<N>& v)
{
	return vmvnq_s32( vceqq_f32(vsubq_f32(v.m_v, v.m_v), vdupq_n_f32(0)) );
}

template<uint N>
FFTL_FORCEINLINE vecT<N> ZeroNegative(const vecT<N>& v)
{
	const uint32x4_t vMask = vcgeq_f32(v.m_v, vdupq_n_f32(0));
	return vreinterpretq_f32_u32( vandq_s32(v.m_v, vMask) );
}












//
//	vecT<N> functions outside the class.
//

template<uint N>
FFTL_FORCEINLINE vecT<N> Min(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>(vminq_f32(a.AsIntrinsic(), b.AsIntrinsic()));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Max(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>(vmaxq_f32(a.AsIntrinsic(), b.AsIntrinsic()));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Sqrt(const vecT<N>& v)
{
#if defined(FFTL_64BIT)
	return vecT<N>(vsqrtq_f32(v.m_v));
#else
	Vec4f r = vrsqrteq_f32(v);
	return vecT<N>(vmulq_f32(r, v.AsIntrinsic()));
#endif
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Abs(const vecT<N>& a)
{
	return vabsq_f32(a.m_v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> AbsDiff(const vecT<N>& a, const vecT<N>& b)
{
	return vabdq_f32(a.m_v, b.m_v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Pow4(const vecT<N>& a, f32 exp)
{
	//	TODO!
	return vecT<N>(powf(a.GetX(), exp), powf(a.GetY(), exp), powf(a.GetZ(), exp), powf(a.GetW(), exp));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Sign(const vecT<N>& a)
{
	float32x4_t v = a.AsIntrinsic();
	const uint32x4_t vOne = vdupq_n_u32(0x3f800000);
	const uint32x4_t vMsk = vdupq_n_f32(0x80000000);
	const uint32x4_t vCmpNZ = vceqq_f32(v, vdupq_n_f32(0));	// Get a full mask if anything but zero
	const uint32x4_t vSignBit = vandq_s32(vreinterpretq_u32_f32(v), vMsk);			// Extract the sign bit
	uint32x4_t r = vorrq_u32(vOne, vSignBit);				// Set the sign bit in 1.0 to negate if val was negative
	r = vbicq_u32(r, vCmpNZ);								// If val was 0, r becomes 0
	return vreinterpretq_f32_u32(r);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> XXXX(const vecT<N>& a)
{
	return V4fPermute<0,0,0,0>(a.m_v);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> YYYY(const vecT<N>& a)
{
	return V4fPermute<1,1,1,1>(a.m_v);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> ZZZZ(const vecT<N>& a)
{
	return V4fPermute<2,2,2,2>(a.m_v);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> WWWW(const vecT<N>& a)
{
	return V4fPermute<3,3,3,3>(a.m_v);
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& v)
{
	return V4fPermute<T0, T1, T2, T3>(v.m_v);
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& a, const vecT<N>& b)
{
	return V4fPermute<T0, T1, T2, T3>(a.m_v, b.m_v);
}





//
//	vecT<N> functions outside the class with template params
//

template<uint N>
FFTL_FORCEINLINE f32 Min(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<4>& v)
{
	float32x4_t r = v.AsIntrinsic();
	r = vminq_f32(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = vminq_f32(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
	return vgetq_lane_f32(r, 0);
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<3>& v)
{
	float32x4_t r;
	r = vminq_f32(v, V4fPermute<2, 3, 2, 3>(v));	//	x+z,	y+w,	z+z,	w+z
	r = vminq_f32(r, V4fPermute<1, 0, 0, 0>(v));	//	x+z+y, ....
	return vgetq_lane_f32(r, 0);
}
template<>
FFTL_FORCEINLINE f32 Min(const vecT<2>& v)
{
	return Min( vgetq_lane_f32(v, 0), vgetq_lane_f32(v, 1) );
}

template<uint N>
FFTL_FORCEINLINE f32 Max(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<4>& v)
{
	float32x4_t r = v.AsIntrinsic();
	r = vmaxq_f32(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = vmaxq_f32(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
	return vgetq_lane_f32(r, 0);
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<3>& v)
{
	float32x4_t r;
	r = vmaxq_f32(v, V4fPermute<2, 3, 2, 3>(v.m_v));	//	x+z,	y+w,	z+z,	w+z
	r = vmaxq_f32(r, V4fPermute<1, 0, 0, 0>(v.m_v));	//	x+z+y, ....
	return vgetq_lane_f32(r, 0);
}
template<>
FFTL_FORCEINLINE f32 Max(const vecT<2>& v)
{
	return Max(vgetq_lane_f32(v, 0), vgetq_lane_f32(v, 1));
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
	float32x4_t r = v.AsIntrinsic();
	r = vminq_f32(r, V4fPermute<1, 0, 3, 2>(r));
	r = vminq_f32(r, V4fPermute<2, 2, 0, 0>(r));
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> MinV(const vecT<3>& v)
{
	float32x4_t a = v.AsIntrinsic();
	float32x4_t r;
	r = vminq_f32(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = vminq_f32(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return V4fPermute<0, 0, 0, 0>(r);
}
template<>
FFTL_FORCEINLINE vecT<2> MinV<2>(const vecT<2>& v)
{
	const float32x4_t a = v.AsIntrinsic();
	const float32x4_t x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const float32x4_t y = V4fPermute<1, 1, 1, 1>(a);
	const float32x4_t r = vminq_f32(x, y);
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
	float32x4_t r = v.AsIntrinsic();
	r = vmaxq_f32(r, V4fPermute<1, 0, 3, 2>(r));
	r = vmaxq_f32(r, V4fPermute<2, 2, 0, 0>(r));
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> MaxV(const vecT<3>& v)
{
	float32x4_t a = v.AsIntrinsic();
	float32x4_t r;
	r = vmaxq_f32(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = vmaxq_f32(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return V4fPermute<0, 0, 0, 0>(r);
}
template<>
FFTL_FORCEINLINE vecT<2> MaxV<2>(const vecT<2>& v)
{
	const float32x4_t a = v.AsIntrinsic();
	const float32x4_t x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const float32x4_t y = V4fPermute<1, 1, 1, 1>(a);
	const float32x4_t r = vmaxq_f32(x, y);
	return r;
}

template<uint N>
FFTL_FORCEINLINE f32 HSum(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	return 0;
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<4>& v)
{
	float32x4_t r = v.AsIntrinsic();
#if 1
	float32x2_t a = vadd_f32(vget_high_f32(r), vget_low_f32(r));
	return vget_lane_f32(vpadd_f32(a, a), 0);
#else
	r = vaddq_f32(r, V4fPermute<2, 3, 2, 3>(r));	//	x+z,	y+w,	z+z,	w+z
	r = vaddq_f32(r, V4fPermute<1, 0, 0, 0>(r));	//	x+z+y+w, ....
	return vgetq_lane_f32(r, 0);
#endif
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<3>& v)
{
	float32x4_t r;
	r = vaddq_f32(v, V4fPermute<2, 3, 2, 3>(v));	//	x+z,	y+w,	z+z,	w+z
	r = vaddq_f32(r, V4fPermute<1, 0, 0, 0>(v));	//	x+z+y, ....
	return vgetq_lane_f32(r, 0);
}
template<>
FFTL_FORCEINLINE f32 HSum(const vecT<2>& v)
{
	const float32x4_t r = vaddq_f32(v, V4fPermute<1, 0, 0, 0>(v));
	return vgetq_lane_f32(r, 0);
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
	float32x4_t r = v.AsIntrinsic();
#if 0//defined(FFTL_VECTOR_USE_SSE4)
	r = _mm_hadd_ps(r, r);
	r = _mm_hadd_ps(r, r);
#else
	r = vaddq_f32(r, V4fPermute<1, 0, 3, 2>(r));
	r = vaddq_f32(r, V4fPermute<2, 2, 0, 0>(r));
#endif
	return r;
}
template<>
FFTL_FORCEINLINE vecT<3> HSumV(const vecT<3>& v)
{
	float32x4_t a = v.AsIntrinsic();
	float32x4_t r;
	r = vaddq_f32(a, V4fPermute<2, 3, 2, 3>(a));	//	x+z,	y+w,	z+z,	w+z
	r = vaddq_f32(r, V4fPermute<1, 0, 0, 0>(a));	//	x+z+y, ....
	return V4fPermute<0, 0, 0, 0>(r);
}
template<>
FFTL_FORCEINLINE vecT<2> HSumV<2>(const vecT<2>& v)
{
	const float32x4_t a = v.AsIntrinsic();
	const float32x4_t x = V4fPermute<0, 0, 0, 0>(a);	// Depending on the CPU, these 2 shuffles can often happen in parallel because they are independent.
	const float32x4_t y = V4fPermute<1, 1, 1, 1>(a);
	const float32x4_t r = vaddq_f32(x, y);
	return r;
}

template<uint N>
FFTL_FORCEINLINE f32 Dot(const vecT<N>& a, const vecT<N>& b)
{
	return HSum(a * b);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
	return HSumV(a * b);
}

template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
	return HSumV(a * b);
}



} // namespace FFTL




#endif // defined(FFTL_SSE)
