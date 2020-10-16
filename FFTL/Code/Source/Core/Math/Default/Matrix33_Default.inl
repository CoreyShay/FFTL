#ifndef __MATRIX33_DEFAULT_INL__
#define __MATRIX33_DEFAULT_INL__
#else
#error "Already included"
#endif // !__MATRIX33_DEFAULT_INL__

namespace FFTL
{


inline void mat33::Identity()
{
	Row<0>().Set(1, 0, 0, 0);
	Row<1>().Set(0, 1, 0, 0);
	Row<2>().Set(0, 0, 1, 0);
}

inline bool mat33::operator==(const mat33& rhs) const
{
	return Row<0>() == rhs.Row<0>() && Row<1>() == rhs.Row<1>() && Row<2>() == rhs.Row<2>();
}
inline bool mat33::operator!=(const mat33& rhs) const
{
	return Row<0>() != rhs.Row<0>() || Row<1>() != rhs.Row<1>() || Row<2>() != rhs.Row<2>();
}

inline bool IsNearEqual(const mat33& a, const mat33& b, f32 tol)
{
	const f32(*pA)[4] = reinterpret_cast<const f32(*)[4]>(&a);
	const f32(*pB)[4] = reinterpret_cast<const f32(*)[4]>(&b);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if(!FFTL::IsNearEqual(pA[i][j], pB[i][j], tol))
				return false;
	return true;
}

inline bool mat33::IsNan() const
{
	return FFTL::IsNan(Row<0>()) || FFTL::IsNan(Row<1>()) || FFTL::IsNan(Row<2>());
}
inline bool	mat33::IsInf() const
{
	return FFTL::IsInf(Row<0>()) || FFTL::IsInf(Row<1>()) || FFTL::IsInf(Row<2>());
}
inline bool mat33::IsFinite() const
{
	return FFTL::IsFinite(Row<0>()) && FFTL::IsFinite(Row<1>()) && FFTL::IsFinite(Row<2>());
}
inline bool	mat33::IsNonFinite() const
{
	return FFTL::IsNonFinite(Row<0>()) || FFTL::IsNonFinite(Row<1>()) || FFTL::IsNonFinite(Row<2>());
}


} // namespace FFTL
