#ifndef VECTOR_DEFAULT_INL
#define VECTOR_DEFAULT_INL
#else
#error "Already included"
#endif // !VECTOR_DEFAULT_INL

namespace FFTL
{

template<uint N>
inline vecT<N>::vecT()
	: f32_4()
{
#if defined(FFTL_ENABLE_ASSERT)
	//	Fill with invalid stuff that will be more obvious if used accidentally
	const f32 f = ReinterpretAs<f32>(0xffffffff);
	m_v[0] = f;
	m_v[1] = f;
	m_v[2] = f;
	m_v[3] = f;
#endif
}

template<uint N>
constexpr vecT<N>::vecT(const vecT<N>& v)
	: f32_4(v)
{
}

template<uint N>
constexpr vecT<N>::vecT(const f32_4& v)
	: f32_4(v)
{
}

template<uint N>
inline vecT<N>& vecT<N>::operator=(const vecT<N>& v)
{
	m_v[0] = v.m_v[0];
	m_v[1] = v.m_v[1];
	m_v[2] = v.m_v[2];
	m_v[3] = v.m_v[3];
	return *this;
}

template<uint N>
inline vecT<N>::vecT(f32 x, f32 y, f32 z)
{
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = z;
	m_v[3] = 0;
}
template<uint N>
inline vecT<N>::vecT(f32 x, f32 y)
{
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = 0;
	m_v[3] = 0;
}
template<uint N>
inline vecT<N>::vecT(f32 x)
{
	m_v[0] = x;
	m_v[1] = 0;
	m_v[2] = 0;
	m_v[3] = 0;
}

///	Use these template sets when we know the input values at compile time to eliminate unnecessary code generation.
/// Input integer bits will be reinterpreted as floats.
///	If less than 4 parameters, remaining components will be zeroed.
/// { x = *(f32*)(&x); y = *(f32*)(&y); z = *(f32*)(&z); w = *(f32*)(&w);
template<uint N>
template<s32 x, s32 y, s32 z, s32 w>
inline void vecT<N>::Set()
{
	const s32 i[] = { x, y, z, w };
	m_v[0] = *reinterpret_cast<const f32*>(i+0);
	m_v[1] = *reinterpret_cast<const f32*>(i+1);
	m_v[2] = *reinterpret_cast<const f32*>(i+2);
	m_v[3] = *reinterpret_cast<const f32*>(i+3);
}
template<uint N>
template<s32 x, s32 y, s32 z>
inline void vecT<N>::Set()
{
	const s32 i[] = { x, y, z };
	m_v[0] = *reinterpret_cast<const f32*>(i + 0);
	m_v[1] = *reinterpret_cast<const f32*>(i + 1);
	m_v[2] = *reinterpret_cast<const f32*>(i + 2);
	m_v[3] = 0.f;
}
template<uint N>
template<s32 x, s32 y>
inline void vecT<N>::Set()
{
	const s32 i[] = { x, y };
	m_v[0] = *reinterpret_cast<const f32*>(i + 0);
	m_v[1] = *reinterpret_cast<const f32*>(i + 1);
	m_v[2] = 0.f;
	m_v[3] = 0.f;
}
template<uint N>
template<s32 x>
inline void vecT<N>::Set()
{
	const s32 i = x;
	m_v[0] = *reinterpret_cast<const f32*>(&i);
	m_v[1] = 0.f;
	m_v[2] = 0.f;
	m_v[3] = 0.f;
}

template<uint N>
inline void vecT<N>::Set(f32 x, f32 y, f32 z, f32 w)
{
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = z;
	m_v[3] = w;
}
template<uint N>
inline void vecT<N>::Set(f32 x, f32 y, f32 z)
{
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = z;
	m_v[3] = 0;
}
template<uint N>
inline void vecT<N>::Set(f32 x, f32 y)
{
	m_v[0] = x;
	m_v[1] = y;
	m_v[2] = 0;
	m_v[3] = 0;
}
template<uint N>
inline void vecT<N>::Set(f32 x)
{
	m_v[0] = x;
	m_v[1] = 0;
	m_v[2] = 0;
	m_v[3] = 0;
}


//
// Getters
//

template<uint N>
inline f32 vecT<N>::GetX() const
{
	return m_v[0];
}
template<uint N>
inline f32 vecT<N>::GetY() const
{
	return m_v[1];
}
template<uint N>
inline f32 vecT<N>::GetZ() const
{
	return m_v[2];
}
template<uint N>
inline f32 vecT<N>::GetW() const
{
	return m_v[3];
}

template<uint N>
inline int vecT<N>::GetAsIntX() const
{
	return *reinterpret_cast<const int*>(m_v+0);
}
template<uint N>
inline int vecT<N>::GetAsIntY() const
{
	return *reinterpret_cast<const int*>(m_v+1);
}
template<uint N>
inline int vecT<N>::GetAsIntZ() const
{
	return *reinterpret_cast<const int*>(m_v+2);
}
template<uint N>
inline int vecT<N>::GetAsIntW() const
{
	return *reinterpret_cast<const int*>(m_v+3);
}

//
// Setters
//

template<uint N>
inline vecT<N> vecT<N>::InsertX(f32 f) const
{
	auto r = m_v;
	r[0] = f;
	return r;
}
template<uint N>
inline vecT<N> vecT<N>::InsertY(f32 f) const
{
	auto r = m_v;
	r[1] = f;
	return r;
}
template<uint N>
inline vecT<N> vecT<N>::InsertZ(f32 f) const
{
	auto r = m_v;
	r[2] = f;
	return r;
}
template<uint N>
inline vecT<N> vecT<N>::InsertW(f32 f) const
{
	auto r = m_v;
	r[3] = f;
	return r;
}

template<uint N>
inline void vecT<N>::SetAll(f32 f)
{
	m_v[0] = f;
	m_v[1] = f;
	m_v[2] = f;
	m_v[3] = f;
}

template<bool bX, bool bY, bool bZ, bool bW, uint M>
inline vecT<M> ZeroElements(const vecT<M>& v)
{
	auto r = v.m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] = 0;
	FFTL_IF_CONSTEXPR (bY) r[1] = 0;
	FFTL_IF_CONSTEXPR (bZ) r[2] = 0;
	FFTL_IF_CONSTEXPR (bW) r[3] = 0;
	return r;
}

template<uint N>
template<u32 x, u32 y, u32 z, u32 w>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	const s32 x = x;
	const s32 y = y;
	const s32 z = z;
	const s32 w = w;

	vecT<N> r;
	r.m_v[0] = *reinterpret_cast<const f32*>(&x);
	r.m_v[1] = *reinterpret_cast<const f32*>(&y);
	r.m_v[2] = *reinterpret_cast<const f32*>(&z);
	r.m_v[3] = *reinterpret_cast<const f32*>(&w);
	return r;
}

template<uint N>
template<u32 val>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	const s32 v = val;
	
	vecT<N> r;
	r.m_v[0] = *reinterpret_cast<const f32*>(&v);
	r.m_v[1] = *reinterpret_cast<const f32*>(&v);
	r.m_v[2] = *reinterpret_cast<const f32*>(&v);
	r.m_v[3] = *reinterpret_cast<const f32*>(&v);
	return r;
}


//
// Operators
//

template<uint N>
inline vecT<N> vecT<N>::operator-() const
{
	return vecT<N>(-m_v[0], -m_v[1], -m_v[2], -m_v[3]);
}
template<uint N>
inline vecT<N> vecT<N>::operator+(const vecT<N>& b) const
{
	return vecT<N>(m_v[0]+b.m_v[0], m_v[1]+b.m_v[1], m_v[2]+b.m_v[2], m_v[3]+b.m_v[3]);
}
template<uint N>
inline vecT<N>& vecT<N>::operator+=(const vecT<N>& b)
{
	return *this = *this + b;
}
template<uint N>
inline vecT<N> vecT<N>::operator-(const vecT<N>& b) const
{
	return vecT<N>(m_v[0] - b.m_v[0], m_v[1] - b.m_v[1], m_v[2] - b.m_v[2], m_v[3] - b.m_v[3]);
}
template<uint N>
inline vecT<N>& vecT<N>::operator-=(const vecT<N>& b)
{
	return *this = *this - b;
}
template<uint N>
inline vecT<N> vecT<N>::operator*(const vecT<N>& b) const
{
	return vecT<N>(m_v[0] * b.m_v[0], m_v[1] * b.m_v[1], m_v[2] * b.m_v[2], m_v[3] * b.m_v[3]);
}
template<uint N>
inline vecT<N>& vecT<N>::operator*=(const vecT<N>& b)
{
	return *this = *this * b;
}
template<uint N>
inline vecT<N> vecT<N>::operator/(const vecT<N>& b) const
{
	return vecT<N>(m_v[0] / b.m_v[0], m_v[1] / b.m_v[1], m_v[2] / b.m_v[2], m_v[3] / b.m_v[3]);
}
template<uint N>
inline vecT<N>& vecT<N>::operator/=(const vecT& b)
{
	return *this = *this / b;
}

template<uint N>
inline bool vecT<N>::operator==(const vecT<N>& b) const
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	
	bool r = m_v[0] == b.m_v[0];
	for (uint i = 1; i < N; ++i)
		r &= m_v[i] == b.m_v[i];
	return r;
}
template<uint N>
inline bool vecT<N>::operator!=(const vecT<N>& b) const
{
	static_assert(N <= 4 && N >= 1, "Not implemented");

	bool r = m_v[0] != b.m_v[0];
	for (uint i = 1; i < N; ++i)
		r |= m_v[i] != b.m_v[i];
	return r;
}


//
// Surgical arithmetic affecting only the selected elements
//

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Add(f32 f) const
{
	auto r = m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] += f;
	FFTL_IF_CONSTEXPR (bY) r[1] += f;
	FFTL_IF_CONSTEXPR (bZ) r[2] += f;
	FFTL_IF_CONSTEXPR (bW) r[3] += f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Sub(f32 f) const
{
	auto r = m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] -= f;
	FFTL_IF_CONSTEXPR (bY) r[1] -= f;
	FFTL_IF_CONSTEXPR (bZ) r[2] -= f;
	FFTL_IF_CONSTEXPR (bW) r[3] -= f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Mul(f32 f) const
{
	auto r = m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] *= f;
	FFTL_IF_CONSTEXPR (bY) r[1] *= f;
	FFTL_IF_CONSTEXPR (bZ) r[2] *= f;
	FFTL_IF_CONSTEXPR (bW) r[3] *= f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Div(f32 f) const
{
	auto r = m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] += f;
	FFTL_IF_CONSTEXPR (bY) r[1] += f;
	FFTL_IF_CONSTEXPR (bZ) r[2] += f;
	FFTL_IF_CONSTEXPR (bW) r[3] += f;
	return r;
}

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Negate() const
{
	auto r = m_v;
	FFTL_IF_CONSTEXPR (bX) r[0] = -m_v[0];
	FFTL_IF_CONSTEXPR (bY) r[1] = -m_v[1];
	FFTL_IF_CONSTEXPR (bZ) r[2] = -m_v[2];
	FFTL_IF_CONSTEXPR (bW) r[3] = -m_v[3];
	return r;
}


//
//	Requiring no template arguments.
//

template<uint N>
inline vecT<N> vecT<N>::Negate() const
{
	return -(*this);
}

template<uint N>
inline vecT<N> vecT<N>::ZeroNonFinite() const
{
	const vecT<N> vShouldBeZero = *this - *this;
	f32 alignas(16) r[4];
	r[0] = vShouldBeZero.m_v[0] == 0 ? m_v[0] : 0;
	r[1] = vShouldBeZero.m_v[1] == 0 ? m_v[1] : 0;
	r[2] = vShouldBeZero.m_v[2] == 0 ? m_v[2] : 0;
	r[3] = vShouldBeZero.m_v[3] == 0 ? m_v[3] : 0;
	return r;
}
template<uint N>
inline vecT<N> vecT<N>::ReplaceNonFinite(const vecT& a) const
{
	const vecT<N> vShouldBeZero = *this - *this;
	f32 alignas(16) r[4];
	r[0] = vShouldBeZero.m_v[0] == 0 ? m_v[0] : a.m_v[0];
	r[1] = vShouldBeZero.m_v[1] == 0 ? m_v[1] : a.m_v[1];
	r[2] = vShouldBeZero.m_v[2] == 0 ? m_v[2] : a.m_v[2];
	r[3] = vShouldBeZero.m_v[3] == 0 ? m_v[3] : a.m_v[3];
	return r;
}




//
// 2, 3, or 4 goes in as template argument.
//

template<uint N>
inline vecT<N> vecT<N>::LoadA(const f32* p)
{
	static_assert(N <= 4, "Not implemented");
	vecT<N> v;
	uint i = 0;
	for (; i < N; ++i)
		v.m_v[i] = p[i];
	for (; i < 4; ++i)
		v.m_v[i] = 0;
	return v;
}
template<uint N>
inline vecT<N> vecT<N>::LoadU(const f32* p)
{
	return LoadA(p);
}

template<uint N>
inline vecT<N> vecT<N>::Splat(f32 f)
{
	vecT<N> v;
	v.SetAll(f);
	return v;
}

template<uint N>
inline void StoreA(f32* p, const vecT<N>& v)
{
	for (uint i = 0; i < N; ++i)
		p[i] = v.m_v[i];
}

template<uint N>
inline void StoreU(f32* p, const vecT<N>& v) const
{
	StoreA(p, v);
}

template<uint N>
inline vecT<N> vecT<N>::X000(f32 f)
{
	return vecT<N>(f, 0, 0, 0);
}

template<uint N>
inline bool IsNearEqual(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const vecmask vMask = CmpLe(Abs(a - b), tol);
	const uint uMask = (1 << N) - 1;
	return (vMask.ToIntMask() & uMask) == uMask;
}

template<uint N>
inline vecmask IsNearEqualV(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const vecmask vMask = CmpLe(Abs(a - b), tol);
	return vMask;
}

template<uint N>
inline bool IsNearZero(const vecT<N>& a, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const vecmask vMask = CmpLe(Abs(a), tol);
	const uint uMask = (1 << N) - 1;
	return (vMask.ToIntMask() & uMask) == uMask;
}

template<uint N>
inline vecmask IsNearZeroV(const vecT<N>& a, const vecT<N>& tol)
{
	const vecT<N> vAbs = Abs(a);
	return CmpLe(vAbs, tol);
}

template<uint N>
inline bool vecT<N>::IsNan(const vecT<N>& v) 
{
	bool r = (v.m_v[0] != v.m_v[0]);
	for (uint i = 1; i < N; ++i)
		r |= (v.m_v[i] != v.m_v[i]);
	return r;
}

template<uint N>
inline vecmask IsNanV(const vecT<N>& v)
{
	return CmpEq(v, v);
}

template<uint N>
inline bool IsInf(const vecT<N>& v)
{
	bool r = IsInfinite(v.m_v[0]);
	for (uint i = 1; i < N; ++i)
		r |= (IsInfinite(v.m_v[i]));
	return r;
}

template<uint N>
inline vecmask vecT<N>::IsInfV(const vecT<N>& v)
{
	return CmpEq(vecT<N>::Infinity() ^ *reinterpret_cast<const vecmask*>(&v), vecT<N>::Zero());
}

template<uint N>
inline bool vecT<N>::IsFinite(const vecT<N>& v) const
{
	bool r = (m_v[0] - m_v[0] == 0);
	for (uint i = 1; i < N; ++i)
		r &= (m_v[i] - m_v[i] == 0);
	return r;
}

template<uint N>
inline vecmask IsFiniteV(const vecT<N>& v)
{
	return (v - v).CmpEq(vecT<N>::Zero());
}

template<uint N>
inline bool IsOutrageous(const vecT<N>& v)
{
	bool r = (v.m_v[0] - v.m_v[0] != 0);
	for (uint i = 1; i < N; ++i)
		r |= (v.m_v[i] - v.m_v[i] != 0);
	return r;
}

template<uint N>
inline vecmask IsOutrageousV(const vecT<N>& v)
{
	return CmpNe(v - v, vecT<N>::Zero());
}

template<uint N>
inline vecT<N> Normalize(const vecT<N>& v)
{
	return Normalize<4>(v);
}

template<uint N>
template<uint R>
FFTL_FORCEINLINE vecT<N> Normalize(const vecT<N>& v)
{
	const f32 len = Length(v);

	FFTL_MATH_ASSERT(len != 0);

	const f32 invlen = newLen / len;
	f32 alignas(16) r[4];
	r[0] = R > 0 ? m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? m_v[3] * invlen : 0.f;
	return r;
}

template<uint N>
inline vecT<N> Normalize(const vecT<N>& v, f32& out_fLen)
{
	return Normalize<4>(v, out_fLen);
}

template<uint N>
template<uint R>
inline vecT<N> Normalize(const vecT<N>& v, f32& out_fLen)
{
	const f32 len = Length(v);
	out_fLen = len;

	FFTL_MATH_ASSERT(len != 0);

	const f32 invlen = newLen / len;
	f32 alignas(16) r[4];
	r[0] = R > 0 ? m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? m_v[3] * invlen : 0.f;
	return r;
}


template<uint N>
inline vecT<N> NormalizeSafe(const vecT<N>& v)
{
	return NormalizeSafe<4>(v);
}

template<uint N>
template<uint R>
inline vecT<N> NormalizeSafe(const vecT<N>& v)
{
	const f32 len = Length(v);
	if (len == 0)
		return *this;
	const f32 invlen = newLen / len;
	f32 alignas(16) r[4];
	r[0] = R > 0 ? m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? m_v[3] * invlen : 0.f;
	return r;
}

template<uint N>
inline vecT<N> NormalizeSafe(const vecT<N>& v, f32& out_fLen)
{
	return NormalizeSafe<4>(v, out_fLen);
}

template<uint N>
template<uint R>
inline vecT<N> NormalizeSafe(const vecT<N>& v, f32& out_fLen)
{
	const f32 len = Length(v);
	out_fLen = len;
	if (len == 0)
		return *this;
	const f32 invlen = newLen / len;
	f32 alignas(16) r[4];
	r[0] = R > 0 ? v,m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? v,m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? v,m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? v,m_v[3] * invlen : 0.f;
	return r;
}

template<uint N>
inline vecT<N> NormalizeFast(const vecT<N>& v)
{
	return NormalizeFast<4>(v);
}

template<uint N>
template<uint R>
inline vecT<N> NormalizeFast(const vecT<N>& v)
{
	const f32 invlen = 1 / Sqrt(Length_sq(v));
	f32 alignas(16) r[4];
	r[0] = R > 0 ? v.m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? v.m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? v.m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? v.m_v[3] * invlen : 0.f;
	return r;
}

template<uint N>
inline vecT<N> NormalizeFastSafe(const vecT<N>& v)
{
	return NormalizeFastSafe<4>(v);
}

template<uint N>
template<uint R>
inline vecT<N> NormalizeFastSafe(const vecT<N>& v)
{
	const f32 invlen = 1 / Sqrt(Length_sq(v));
	f32 alignas(16) r[4];
	r[0] = R > 0 ? m_v[0] * invlen : 0.f;
	r[1] = R > 1 ? m_v[1] * invlen : 0.f;
	r[2] = R > 2 ? m_v[2] * invlen : 0.f;
	r[3] = R > 3 ? m_v[3] * invlen : 0.f;
	return ZeroNonFinite(r);
}

template<uint N>
inline vecT<N> ZeroNegative(const vecT<N>& v)
{
	f32 alignas(16) r[4];
	r[0] = m_v[0] <= 0 ? m_v[0] : 0;
	r[1] = m_v[1] <= 0 ? m_v[1] : 0;
	r[2] = m_v[2] <= 0 ? m_v[2] : 0;
	r[3] = m_v[3] <= 0 ? m_v[3] : 0;
	return r;
}







//
//	vecT<N> functions outside the class.
//

template<uint N>
inline vecT<N> Min(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>( Min(a.GetX(), b.GetX()), Min(a.GetY(), b.GetY()), Min(a.GetZ(), b.GetZ()), Min(a.GetW(), b.GetW()) );
}
template<uint N>
inline vecT<N> Max(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>( Max(a.GetX(), b.GetX()), Max(a.GetY(), b.GetY()), Max(a.GetZ(), b.GetZ()), Max(a.GetW(), b.GetW()) );
}
template<uint N>
inline vecT<N> MinV(const vecT<N>& v)
{
	return vecT<N>::Splat(Min(v));
}
template<uint N>
inline vecT<N> MaxV(const vecT<N>& v)
{
	return vecT<N>::Splat(Max(v));
}
template<uint N>
inline vecT<N> Sqrt(const vecT<N>& v)
{
	return vecT<N>(::sqrt(v.GetX()), ::sqrt(v.GetY()), ::sqrt(v.GetZ()), ::sqrt(v.GetW()));
}
template<uint N>
inline vecT<N> RSqrt(const vecT<N>& v)
{
	return vecT<N>(::sqrt(v.GetX()), ::sqrt(v.GetY()), ::sqrt(v.GetZ()), ::sqrt(v.GetW()));
}

template<uint N>
inline vecT<N> Abs(const vecT<N>& a)
{
	return vecT<N>(fabs(a.GetX()), fabs(a.GetY()), fabs(a.GetZ()), fabs(a.GetW()));
}

template<uint N>
inline vecT<N> AbsDiff(const vecT<N>& a, const vecT<N>& b)
{
	return Abs(a - b);
}

template<uint N>
inline vecT<N> Pow4(const vecT<N>& a, float exp)
{
	//	TODO!
	return vecT<N>(powf(a.GetX(), exp), powf(a.GetY(), exp), powf(a.GetZ(), exp), powf(a.GetW(), exp));
}
template<uint N>
inline vecT<N> Sign(const vecT<N>& a)
{
	return vecT<N>(Sign(a.GetX()), Sign(a.GetY()), Sign(a.GetZ()), Sign(a.GetW()));
}

template<uint N>
inline vecT<N> XXXX(const vecT<N>& a)
{
	const f32 f = a.GetX();
	vecT<N> r;
	r.SetAll(f);
	return r;
}
template<uint N>
inline vecT<N> YYYY(const vecT<N>& a)
{
	const f32 f = a.GetY();
	vecT<N> r;
	r.SetAll(f);
	return r;
}
template<uint N>
inline vecT<N> ZZZZ(const vecT<N>& a)
{
	const f32 f = a.GetZ();
	vecT<N> r;
	r.SetAll(f);
	return r;
}
template<uint N>
inline vecT<N> WWWW(const vecT<N>& a)
{
	const f32 f = a.GetW();
	vecT<N> r;
	r.SetAll(f);
	return r;
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& v)
{
	static_assert((T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3),
		"Template parameters need to be in range");

	vecT<N> r;
	r.m_v[0] = v.m_v[T0];
	r.m_v[1] = v.m_v[T1];
	r.m_v[2] = v.m_v[T2];
	r.m_v[3] = v.m_v[T3];
	return r;
}

template<int T0, int T1, int T2, int T3, uint N>
FFTL_FORCEINLINE vecT<N> Permute(const vecT<N>& a, const vecT<N>& b)
{
	static_assert(
		(T0==A0 || T0==A1 || T0==A2 || T0==A3	||
		T0==B0 || T0==B1 || T0==B2 || T0==B3 )	&&
		(T1==A0 || T1==A1 || T1==A2 || T1==A3	||
		T1==B0 || T1==B1 || T1==B2 || T1==B3 )	&&
		(T2==A0 || T2==A1 || T2==A2 || T2==A3	||
		T2==B0 || T2==B1 || T2==B2 || T2==B3 )	&&
		(T3==A0 || T3==A1 || T3==A2 || T3==A3	||
		T3==B0 || T3==B1 || T3==B2 || T3==B3 ),
		"Template inputs need to be shuffle enumerations a/b 0-3");

	vecT<N> r;
	r.m_v[0] = T0 > 3 ? b.m_v[T0 & 3] : a.m_v[T0];
	r.m_v[1] = T1 > 3 ? b.m_v[T1 & 3] : a.m_v[T1];
	r.m_v[2] = T2 > 3 ? b.m_v[T2 & 3] : a.m_v[T2];
	r.m_v[3] = T3 > 3 ? b.m_v[T3 & 3] : a.m_v[T3];
	return r;
}






//
//	vecT<N> functions outside the class with template params
//

template<uint N>
inline f32 Min(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v[0];
	for (uint i = 1; i < N; ++i)
		r = Min(r, v.m_v[i]);
	return r;
}

template<uint N>
inline f32 Max(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v[0];
	for (uint i = 1; i < N; ++i)
		r = Max(r, v.m_v[i]);
	return r;
}

template<uint N>
inline f32 HSum(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v[0];
	for (uint i = 1; i < N; ++i)
		r = r + v.m_v[i];
	return r;
}

template<uint N>
inline vecT<N> HSumV(const vecT<N>& v)
{
	const f32 fSum = HSum(v);
	return vecT<N>(fSum, fSum, fSum, fSum);
}

template<uint N>
inline f32 Dot(const vecT<N>& a, const vecT<N>& b)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = a.m_v[0] * b.m_v[0];
	for (uint i = 1; i < N; ++i)
		r = r + a.m_v[i] * b.m_v[i];
	return r;
}

template<uint N>
inline vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
	const f32 fDot = Dot(a, b);
	return vecT<N>(fDot, fDot, fDot, fDot);
}

template<uint R, uint N>
inline vecT<N> DotV(const vecT<N>& a, const vecT<N>& b)
{
	const f32 r = Dot(a, b);
	vecT<N> vr = vecT<N>::Zero();
	for (int i = 0; i < R; ++i)
		vr.Ptr()[i] = r;
	return vr;
}

//	Utility functions
template<bool bX, bool bY, bool bZ, bool bW, uint N>
inline vecT<N> Blend(const vecT<N>& a, const vecT<N>& b)
{
	return vecT<N>(bX ? b.GetX() : a.GetX(), bY ? b.GetY() : a.GetY(), bZ ? b.GetZ() : a.GetZ(), bW ? b.GetW() : a.GetW());
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Blend(const vecT<N>& a, const vecT<N>& b, const vecmask& msk)
{
	return vecT<N>(msk.Get(0) ? b.GetX() : a.GetX(), msk.Get(1) ? b.GetY() : a.GetY(), msk.Get(2) ? b.GetZ() : a.GetZ(), msk.Get(3) ? b.GetW() : a.GetW());
}







inline constexpr vecmask::vecmask(const vecmask& v)
	: m_v{ v.m_v }
{
}

FFTL_FORCEINLINE vecmask& vecmask::operator=(const vecmask& v)
{
	m_v[0] = v.m_v[0];
	m_v[1] = v.m_v[1];
	m_v[2] = v.m_v[2];
	m_v[3] = v.m_v[3];
	return *this;
}

FFTL_FORCEINLINE vecmask vecmask::operator|(const vecmask& b) const
{
	vecmask r;
	r.m_v[0] = m_v[0] | b.m_v[0];
	r.m_v[1] = m_v[1] | b.m_v[1];
	r.m_v[2] = m_v[2] | b.m_v[2];
	r.m_v[3] = m_v[3] | b.m_v[3];
	return r;
}
FFTL_FORCEINLINE vecmask vecmask::operator&(const vecmask& b) const
{
	vecmask r;
	r.m_v[0] = m_v[0] & b.m_v[0];
	r.m_v[1] = m_v[1] & b.m_v[1];
	r.m_v[2] = m_v[2] & b.m_v[2];
	r.m_v[3] = m_v[3] & b.m_v[3];
	return r;
}
FFTL_FORCEINLINE vecmask vecmask::operator^(const vecmask& b) const
{
	vecmask r;
	r.m_v[0] = m_v[0] ^ b.m_v[0];
	r.m_v[1] = m_v[1] ^ b.m_v[1];
	r.m_v[2] = m_v[2] ^ b.m_v[2];
	r.m_v[3] = m_v[3] ^ b.m_v[3];
	return r;
}

template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator|(const vecT<N>& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(b.Ptr());
	r.m_v[0] = m_v[0] | p[0];
	r.m_v[1] = m_v[1] | p[1];
	r.m_v[2] = m_v[2] | p[2];
	r.m_v[3] = m_v[3] | p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator&(const vecT<N>& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(b.Ptr());
	r.m_v[0] = m_v[0] & p[0];
	r.m_v[1] = m_v[1] & p[1];
	r.m_v[2] = m_v[2] & p[2];
	r.m_v[3] = m_v[3] & p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecmask::operator^(const vecT<N>& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(b.Ptr());
	r.m_v[0] = m_v[0] ^ p[0];
	r.m_v[1] = m_v[1] ^ p[1];
	r.m_v[2] = m_v[2] ^ p[2];
	r.m_v[3] = m_v[3] ^ p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> AndNot(const vecmask& a, const vecT<N>& b)
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(b.Ptr());
	r.m_v[0] = a.m_v[0] & ~p[0];
	r.m_v[1] = a.m_v[1] & ~p[1];
	r.m_v[2] = a.m_v[2] & ~p[2];
	r.m_v[3] = a.m_v[3] & ~p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> AndNot(const vecT<N>& a, const vecmask& b)
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(a.Ptr());
	r.m_v[0] =  p[0] & ~b.m_v[0];
	r.m_v[1] =  p[1] & ~b.m_v[1];
	r.m_v[2] =  p[2] & ~b.m_v[2];
	r.m_v[3] =  p[3] & ~b.m_v[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecmask AndNot(const vecmask& a, const vecmask& b)
{
	vecmask r;
	r.m_v[0] =  a.m_v[0] & ~b.m_v[0];
	r.m_v[1] =  a.m_v[1] & ~b.m_v[1];
	r.m_v[2] =  a.m_v[2] & ~b.m_v[2];
	r.m_v[3] =  a.m_v[3] & ~b.m_v[3];
	return r;
}

template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator|(const vecmask& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(this->Ptr());
	r.m_v[0] = b.m_v[0] | p[0];
	r.m_v[1] = b.m_v[1] | p[1];
	r.m_v[2] = b.m_v[2] | p[2];
	r.m_v[3] = b.m_v[3] | p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator&(const vecmask& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(this->Ptr());
	r.m_v[0] = b.m_v[0] & p[0];
	r.m_v[1] = b.m_v[1] & p[1];
	r.m_v[2] = b.m_v[2] & p[2];
	r.m_v[3] = b.m_v[3] & p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}
template<uint N> FFTL_FORCEINLINE vecT<N> vecT<N>::operator^(const vecmask& b) const
{
	vecmask r;
	const u32* p = reinterpret_cast<const u32*>(this->Ptr());
	r.m_v[0] = b.m_v[0] ^ p[0];
	r.m_v[1] = b.m_v[1] ^ p[1];
	r.m_v[2] = b.m_v[2] ^ p[2];
	r.m_v[3] = b.m_v[3] ^ p[3];
	return *reinterpret_cast<const vecT<N>*>(&r);
}

template<uint N> FFTL_FORCEINLINE vecmask CmpEq(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] == b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] == b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] == b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] == b.m_v[3] ? 0xffffffff : 0;
	return r;
}
template<uint N> FFTL_FORCEINLINE vecmask CmpNe(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] != b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] != b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] != b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] != b.m_v[3] ? 0xffffffff : 0;
	return r;
}
template<uint N> FFTL_FORCEINLINE vecmask CmpLt(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] < b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] < b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] < b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] < b.m_v[3] ? 0xffffffff : 0;
	return r;
}
template<uint N> FFTL_FORCEINLINE vecmask CmpLe(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] <= b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] <= b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] <= b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] <= b.m_v[3] ? 0xffffffff : 0;
	return r;
}
template<uint N> FFTL_FORCEINLINE vecmask CmpGt(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] > b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] > b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] > b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] > b.m_v[3] ? 0xffffffff : 0;
	return r;
}
template<uint N> FFTL_FORCEINLINE vecmask CmpGe(const vecT<N>& a, const vecT<N>& b)
{
	vecmask r;
	r.m_v[0] = a.m_v[0] >= b.m_v[0] ? 0xffffffff : 0;
	r.m_v[1] = a.m_v[1] >= b.m_v[1] ? 0xffffffff : 0;
	r.m_v[2] = a.m_v[2] >= b.m_v[2] ? 0xffffffff : 0;
	r.m_v[3] = a.m_v[3] >= b.m_v[3] ? 0xffffffff : 0;
	return r;
}

FFTL_FORCEINLINE int vecmask::ToIntMask() const
{
	return static_cast<int>( ((m_v[0] >> 31) << 0) | ((m_v[1] >> 31) << 1) | ((m_v[2] >> 31) << 2) | ((m_v[3] >> 31) << 3) );
}

template<s32 x, s32 y, s32 z, s32 w>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromInts()
{
	vecmask r;
	r.m_v[0] = static_cast<u32>(x);
	r.m_v[1] = static_cast<u32>(y);
	r.m_v[2] = static_cast<u32>(z);
	r.m_v[3] = static_cast<u32>(w);
	return r;
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecmask vecmask::GenMaskFromBools()
{
	vecmask r;
	r.m_v[0] = bX ? 0xffffffff : 0;
	r.m_v[1] = bY ? 0xffffffff : 0;
	r.m_v[2] = bZ ? 0xffffffff : 0;
	r.m_v[3] = bW ? 0xffffffff : 0;
	return r;
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE vecmask vecmask::PropagateInt(int i)
{
	vecmask r;
	r.m_v[0] = bX ? i : 0;
	r.m_v[1] = bY ? i : 0;
	r.m_v[2] = bZ ? i : 0;
	r.m_v[3] = bW ? i : 0;
	return r;
}




} // namespace FFTL

