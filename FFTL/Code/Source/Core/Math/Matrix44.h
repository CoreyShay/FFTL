/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Matrix44 - Matrix class with 4 x 4 components, 16 byte aligned and using SIMD

#pragma once

#include "Vector4.h"


namespace FFTL
{

class quat;
class mat33;
class mat43;


class mat44
{
public:

	static constexpr uint N_ROWS = 4;
	static constexpr uint N_COLS = 4;
	static constexpr uint VEC_WIDTH = 4;

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
	mat44() = default;
	mat44(const mat44& mat);
//	mat44(f32 m00, f32 m01, f32 m02, f32 m03,
//		  f32 m10, f32 m11, f32 m12, f32 m13,
//		  f32 m20, f32 m21, f32 m22, f32 m23,
//		  f32 m30, f32 m31, f32 m32, f32 m33);
//	mat44(f32 m00, f32 m01, f32 m02,
//		  f32 m10, f32 m11, f32 m12,
//		  f32 m20, f32 m21, f32 m22); // Initializes m33 to 1 and the rest to 0
	mat44(const vec4& row0, const vec4& row1, const vec4& row2, const vec4& row3);
	explicit mat44(const f32 vals[16] );
	explicit mat44(const f32 vals[4][4] );

	//
	// Setters
	//

	/// Overrides the 3x3 part only
	void			SetAxes(const vec3& right, const vec3& fwd, const vec3& up);
	/// Overrides the 3x3 part only.
	/// Assumes that rot is normalized.
	void			SetRotation(const quat& rot);
	void			SetRotationScaled(const quat& rot);
	/// Scales right() to sx, forward() to sy and up() to sz
	void			SetScale(f32 sx, f32 sy, f32 sz);
	/// Scales right() to s.x, forward() to s.y and up() to s.z
	void			SetScale(const vec3& s);
	/// Scales right(), up() and forward() to s
	void			SetScale(f32 s);
	/// Set matrix to identity
	void			Identity();
	/// Set matrix to zero
	void			Zero();

	//
	// Operators
	//

	FFTL_NODISCARD mat44	operator*(f32 rhs) const;
	FFTL_NODISCARD mat44	operator/(f32 rhs) const;
	FFTL_NODISCARD mat44 	operator+(const mat44& rhs) const;
	FFTL_NODISCARD mat44 	operator-(const mat44& rhs) const;
	FFTL_NODISCARD bool		operator==(const mat44& rhs) const;
	FFTL_NODISCARD bool		operator!=(const mat44& rhs) const;
	mat44&					operator=(const mat44& rhs);
	mat44&					operator*=(f32 rhs);
	mat44&					operator/=(f32 rhs);
	mat44&					operator+=(const mat44& rhs);
	mat44&					operator-=(const mat44& rhs);

	//
	// Conversion
	//

	friend void					Store(f32* p, const mat44& m);
	friend void					StoreA(f32* p, const mat44& m);
	FFTL_NODISCARD const f32*	Ptr() const;
	FFTL_NODISCARD f32*			Ptr();
	FFTL_NODISCARD const mat33&	AsMat33() const;
	FFTL_NODISCARD mat33&		AsMat33();
	FFTL_NODISCARD const mat43&	AsMat43f() const;
	FFTL_NODISCARD mat43&		AsMat43f();

	//
	// Functions of the matrix
	//

	FFTL_NODISCARD bool			IsNearEqual(const mat44& rhs, f32 tol = 0.001f) const;
	FFTL_NODISCARD bool			IsNan() const;
	FFTL_NODISCARD bool			IsInf() const;
	FFTL_NODISCARD bool			IsFinite() const;
	FFTL_NODISCARD bool			IsOutrageous() const;
	/// Checks whether any element is larger than the given threshold
	FFTL_NODISCARD f32			Determinant() const;

	template<uint ROW>
	FFTL_NODISCARD const vec4&	Row() const;
	template<uint ROW>
	FFTL_NODISCARD vec4&		Row();
	
	//
	// Transforms
	//
	
	/// Computes (*this) * p
	FFTL_NODISCARD vec4			TransformTransposed(const vec4& p) const;
	/// Computes p * (*this)
	FFTL_NODISCARD vec4			Transform(const vec4& p) const;

	//
	// In-place operations (do not return *this - this is inefficient and error-prone)
	//

	/// Computes *this = inverse of *this
	void			Invert();
	/// Computes *this = transpose of *this (swaps row and column index)
	void			Transpose();
	/// Copies this matrix from rhs
	void			CopyFrom(const mat44& rhs);

	//
	// Out-of-place operations
	//

	FFTL_NODISCARD FFTL_FORCEINLINE mat44	GetInverse() const		{ mat44 m = *this; m.Invert(); return m; }
	FFTL_NODISCARD FFTL_FORCEINLINE mat44	GetTranspose() const	{ mat44 m = *this; m.Transpose(); return m; }


private:

	vec4 m_v[4];
};

/// General matrix multiplication (no assumptions about orthonormal etc)
void					Mul4x4(const mat44& a, const mat44& b, mat44& out);
/// General matrix multiplication (no assumptions about orthonormal etc)
FFTL_NODISCARD mat44	Mul4x4(const mat44& a, const mat44& b);

FFTL_NODISCARD mat44	Add(const mat44& a, const mat44& b);
FFTL_NODISCARD mat44	Sub(const mat44& a, const mat44& b);

FFTL_NODISCARD mat44	Abs(const mat44& a);




} // namespace FFTL

#include "Matrix44.inl"

