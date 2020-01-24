
#ifndef __MATRIX44_INL__
#define __MATRIX44_INL__
#else
#error "Already included"
#endif // !__MATRIX44_INL__

#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix33.h"
#include "Matrix43.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4127)
#endif

#if defined(FFTL_SSE)
#	include "SSE/Matrix44_SSE.inl"
#elif(defined(FFTL_ARM_NEON))
#	include "NEON/Matrix44_NEON.inl"
#else
#	include "Default/Matrix44_Default.inl"
#endif

namespace FFTL
{


FFTL_FORCEINLINE mat44::mat44()
{
}

FFTL_FORCEINLINE mat44::mat44(const mat44& mat)
{
	CopyFrom(mat);
}

FFTL_FORCEINLINE mat44::mat44(const vec4& a, const vec4& b, const vec4& c, const vec4& d)
{
	Row<0>() = a;
	Row<1>() = b;
	Row<2>() = c;
	Row<3>() = d;
}

FFTL_FORCEINLINE mat44::mat44(const f32 vals[16])
{
	Row<0>() = vec4::LoadU(vals + 0);
	Row<1>() = vec4::LoadU(vals + 4);
	Row<2>() = vec4::LoadU(vals + 8);
	Row<3>() = vec4::LoadU(vals + 12);
}
FFTL_FORCEINLINE mat44::mat44(const f32 vals[4][4])
{
	Row<0>() = vec4::LoadU(vals[0]);
	Row<1>() = vec4::LoadU(vals[1]);
	Row<2>() = vec4::LoadU(vals[2]);
	Row<3>() = vec4::LoadU(vals[3]);
}

FFTL_FORCEINLINE void mat44::Zero()
{
	const vec4 v = vec4::Zero();

	Row<0>() = v;
	Row<1>() = v;
	Row<2>() = v;
	Row<3>() = v;
}

FFTL_FORCEINLINE mat44& mat44::operator=(const mat44& rhs)
{
	CopyFrom(rhs); return (*this);
}

FFTL_FORCEINLINE mat44 mat44::operator*(f32 rhs) const
{
	vec4 v;
	v.SetAll(rhs);

	return mat44(
		Row<0>() * v,
		Row<1>() * v,
		Row<2>() * v,
		Row<3>() * v
		);
}
FFTL_FORCEINLINE mat44& mat44::operator*=(f32 rhs)
{
	vec4 v;
	v.SetAll(rhs);

	Row<0>() *= v;
	Row<1>() *= v;
	Row<2>() *= v;
	Row<3>() *= v;

	return *this;
}
FFTL_FORCEINLINE mat44 mat44::operator/(f32 rhs) const
{
	vec4 v;
	v.SetAll(rhs);

	return mat44(
		Row<0>() / v,
		Row<1>() / v,
		Row<2>() / v,
		Row<3>() / v
		);
}
FFTL_FORCEINLINE mat44& mat44::operator/=(f32 rhs)
{
	vec4 v;
	v.SetAll(rhs);

	Row<0>() /= v;
	Row<1>() /= v;
	Row<2>() /= v;
	Row<3>() /= v;

	return *this;
}
FFTL_FORCEINLINE mat44 mat44::operator+(const mat44& rhs) const
{
	return mat44(
		Row<0>() + rhs.Row<0>(),
		Row<1>() + rhs.Row<1>(),
		Row<2>() + rhs.Row<2>(),
		Row<3>() + rhs.Row<3>()
		);
}
FFTL_FORCEINLINE mat44& mat44::operator+=(const mat44& rhs)
{
	Row<0>() += rhs.Row<0>();
	Row<1>() += rhs.Row<1>();
	Row<2>() += rhs.Row<2>();
	Row<3>() += rhs.Row<3>();

	return *this;
}
FFTL_FORCEINLINE mat44 mat44::operator-(const mat44& rhs) const
{
	return mat44(
		Row<0>() - rhs.Row<0>(),
		Row<1>() - rhs.Row<1>(),
		Row<2>() - rhs.Row<2>(),
		Row<3>() - rhs.Row<3>()
		);
}
FFTL_FORCEINLINE mat44& mat44::operator-=(const mat44& rhs)
{
	Row<0>() -= rhs.Row<0>();
	Row<1>() -= rhs.Row<1>();
	Row<2>() -= rhs.Row<2>();
	Row<3>() -= rhs.Row<3>();

	return *this;
}

FFTL_FORCEINLINE void Store(f32* p, const mat44& m)
{
	StoreU(p +  0, m.Row<0>());
	StoreU(p +  4, m.Row<1>());
	StoreU(p +  8, m.Row<2>());
	StoreU(p + 12, m.Row<3>());
}
FFTL_FORCEINLINE void StoreA(f32* p, const mat44& m)
{
	StoreA(p +  0, m.Row<0>());
	StoreA(p +  4, m.Row<1>());
	StoreA(p +  8, m.Row<2>());
	StoreA(p + 12, m.Row<3>());
}

FFTL_FORCEINLINE const f32* mat44::Ptr() const
{
	return reinterpret_cast<const f32*>(this);
}
FFTL_FORCEINLINE f32* mat44::Ptr()
{
	return reinterpret_cast<f32*>(this);
}
FFTL_FORCEINLINE const mat33& mat44::AsMat33() const
{
	return *reinterpret_cast<const mat33*>(this);
}
FFTL_FORCEINLINE mat33& mat44::AsMat33()
{
	return *reinterpret_cast<mat33*>(this);
}
FFTL_FORCEINLINE const mat43& mat44::AsMat43f() const
{
	return *reinterpret_cast<const mat43*>(this);
}
FFTL_FORCEINLINE mat43& mat44::AsMat43f()
{
	return *reinterpret_cast<mat43*>(this);
}

FFTL_FORCEINLINE f32 mat44::Determinant() const
{
	return Dot(Row<0>(), Cross(Row<1>(), Row<2>(), Row<3>()));
}

template<uint row>
FFTL_FORCEINLINE const vec4& mat44::Row() const
{
	static_assert(row <= 3, "Only 1st 4 rows are valid");
	return m_v[row];
}
template<uint row>
FFTL_FORCEINLINE vec4& mat44::Row()
{
	static_assert(row <= 3, "Only 1st 4 rows are valid");
	return m_v[row];
}

FFTL_FORCEINLINE vec4 mat44::TransformTransposed(const vec4& p) const
{
	const mat44 tr = GetTranspose();
	return tr.Transform(p);
}

FFTL_FORCEINLINE vec4 mat44::Transform(const vec4& p) const
{
	vec4 r;
	r =  XXXX(p) * Row<0>();
	r += YYYY(p) * Row<1>();
	r += ZZZZ(p) * Row<2>();
	r += WWWW(p) * Row<3>();
	return r;
}

FFTL_FORCEINLINE void mat44::Transpose()
{
	FFTL::Transpose4x4(Row<0>(), Row<1>(), Row<2>(), Row<3>());
}

FFTL_FORCEINLINE void mat44::CopyFrom(const mat44& rhs)
{
	Row<0>() = rhs.Row<0>();
	Row<1>() = rhs.Row<1>();
	Row<2>() = rhs.Row<2>();
	Row<3>() = rhs.Row<3>();
}

FFTL_FORCEINLINE void Mul4x4(const mat44& a, const mat44& b, mat44& out)
{
	out.Row<0>() = b.Transform(a.Row<0>());
	out.Row<1>() = b.Transform(a.Row<1>());
	out.Row<2>() = b.Transform(a.Row<2>());
	out.Row<3>() = b.Transform(a.Row<3>());
}

FFTL_FORCEINLINE mat44 Mul4x4(const mat44& a, const mat44& b)
{
	mat44 r;
	Mul4x4(a, b, r);
	return r;
}

FFTL_FORCEINLINE mat44 Add(const mat44& a, const mat44& b)
{
	return a + b;
}
FFTL_FORCEINLINE mat44 Sub(const mat44& a, const mat44& b)
{
	return a - b;
}
FFTL_FORCEINLINE mat44 Abs(const mat44& a)
{
	return mat44(
		Abs(a.Row<0>()),
		Abs(a.Row<1>()),
		Abs(a.Row<2>()),
		Abs(a.Row<3>())
		);
}




} // namespace FFTL


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

