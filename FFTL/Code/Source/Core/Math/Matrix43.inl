
#ifndef __MATRIX43_INL__
#define __MATRIX43_INL__
#else
#error "Already included"
#endif // !__MATRIX43_INL__

#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix33.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4127)
#endif

#if defined(FFTL_SSE)
#	include "SSE/Matrix43_SSE.inl"
#elif(defined(FFTL_ARM_NEON))
#	include "NEON/Matrix43_NEON.inl"
#else
#	include "Default/Matrix43_Default.inl"
#endif

namespace FFTL
{


inline mat43::mat43()
{
}

inline mat43::mat43(enIndentityType)
{
	Identity();
}

inline mat43::mat43(const mat43& mat)
{
	CopyFrom(mat);
}

inline mat43::mat43(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
{
	Row<0>() = a;
	Row<1>() = b;
	Row<2>() = c;
	Row<3>() = d;
}

inline mat43::mat43(const mat33& axis, const vec3& pos)
{
	Row<0>() = axis.Row<0>();
	Row<1>() = axis.Row<1>();
	Row<2>() = axis.Row<2>();
	Row<3>() = pos;
}
inline mat43::mat43(const vec3& pos, const mat33& axis)
{
	Row<0>() = axis.Row<0>();
	Row<1>() = axis.Row<1>();
	Row<2>() = axis.Row<2>();
	Row<3>() = pos;
}

template<bool ALIGNED, bool TRANSPOSED>
inline mat43::mat43(const mat3Store<ALIGNED, TRANSPOSED>& axis, const vec3& pos)
{
	Row<0>() = axis.LoadRow0();
	Row<1>() = axis.LoadRow1();
	Row<2>() = axis.LoadRow2();
	Row<3>() = pos;
}
template<bool ALIGNED, bool TRANSPOSED>
inline mat43::mat43(const vec3& pos, const mat3Store<ALIGNED, TRANSPOSED>& axis)
{
	Row<0>() = axis.LoadRow0();
	Row<1>() = axis.LoadRow1();
	Row<2>() = axis.LoadRow2();
	Row<3>() = pos;
}

//inline mat43::mat43(const f32 vals[12])
//{
//	//	TODO: LoadU 3x then shuffle
//	Row<0>() = vec3::loadU(vals + 0);
//	Row<1>() = vec3::loadU(vals + 3);
//	Row<2>() = vec3::loadU(vals + 6);
//	Row<3>() = vec3::loadU(vals + 9);
//}
//inline mat43::mat43(const f32 vals[4][3])
//{
//	//	TODO: LoadU 3x then shuffle
//	Row<0>() = vec3::loadU(vals[0]);
//	Row<1>() = vec3::loadU(vals[1]);
//	Row<2>() = vec3::loadU(vals[2]);
//	Row<3>() = vec3::loadU(vals[3]);
//}

inline void mat43::zero()
{
	const vec3 v = vec3::Zero();

	Row<0>() = v;
	Row<1>() = v;
	Row<2>() = v;
	Row<3>() = v;
}

inline mat43& mat43::operator=(const mat43& rhs)
{
	CopyFrom(rhs); return (*this);
}

inline mat43 mat43::operator*(f32 rhs) const
{
	const vec3 v = vec3::Splat(rhs);
	return mat43(
		Row<0>() * v,
		Row<1>() * v,
		Row<2>() * v,
		Row<3>() * v
		);
}
inline mat43& mat43::operator*=(f32 rhs)
{
	const vec3 v = vec3::Splat(rhs);
	Row<0>() *= v;
	Row<1>() *= v;
	Row<2>() *= v;
	Row<3>() *= v;

	return *this;
}
inline mat43 mat43::operator/(f32 rhs) const
{
	const vec3 v = vec3::Splat(rhs);
	return mat43(
		Row<0>() / v,
		Row<1>() / v,
		Row<2>() / v,
		Row<3>() / v
		);
}
inline mat43& mat43::operator/=(f32 rhs)
{
	const vec3 v = vec3::Splat(rhs);
	Row<0>() /= v;
	Row<1>() /= v;
	Row<2>() /= v;
	Row<3>() /= v;

	return *this;
}
inline mat43 mat43::operator+(const mat43& rhs) const
{
	return mat43(
		Row<0>() + rhs.Row<0>(),
		Row<1>() + rhs.Row<1>(),
		Row<2>() + rhs.Row<2>(),
		Row<3>() + rhs.Row<3>()
		);
}
inline mat43& mat43::operator+=(const mat43& rhs)
{
	Row<0>() += rhs.Row<0>();
	Row<1>() += rhs.Row<1>();
	Row<2>() += rhs.Row<2>();
	Row<3>() += rhs.Row<3>();

	return *this;
}
inline mat43 mat43::operator-(const mat43& rhs) const
{
	return mat43(
		Row<0>() - rhs.Row<0>(),
		Row<1>() - rhs.Row<1>(),
		Row<2>() - rhs.Row<2>(),
		Row<3>() - rhs.Row<3>()
		);
}
inline mat43& mat43::operator-=(const mat43& rhs)
{
	Row<0>() -= rhs.Row<0>();
	Row<1>() -= rhs.Row<1>();
	Row<2>() -= rhs.Row<2>();
	Row<3>() -= rhs.Row<3>();

	return *this;
}


//inline void mat43::store(f32* m) const
//{
//	Row<0>().StoreU(m + 0);
//	Row<1>().StoreU(m + 4);
//	Row<2>().StoreU(m + 8);
//	Row<3>().StoreU(m + 12);
//}
//
//inline void mat43::toArray(f32* m) const
//{
//	store(m);
//}

inline const f32* mat43::Ptr() const
{
	return reinterpret_cast<const f32*>(this);
}
inline f32* mat43::Ptr()
{
	return reinterpret_cast<f32*>(this);
}

inline const mat33& mat43::AsMat33() const
{
	return *reinterpret_cast<const mat33*>(this);
}
inline mat33& mat43::AsMat33()
{
	return *reinterpret_cast<mat33*>(this);
}
inline const mat44& mat43::AsMat44() const
{
	return *reinterpret_cast<const mat44*>(this);
}
inline mat44& mat43::AsMat44()
{
	return *reinterpret_cast<mat44*>(this);
}

inline bool mat43::IsOrthogonal3x3() const
{
	return AsMat33().IsOrthogonal();
}

inline bool mat43::IsOrthonormal3x3() const
{
	return AsMat33().IsOrthonormal();
}

inline bool mat43::IsOrthonormal() const
{
	return IsOrthonormal3x3();
}

inline f32 mat43::Determinant3x3() const
{
	return AsMat33().Determinant();
}

inline const vec3& mat43::Right() const
{
	return Row<0>();
}
inline const vec3& mat43::Forward() const
{
	return Row<1>();
}
inline const vec3& mat43::Up() const
{
	return Row<2>();
}
inline const vec3& mat43::Position() const
{
	return Row<3>();
}
inline vec3& mat43::Right()
{
	return Row<0>();
}
inline vec3& mat43::Forward()
{
	return Row<1>();
}
inline vec3& mat43::Up()
{
	return Row<2>();
}
inline vec3& mat43::Position()
{
	return Row<3>();
}

template<uint ROW>
inline const vec3& mat43::Row() const
{
	static_assert(ROW <= 3, "Only 1st 3 rows are valid");
	return m_v[ROW].CastTo<3>();
}
template<uint ROW>
inline vec3& mat43::Row()
{
	static_assert(ROW <= 3, "Only 1st 3 rows are valid");
	return m_v[ROW].CastTo<3>();
}

inline vec3 mat43::GetAngles() const
{
	return AsMat33().GetAngles();
}

inline vec3 mat43::GetScale() const
{
	return AsMat33().GetScale();
}

inline vec3 mat43::GetScale_sq() const
{
	return AsMat33().GetScale_sq();
}

inline vec3 mat43::Transform(const vec3& p) const
{
	vec3 a = Row<0>();
	vec3 b = Row<1>();
	vec3 c = Row<2>();

	vec3 r;
	r = XXXX(p) * a;
	r = V4fAddMul(r, YYYY(p), b);
	r = V4fAddMul(r, ZZZZ(p), c);
	r += Row<3>();

	return r;
}

inline vec3 mat43::Transform3x3(const vec3& p) const
{
	return AsMat33().Transform(p);
}

//inline vec3 mat43::TransformTransposed(const vec3& p) const
//{
//	//	TODO!
//	const mat43 tr = getTranspose();
//	return tr.transform(p);
//}

//inline vec3 mat43::TransformVectorTransposed(const vec3& p) const
//{
//	vec3 a = Row<0>();
//	vec3 b = Row<1>();
//	vec3 c = Row<2>();
//	FFTL::transpose3x3(a, b, c);
//
//	vec3 r;
//	r =  XXXX(p) * a;
//	r += YYYY(p) * b;
//	r += ZZZZ(p) * c;
//
//	return r;
//}

inline mat43 mat43::CreateLookAt(const vec3& lookAt, const vec3& position)
{
	mat43 m;
	m.Position() = position;
	m.Lookat(lookAt);
	return m;
}

inline void mat43::Lookat(const vec3& target)
{
	AsMat33().Lookat(target, Position());
}
inline void mat43::Orient(const vec3& vFw, const vec3& _up)
{
	AsMat33().Orient(vFw, _up);
}
inline void mat43::Orient(const vec3& vFw)
{
	AsMat33().Orient(vFw);
}

inline void mat43::TranslatePre(f32 x, f32 y, f32 z)
{
	Row<3>() += Row<0>() * x + Row<1>() * y + Row<2>() * z;
}

inline void mat43::TranslatePre(const vec3& v)
{
	Row<3>() += Row<0>() * XXXX(v) + Row<1>() * YYYY(v) + Row<2>() * ZZZZ(v);
}

inline void mat43::TranslatePost(const vec3& v)
{
	Row<3>() += v;
}

inline void mat43::ScalePre(const vec3& v)
{
	AsMat33().ScalePre(v);
}

inline void mat43::ScalePre(f32 sx, f32 sy, f32 sz)
{
	AsMat33().ScalePre(sx, sy, sz);
}

inline void mat43::ScalePre(f32 s)
{
	AsMat33().ScalePre(s);
}

inline void mat43::ScalePost(const vec3& v)
{
	Row<0>() *= v;
	Row<1>() *= v;
	Row<2>() *= v;
	Row<3>() *= v;

	//	m[0][0] *= sx; m[0][1] *= sy; m[0][2] *= sz;
	//	m[1][0] *= sx; m[1][1] *= sy; m[1][2] *= sz;
	//	m[2][0] *= sx; m[2][1] *= sy; m[2][2] *= sz;
	//	m[3][0] *= sx; m[3][1] *= sy; m[3][2] *= sz;
}

inline void mat43::ScalePost(f32 s)
{
	ScalePost(vec3::Splat(s));
}

inline void mat43::Invert3x3()
{
	AsMat33().Invert();
}

inline void mat43::Invert()
{
	Invert3x3();
	Position() = -Mul3x3(Position(), *this);
}

inline void mat43::InvertOrthonormal()
{
	FFTL_MATH_ASSERT(IsOrthonormal());

	//	Run the translation component backwards through the 3x3 transform
	Row<3>() = -Mul3x3(*this, Row<3>());

	// Transpose 3x3 part
	Transpose3x3();
}

inline void mat43::Transpose3x3()
{
	FFTL::Transpose3x3(Row<0>(), Row<1>(), Row<2>());
}

inline void mat43::Orthogonalize()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();

	f = f - Project(r, f);
	u = u - Project(r, u) + Project(f, u);

	FFTL_MATH_ASSERT(IsOrthogonal3x3());
}

inline void mat43::Orthonormalize()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();
	vec3& p = Position();

	r = Normalize(r);
	f = Normalize(f - (DotV(r, f) * r));
	u = Normalize(u - (DotV(r, u) * r + DotV(f, u) * f));
	p.InsertW(1.f);

	FFTL_MATH_ASSERT(IsOrthonormal3x3());
}

inline void mat43::OrthonormalizeFast()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();
	vec3& p = Position();

	r = NormalizeFast(r);
	f = NormalizeFast(f - (DotV(r, f) * r));
	u = NormalizeFast(u - (DotV(r, u) * r + DotV(f, u) * f));
	p.InsertW(1.f);
}

inline void mat43::CopyFrom(const mat43& rhs)
{
	Row<0>() = rhs.Row<0>();
	Row<1>() = rhs.Row<1>();
	Row<2>() = rhs.Row<2>();
	Row<3>() = rhs.Row<3>();
}

inline void mat43::NormalizeAxis()
{
	AsMat33() = Normalize(AsMat33());
}
inline void mat43::NormalizeAxisFast()
{
	AsMat33() = NormalizeFast(AsMat33());
}
inline void mat43::NormalizeAxisSafe()
{
	AsMat33() = NormalizeSafe(AsMat33());
}

inline void Mul(const mat43& a, const mat43& b, mat43& out)
{
	FFTL_MATH_ASSERT(&out != &a && &out != &b, "Bad usage of trinary mul operator");

	vec3 r, x, y, z;

	const vec3 b0 = b.Row<0>();
	const vec3 b1 = b.Row<1>();
	const vec3 b2 = b.Row<2>();

	//	3x3 transform
	const vec3 a0 = a.Row<0>();
	x = Permute<0, 0, 0, 0>(a0);
	y = Permute<1, 1, 1, 1>(a0);
	z = Permute<2, 2, 2, 2>(a0);
	r = V4fMul(x, b0);
	r = V4fAddMul(r, y, b1);
	r = V4fAddMul(r, z, b2);
	out.Row<0>() = r;

	//	3x3 transform
	const vec3 a1 = a.Row<1>();
	x = Permute<0, 0, 0, 0>(a1);
	y = Permute<1, 1, 1, 1>(a1);
	z = Permute<2, 2, 2, 2>(a1);
	r = V4fMul(x, b0);
	r = V4fAddMul(r, y, b1);
	r = V4fAddMul(r, z, b2);
	out.Row<1>() = r;

	//	3x3 transform
	const vec3 a2 = a.Row<2>();
	x = Permute<0, 0, 0, 0>(a2);
	y = Permute<1, 1, 1, 1>(a2);
	z = Permute<2, 2, 2, 2>(a2);
	r = V4fMul(x, b0);
	r = V4fAddMul(r, y, b1);
	r = V4fAddMul(r, z, b2);
	out.Row<2>() = r;

	//	4x3 transform
	const vec3 a3 = a.Row<3>();
	const vec3 b3 = b.Row<3>();
	x = Permute<0, 0, 0, 0>(a3);
	y = Permute<1, 1, 1, 1>(a3);
	z = Permute<2, 2, 2, 2>(a3);
	r = V4fMul(x, b0);
	r = V4fAddMul(r, y, b1);
	r = V4fAddMul(r, z, b2);
	r += b3;
	out.Row<3>() = r;
}

inline void Mul(const mat43& a, const mat33& b, mat43& out)
{
	FFTL_MATH_ASSERT((void*)&out != (void*)&a && (void*)&out != (void*)&b);

	out.Row<0>() = b.Transform(a.Row<0>());
	out.Row<1>() = b.Transform(a.Row<1>());
	out.Row<2>() = b.Transform(a.Row<2>());
	out.Row<3>() = b.Transform(a.Row<3>());
}

inline mat43 Mul(const mat43& a, const mat33& b)
{
	mat43 out;
	Mul(a, b, out);
	return out;
}

inline mat43 Mul(const mat43& a, const mat43& b)
{
	mat43 out;
	Mul(a, b, out);
	return out;
}

inline void Mul3x3(const mat43& a, const mat43& b, mat43& out)
{
	Mul(a.AsMat33(), b.AsMat33(), out.AsMat33());
	out.Row<0>().Zero();
}

inline mat43 Mul3x3(const mat43& a, const mat43& b)
{
	mat43 r;
	Mul3x3(a, b, r);
	return r;
}

inline void Mul3x3Part(const mat43& a, const mat43& b, mat43& out)
{
	FFTL_MATH_ASSERT(&out != &a && &out != &b);

	out.Row<0>() = b.Transform3x3(a.Row<0>());
	out.Row<1>() = b.Transform3x3(a.Row<1>());
	out.Row<2>() = b.Transform3x3(a.Row<2>());
	out.Row<3>().Zero();
}

inline mat43 Mul3x3Part(const mat43& a, const mat43& b)
{
	mat43 r;
	Mul3x3Part(a, b, r);
	return r;
}

inline vec3 Mul3x3(const mat43& m, const vec3& v)
{
	const f32 x = Dot(v, m.Row<0>());
	const f32 y = Dot(v, m.Row<1>());
	const f32 z = Dot(v, m.Row<2>());
	return vec3(x, y, z);
}

inline vec3 Mul3x3(const vec3& v, const mat43& m)
{
	return m.Transform3x3(v);
}

inline mat43 Add(const mat43& a, const mat43& b)
{
	return a + b;
}
inline mat43 Sub(const mat43& a, const mat43& b)
{
	return a - b;
}
inline mat43 Abs(const mat43& a)
{
	return mat43(
		Abs(a.Row<0>()),
		Abs(a.Row<1>()),
		Abs(a.Row<2>()),
		Abs(a.Row<3>())
		);
}




template<bool ALIGNED, bool TRANSPOSED>
inline mat43StoreBase<ALIGNED, TRANSPOSED>::mat43StoreBase() {}

template<bool ALIGNED, bool TRANSPOSED>
inline mat43StoreBase<ALIGNED, TRANSPOSED>::mat43StoreBase(mat43::enIndentityType)
{
	Identity();
}

template<bool ALIGNED, bool TRANSPOSED>
inline void mat43StoreBase<ALIGNED, TRANSPOSED>::Identity()
{
	f32* m_v = Ptr();
	//	Storing (1,0,0,0) 3 times will give us what we need because of the 4/3 offset
	const vec4 idVec(1.f);
	idVec.Store<ALIGNED>(m_v + 0);
	idVec.Store<ALIGNED>(m_v + 4);
	idVec.Store<ALIGNED>(m_v + 8);
}

template<bool ALIGNED, bool TRANSPOSED>
inline void mat43StoreBase<ALIGNED, TRANSPOSED>::Store(const mat43& in)
{
	FFTL_MATH_ASSERT(!ALIGNED || (size_t(this) & 15) == 0); // ensure target destination is aligned to 16 bytes
	f32* m_v = Ptr();

	if (TRANSPOSED)
	{
		vec4 r0 = in.Row<0>().CastTo<4>();
		vec4 r1 = in.Row<1>().CastTo<4>();
		vec4 r2 = in.Row<2>().CastTo<4>();
		vec4 r3 = in.Row<3>().CastTo<4>();
		FFTL::Transpose4x3(r0, r1, r2, r3);
		r0.Store<ALIGNED>(m_v + 0);
		r1.Store<ALIGNED>(m_v + 4);
		r2.Store<ALIGNED>(m_v + 8);
		//	r3 isn't needed after transposing because all the data we need gets packed into the 1st 3 rows
	}
	else
	{
		const vec4 r0 = in.Row<0>().CastTo<4>();
		const vec4 r1 = in.Row<1>().CastTo<4>();
		const vec4 r2 = in.Row<2>().CastTo<4>();
		const vec4 r3 = in.Row<3>().CastTo<4>();
		const vec4 a = Permute<A0, A1, A2, B0>(r0, r1);
		const vec4 b = Permute<A1, A2, B0, B1>(r1, r2);
		const vec4 c = Permute<A2, B0, B1, B2>(r2, r3);
		a.Store<ALIGNED>(m_v + 0);
		b.Store<ALIGNED>(m_v + 4);
		c.Store<ALIGNED>(m_v + 8);
	}
}

template<bool ALIGNED, bool TRANSPOSED>
inline void mat43StoreBase<ALIGNED, TRANSPOSED>::Load(mat43& out) const
{
	FFTL_MATH_ASSERT(!ALIGNED || (size_t(this) & 15) == 0); // ensure target destination is aligned to 16 bytes
	const f32* m_v = Ptr();

	vec4 a = vec4::Load<ALIGNED>(m_v + 0);
	vec4 b = vec4::Load<ALIGNED>(m_v + 4);
	vec4 c = vec4::Load<ALIGNED>(m_v + 8);

	if (TRANSPOSED)
	{
		vec4 d;
		Transpose3x4(a, b, c, d);
		out.Row<0>() = a.CastTo<3>();
		out.Row<1>() = b.CastTo<3>();
		out.Row<2>() = c.CastTo<3>();
		out.Row<3>() = d.CastTo<3>();
	}
	else
	{
		out.Row<0>() = a.CastTo<3>();
		out.Row<1>() = Permute<A3, B0, B1, B2>(a, b).CastTo<3>();
		out.Row<2>() = Permute<A2, A3, B0, B1>(b, c).CastTo<3>();
		out.Row<3>() = Permute<1, 2, 3, 0>(c).CastTo<3>();
	}
}

template<bool ALIGNED, bool TRANSPOSED>
inline void mat43StoreBase<ALIGNED, TRANSPOSED>::Load3x3(mat33& out) const
{
	FFTL_MATH_ASSERT(!ALIGNED || (size_t(this) & 15) == 0); // ensure target destination is aligned to 16 bytes
	const f32* m_v = Ptr();

	vec4 a = vec4::Load<ALIGNED>(m_v + 0);
	vec4 b = vec4::Load<ALIGNED>(m_v + 4);
	vec4 c = vec4::Load<ALIGNED>(m_v + 8);

	if (TRANSPOSED)
	{
		Transpose3x4_3x3(a, b, c);
		out.Row<0>() = a.CastTo<3>();
		out.Row<1>() = b.CastTo<3>();
		out.Row<2>() = c.CastTo<3>();
	}
	else
	{
		out.Row<0>() = a.CastTo<3>();
		out.Row<1>() = Permute<A3, B0, B1, B2>(a, b).CastTo<3>();
		out.Row<2>() = Permute<A2, A3, B0, B1>(b, c).CastTo<3>();
	}
}

template<bool ALIGNED, bool TRANSPOSED>
template<uint ROW>
inline vec3 mat43StoreBase<ALIGNED, TRANSPOSED>::LoadRow() const
{
	static_assert(ROW < 4, "Only 4 rows in a 4x3 matrix");
	const f32* m_v = Ptr();
	if (TRANSPOSED)
		return vec3(m_v[0 + ROW], m_v[4 + ROW], m_v[8 + ROW]);
	else
		return ALIGNED ? vec4::LoadA(m_v + 3 * ROW).CastTo<3>() : vec4::LoadU(m_v + 3 * ROW).CastTo<3>();
}

//	Create from condensed data
template<bool ALIGNED, bool TRANSPOSED>
inline mat43::mat43(const mat43Store<ALIGNED, TRANSPOSED>& mat)
{
	mat.Load(*this);
}

template<bool ALIGNED, bool TRANSPOSED>
inline mat43& mat43::operator=(const mat43Store<ALIGNED, TRANSPOSED>& mat)
{
	mat.Load(*this);
	return *this;
}



} // namespace FFTL

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

