#ifndef __VECTOR_INL__
#define __VECTOR_INL__
#else
#error "Already included"
#endif // !__VECTOR_INL__

#if !defined(__VECTOR_H__)
#	error "Do not include this file. Vector.h must be included first."
#endif

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4127)// conditional expression is constant
#endif


#if defined(FFTL_SSE)
#	include "SSE/Vector_SSE.inl"
#elif defined(FFTL_ARM_NEON)
#	include "NEON/Vector_NEON.inl"
#else
#	include "Default/Vector_Default.inl"
#endif


namespace FFTL
{

template<uint N>
FFTL_FORCEINLINE vecT<N>::vecT(enZeroType)
{
	*this = Zero();
}

template<>
FFTL_FORCEINLINE vecT<4>::vecT(const vecT<3>& xyz, f32 w)
{
	*this = Permute<A0, A1, A2, B0>(xyz.CastTo<4>(), vec4(w));
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::operator+() const
{
	return *this;
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::AddX(f32 f) const
{
	return Add<1, 0, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::AddY(f32 f) const
{
	return Add<0, 1, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::AddZ(f32 f) const
{
	return Add<0, 0, 1, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::AddW(f32 f) const
{
	return Add<0, 0, 0, 1>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::SubX(f32 f) const
{
	return Sub<1, 0, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::SubY(f32 f) const
{
	return Sub<0, 1, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::SubZ(f32 f) const
{
	return Sub<0, 0, 1, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::SubW(f32 f) const
{
	return Sub<0, 0, 0, 1>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::MulX(f32 f) const
{
	return Mul<1, 0, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::MulY(f32 f) const
{
	return Mul<0, 1, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::MulZ(f32 f) const
{
	return Mul<0, 0, 1, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::MulW(f32 f) const
{
	return Mul<0, 0, 0, 1>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::DivX(f32 f) const
{
	return Div<1, 0, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::DivY(f32 f) const
{
	return Div<0, 1, 0, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::DivZ(f32 f) const
{
	return Div<0, 0, 1, 0>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::DivW(f32 f) const
{
	return Div<0, 0, 0, 1>(f);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::NegateX() const
{
	return Negate<1, 0, 0, 0>();
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::NegateY() const
{
	return Negate<0, 1, 0, 0>();
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::NegateZ() const
{
	return Negate<0, 0, 1, 0>();
}
template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::NegateW() const
{
	return Negate<0, 0, 0, 1>();
}

template<uint N> template<bool ALIGNED>
vecT<N> vecT<N>::Load(const f32* p)
{
	return ALIGNED ? LoadA(p) : LoadU(p);
}

template<bool ALIGNED, uint N>
void Store(f32* p, const vecT<N>& v)
{
	ALIGNED ? StoreA(p, v) : StoreU(p, v);
}

template<uint N>
FFTL_FORCEINLINE bool IsNearEqual(const vecT<N>& a, const vecT<N>& b, f32 tol)
{
	return IsNearEqual(a, b, vecT<N>::Splat(tol));
}

template<uint N>
FFTL_FORCEINLINE vecmask IsNearEqualV(const vecT<N>& a, const vecT<N>& b, f32 tol)
{
	return IsNearEqualV(a, b, vecT<N>::Splat(tol));
}

template<uint N>
FFTL_FORCEINLINE bool IsNearZero(const vecT<N>& v, f32 tol)
{
	return IsNearZero(v, vecT<N>::Splat(tol));
}

template<uint N>
FFTL_FORCEINLINE vecmask IsNearZeroV(const vecT<N>& v, f32 tol)
{
	return IsNearZeroV(v, vecT<N>::Splat(tol));
}

template<uint N>
FFTL_FORCEINLINE bool IsNormal(const vecT<N>& v)
{
	return IsNearEqual(Length_sq(v), 1.0f);
}
template<uint N>
FFTL_FORCEINLINE f32 Length_sq(const vecT<N>& v)
{
	return Dot(v, v);
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Length_sqV(const vecT<N>& v)
{
	return DotV(v, v);
}
template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> Length_sqV(const vecT<N>& v)
{
	return DotV<R, N>(v, v);
}
template<uint N>
FFTL_FORCEINLINE f32 Length(const vecT<N>& v)
{
	return Sqrt(Length_sq(v));
}
template<uint N>
FFTL_FORCEINLINE vecT<N> LengthV(const vecT<N>& v)
{
	return Sqrt(Length_sqV(v));
}
template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> LengthV(const vecT<N>& v)
{
	return Sqrt(Length_sqV<R>(v));
}

template<uint N>
FFTL_FORCEINLINE f32 Length_fast(const vecT<N>& v)
{
	//	This is fast enough on most modern platforms compared to the much less precise rsqrtss + rcpss chain, which is usually 4-6 cycles to get sqrt vs 7-18.
	return Length(v);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Normalize(const vecT<N>& v)
{
	const auto vLen = LengthV(v).AsIntrinsic();
	const auto r = V4fDiv(v.m_v, vLen);
	return r;
}

template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> Normalize(const vecT<N>& v)
{
	const auto vLen = LengthV<R>(v).AsIntrinsic();
	const auto r = V4fDiv(v.m_v, vLen);
	return r;
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Normalize(const vecT<N>& v, f32& out_fLen)
{
	const auto vLen = LengthV(v).AsIntrinsic();
	out_fLen = V4fGetX(vLen);
	const auto r = V4fDiv(v.m_v, vLen);
	return r;
}

template<uint R, uint N>
FFTL_FORCEINLINE vecT<N> Normalize(const vecT<N>& v, f32& out_fLen)
{
	constexpr int ix = R <= 0;
	constexpr int iy = R <= 1;
	constexpr int iz = R <= 2;
	constexpr int iw = R <= 3;

	const f32 len = Length(v);
	out_fLen = len;
	const auto invlen = V4fSet1(1.f / len);
	const auto r = V4fMul(v.m_v, V4fPermute<ix, iy, iz, iw>(invlen));
	return r;
}

template<uint N>
FFTL_FORCEINLINE vecT<N> NormalizeSafe(const vecT<N>& v)
{
	const auto vLen = LengthV(v).AsIntrinsic();
	const auto r = V4fDiv(v.m_v, vLen);

	//	If the normalized result turns out to be non finite, this will mask it off and force it to zero.
	const auto vMask = V4fCompareEqual(V4fSub(r, r), V4fZero());
	return V4fAnd(r, vMask);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> NormalizeFast(const vecT<N>& v)
{
	const auto vInvSqrt = V4fRcpSqrt(Length_sqV(v).AsIntrinsic());
	return V4fMul(v.m_v, vInvSqrt);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> NormalizeFastSafe(const vecT<N>& v)
{
	const auto vInvSqrt = V4fRcpSqrt(Length_sqV(v).AsIntrinsic());
	const auto r = V4fMul(v.m_v, vInvSqrt);

	//	If the normalized result turns out to be non finite, this will mask it off and force it to zero.
	const auto vMask = V4fCompareEqual(V4fSub(r, r), V4fZero());
	return V4fAnd(r, vMask);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Reflect(const vecT<N>& a, const vecT<N>& n)
{
	const vecT<N> vDot = DotV(a, n);
	const vecT<N> v2Dot = vDot + vDot;
	return a - v2Dot * n;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Project(const vecT<N>& a, const vecT<N>& p)
{
	return a * (DotV(a, p) / Length_sqV(a));
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Zero()
{
	return GenConstRtp<0>();
}

template<uint N>
FFTL_FORCEINLINE vecT<N> vecT<N>::Infinity()
{
	return GenConstRtp<0x7F800000>();
}








template<uint M>
const vecT<2>& AsVec2(const vecT<M>& a)
{
	return *reinterpret_cast<const vecT<2>*>(&a);
}
template<uint M>
FFTL_FORCEINLINE const vecT<3>&	AsVec3(const vecT<M>& a)
{
	return *reinterpret_cast<const vecT<3>*>(&a);
}
template<uint M>
FFTL_FORCEINLINE const vecT<4>&	AsVec4(const vecT<M>& a)
{
	return *reinterpret_cast<const vecT<4>*>(&a);
}
template<uint M>
FFTL_FORCEINLINE vecT<2>& AsVec2(vecT<M>& a)
{
	return *reinterpret_cast<vecT<2>*>(&a);
}
template<uint M>
FFTL_FORCEINLINE vecT<3>& AsVec3(vecT<M>& a)
{
	return *reinterpret_cast<vecT<3>*>(&a);
}
template<uint M>
FFTL_FORCEINLINE vecT<4>& AsVec4(vecT<M>& a)
{
	return *reinterpret_cast<vecT<4>*>(&a);
}

template<uint N>
FFTL_FORCEINLINE f32 MinAbs(const vecT<N>& v)
{
	return Min(Abs(v));
}
template<uint N>
FFTL_FORCEINLINE f32 MaxAbs(const vecT<N>& v)
{
	return Max(Abs(v));
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Clamp(const vecT<N>& val, const vecT<N>& lhs, const vecT<N>& b)
{
	return Min(Max(val, lhs), b);
}

template<uint N>
FFTL_FORCEINLINE u32 MinIndex(const vecT<N>& v)
{
	const uint uMask = (1 << N) - 1;
	return LS1Bit( CmpEq(v, MinV(v)).ToIntMask() & uMask );
}
template<>
FFTL_FORCEINLINE u32 MinIndex(const vecT<4>& v)
{
	return LS1Bit( CmpEq(v, MinV(v)).ToIntMask() );
}
template<uint N>
FFTL_FORCEINLINE u32 MaxIndex(const vecT<N>& v)
{
	const uint uMask = (1 << N) - 1;
	return LS1Bit(v.CmpEq(MaxV(v)).ToIntMask() & uMask);
}
template<>
FFTL_FORCEINLINE u32 MaxIndex(const vecT<4>& v)
{
	return LS1Bit(CmpEq(v, MaxV(v)).ToIntMask());
}

template<uint N>
FFTL_FORCEINLINE vecT<N> Lerp(const vecT<N>& lhs, const vecT<N>& b, f32 amt)
{
	return lhs + (b - lhs) * amt;
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Deg2Rad(const vecT<N>& degrees)
{
	return degrees * vec4_PI_180.CastTo<N>();
}
template<uint N>
FFTL_FORCEINLINE vecT<N> Rad2deg(const vecT<N>& radians)
{
	return radians * vec4_180_PI.CastTo<N>();
}




template<uint N>
FFTL_FORCEINLINE f32 Distance(const vecT<N>& lhs, const vecT<N>& b)
{
	return Length(lhs - b);
}
template<uint N>
FFTL_FORCEINLINE f32 Distance_sq(const vecT<N>& lhs, const vecT<N>& b)
{
	return Length_sq(lhs - b);
}
template<uint N>
FFTL_FORCEINLINE f32 Mean(const vecT<N>& v)
{
	return HSum(v) * (1.f / N);
}

template<uint N>
FFTL_FORCEINLINE vecT<N> pointOnRay(const vecT<N>& start, const vecT<N>& dir, f32 dist)
{
	return start + dir * dist;
}

template<uint N>
FFTL_FORCEINLINE vecT<N> ClosestPointOnLineSegment(const vecT<N>& a, const vecT<N>& b, const vecT<N>& p)
{
	vecT<N> ab = b - a;
	vecT<N> ab2 = ab.Length_sqV();
	vecT<N> t = DotV(ab, p - a) / ab2;
	t = Clamp(t, vecT<N>::Zero(), vecT<N>::Splat(1.f));
	vecT<N> r = (a + ab * t);
	r.ReplaceNonFinite(a);
	return r;
}


FFTL_FORCEINLINE vecmask::vecmask()
{
}

FFTL_FORCEINLINE vecmask& vecmask::operator|=(const vecmask& b)
{
	return *this = *this | b;
}
FFTL_FORCEINLINE vecmask& vecmask::operator&=(const vecmask& b)
{
	return *this = *this & b;
}
FFTL_FORCEINLINE vecmask& vecmask::operator^=(const vecmask& b)
{
	return *this = *this ^ b;
}

template<uint N> FFTL_FORCEINLINE vecT<N>& vecT<N>::operator|=(const vecmask& b)
{
	return *this = *this | b;
}
template<uint N> FFTL_FORCEINLINE vecT<N>& vecT<N>::operator&=(const vecmask& b)
{
	return *this = *this & b;
}
template<uint N> FFTL_FORCEINLINE vecT<N>& vecT<N>::operator^=(const vecmask& b)
{
	return *this = *this ^ b;
}

template<uint M>
FFTL_FORCEINLINE vecT<M> Sin(const vecT<M>& y)
{
	vecT<M> s, c;
	SinCos(y, s, c);
	return s;
}

template<uint M>
FFTL_FORCEINLINE vecT<M> Cos(const vecT<M>& y)
{
	vecT<M> s, c;
	SinCos(y, s, c);
	return c;
}

template<uint M>
inline void SinCos(const vecT<M>& a, vecT<M>& s, vecT<M>& c)
{
#if defined(FFTL_SSE)
	sse_SinCos(a.m_v, s.m_v, c.m_v);
#elif defined(FFTL_ARM_NEON)
	neon_SinCos(a.m_v, s.m_v, c.m_v);
#else
	SinCos(a.m_v, s.m_v, c.m_v);
#endif
}





} // namespace FFTL

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif
