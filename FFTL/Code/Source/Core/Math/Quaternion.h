/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: quat - Quaternion class using SIMD instructions where possible.

#pragma once

#include "Vector3.h"
#include "Vector4.h"

namespace FFTL
{

class mat33;
class mat44;

class quat
{
public:

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
				quat() = default;
				quat(const quat& copy) = default;
				quat(f32 x, f32 y, f32 z, f32 w);
				quat(const vec3& axis, f32 angleRad);
	explicit	quat(const vec4& vec);

#if 0//defined(FFTL_SSE)
	quat(const __m128& v);
	quat& operator=(const __m128& v);
	operator const __m128&() const;
	operator __m128&();
#endif

	//
	// Getters
	//

	[[nodiscard]] f32		GetX() const;
	[[nodiscard]] f32		GetY() const;
	[[nodiscard]] f32		GetZ() const;
	[[nodiscard]] f32		GetW() const;

	//
	// Element insertion (preserves the remaining elements in this)
	//

	friend quat				InsertX(const quat& q, f32 f);
	friend quat				InsertY(const quat& q, f32 f);
	friend quat				InsertZ(const quat& q, f32 f);
	friend quat				InsertW(const quat& q, f32 f);

	//
	// Setters
	//

	void					Set(f32 x, f32 y, f32 z, f32 w);
	void					Identity();


	//
	// Operators
	//

	quat&					operator*=(const quat& rot);
	[[nodiscard]] quat		operator*(const quat& b) const;
	[[nodiscard]] quat		operator-() const;
	[[nodiscard]] quat		operator+(const quat& other) const;
	[[nodiscard]] quat		operator-(const quat& other) const;

	[[nodiscard]] bool		operator==(const quat& b) const;
	[[nodiscard]] bool		operator!=(const quat& b) const;

	//
	// Conversion
	//

	/// Create a rotation matrix in dst from the quat.
	/// Assumes the quat to be normalized().
	void						ToMat33(mat33& dst) const;
	/// Returns a rotation matrix from the quat.
	/// Assumes the quat to be normalized().
	[[nodiscard]] mat33		ToMat33() const;
	[[nodiscard]] const vec4&	AsVec4() const;
	friend void					StoreU(f32 *p, const quat& q);
	[[nodiscard]] vec3			ToForward() const;

	//
	// Functions of the quaternion
	//

	friend bool		IsNearEqual(const quat& a, const quat& b, f32 tol);
	friend bool		IsNormal(const quat& q);
	friend bool		IsNan(const quat& q);
	friend bool		IsInf(const quat& q);
	friend bool		IsNonFinite(const quat& q);
	friend f32		Length_sq(const quat& q);
	friend f32		Length(const quat& q);
	friend quat		Normalize(const quat& q);
	friend quat		NormalizeFast(const quat& q);

	[[nodiscard]] bool			HasValidAxis() const;
	[[nodiscard]] vec3			GetAxis() const;
	[[nodiscard]] f32			GetAngle() const;
	[[nodiscard]] vec3			Transform(const vec3& vec) const;
	[[nodiscard]] vec3			MakeRelative(const vec3& vec) const;

	//
	// In-place operations (do not return *this - this is inefficient and error-prone)
	//

	void			Rotate(const quat& in);
	void			Conjugate();
	void			NegateW();

	//
	// Out-of-place operations
	//

	[[nodiscard]] quat			GetRotated(const quat& rot) const;
	[[nodiscard]] quat			GetConjugated() const;

	//
	// In-place builders (do not return *this - this is inefficient and error-prone)
	//

	/// Assumes the matrix is orthogonal, but not orthonormal
	void 			BuildFromMatScaled(const mat33& mat);
	/// Assumes the matrix is orthonormal
	void 			BuildFromMatOrtho(const mat33& mat);
	void 			BuildFromAxisAngle(const vec3& axis, f32 angleRad);
	void 			BuildFromEuler(f32 anglex, f32 angley, f32 anglez);
	void 			BuildFromEuler(const vec3& angles);
	void 			BuildSlerp(const quat& qFrom, const quat& qTo, f32 t);
	void 			BuildSlerpFast(const quat& qFrom, const quat& qTo, f32 t, f32 fromDotTo);
	void 			BuildSlerpFast(const quat& qFrom, const quat& qTo, f32 t);

	// 
	// Out-of-place builders
	//

	[[nodiscard]] FFTL_FORCEINLINE static quat	FromMatScaled(const mat33& mat)					{ quat q; q.BuildFromMatScaled(mat); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	FromMatOrtho(const mat33& mat)					{ quat q; q.BuildFromMatOrtho(mat);  return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	FromAxisAngle(const vec3& axis, f32 angleRad)	{ quat q; q.BuildFromAxisAngle(axis, angleRad); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	FromEuler(f32 anglex, f32 angley, f32 anglez)	{ quat q; q.BuildFromEuler(anglex, angley, anglez); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	FromEuler(const vec3& angles)					{ quat q; q.BuildFromEuler(angles); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	SLerp(const quat& q0, const quat& q1, f32 t)	{ quat q; q.BuildSlerp(q0, q1, t); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	SLerpFast(quat qFrom, const quat& qTo, f32 t)	{ quat q; q.BuildSlerpFast(qFrom, qTo, t); return q; }
	[[nodiscard]] FFTL_FORCEINLINE static quat	SLerpFast(const quat& qFrom, const quat& qTo, f32 t, f32 fromDotTo) { quat q; q.BuildSlerpFast(qFrom, qTo, t, fromDotTo); return q; }

	//
	// Other statics
	//

	[[nodiscard]] static f32	AngularDifference(const quat& a, const quat& b);
	[[nodiscard]] static quat	LoadA(const f32* p);
	[[nodiscard]] static quat	LoadU(const f32* p);
	[[nodiscard]] static quat	genIdentity();

private:

	vec4 m_v;
};



//
// Element insertion (preserves the remaining elements in this)
//

[[nodiscard]] quat				InsertX(const quat& q, f32 f);
[[nodiscard]] quat				InsertY(const quat& q, f32 f);
[[nodiscard]] quat				InsertZ(const quat& q, f32 f);
[[nodiscard]] quat				InsertW(const quat& q, f32 f);

[[nodiscard]] f32	Dot(const quat& a, const quat& b);
[[nodiscard]] quat	Lerp(f32 mu, const quat& from, const quat& to);

} // namespace FFTL

#include "Quaternion.inl"
