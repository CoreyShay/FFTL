
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
	return Row<0>() != Row<0>() || Row<1>() != Row<1>() || Row<2>() != Row<2>() || Row<3>() != Row<3>();
}
inline bool	mat43::IsInf() const
{
	return Row<0>().IsInf() || Row<1>().IsInf() || Row<2>().IsInf() || Row<3>().IsInf();
}
inline bool mat43::IsFinite() const
{
	return Row<0>().IsFinite() && Row<1>().IsFinite() && Row<2>().IsFinite() && Row<3>().IsFinite();
}
inline bool	mat43::IsOutrageous() const
{
	return Row<0>().IsOutrageous() || Row<1>().IsOutrageous() || Row<2>().IsOutrageous() || Row<3>().IsOutrageous();
}


} // namespace FFTL
