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
	FFTL_BIT_CAST_CONSTEXPR f32 f = bit_cast<f32>(0xffffffff);
	m_v.x = f;
	m_v.y = f;
	m_v.z = f;
	m_v.w = f;
#endif
}

template<uint N>
inline vecT<N>::vecT(f32 x, f32 y, f32 z)
{
	m_v.x = x;
	m_v.y = y;
	m_v.z = z;
	m_v.w = 0;
}
template<uint N>
inline vecT<N>::vecT(f32 x, f32 y)
{
	m_v.x = x;
	m_v.y = y;
	m_v.z = 0;
	m_v.w = 0;
}
template<uint N>
inline vecT<N>::vecT(f32 x)
{
	m_v.x = x;
	m_v.y = 0;
	m_v.z = 0;
	m_v.w = 0;
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
	m_v.x = *reinterpret_cast<const f32*>(i+0);
	m_v.y = *reinterpret_cast<const f32*>(i+1);
	m_v.z = *reinterpret_cast<const f32*>(i+2);
	m_v.w = *reinterpret_cast<const f32*>(i+3);
}
template<uint N>
template<s32 x, s32 y, s32 z>
inline void vecT<N>::Set()
{
	const s32 i[] = { x, y, z };
	m_v.x = *reinterpret_cast<const f32*>(i + 0);
	m_v.y = *reinterpret_cast<const f32*>(i + 1);
	m_v.z = *reinterpret_cast<const f32*>(i + 2);
	m_v.w = 0.f;
}
template<uint N>
template<s32 x, s32 y>
inline void vecT<N>::Set()
{
	const s32 i[] = { x, y };
	m_v.x = *reinterpret_cast<const f32*>(i + 0);
	m_v.y = *reinterpret_cast<const f32*>(i + 1);
	m_v.z = 0.f;
	m_v.w = 0.f;
}
template<uint N>
template<s32 x>
inline void vecT<N>::Set()
{
	const s32 i = x;
	m_v.x = *reinterpret_cast<const f32*>(&i);
	m_v.y = 0.f;
	m_v.z = 0.f;
	m_v.w = 0.f;
}

template<uint N>
inline void vecT<N>::Set(f32 x, f32 y, f32 z, f32 w)
{
	m_v.x = x;
	m_v.y = y;
	m_v.z = z;
	m_v.w = w;
}
template<uint N>
inline void vecT<N>::Set(f32 x, f32 y, f32 z)
{
	m_v.x = x;
	m_v.y = y;
	m_v.z = z;
	m_v.w = 0;
}
template<uint N>
inline void vecT<N>::Set(f32 x, f32 y)
{
	m_v.x = x;
	m_v.y = y;
	m_v.z = 0;
	m_v.w = 0;
}
template<uint N>
inline void vecT<N>::Set(f32 x)
{
	m_v.x = x;
	m_v.y = 0;
	m_v.z = 0;
	m_v.w = 0;
}


//
// Getters
//

template<uint N>
inline f32 vecT<N>::GetX() const
{
	return m_v.x;
}
template<uint N>
inline f32 vecT<N>::GetY() const
{
	return m_v.y;
}
template<uint N>
inline f32 vecT<N>::GetZ() const
{
	return m_v.z;
}
template<uint N>
inline f32 vecT<N>::GetW() const
{
	return m_v.w;
}

template<uint N>
inline int vecT<N>::GetAsIntX() const
{
	return *reinterpret_cast<const int*>(&m_v) + 0;
}
template<uint N>
inline int vecT<N>::GetAsIntY() const
{
	return *reinterpret_cast<const int*>(&m_v) + 1;
}
template<uint N>
inline int vecT<N>::GetAsIntZ() const
{
	return *reinterpret_cast<const int*>(&m_v) + 2;
}
template<uint N>
inline int vecT<N>::GetAsIntW() const
{
	return *reinterpret_cast<const int*>(&m_v) + 3;
}

//
// Setters
//

template<uint N>
inline vecT<N> InsertX(const vecT<N>& v, f32 f)
{
	auto r = v.m_v;
	r.x = f;
	return r;
}
template<uint N>
inline vecT<N> InsertY(const vecT<N>& v, f32 f)
{
	auto r = v.m_v;
	r.y = f;
	return r;
}
template<uint N>
inline vecT<N> InsertZ(const vecT<N>& v, f32 f)
{
	auto r = v.m_v;
	r.z = f;
	return r;
}
template<uint N>
inline vecT<N> InsertW(const vecT<N>& v, f32 f)
{
	auto r = v.m_v;
	r.w = f;
	return r;
}

template<uint N>
inline void vecT<N>::SetAll(f32 f)
{
	m_v.x = f;
	m_v.y = f;
	m_v.z = f;
	m_v.w = f;
}

template<bool bX, bool bY, bool bZ, bool bW, uint M>
inline vecT<M> ZeroElements(const vecT<M>& v)
{
	auto r = v.m_v;
	if constexpr (bX) r.x = 0;
	if constexpr (bY) r.y = 0;
	if constexpr (bZ) r.z = 0;
	if constexpr (bW) r.w = 0;
	return r;
}

template<uint N>
template<u32 x, u32 y, u32 z, u32 w>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	const s32 _x = x;
	const s32 _y = y;
	const s32 _z = z;
	const s32 _w = w;

	vecT<N> r;
	r.m_v.x = *reinterpret_cast<const f32*>(&_x);
	r.m_v.y = *reinterpret_cast<const f32*>(&_y);
	r.m_v.z = *reinterpret_cast<const f32*>(&_z);
	r.m_v.w = *reinterpret_cast<const f32*>(&_w);
	return r;
}

template<uint N>
template<u32 val>
FFTL_FORCEINLINE vecT<N> vecT<N>::GenConstRtp()
{
	const s32 v = val;
	
	vecT<N> r;
	r.m_v.x = *reinterpret_cast<const f32*>(&v);
	r.m_v.y = *reinterpret_cast<const f32*>(&v);
	r.m_v.z = *reinterpret_cast<const f32*>(&v);
	r.m_v.w = *reinterpret_cast<const f32*>(&v);
	return r;
}


//
// Operators
//

template<uint N>
inline vecT<N> vecT<N>::operator-() const
{
	return vecT<N>(-m_v.x, -m_v.y, -m_v.z, -m_v.w);
}
template<uint N>
inline vecT<N> vecT<N>::operator+(const vecT<N>& b) const
{
	return vecT<N>(m_v.x+b.m_v.x, m_v.y+b.m_v.y, m_v.z+b.m_v.z, m_v.w+b.m_v.w);
}
template<uint N>
inline vecT<N>& vecT<N>::operator+=(const vecT<N>& b)
{
	return *this = *this + b;
}
template<uint N>
inline vecT<N> vecT<N>::operator-(const vecT<N>& b) const
{
	return vecT<N>(m_v.x - b.m_v.x, m_v.y - b.m_v.y, m_v.z - b.m_v.z, m_v.w - b.m_v.w);
}
template<uint N>
inline vecT<N>& vecT<N>::operator-=(const vecT<N>& b)
{
	return *this = *this - b;
}
template<uint N>
inline vecT<N> vecT<N>::operator*(const vecT<N>& b) const
{
	return vecT<N>(m_v.x * b.m_v.x, m_v.y * b.m_v.y, m_v.z * b.m_v.z, m_v.w * b.m_v.w);
}
template<uint N>
inline vecT<N>& vecT<N>::operator*=(const vecT<N>& b)
{
	return *this = *this * b;
}
template<uint N>
inline vecT<N> vecT<N>::operator/(const vecT<N>& b) const
{
	return vecT<N>(m_v.x / b.m_v.x, m_v.y / b.m_v.y, m_v.z / b.m_v.z, m_v.w / b.m_v.w);
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
	
	bool r = m_v.x == b.m_v.x;
	for (uint i = 1; i < N; ++i)
		r &= Get(i) == b.Get(i);
	return r;
}
template<uint N>
inline bool vecT<N>::operator!=(const vecT<N>& b) const
{
	static_assert(N <= 4 && N >= 1, "Not implemented");

	bool r = m_v.x != b.m_v.x;
	for (uint i = 1; i < N; ++i)
		r |= Get(i) != b.Get(i);
	return r;
}


//
// Surgical arithmetic affecting only the selected elements
//

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Add(f32 f) const
{
	auto r = m_v;
	if constexpr (bX) r[0] += f;
	if constexpr (bY) r[1] += f;
	if constexpr (bZ) r[2] += f;
	if constexpr (bW) r[3] += f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Sub(f32 f) const
{
	auto r = m_v;
	if constexpr (bX) r[0] -= f;
	if constexpr (bY) r[1] -= f;
	if constexpr (bZ) r[2] -= f;
	if constexpr (bW) r[3] -= f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Mul(f32 f) const
{
	auto r = m_v;
	if constexpr (bX) r[0] *= f;
	if constexpr (bY) r[1] *= f;
	if constexpr (bZ) r[2] *= f;
	if constexpr (bW) r[3] *= f;
	return r;
}
template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Div(f32 f) const
{
	auto r = m_v;
	if constexpr (bX) r[0] += f;
	if constexpr (bY) r[1] += f;
	if constexpr (bZ) r[2] += f;
	if constexpr (bW) r[3] += f;
	return r;
}

template<uint N> template<bool bX, bool bY, bool bZ, bool bW>
inline vecT<N> vecT<N>::Negate() const
{
	auto r = *this;
	if constexpr (bX) r.Ptr()[0] = -m_v.x;
	if constexpr (bY) r.Ptr()[1] = -m_v.y;
	if constexpr (bZ) r.Ptr()[2] = -m_v.z;
	if constexpr (bW) r.Ptr()[3] = -m_v.w;
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
	alignas(16) f32 r[4];
	r[0] = vShouldBeZero.m_v.x == 0 ? m_v.x : 0;
	r[1] = vShouldBeZero.m_v.y == 0 ? m_v.y : 0;
	r[2] = vShouldBeZero.m_v.z == 0 ? m_v.z : 0;
	r[3] = vShouldBeZero.m_v.w == 0 ? m_v.w : 0;
	return r;
}
template<uint N>
inline vecT<N> vecT<N>::ReplaceNonFinite(const vecT& a) const
{
	const vecT<N> vShouldBeZero = *this - *this;
	alignas(16) f32 r[4];
	r[0] = vShouldBeZero.m_v.x == 0 ? m_v.x : a.m_v.x;
	r[1] = vShouldBeZero.m_v.y == 0 ? m_v.y : a.m_v.y;
	r[2] = vShouldBeZero.m_v.z == 0 ? m_v.z : a.m_v.z;
	r[3] = vShouldBeZero.m_v.w == 0 ? m_v.w : a.m_v.w;
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
		v.Ptr()[i] = p[i];
	for (; i < 4; ++i)
		v.Ptr()[i] = 0;
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
		p[i] = v.Get(i);
}

template<uint N>
inline void StoreU(f32* p, const vecT<N>& v)
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
	const mask32x4 vMask = CmpLe(Abs(a - b), tol);
	const uint uMask = (1 << N) - 1;
	return (vMask.ToIntMask() & uMask) == uMask;
}

template<uint N>
inline mask32x4 IsNearEqualV(const vecT<N>& a, const vecT<N>& b, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const mask32x4 vMask = CmpLe(Abs(a - b), tol);
	return vMask;
}

template<uint N>
inline bool IsNearZero(const vecT<N>& a, const vecT<N>& tol)
{
	static_assert(N <= 4 && N >= 1, "Not implemented");
	const mask32x4 vMask = CmpLe(Abs(a), tol);
	const uint uMask = (1 << N) - 1;
	return (vMask.ToIntMask() & uMask) == uMask;
}

template<uint N>
inline mask32x4 IsNearZeroV(const vecT<N>& a, const vecT<N>& tol)
{
	const vecT<N> vAbs = Abs(a);
	return CmpLe(vAbs, tol);
}

template<uint N>
inline bool IsNan(const vecT<N>& v) 
{
	bool r = (v.m_v.x != v.m_v.x);
	for (uint i = 1; i < N; ++i)
		r |= IsNan(v.Get(i));
	return r;
}

template<uint N>
inline mask32x4 IsNanV(const vecT<N>& v)
{
	return CmpEq(v, v);
}

template<uint N>
inline bool IsInf(const vecT<N>& v)
{
	bool r = IsInfinite(v.m_v.x);
	for (uint i = 1; i < N; ++i)
		r |= (IsInfinite(v.Get(i)));
	return r;
}

template<uint N>
inline mask32x4 IsInfV(const vecT<N>& v)
{
	return CmpEq(vecT<N>::Infinity() ^ *reinterpret_cast<const mask32x4*>(&v), vecT<N>::Zero());
}

template<uint N>
inline bool IsFinite(const vecT<N>& v)
{
	bool r = (v.m_v.x - v.m_v.x == 0);
	for (uint i = 1; i < N; ++i)
		r &= IsFinite(v.Get(i));
	return r;
}

template<uint N>
inline mask32x4 IsFiniteV(const vecT<N>& v)
{
	return (v - v).CmpEq(vecT<N>::Zero());
}

template<uint N>
inline bool IsNonFinite(const vecT<N>& v)
{
	bool r = (v.m_v.x - v.m_v.x != 0);
	for (uint i = 1; i < N; ++i)
		r |= IsNonFinite(v.Get(i));
	return r;
}

template<uint N>
inline mask32x4 IsNonFiniteV(const vecT<N>& v)
{
	return CmpNe(v - v, vecT<N>::Zero());
}

template<uint N>
inline vecT<N> ZeroNegative(const vecT<N>& v)
{
	Vec4f r;
	r.x = v.m_v.x >= 0 ? v.m_v.x : 0;
	r.y = v.m_v.y >= 0 ? v.m_v.y : 0;
	r.z = v.m_v.z >= 0 ? v.m_v.z : 0;
	r.w = v.m_v.w >= 0 ? v.m_v.w : 0;
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
	r.m_v.x = v.Get(T0);
	r.m_v.y = v.Get(T1);
	r.m_v.z = v.Get(T2);
	r.m_v.w = v.Get(T3);
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
	r.m_v.x = T0 > 3 ? b.Get(T0 & 3) : a.Get(T0);
	r.m_v.y = T1 > 3 ? b.Get(T1 & 3) : a.Get(T1);
	r.m_v.z = T2 > 3 ? b.Get(T2 & 3) : a.Get(T2);
	r.m_v.w = T3 > 3 ? b.Get(T3 & 3) : a.Get(T3);
	return r;
}






//
//	vecT<N> functions outside the class with template params
//

template<uint N>
inline f32 Min(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v.x;
	for (uint i = 1; i < N; ++i)
		r = Min(r, v.Get(i));
	return r;
}

template<uint N>
inline f32 Max(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v.x;
	for (uint i = 1; i < N; ++i)
		r = Max(r, v.Get(i));
	return r;
}

template<uint N>
inline f32 HSum(const vecT<N>& v)
{
	static_assert(N <= 4 && N >= 2, "Not implemented");
	f32 r = v.m_v.x;
	for (uint i = 1; i < N; ++i)
		r = r + v.Get(i);
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
	f32 r = a.m_v.x * b.m_v.x;
	for (uint i = 1; i < N; ++i)
		r = r + a.Get(i) * b.Get(i);
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




} // namespace FFTL

