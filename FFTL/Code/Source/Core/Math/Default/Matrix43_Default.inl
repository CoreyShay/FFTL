
#ifndef __MATRIX43_DEFAULT_INL__
#define __MATRIX43_DEFAULT_INL__
#else
#error "Already included"
#endif // !__MATRIX43_DEFAULT_INL__

namespace FFTL
{


inline void mat43::Identity()
{
	Row<0>().Set(1, 0, 0, 0);
	Row<1>().Set(0, 1, 0, 0);
	Row<2>().Set(0, 0, 1, 0);
	Row<3>().Set(0, 0, 0, 1);
}

inline bool mat43::operator==(const mat43& rhs) const
{
	return Row<0>() == rhs.Row<0>() && Row<1>() == rhs.Row<1>() && Row<2>() == rhs.Row<2>() && Row<3>() == rhs.Row<3>();
}
inline bool mat43::operator!=(const mat43& rhs) const
{
	return Row<0>() != rhs.Row<0>() || Row<1>() != rhs.Row<1>() || Row<2>() != rhs.Row<2>() || Row<3>() != rhs.Row<3>();
}

inline bool mat43::IsNearEqual(const mat43& rhs, f32 tol) const
{
	const f32(*m)[4] = reinterpret_cast<const f32(*)[4]>(this);
	const f32(*mr)[4] = reinterpret_cast<const f32(*)[4]>(&rhs);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 3; ++j)
			if(!FFTL::IsNearEqual(m[i][j], mr[i][j], tol))
				return false;
	return true;
}

inline bool mat43::IsNan() const
{
	return FFTL::IsNan(Row<0>()) || FFTL::IsNan(Row<1>()) || FFTL::IsNan(Row<2>()) || FFTL::IsNan(Row<3>());
}
inline bool	mat43::IsInf() const
{
	return FFTL::IsInf(Row<0>()) || FFTL::IsInf(Row<1>()) || FFTL::IsInf(Row<2>()) || FFTL::IsInf(Row<3>());
}
inline bool mat43::IsFinite() const
{
	return FFTL::IsFinite(Row<0>()) && FFTL::IsFinite(Row<1>()) && FFTL::IsFinite(Row<2>()) && FFTL::IsFinite(Row<3>());
}
inline bool	mat43::IsNonFinite() const
{
	return FFTL::IsNonFinite(Row<0>()) || FFTL::IsNonFinite(Row<1>()) || FFTL::IsNonFinite(Row<2>()) || FFTL::IsNonFinite(Row<3>());
}


} // namespace FFTL
