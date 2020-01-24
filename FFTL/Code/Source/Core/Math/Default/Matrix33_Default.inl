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

inline bool mat33::IsNearEqual(const mat33& rhs, f32 tol) const
{
	const f32(*m)[4] = reinterpret_cast<const f32(*)[4]>(this);
	const f32(*mr)[4] = reinterpret_cast<const f32(*)[4]>(&rhs);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if(!FFTL::IsNearEqual(m[i][j], mr[i][j], tol))
				return false;
	return true;
}

inline bool mat33::IsNan() const
{
	return Row<0>() != Row<0>() || Row<1>() != Row<1>() || Row<2>() != Row<2>();
}
inline bool	mat33::IsInf() const
{
	return Row<0>().IsInf() || Row<1>().IsInf() || Row<2>().IsInf();
}
inline bool mat33::IsFinite() const
{
	return IsFinite(Row<0>()) && IsFinite(Row<1>()) && IsFinite(Row<2>());
}
inline bool	mat33::IsOutrageous() const
{
	return Row<0>().IsOutrageous() || Row<1>().IsOutrageous() || Row<2>().IsOutrageous();
}


} // namespace FFTL
