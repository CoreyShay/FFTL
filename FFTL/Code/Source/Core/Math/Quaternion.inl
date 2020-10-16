#include "Matrix33.h"
#include "Matrix44.h"

namespace FFTL
{

//
// Constructors
//

FFTL_FORCEINLINE quat::quat()
= default;

FFTL_FORCEINLINE quat::quat(f32 x, f32 y, f32 z, f32 w)
: m_v(x, y, z, w)
{
}
FFTL_FORCEINLINE quat::quat(const quat& copy)
 
= default;
FFTL_FORCEINLINE quat::quat(const vec3& axis, f32 angleRad)
{
	BuildFromAxisAngle(axis, angleRad);
}
FFTL_FORCEINLINE quat::quat(const vec4& vec)
: m_v(vec)
{
}

#if 0//defined(FFTL_SSE)
quat::quat(const __m128& v);
quat& quat::operator=(const __m128& v);
quat::operator const __m128&() const;
quat::operator __m128&();
#endif


FFTL_FORCEINLINE f32 quat::GetX() const
{
	return m_v.GetX();
}
FFTL_FORCEINLINE f32 quat::GetY() const
{
	return m_v.GetY();
}
FFTL_FORCEINLINE f32 quat::GetZ() const
{
	return m_v.GetZ();
}
FFTL_FORCEINLINE f32 quat::GetW() const
{
	return m_v.GetW();
}

//
// Element insertion (preserves the remaining elements in this)
//

FFTL_FORCEINLINE quat InsertX(const quat& q, f32 f)
{
	return quat(InsertX(q.m_v, f));
}
FFTL_FORCEINLINE quat InsertY(const quat& q, f32 f)
{
	return quat(InsertY(q.m_v, f));
}
FFTL_FORCEINLINE quat InsertZ(const quat& q, f32 f)
{
	return quat(InsertZ(q.m_v, f));
}
FFTL_FORCEINLINE quat InsertW(const quat& q, f32 f)
{
	return quat(InsertW(q.m_v, f));
}


FFTL_FORCEINLINE void quat::Set(f32 x, f32 y, f32 z, f32 w)
{
	m_v.Set(x, y, z, w);
}

FFTL_FORCEINLINE void quat::Identity()
{
	*this = genIdentity();
}

//
// Operators
//

FFTL_FORCEINLINE quat& quat::operator*=(const quat& rot)
{
	Rotate(rot);
	return *this;
}

FFTL_FORCEINLINE quat quat::operator*(const quat& b) const
{
	quat res = *this;
	res.Rotate(b);
	return res;
}

FFTL_FORCEINLINE quat quat::operator-() const
{
	return quat(-m_v);
}

FFTL_FORCEINLINE quat quat::operator+(const quat& other) const
{
	return quat(m_v + other.m_v);
}

FFTL_FORCEINLINE quat quat::operator-(const quat& other) const
{
	return quat(m_v - other.m_v);
}

FFTL_FORCEINLINE bool quat::operator==(const quat& b) const
{
	return m_v == b.m_v;
}
FFTL_FORCEINLINE bool quat::operator!=(const quat& b) const
{
	return m_v != b.m_v;
}

/// Create a rotation matrix in dst from the quat.
/// Assumes the quat to be normalized().
FFTL_FORCEINLINE void quat::ToMat33(mat33& dst) const
{
	dst.BuildFromQuat(*this);
}
/// Returns a rotation matrix from the quat.
/// Assumes the quat to be normalized().
FFTL_FORCEINLINE mat33 quat::ToMat33() const
{
	return mat33::FromQuat(*this);
}

FFTL_FORCEINLINE const vec4& quat::AsVec4() const
{
	return m_v;
}

FFTL_FORCEINLINE void StoreU(f32* p, const quat& q)
{
	StoreU(p, q.m_v);
}


//
// Functions of the quaternion
//


FFTL_FORCEINLINE bool IsNearEqual(const quat& a, const quat& b, f32 tol)
{
	return IsNearEqual(a.m_v, b.m_v, tol);
}
FFTL_FORCEINLINE bool IsNormal(const quat& q)
{
	return IsNearEqual(Length_sq(q), 1.0f, 0.001f);
}
FFTL_FORCEINLINE bool IsNan(const quat& q)
{
	return IsNan(q.m_v);
}
FFTL_FORCEINLINE bool IsInf(const quat& q)
{
	return IsFinite(q.m_v);
}
FFTL_FORCEINLINE bool IsNonFinite(const quat& q)
{
	return IsNonFinite(q.m_v);
}
FFTL_FORCEINLINE f32 Length_sq(const quat& q)
{
	return Length_sq(q.m_v);
}
FFTL_FORCEINLINE f32 Length(const quat& q)
{
	return Length(q.m_v);
}

FFTL_FORCEINLINE bool quat::HasValidAxis() const
{
	return !IsNearZero(m_v.CastTo<3>(), 0.001f);
}
FFTL_FORCEINLINE vec3 quat::GetAxis() const
{
	return Normalize(m_v.CastTo<3>());
}
FFTL_FORCEINLINE f32 quat::GetAngle() const
{
	const f32 r = acosf(GetW());
	return r + r;
}

FFTL_FORCEINLINE vec3 quat::Transform(const vec3& v) const
{
#if 1
	// Simplification of the sandwich product:
	// v' = q * v * conj(q)
	const vec3 u = m_v.CastTo<3>();
	const vec3 w = WWWW(u);
	const vec3 _2w = w + w;
	const vec3 _2u = u + u;
	const vec3 ww = w * w;
	const vec3 dUV = DotV(u, v);
	const vec3 dUU = Length_sqV(u);
	const vec3 cVU = Cross(v, u);

	const vec3 a = dUV * _2u;
	const vec3 b = SubMul(ww, dUU, v);
	const vec3 c = _2w * cVU;

	return a + b + c;
#else
	return ToMat33().Transform3x3(v);
#endif
}

FFTL_FORCEINLINE vec3 quat::MakeRelative(const vec3& vec) const
{
	const mat33 mat = ToMat33();
	
	const f32 x = Dot(mat.Right(), vec);
	const f32 y = Dot(mat.Forward(), vec);
	const f32 z = Dot(mat.Up(), vec);

	return vec3(x, y, z);
}

FFTL_FORCEINLINE quat Normalize(const quat& q)
{
	return quat(Normalize(q.m_v));
}

FFTL_FORCEINLINE quat NormalizeFast(const quat& q)
{
	return quat(NormalizeFast(q.m_v));
}

FFTL_FORCEINLINE void quat::Rotate(const quat& in)
{
	vec4 v = Cross(m_v.CastTo<3>(), in.m_v.CastTo<3>()).CastTo<4>();
	const vec4 ab = m_v * in.m_v;
	const vec4 vDot = HSumV(ab.CastTo<3>()).CastTo<4>();
	const vec4 aW = WWWW(m_v);
	const vec4 bW = WWWW(in.m_v);
	v += aW * in.m_v;
	v += bW * m_v;
	const vec4 w = ab - vDot;
	m_v = Permute<A0, A1, A2, B3>(v, w);
}

FFTL_FORCEINLINE void quat::Conjugate()
{
	m_v = Blend<1, 1, 1, 0>(-m_v, m_v);
}

FFTL_FORCEINLINE void quat::NegateW()
{
	m_v = Blend<0, 0, 0, 1>(-m_v, m_v);
}

FFTL_FORCEINLINE quat quat::GetRotated(const quat& rot) const
{
	quat q = *this; q.Rotate(rot); return q;
}
FFTL_FORCEINLINE quat quat::GetConjugated() const
{
	quat q = *this; q.Conjugate(); return q;
}

FFTL_FORCEINLINE void quat::BuildFromMatScaled(const mat33& m)
{
	const mat33 copy
	(
		Normalize(m.Right()),
		Normalize(m.Forward()),
		Normalize(m.Up())
	);
	BuildFromMatOrtho(copy);
}

/** quat::fromMatOrtho algorithm from Ken Shoemake's 1987 SIGGRAPH
	course notes article "Quaternion Calculus and Fast Animation". */
inline void quat::BuildFromMatOrtho(const mat33& m)
{
#if 0
	const float m00 = m.Row<0>().GetX();
	const float m01 = m.Row<0>().GetY();
	const float m02 = m.Row<0>().getZ();
	const float m10 = m.Row<1>().GetX();
	const float m11 = m.Row<1>().GetY();
	const float m12 = m.Row<1>().getZ();
	const float m20 = m.Row<2>().GetX();
	const float m21 = m.Row<2>().GetY();
	const float m22 = m.Row<2>().getZ();

	vec4 q(
		m00 - m11 - m22,
		m11 - m00 - m22,
		m22 - m00 - m11,
		m00 + m11 + m22
		);
	q += vec4::splat(1.f);
	q = max(q, vec4::genZero());
	q = Sqrt(q);
	q *= vec4::splat(0.5f);
	const mask32x4 negxyz = vec4(-0.f, -0.f, -0.f, +0.f).asVecfcmp();
	q.AndNotAsn(negxyz);
	vec4 copySign(
		m21 - m12,
		m02 - m20,
		m10 - m01,
		0.f
		);
	copySign ^= copySign.andnot(negxyz).asVecfcmp(); // this will result in only the signs of the subtractions
	q |= copySign.asVecfcmp(); // copy the signs over to q
	m_v = q;

#else
	f32 trace;
	f32 s, t;
	int i, j, k;

	const auto mf = reinterpret_cast<const f32(*)[4]>(&m);

	trace = mf[0][0] + mf[1][1] + mf[2][2];

	if ( trace > 0.0f )
	{
		t = trace + 1.0f;
		s = 0.5f / Sqrt(t);
		vec4 q(
			mf[2][1] - mf[1][2],
			mf[0][2] - mf[2][0],
			mf[1][0] - mf[0][1],
			t
			);
		q *= s;
		m_v = q;
	}
	else
	{
		f32* q = reinterpret_cast<f32*>(this);
		const int next[3] = { 1, 2, 0 };
		i = 0;
		if ( mf[1][1] > mf[0][0] )
			i = 1;	
		if ( mf[2][2] > mf[i][i] )
			i = 2;

		j = next[i];
		k = next[j];

		t = ( mf[i][i] - ( mf[j][j] + mf[k][k] ) ) + 1.0f;
		s = 0.5f / Sqrt(t);

		q[i] = s * t;
		q[3] = ( mf[k][j] - mf[j][k] ) * s;
		q[j] = ( mf[j][i] + mf[i][j] ) * s;
		q[k] = ( mf[k][i] + mf[i][k] ) * s;
	}
#endif

	// This was needed in order to keep going back and forth from quats to matrices over and over
	// from creating non-orthonormalized matrices
	const f32 distsq = Length_sq(m_v);

	if (!IsNearEqual(distsq, 1.f, 0.0001f))
	{
		if (distsq <= 0.0f)
		{
			m_v = InsertW(m_v, 1.f);
		}
		else
		{
			m_v /= Sqrt(distsq);
		}
	}
}

FFTL_FORCEINLINE void quat::BuildFromAxisAngle(const vec3& axis, f32 angleRad)
{
	angleRad *= 0.5f;
	const f32 s = sinf( angleRad );
	vec4 v = Normalize(axis).CastTo<4>() * s;
	v = InsertW(v, cosf(angleRad));
	m_v = v;
}

// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
inline void quat::BuildFromEuler(f32 anglex, f32 angley, f32 anglez)
{
	const f32 c1 = cosf(angley / 2);
	const f32 s1 = sinf(angley / 2);
	const f32 c2 = cosf(anglez / 2);
	const f32 s2 = sinf(anglez / 2);
	const f32 c3 = cosf(anglex / 2);
	const f32 s3 = sinf(anglex / 2);
	const f32 c1c2 = c1*c2;
	const f32 s1s2 = s1*s2;

	const f32 w = c1c2*c3 - s1s2*s3;
	const f32 x = c1c2*s3 + s1s2*c3;
	const f32 y = s1*c2*c3 + c1*s2*s3;
	const f32 z = c1*s2*c3 - s1*c2*s3;

	Set(x, y, z, w);
}

FFTL_FORCEINLINE void quat::BuildFromEuler(const vec3& angles)
{
	BuildFromEuler(angles.GetX(), angles.GetY(), angles.GetZ());
}

// This function hasn't been tested yet and was adapted from http://www.inxbus.net/hldoc/d8/d45/quaternion_8cpp-source.html
// in case it isn't working
inline void quat::BuildSlerp(const quat& qFrom, const quat& qTo, f32 t)
{
	constexpr f32 SLERP_MIN_COS_ANGLE = 0.990f; // cos( 8 * (M_PI/180.0) ) == 0.990

	vec4 q1 = qTo.m_v;
	vec4 vCosTheta = DotV(qFrom.m_v, q1);

#if 1
	const mask32x4 cmlLT = CmpLt(vCosTheta, vec4::Zero());
	vCosTheta =	Blend(cmlLT, -vCosTheta, vCosTheta);
	q1 =		Blend(cmlLT, -q1, q1);
#else
	if (cos_theta.GetX() < 0.0f)
	{
		cos_theta = -cos_theta;
		q1.negate();
	}
#endif

	const f32 cos_theta = vCosTheta.GetX();

	if (cos_theta < SLERP_MIN_COS_ANGLE) // if angle between rotations is too big
	{
		f32 theta = acosf( cos_theta );
		f32 i_sin_theta = 1.0f / Sqrt( 1.0f - cos_theta * cos_theta );
		f32 t_theta = t * theta;
		f32 t0 = sinf( theta - t_theta ) * i_sin_theta;
		f32 t1 = sinf(         t_theta ) * i_sin_theta;

		m_v = qFrom.m_v * t0 + q1 * t1;
	}
	else
	{
		m_v = qFrom.m_v + ( q1 - qFrom.m_v ) * t;
	}

	m_v = Normalize(m_v);
}

inline void quat::BuildSlerpFast(const quat& qFrom, const quat& qTo, f32 t)
{
	quat q0 = qFrom;
	vec4 vCosTheta = DotV(q0.m_v, qTo.m_v);

#if 1
	const mask32x4 cmlLT = CmpLt(vCosTheta, vec4::Zero());
	vCosTheta = Blend(cmlLT, -vCosTheta, vCosTheta);
	q0.m_v = Blend(cmlLT, -q0.m_v, q0.m_v);
#else
	if (cos_theta.GetX() < 0.0f)
	{
		cos_theta = -cos_theta;
		q0 = -q0;
	}
#endif

	const f32 cos_theta = vCosTheta.GetX();

	FFTL_MATH_ASSERT(cos_theta >= 0);
	BuildSlerpFast(q0, qTo, t, cos_theta);
}

// NOTE: adapted from NaturalMotion's NMP::Quat::fastSlerp
// -found in /svn/libsrc/morpheme/common/NMPlatform/include/NMPlatform/NMQuat_vanilla.inl
inline void quat::BuildSlerpFast(const quat& qFrom, const quat& qTo, f32 t, f32 fromDotTo)
{
	FFTL_MATH_ASSERT(fromDotTo >= 0); // Quats must be in the same semi-arc - just negate one if they're not

	f32 recipOnePlusFromdotTo;
	f32 c1, c3, c5, c7;
	f32 startWeight, endWeight;
	f32 T, t2, T2;

#if 1
	const vec4 magic0(+1.570994357000f, -0.646139638200f, +0.079498235210f, -0.004354102836f);
	const vec4 magic1(+0.564292985900f, +0.594565793600f, -0.173043693100f, +0.014189627360f);
	const vec4 magic2(-0.178365771700f, +0.086103239530f, +0.107927959900f, -0.015671896910f);
	const vec4 magic3(+0.043199493520f, -0.034651229280f, -0.014393978010f, +0.005848706227f);
	const vec4 vFromDotTo = vec4::Splat(fromDotTo);
	const vec4 c = magic0 + ( magic1 + (magic2 + magic3 * vFromDotTo) * vFromDotTo) * vFromDotTo;;
	c1 = c.GetX();
	c3 = c.GetY();
	c5 = c.GetZ();
	c7 = c.GetW();
#else	
	c1 =  1.570994357000f + ( 0.56429298590f + (-0.17836577170f + 0.043199493520f * fromDotTo) * fromDotTo) * fromDotTo;
	c3 = -0.646139638200f + ( 0.59456579360f + ( 0.08610323953f - 0.034651229280f * fromDotTo) * fromDotTo) * fromDotTo;
	c5 =  0.079498235210f + (-0.17304369310f + ( 0.10792795990f - 0.014393978010f * fromDotTo) * fromDotTo) * fromDotTo;
	c7 = -0.004354102836f + ( 0.01418962736f + (-0.01567189691f + 0.005848706227f * fromDotTo) * fromDotTo) * fromDotTo;
#endif

	recipOnePlusFromdotTo = 1.0f / (1.0f + fromDotTo);

	T = 1 - t;
	t2 = t * t;
	T2 = T * T;
	startWeight = (c1 + (c3+(c5+c7*T2)*T2)*T2) * T * recipOnePlusFromdotTo;
	endWeight   = (c1 + (c3+(c5+c7*t2)*t2)*t2) * t * recipOnePlusFromdotTo;

	m_v = vec4::Splat(startWeight) * qFrom.m_v + vec4::Splat(endWeight) * qTo.m_v;
}

FFTL_FORCEINLINE vec3 quat::ToForward() const
{
#if 1
	const vec4 _py = m_v * m_v;
	const vec4 _2py = _py + _py;
	const vec4 _pxz = m_v * Permute<1, 2, 3, 0>(m_v);
	const vec4 _2pxz = _pxz + _pxz;
	const vec4 temp = Permute<2, 3, 0, 1>(_2pxz);
	const vec4 rx = _2pxz + temp;
	const vec4 rz = _2pxz - temp;
	const vec4 ry = vec4::X000(1.f) - (_2py + Permute<2, 3, 2, 3>(_2py));

	vec4 r = Permute<A0, B0, A1, B1>(rx, ry);
	r = Permute<A0, A1, B1, B1>(r, rz);
	return r.CastTo<3>();
#else
	const f32 x = GetX();
	const f32 y = GetY();
	const f32 z = GetZ();
	const f32 w = GetW();
	
	const vec3 r(
	2.0f * (x * y + z * w),
	1.0f - 2.0f * ( x * x + z * z ),
	2.0f * (y * z - w * x));

	return r;
#endif
}

FFTL_FORCEINLINE f32 quat::AngularDifference(const quat& a, const quat& b)
{
	return 2.f*acosf(Clamp(Dot(a, b), -1.f, 1.f));
}

FFTL_FORCEINLINE quat quat::LoadA(const f32* p)
{
	return quat(vec4::LoadA(p));
}
FFTL_FORCEINLINE quat quat::LoadU(const f32* p)
{
	return quat(vec4::LoadU(p));
}

FFTL_FORCEINLINE quat quat::genIdentity()
{
	return quat(0, 0, 0, 1);
}

FFTL_FORCEINLINE f32 Dot(const quat& a, const quat& b)
{
	return Dot(a.AsVec4(), b.AsVec4() );
}

FFTL_FORCEINLINE quat Lerp(f32 mu, const quat& from, const quat& to)
{
	vec4 _b = to.AsVec4();

	vec4 vCosTheta = DotV(from.AsVec4(), to.AsVec4());
#if 1
	const mask32x4 cmlLT = CmpLt(vCosTheta, vec4::Zero());
	_b = Blend(cmlLT, -_b, _b);
#else
	if (cos_theta.GetX() < 0.0f)
		_b = -_b;
#endif

	return quat(Lerp(mu, from.AsVec4(), _b));
}


} // FFTL
