#ifndef __MATRIX33_INL__
#define __MATRIX33_INL__
#else
#error "Already included"
#endif // !__MATRIX33_INL__

#include "Vector3.h"
#include "Quaternion.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4127)
#endif

#if defined(FFTL_SSE)
#	include "SSE/Matrix33_SSE.inl"
#elif(defined(FFTL_ARM_NEON))
#	include "NEON/Matrix33_NEON.inl"
#else
#	include "Default/Matrix33_Default.inl"
#endif

namespace FFTL
{


FFTL_FORCEINLINE mat33::mat33()
= default;

FFTL_FORCEINLINE mat33::mat33(enIndentityType)
{
	Identity();
}

FFTL_FORCEINLINE mat33::mat33(const mat33& mat)
{
	CopyFrom(mat);
}

FFTL_FORCEINLINE mat33::mat33(const vec3& row0, const vec3& row1, const vec3& row2)
{
	Row<0>() = row0;
	Row<1>() = row1;
	Row<2>() = row2;
}

FFTL_FORCEINLINE void mat33::Set(const vec3& right, const vec3& fwd, const vec3& up)
{
	Row<0>() = right;
	Row<1>() = fwd;
	Row<2>() = up;
}

FFTL_FORCEINLINE void mat33::SetRotation(const quat& q)
{
	const vec4 q2 = q.AsVec4() + q.AsVec4();

	const vec4 x = XXXX(q.AsVec4()) * q2;
	const vec4 w = WWWW(q.AsVec4()) * q2;

	const f32 xx = x.GetX();
	const f32 xy = x.GetY();
	const f32 xz = x.GetZ();

	const f32 qY = q.GetY();
	const f32 q2Z = q2.GetZ();
	const f32 yy = qY * q2.GetY();
	const f32 yz = qY * q2Z;
	const f32 zz = q.GetZ() * q2Z;

	const f32 wx = w.GetX();
	const f32 wy = w.GetY();
	const f32 wz = w.GetZ();

	Row<0>().Set(
		1.0f - (yy + zz),
		xy - wz,
		xz + wy);

	Row<1>().Set(
		xy + wz,
		1.0f - (xx + zz),
		yz - wx);

	Row<2>().Set(
		xz - wy,
		yz + wx,
		1.0f - (xx + yy));
}

FFTL_FORCEINLINE void mat33::Zero()
{
	const vec3 v = vec3::Zero();

	Row<0>() = v;
	Row<1>() = v;
	Row<2>() = v;
}

FFTL_FORCEINLINE mat33& mat33::operator=(const mat33& rhs)
{
	CopyFrom(rhs); return (*this);
}

FFTL_FORCEINLINE mat33 mat33::operator*(f32 rhs) const
{
	const vec3 v = vec3::Splat(rhs);

	return mat33(
		Row<0>() * v,
		Row<1>() * v,
		Row<2>() * v
		);
}
FFTL_FORCEINLINE mat33 mat33::operator*(const mat33& rhs) const
{
	return Mul(*this, rhs);
}
FFTL_FORCEINLINE mat33& mat33::operator*=(f32 rhs)
{
	const vec3 v = vec3::Splat(rhs);

	Row<0>() *= v;
	Row<1>() *= v;
	Row<2>() *= v;

	return *this;
}
FFTL_FORCEINLINE mat33& mat33::operator*=(const mat33& rhs)
{
	return *this = Mul(*this, rhs);
}
FFTL_FORCEINLINE mat33 mat33::operator/(f32 rhs) const
{
	const vec3 v = vec3::Splat(rhs);

	return mat33(
		Row<0>() / v,
		Row<1>() / v,
		Row<2>() / v
		);
}
FFTL_FORCEINLINE mat33& mat33::operator/=(f32 rhs)
{
	const vec3 v = vec3::Splat(rhs);

	Row<0>() /= v;
	Row<1>() /= v;
	Row<2>() /= v;

	return *this;
}
FFTL_FORCEINLINE mat33 mat33::operator+(const mat33& rhs) const
{
	return mat33(
		Row<0>() + rhs.Row<0>(),
		Row<1>() + rhs.Row<1>(),
		Row<2>() + rhs.Row<2>()
		);
}
FFTL_FORCEINLINE mat33& mat33::operator+=(const mat33& rhs)
{
	Row<0>() += rhs.Row<0>();
	Row<1>() += rhs.Row<1>();
	Row<2>() += rhs.Row<2>();

	return *this;
}
FFTL_FORCEINLINE mat33 mat33::operator-(const mat33& rhs) const
{
	return mat33(
		Row<0>() - rhs.Row<0>(),
		Row<1>() - rhs.Row<1>(),
		Row<2>() - rhs.Row<2>()
		);
}
FFTL_FORCEINLINE mat33& mat33::operator-=(const mat33& rhs)
{
	Row<0>() -= rhs.Row<0>();
	Row<1>() -= rhs.Row<1>();
	Row<2>() -= rhs.Row<2>();

	return *this;
}

FFTL_FORCEINLINE void mat33::Store(f32* m) const
{
	StoreU(m + 0, Row<0>());
	StoreU(m + 3, Row<1>());
	StoreU(m + 6, Row<2>());
}

FFTL_FORCEINLINE const f32* mat33::Ptr() const
{
	return reinterpret_cast<const f32*>(this);
}
FFTL_FORCEINLINE f32* mat33::Ptr()
{
	return reinterpret_cast<f32*>(this);
}

FFTL_FORCEINLINE bool mat33::IsOrthogonal() const
{
	constexpr f32 TOL_MATRIX_ORTH = 0.01f;

	// Check for orthogonal axes
	const f32 xSq = Length_sq(Row<0>());
	const f32 ySq = Length_sq(Row<1>());
	const f32 zSq = Length_sq(Row<2>());

	//	Check for near equality in one go by vectorizing
	const vec3 vTols = vec3(Max(xSq, ySq), Max(ySq, zSq), Max(zSq, xSq)) * TOL_MATRIX_ORTH;
	const f32 d0 = Dot(Row<0>(), Row<1>());
	const f32 d1 = Dot(Row<1>(), Row<2>());
	const f32 d2 = Dot(Row<2>(), Row<0>());
	const vec3 vDots(d0, d1, d2);
	return IsNearZero(vDots, vTols);
}

FFTL_FORCEINLINE bool mat33::IsOrthonormal() const
{
	constexpr f32 TOL_MATRIX_ORTH = 0.01f;

	// OrthoNORMAL = orthoGONAL && all axes unit length
	const f32 xSq = Length_sq(Row<0>());
	const f32 ySq = Length_sq(Row<1>());
	const f32 zSq = Length_sq(Row<2>());

	//	Check for near equality in one go by vectorizing
	const vec3 vTols = vec3(Max(xSq, ySq), Max(ySq, zSq), Max(zSq, xSq)) * TOL_MATRIX_ORTH;
	const vec3 vLsqs(xSq, ySq, zSq);
	const f32 d0 = Dot(Row<0>(), Row<1>());
	const f32 d1 = Dot(Row<1>(), Row<2>());
	const f32 d2 = Dot(Row<2>(), Row<0>());
	const vec3 vDots(d0, d1, d2);
	return ((IsNearZeroV(vDots, vTols) & IsNearEqualV(vLsqs, vec3::Splat(1.f), TOL_VECTOR_LEN)).ToIntMask() & 7) == 7;
}

FFTL_FORCEINLINE f32 mat33::Determinant() const
{
	return Dot(Row<0>(), Cross(Row<1>(), Row<2>()));
}

FFTL_FORCEINLINE const vec3& mat33::Right() const
{
	return Row<0>();
}
FFTL_FORCEINLINE const vec3& mat33::Forward() const
{
	return Row<1>();
}
FFTL_FORCEINLINE const vec3& mat33::Up() const
{
	return Row<2>();
}
FFTL_FORCEINLINE vec3& mat33::Right()
{
	return reinterpret_cast<vec3&>(Row<0>());
}
FFTL_FORCEINLINE vec3& mat33::Forward()
{
	return reinterpret_cast<vec3&>(Row<1>());
}
FFTL_FORCEINLINE vec3& mat33::Up()
{
	return reinterpret_cast<vec3&>(Row<2>());
}

template<uint R>
FFTL_FORCEINLINE const vec3& mat33::Row() const
{
	static_assert(R <= 2, "Only 1st 3 rows are valid");
	return m_v[R];
}
template<uint R>
FFTL_FORCEINLINE vec3& mat33::Row()
{
	static_assert(R <= 2, "Only 1st 3 rows are valid");
	return m_v[R];
}

inline vec3 mat33::GetAngles() const
{
	//	TODO: Vectorize trig functions!
	const auto m = reinterpret_cast<const f32(*)[4]>(this);
	f32 angX, angY, angZ;

	// make sure m[0][2] is valid for arc sin [-1, 1]
	angY = -asinf(Clamp(m[0][2], -1.f, 1.f));
	if (fabsf(m[0][2]) < 0.9998f)
	{
		angX = atan2(m[1][2], m[2][2]);
		angZ = atan2(m[0][1], m[0][0]);
	}
	else
	{
		// Gimbal lock
		angX = 0;
		angZ = -atan2(m[1][0], m[1][1]);
	}

	return vec3(angX, angY, angZ);
}

FFTL_FORCEINLINE quat mat33::GetRotation() const
{
	return quat::FromMatOrtho(*this);
}

FFTL_FORCEINLINE quat mat33::GetRotationScaled() const
{
	return quat::FromMatScaled(*this);
}

FFTL_FORCEINLINE vec3 mat33::GetScale() const
{
	return Sqrt(GetScale_sq());
}

FFTL_FORCEINLINE vec3 mat33::GetScale_sq() const
{
	const f32 x = Length_sq(Row<0>());
	const f32 y = Length_sq(Row<1>());
	const f32 z = Length_sq(Row<2>());
	return vec3(x, y, z);
}

FFTL_FORCEINLINE vec3 mat33::Transform(const vec3& v) const
{
	vec3 a = Row<0>();
	vec3 b = Row<1>();
	vec3 c = Row<2>();

	vec3 r;
	r = XXXX(v) * a;
	r = V4fAddMul(r, YYYY(v), b);
	r = V4fAddMul(r, ZZZZ(v), c);

	return r;
}

FFTL_FORCEINLINE vec3 mat33::TransformTransposed(const vec3& p) const
{
	vec3 a = Row<0>();
	vec3 b = Row<1>();
	vec3 c = Row<2>();
	FFTL::Transpose3x3(a, b, c);

	vec3 r;
	r = XXXX(p) * a;
	r = V4fAddMul(r, YYYY(p), b);
	r = V4fAddMul(r, ZZZZ(p), c);

	return r;
}

FFTL_FORCEINLINE void mat33::Lookat(const vec3& target, const vec3& position)
{
	vec3 dir = target - position;
	f32 len;
	dir = Normalize(dir, len);
	if (len > 0.0f)
		Orient(dir);
}

inline void mat33::Orient(const vec3& fwd, const vec3& up)
{
	FFTL_MATH_ASSERT(IsNearEqual(Length_sq(fwd), 1.0f, 0.001f));
	FFTL_MATH_ASSERT(IsNearEqual(Length_sq(up), 1.0f, 0.001f));

	const vec3 vRt = Normalize(Cross(fwd, up));
	const vec3 vUp = Normalize(Cross(vRt, fwd));
	
	Forward() = fwd;
	Right() = vRt;
	Up() = vUp;
}

inline void mat33::Orient(const vec3& fwd)
{
	FFTL_MATH_ASSERT(IsNearEqual(Length_sq(fwd), 1.0f, 0.001f));

	const vec3 _up = fabs(fwd.GetZ()) > 0.99f ? vec3_yaxis : vec3_zaxis;

	const vec3 vRt = Normalize(Cross(fwd, _up));
	const vec3 vUp = Cross(vRt, fwd);

	Forward() = fwd;
	Right() = vRt;
	Up() = vUp;
}

FFTL_FORCEINLINE void mat33::ScalePre(const vec3& s)
{
	Row<0>() *= XXXX(s);
	Row<1>() *= YYYY(s);
	Row<2>() *= ZZZZ(s);
}

FFTL_FORCEINLINE void mat33::ScalePre(f32 sx, f32 sy, f32 sz)
{
	Row<0>() *= sx;
	Row<1>() *= sy;
	Row<2>() *= sz;
}

FFTL_FORCEINLINE void mat33::ScalePre(f32 s)
{
	const vec3 vS = vec3::Splat(s);
	Row<0>() *= vS;
	Row<1>() *= vS;
	Row<2>() *= vS;
}

FFTL_FORCEINLINE void mat33::ScalePost(const vec3& s)
{
	Row<0>() *= s;
	Row<1>() *= s;
	Row<2>() *= s;
}

FFTL_FORCEINLINE void mat33::ScalePost(f32 s)
{
	ScalePost(vec3::Splat(s));
}

inline void mat33::Invert()
{
	const vec3 vOne = vec3::Splat(1.f);

	const vec3 c01 = Cross(Row<0>(), Row<1>());
	const vec3 c20 = Cross(Row<2>(), Row<0>());
	const vec3 c12 = Cross(Row<1>(), Row<2>());

	vec3 det = DotV(Row<2>(), c01);
	const mask32x4 msk = CmpEq(det + vOne, vOne); // if (abs(det) < FLT_EPSILON) det = 1.f;
	det = Blend(msk, vOne, det);
	const vec3 invDet = vOne / det;

	vec3 a = c12 * invDet;
	vec3 b = c20 * invDet;
	vec3 c = c01 * invDet;
	FFTL::Transpose3x3(a, b, c);

	Row<0>() = a;
	Row<1>() = b;
	Row<2>() = c;
}

FFTL_FORCEINLINE void mat33::InvertOrthonormal()
{
	FFTL_MATH_ASSERT(IsOrthonormal());

	// Transpose 3x3 part
	Transpose();
}

FFTL_FORCEINLINE void mat33::Transpose()
{
	FFTL::Transpose3x3SaveW(Row<0>(), Row<1>(), Row<2>());
}

FFTL_FORCEINLINE void mat33::Orthogonalize()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();

	f -= Project(r, f);
	u -= Project(r, u) + Project(f, u);
	FFTL_MATH_ASSERT(IsOrthogonal());
}

FFTL_FORCEINLINE void mat33::Orthonormalize()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();

	r = Normalize(r);
	f = Normalize(f - (DotV(r, f) * r));
	u = Normalize(u - (DotV(r, u) * r + DotV(f, u) * f));

	FFTL_MATH_ASSERT(IsOrthonormal());
}

FFTL_FORCEINLINE void mat33::OrthonormalizeFast()
{
	vec3& r = Right();
	vec3& f = Forward();
	vec3& u = Up();

	r = NormalizeFast(r);
	f = NormalizeFast(f - (DotV(r, f) * r));
	u = NormalizeFast(u - (DotV(r, u) * r + DotV(f, u) * f));
}

FFTL_FORCEINLINE void mat33::CopyFrom(const mat33& rhs)
{
	Row<0>() = rhs.Row<0>();
	Row<1>() = rhs.Row<1>();
	Row<2>() = rhs.Row<2>();
}

FFTL_FORCEINLINE void mat33::CopyFrom(const mat44& rhs)
{
	Row<0>() = AsVec3(reinterpret_cast<const mat33*>(&rhs)->Row<0>());
	Row<1>() = AsVec3(reinterpret_cast<const mat33*>(&rhs)->Row<1>());
	Row<2>() = AsVec3(reinterpret_cast<const mat33*>(&rhs)->Row<2>());
}

FFTL_FORCEINLINE mat33 Normalize(const mat33& m)
{
	return mat33
	(
		Normalize(m.Row<0>()),
		Normalize(m.Row<1>()),
		Normalize(m.Row<2>())
	);
}
FFTL_FORCEINLINE mat33 NormalizeFast(const mat33& m)
{
	return mat33
	(
		NormalizeFast(m.Row<0>()),
		NormalizeFast(m.Row<1>()),
		NormalizeFast(m.Row<2>())
	);
}
FFTL_FORCEINLINE mat33 NormalizeSafe(const mat33& m)
{
	return mat33
	(
		NormalizeSafe(m.Row<0>()),
		NormalizeSafe(m.Row<1>()),
		NormalizeSafe(m.Row<2>())
	);
}

template<int R>
inline void mat33::BuildFromVector(const vec3& dir)
{
	FFTL_MATH_ASSERT(IsNearEqual(Length_sq(dir), 1.0f, TOL_VECTOR_LEN));

	vec3& v0 = Row<(R + 0) % 3>();
	vec3& v1 = Row<(R + 1) % 3>();
	vec3& v2 = Row<(R + 2) % 3>();

	const vec3 absDir = Abs(dir);
	const mask32x4 msk = CmpLt(XXXX(absDir), YYYY(absDir));
	const vec3 v1000(1, 0, 0, 0);
	const vec3 v0100 = Permute<1, 0, 1, 1>(v1000);
	v0 = dir;
//	v1 = cross(dir, (fabs(dir.x) < fabs(dir.y)) ? vec3(-1, 0, 0) : vec3(0, 1, 0));
	v1 = Normalize(Cross(dir, Blend(msk, v1000, v0100)));
	v2 = Cross(v0, v1);
}
inline void mat33::BuildFromVector(const vec3& dir, int R)
{
	FFTL_MATH_ASSERT(IsNearEqual(Length_sq(dir), 1.0f, TOL_VECTOR_LEN));

	vec3& v0 = m_v[(R + 0) % 3].CastTo<3>();
	vec3& v1 = m_v[(R + 1) % 3].CastTo<3>();
	vec3& v2 = m_v[(R + 2) % 3].CastTo<3>();

	const vec3 absDir = Abs(dir);
	const mask32x4 msk = CmpLt(XXXX(absDir), YYYY(absDir));
	const vec3 v1000(1, 0, 0, 0);
	const vec3 v0100 = Permute<1, 0, 1, 1>(v1000);
	v0 = dir;
//	v1 = cross(dir, (fabs(dir.x) < fabs(dir.y)) ? vec3(-1, 0, 0) : vec3(0, 1, 0));
	v1 = Normalize(Cross(dir, Blend(msk, v1000, v0100)));
	v2 = Cross(v0, v1);
}

FFTL_FORCEINLINE void mat33::BuildFromQuat(const quat& q)
{
	SetRotation(q);
}

FFTL_FORCEINLINE void Mul(const mat33& a, const mat33& b, mat33& out)
{
	out.Row<0>() = b.Transform(a.Row<0>());
	out.Row<1>() = b.Transform(a.Row<1>());
	out.Row<2>() = b.Transform(a.Row<2>());
}

FFTL_FORCEINLINE mat33 Mul(const mat33& a, const mat33& b)
{
	mat33 r;
	Mul(a, b, r);
	return r;
}

FFTL_FORCEINLINE vec3 Mul(const mat33& m, const vec3& v)
{
	const f32 x = Dot(v, m.Row<0>());
	const f32 y = Dot(v, m.Row<1>());
	const f32 z = Dot(v, m.Row<2>());
	return vec3(x, y, z);
}

FFTL_FORCEINLINE vec3 Mul(const vec3& v, const mat33& m)
{
	return m.Transform(v);
}

FFTL_FORCEINLINE mat33 Add(const mat33& a, const mat33& b)
{
	return a + b;
}
FFTL_FORCEINLINE mat33 Sub(const mat33& a, const mat33& b)
{
	return a - b;
}
FFTL_FORCEINLINE mat33 Abs(const mat33& a)
{
	return mat33(
		Abs(a.Row<0>()),
		Abs(a.Row<1>()),
		Abs(a.Row<2>())
		);
}






template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE mat3StoreBase<ALIGNED, TRANSPOSED>::mat3StoreBase() {}

template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE mat3StoreBase<ALIGNED, TRANSPOSED>::mat3StoreBase(mat33::enIndentityType)
{
	Identity();
}

template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE void mat3StoreBase<ALIGNED, TRANSPOSED>::Identity()
{
	f32* m_v = Ptr();
	//	Storing (1,0,0,0) 3 times will give us what we need because of the 4/3 offset
	const vec4 idVec(1.f);
	idVec.Store<ALIGNED>(m_v + 0);
	idVec.Store<ALIGNED>(m_v + 4);
	idVec.CastTo<1>().StoreU(m_v + 8);
}

template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE void mat3StoreBase<ALIGNED, TRANSPOSED>::Store(const mat33& in)
{
	FFTL_MATH_ASSERT(!ALIGNED || (size_t(this) & 15) == 0); // ensure target destination is aligned to 16 bytes
	f32* m_v = Ptr();

	if (TRANSPOSED)
	{
		vec3 r0 = in.Row<0>();
		vec3 r1 = in.Row<1>();
		vec3 r2 = in.Row<2>();
		FFTL::Transpose3x3(r0, r1, r2);
		r0.Store<ALIGNED>(m_v + 0);
		r1.StoreU(m_v + 3);
		r2.StoreU(m_v + 6);
	}
	else
	{
		const vec4 r0 = in.Row<0>().CastTo<4>();
		const vec4 r1 = in.Row<1>().CastTo<4>();
		const vec4 r2 = in.Row<2>().CastTo<4>();
		const vec4 a = Permute<A0, A1, A2, B0>(r0, r1);
		const vec4 b = Permute<A1, A2, B0, B1>(r1, r2);
		a.Store<ALIGNED>(m_v + 0);
		b.Store<ALIGNED>(m_v + 4);
		m_v[8] = r2.GetZ();
	}
}

template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE void mat3StoreBase<ALIGNED, TRANSPOSED>::Load(mat33& out) const
{
	FFTL_MATH_ASSERT(!ALIGNED || (size_t(this) & 15) == 0); // ensure target destination is aligned to 16 bytes
	const f32* m_v = Ptr();

	vec3 a = ALIGNED ? vec4::LoadA(m_v + 0).CastTo<3>() : vec4::LoadU(m_v + 0).CastTo<3>();
	vec3 b = vec4::LoadU(m_v + 3).CastTo<3>();
	vec3 c = vec3::LoadU(m_v + 6);

	if (TRANSPOSED)
		Transpose3x3(a, b, c);

	out.Row<0>() = a;
	out.Row<1>() = b;
	out.Row<2>() = c;
}

template<bool ALIGNED, bool TRANSPOSED>
template<uint ROW>
FFTL_FORCEINLINE vec3 mat3StoreBase<ALIGNED, TRANSPOSED>::LoadRow() const
{
	static_assert(ROW < 3, "Only 3 rows in a 3x3 matrix");
	const f32* m_v = Ptr();
	if (TRANSPOSED)
		return vec3(m_v[0+ROW], m_v[3+ROW], m_v[6+ROW]);
	else
		return ALIGNED ? vec4::LoadA(m_v + 3*ROW).CastTo<3>() : vec4::LoadU(m_v + 3*ROW).CastTo<3>();
}

template<bool ALIGNED, bool TRANSPOSED>
template<uint ROW>
FFTL_FORCEINLINE void mat3StoreBase<ALIGNED, TRANSPOSED>::SetRow(const vec3& v)
{
	static_assert(ROW < 3, "Only 3 rows in a 3x3 matrix");
	f32* m_v = Ptr();
	if (TRANSPOSED)
	{
		m_v[0+ROW] = v.GetX();
		m_v[3+ROW] = v.GetY();
		m_v[6+ROW] = v.GetZ();
	}
	else
		v.Store<ALIGNED>(m_v + 3*ROW);
}


//	Create from condensed data
template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE mat33::mat33(const mat3Store<ALIGNED, TRANSPOSED>& mat)
{
	mat.Load(*this);
}

template<bool ALIGNED, bool TRANSPOSED>
FFTL_FORCEINLINE mat33& mat33::operator=(const mat3Store<ALIGNED, TRANSPOSED>& mat)
{
	mat.Load(*this);
	return *this;
}




} // namespace FFTL

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

