
#ifndef __MATRIX44_DEFAULT_INL__
#define __MATRIX44_DEFAULT_INL__
#else
#error "Already included"
#endif // !__MATRIX44_DEFAULT_INL__

namespace FFTL
{



inline void mat44::Identity()
{
	Row<0>().Set(1, 0, 0, 0);
	Row<1>().Set(0, 1, 0, 0);
	Row<2>().Set(0, 0, 1, 0);
	Row<3>().Set(0, 0, 0, 1);
}

inline bool mat44::operator==(const mat44& rhs) const
{
	return Row<0>() == rhs.Row<0>() && Row<1>() == rhs.Row<1>() && Row<2>() == rhs.Row<2>() && Row<3>() == rhs.Row<3>();
}
inline bool mat44::operator!=(const mat44& rhs) const
{
	return Row<0>() != rhs.Row<0>() || Row<1>() != rhs.Row<1>() || Row<2>() != rhs.Row<2>() || Row<3>() != rhs.Row<3>();
}

inline bool mat44::IsNearEqual(const mat44& rhs, f32 tol) const
{
	const f32(*m)[4] = reinterpret_cast<const f32(*)[4]>(this);
	const f32(*mr)[4] = reinterpret_cast<const f32(*)[4]>(&rhs);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if(!FFTL::IsNearEqual(m[i][j], mr[i][j], tol))
				return false;
	return true;
}

inline bool mat44::IsNan() const
{
	return Row<0>() != Row<0>() || Row<1>() != Row<1>() || Row<2>() != Row<2>() || Row<3>() != Row<3>();
}
inline bool	mat44::IsInf() const
{
	return Row<0>().IsInf() || Row<1>().IsInf() || Row<2>().IsInf() || Row<3>().IsInf();
}
inline bool mat44::IsFinite() const
{
	return Row<0>().IsFinite() && Row<1>().IsFinite() && Row<2>().IsFinite() && Row<3>().IsFinite();
}
inline bool	mat44::IsOutrageous() const
{
	return Row<0>().IsOutrageous() || Row<1>().IsOutrageous() || Row<2>().IsOutrageous() || Row<3>().IsOutrageous();
}

inline void mat44::Invert()
{
	f32(*m)[4] = reinterpret_cast<f32(*)[4]>(this);

	// 2x2 sub-determinants required to calculate 4x4 determinant
	f32 det2_01_01 = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	f32 det2_01_02 = m[0][0] * m[1][2] - m[0][2] * m[1][0];
	f32 det2_01_03 = m[0][0] * m[1][3] - m[0][3] * m[1][0];
	f32 det2_01_12 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
	f32 det2_01_13 = m[0][1] * m[1][3] - m[0][3] * m[1][1];
	f32 det2_01_23 = m[0][2] * m[1][3] - m[0][3] * m[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	f32 det3_201_012 = m[2][0] * det2_01_12 - m[2][1] * det2_01_02 + m[2][2] * det2_01_01;
	f32 det3_201_013 = m[2][0] * det2_01_13 - m[2][1] * det2_01_03 + m[2][3] * det2_01_01;
	f32 det3_201_023 = m[2][0] * det2_01_23 - m[2][2] * det2_01_03 + m[2][3] * det2_01_02;
	f32 det3_201_123 = m[2][1] * det2_01_23 - m[2][2] * det2_01_13 + m[2][3] * det2_01_12;

	f64 det = (-det3_201_123 * m[3][0] + det3_201_023 * m[3][1] - det3_201_013 * m[3][2] + det3_201_012 * m[3][3]);
	f32 invDet = det == 0.0f ? 1.0f : static_cast<f32>(1.0f / det);

	// remaining 2x2 sub-determinants
	f32 det2_03_01 = m[0][0] * m[3][1] - m[0][1] * m[3][0];
	f32 det2_03_02 = m[0][0] * m[3][2] - m[0][2] * m[3][0];
	f32 det2_03_03 = m[0][0] * m[3][3] - m[0][3] * m[3][0];
	f32 det2_03_12 = m[0][1] * m[3][2] - m[0][2] * m[3][1];
	f32 det2_03_13 = m[0][1] * m[3][3] - m[0][3] * m[3][1];
	f32 det2_03_23 = m[0][2] * m[3][3] - m[0][3] * m[3][2];

	f32 det2_13_01 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
	f32 det2_13_02 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
	f32 det2_13_03 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
	f32 det2_13_12 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
	f32 det2_13_13 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
	f32 det2_13_23 = m[1][2] * m[3][3] - m[1][3] * m[3][2];

	// remaining 3x3 sub-determinants
	f32 det3_203_012 = m[2][0] * det2_03_12 - m[2][1] * det2_03_02 + m[2][2] * det2_03_01;
	f32 det3_203_013 = m[2][0] * det2_03_13 - m[2][1] * det2_03_03 + m[2][3] * det2_03_01;
	f32 det3_203_023 = m[2][0] * det2_03_23 - m[2][2] * det2_03_03 + m[2][3] * det2_03_02;
	f32 det3_203_123 = m[2][1] * det2_03_23 - m[2][2] * det2_03_13 + m[2][3] * det2_03_12;

	f32 det3_213_012 = m[2][0] * det2_13_12 - m[2][1] * det2_13_02 + m[2][2] * det2_13_01;
	f32 det3_213_013 = m[2][0] * det2_13_13 - m[2][1] * det2_13_03 + m[2][3] * det2_13_01;
	f32 det3_213_023 = m[2][0] * det2_13_23 - m[2][2] * det2_13_03 + m[2][3] * det2_13_02;
	f32 det3_213_123 = m[2][1] * det2_13_23 - m[2][2] * det2_13_13 + m[2][3] * det2_13_12;

	f32 det3_301_012 = m[3][0] * det2_01_12 - m[3][1] * det2_01_02 + m[3][2] * det2_01_01;
	f32 det3_301_013 = m[3][0] * det2_01_13 - m[3][1] * det2_01_03 + m[3][3] * det2_01_01;
	f32 det3_301_023 = m[3][0] * det2_01_23 - m[3][2] * det2_01_03 + m[3][3] * det2_01_02;
	f32 det3_301_123 = m[3][1] * det2_01_23 - m[3][2] * det2_01_13 + m[3][3] * det2_01_12;

	m[0][0] = -det3_213_123 * invDet;
	m[1][0] = +det3_213_023 * invDet;
	m[2][0] = -det3_213_013 * invDet;
	m[3][0] = +det3_213_012 * invDet;

	m[0][1] = +det3_203_123 * invDet;
	m[1][1] = -det3_203_023 * invDet;
	m[2][1] = +det3_203_013 * invDet;
	m[3][1] = -det3_203_012 * invDet;

	m[0][2] = +det3_301_123 * invDet;
	m[1][2] = -det3_301_023 * invDet;
	m[2][2] = +det3_301_013 * invDet;
	m[3][2] = -det3_301_012 * invDet;

	m[0][3] = -det3_201_123 * invDet;
	m[1][3] = +det3_201_023 * invDet;
	m[2][3] = -det3_201_013 * invDet;
	m[3][3] = +det3_201_012 * invDet;
}




} // namespace FFTL
