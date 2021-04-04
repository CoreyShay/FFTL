/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/

// Purpose: Matrix33 - Matrix class with 3 x 3 components, 16 byte aligned and using SIMD

#pragma once

#include "Vector3.h"

namespace FFTL
{

class quat;

template<bool ALIGNED, bool TRANSPOSED>
class mat3Store;



class mat33
{
public:

	static constexpr uint N_ROWS = 3;
	static constexpr uint N_COLS = 3;
	static constexpr uint VEC_WIDTH = 4;
	static constexpr f32 TOL_VECTOR_LEN = 0.001f;

	enum enIndentityType { IDENTITY };

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
	mat33();
	mat33(enIndentityType);
	mat33(const mat33& mat);
//	mat33(f32 m00, f32 m01, f32 m02,
//		  f32 m10, f32 m11, f32 m12,
//		  f32 m20, f32 m21, f32 m22);
	mat33(const vec3& row0, const vec3& row1, const vec3& row2);

	//	Create from condensed data
	template<bool ALIGNED, bool TRANSPOSED>
	mat33(const mat3Store<ALIGNED, TRANSPOSED>& mat);

	template<bool ALIGNED, bool TRANSPOSED>
	mat33& operator=(const mat3Store<ALIGNED, TRANSPOSED>& mat);

	//
	// Setters
	//

	/// Overrides the 3x3 part only
	void			Set(const vec3& right, const vec3& fwd, const vec3& up);
	/// Overrides the 3x3 part only.
	/// Assumes that rot is normalized.
	void			SetRotation(const quat& q);
	/// Set matrix to identity
	void			Identity();
	/// Set matrix to zero
	void			Zero();

	//
	// Operators
	//

	mat33&							operator=(const mat33& rhs);
	[[nodiscard]] mat33			operator*(f32 rhs) const;
	[[nodiscard]] mat33			operator*(const mat33& rhs) const;
	mat33&							operator*=(f32 rhs);
	mat33&							operator*=(const mat33& rhs);
	[[nodiscard]] mat33			operator/(f32 rhs) const;
	mat33&							operator/=(f32 rhs);
	[[nodiscard]] mat33 			operator+(const mat33& rhs) const;
	mat33&							operator+=(const mat33& rhs);
	[[nodiscard]] mat33 			operator-(const mat33& rhs) const;
	mat33&							operator-=(const mat33& rhs);
	[[nodiscard]] bool				operator==(const mat33& rhs) const;
	[[nodiscard]] bool				operator!=(const mat33& rhs) const;

	//
	// Conversion
	//

	void						Store(f32* m) const;
	[[nodiscard]] const f32*	Ptr() const;
	[[nodiscard]] f32*			Ptr();

	//
	// Functions of the matrix
	//

	friend bool					IsNearEqual(const mat33& a, const mat33& b, f32 tol);
	[[nodiscard]] bool			IsNan() const;
	[[nodiscard]] bool			IsInf() const;
	[[nodiscard]] bool			IsFinite() const;
	[[nodiscard]] bool			IsNonFinite() const;
	/// Checks if the 3x3 sub-matrix is orthogonal	
	[[nodiscard]] bool			IsOrthogonal() const;
	/// Checks if the 3x3 sub-matrix is orthonormal
	[[nodiscard]] bool			IsOrthonormal() const;
	/// Computes the determinant of the matrix
	[[nodiscard]] f32			Determinant() const;
	/// Returns a vec3 of the 1st row
	[[nodiscard]] const vec3&	Right() const;
	/// Returns a vec3 of the 2nd row
	[[nodiscard]] const vec3&	Forward() const;
	/// Returns a vec3 of the 3rd row
	[[nodiscard]] const vec3&	Up() const;
	/// Returns a vec3 of the 1st row
	[[nodiscard]] vec3&		Right();
	/// Returns a vec3 of the 2nd row
	[[nodiscard]] vec3&		Forward();
	/// Returns a vec3 of the 3rd row
	[[nodiscard]] vec3&		Up();
	template<uint R>
	[[nodiscard]] const vec3&	Row() const;
	template<uint R>
	[[nodiscard]] vec3&		Row();
	/// Returns Euler angles (in degrees) that describe the orientation of the axes
	[[nodiscard]] vec3			GetAngles() const;
	/// Returns a quaternion the describes the orientation of the axes.
	/// Assumes that the matrix is orthonormal. Use the slower getRotationScaled if the matrix is orthogonal, but not orthonormal, i.e. if the axes are scaled.
	[[nodiscard]] quat			GetRotation() const;
	/// Returns a quaternion the describes the orientation of the axes.
	/// Assumes that the matrix is orthogonal, but not orthonormal. Use the faster getRotation() method if matrix is orthonormal.
	[[nodiscard]] quat			GetRotationScaled() const;
	/// Returns the size of the right, forward and up vectors
	[[nodiscard]] vec3			GetScale() const;
	/// Returns the squared size of the right, forward and up vectors
	[[nodiscard]] vec3			GetScale_sq() const;
	
	//
	// Transforms
	//
	
	/// Computes v * (*this)
	[[nodiscard]] vec3			Transform(const vec3& v) const;
	[[nodiscard]] vec3			Transform3x3(const vec3& v) const;
	/// Computes (3x3 part of *this) * v
	[[nodiscard]] vec3			TransformTransposed(const vec3& p) const;

	//
	// In-place operations (do not return *this - this is inefficient and error-prone)
	//

	/// Changes the orientation of this matrix so that forward is parallel to target - position.
	void			Lookat(const vec3& target, const vec3& pos);
	/// Sets this->forward to fwd, this->right orthogonal to fwd and up and this->up orthogonal to fwd and this->right.
	/// Fwd and up must be unit length.
	void			Orient(const vec3& fwd, const vec3& up);
	/// Sets this->forward to fwd and creates right and up to be an orthonormal system. Tries to match up = (0,0,1). 
	void			Orient(const vec3& fwd);
	/// Computes *this = mul(mat33::scaling(sx, sy, sz), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(f32 sx, f32 sy, f32 sz);
	/// Computes *this = mul(mat33::scaling(s.x, s.y, s.z), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(const vec3& s);
	/// Computes *this = mul(mat33::scaling(s), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(f32 s);
	/// Computes *this = mul(*this, mat33::scaling(sx, sy, sz))
//	void			ScalePost(f32 sx, f32 sy, f32 sz);
	/// Computes *this = mul(*this, mat33::scaling(s.x, s.y, s.z))
	void			ScalePost(const vec3& s);
	/// Computes *this = mul(*this, mat33::scaling(s))
	void			ScalePost(f32 s);
	/// Computes *this = inverse of *this
	void			Invert();
	/// Computes *this = inverse of *this
	/// Assumes that the matrix is orthonormal.
	void			InvertOrthonormal();
	/// Computes *this = transpose of *this (swaps row and column index)
	void			Transpose();
	/// Make this matrix orthogonal.
	/// Affects only the 3x3 part.
	void			Orthogonalize();
	/// Make this matrix orthonormal.
	/// Affects the 3x3 part and the 4th column, which is set to (0,0,0,1).
	void			Orthonormalize();
	/// Make this matrix orthonormal, using fast but less precise normalization methods.
	/// Affects the 3x3 part and the 4th column, which is set to (0,0,0,1).
	void			OrthonormalizeFast();
	/// Copies this matrix from rhs
	void			CopyFrom(const mat33& rhs);
	/// Copies the 3x3 part from rhs
	void			CopyFrom(const mat44& rhs);
	/// Normalizes the 3x3 part
	friend mat33	Normalize(const mat33& m);
	/// Normalizes the 3x3 part fast
	friend mat33	NormalizeFast(const mat33& m);
	/// Normalizes the 3x3 part fast
	friend mat33	NormalizeSafe(const mat33& m);

	//
	// Out-of-place operations
	//

	[[nodiscard]] FFTL_FORCEINLINE mat33	GetScaledPre(f32 sx, f32 sy, f32 sz) const				{ mat33 m = *this; m.ScalePre(sx, sy, sz); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetScaledPre(const vec3& s) const						{ mat33 m = *this; m.ScalePre(s); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetScaledPre(f32 s) const								{ mat33 m = *this; m.ScalePre(s); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetScaledPost(const vec3& s) const						{ mat33 m = *this; m.ScalePost(s); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetScaledPost(f32 s) const								{ mat33 m = *this; m.ScalePost(s); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetInverse() const										{ mat33 m = *this; m.Invert(); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetInverseOrthonormal() const							{ mat33 m = *this; m.InvertOrthonormal(); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetTranspose() const									{ mat33 m = *this; m.Transpose(); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetOrthogonalized() const								{ mat33 m = *this; m.Orthogonalize(); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetOrthonormalized() const								{ mat33 m = *this; m.Orthonormalize(); return m; }
	[[nodiscard]] FFTL_FORCEINLINE mat33	GetNormalized() const									{ return Normalize(*this); }

	/// Orients the axis such that fwd is forward and constructs an orthonormal system.
	/// The returned matrix is 3x3.
	/// Assumes that fwd is normalized.
	FFTL_FORCEINLINE void		BuildFromVector(const vec3& fwd) { BuildFromVector<1>(fwd); }
	/// Orients the axis such that dir is oriented in one of the
	/// major directions depending on row: 0->Right 1->forward, 2->up.
	/// The returned matrix is 3x3.
	/// Assumes that dir is normalized.
	template<int ROW> void	BuildFromVector(const vec3& dir);
	void					BuildFromVector(const vec3& dir, int row);
	/// Builds a 3x3 rotation matrix from a quaternion.
	/// Assumes that q is normalized.
	void					BuildFromQuat(const quat& q);

	// 
	// Out-of-place builders
	//

	[[nodiscard]] FFTL_FORCEINLINE static mat33	FromVector(const vec3& fwd)								{ mat33 m; m.BuildFromVector(fwd); return m; }
	template<int ROW>
	[[nodiscard]] FFTL_FORCEINLINE static mat33	FromVector(const vec3& dir)								{ mat33 m; m.BuildFromVector<ROW>(dir); return m; }
	[[nodiscard]] FFTL_FORCEINLINE static mat33	FromVector(const vec3& dir, int row)					{ mat33 m; m.BuildFromVector(dir, row); return m; }
	[[nodiscard]] FFTL_FORCEINLINE static mat33	FromQuat(const quat& q)									{ mat33 m; m.BuildFromQuat(q); return m; }
	[[nodiscard]] FFTL_FORCEINLINE static mat33	CreateLookAt(const vec3& target, const vec3& position)	{ mat33 m; m.Lookat(target, position); return m; }


private:

	vec3 m_v[3];
};

/// Assumes that both matrices are 3x3 (position == 0 and 4th column = (0,0,0,1))
void						Mul(const mat33& a, const mat33& b, mat33& out);
/// Assumes that both matrices are 3x3 (position == 0 and 4th column = (0,0,0,1))
[[nodiscard]] mat33		Mul(const mat33& a, const mat33& b);

/// Computes m * v. m does NOT have to be 3x3.
[[nodiscard]] vec3			Mul(const mat33& m, const vec3& v);
/// Computes v * m. m does NOT have to be 3x3.
[[nodiscard]] vec3			Mul(const vec3& v, const mat33& m);
// FOR OTHER MATRIX-VECTOR TRANSFORMS, USE mat33::transformX

[[nodiscard]] mat33		Add(const mat33& a, const mat33& b);
[[nodiscard]] mat33		Sub(const mat33& a, const mat33& b);

[[nodiscard]] mat33		Abs(const mat33& a);


[[nodiscard]] bool		IsNearEqual(const mat33& a, const mat33& b, f32 tol);
/// Normalizes the 3x3 part
[[nodiscard]] mat33	Normalize(const mat33& m);
/// Normalizes the 3x3 part fast
[[nodiscard]] mat33	NormalizeFast(const mat33& m);
/// Normalizes the 3x3 part fast
[[nodiscard]] mat33	NormalizeSafe(const mat33& m);




//	This is a more compact way to store a mat33 in that it doesn't have the unnecessary w components (9 floats rather than 12)
template<bool ALIGNED, bool TRANSPOSED>
class mat3StoreBase
{
protected:
	FFTL_FORCEINLINE mat3StoreBase();
	FFTL_FORCEINLINE mat3StoreBase(mat33::enIndentityType);
	FFTL_FORCEINLINE mat3StoreBase(const mat33& in)	{ Store(in); }
	[[nodiscard]] FFTL_FORCEINLINE f32* Ptr()					{ return reinterpret_cast<f32*>(this); }
public:
	[[nodiscard]] FFTL_FORCEINLINE const f32* Ptr() const		{ return reinterpret_cast<const f32*>(this); }
	FFTL_FORCEINLINE void Identity();
	FFTL_FORCEINLINE void Store(const mat33& in);
	FFTL_FORCEINLINE void Load(mat33& out) const;
	FFTL_FORCEINLINE mat3StoreBase& operator=(const mat33& in)	{ Store(in); return *this; }

	template<uint ROW>
	[[nodiscard]] vec3 LoadRow() const;
	template<uint ROW>
	void SetRow(const vec3& v);

	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadRow0() const		{ return LoadRow<0>(); }
	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadRow1() const		{ return LoadRow<1>(); }
	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadRow2() const		{ return LoadRow<2>(); }
	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadRight() const		{ return LoadRow<0>(); }
	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadForward() const	{ return LoadRow<1>(); }
	[[nodiscard]] FFTL_FORCEINLINE vec3 LoadUp() const			{ return LoadRow<2>(); }

	FFTL_FORCEINLINE void SetRight(const vec3& v)				{ SetRow<0>(v); }
	FFTL_FORCEINLINE void SetForward(const vec3& v)				{ SetRow<1>(v); }
	FFTL_FORCEINLINE void SetUp(const vec3& v)					{ SetRow<2>(v); }
};

template<bool ALIGNED = false, bool TRANSPOSED = false>
class mat3Store : public mat3StoreBase<ALIGNED, TRANSPOSED>
{
public:
	typedef mat3StoreBase<ALIGNED, TRANSPOSED> _BASE;
	FFTL_FORCEINLINE mat3Store() {}
	FFTL_FORCEINLINE mat3Store(mat33::enIndentityType) : _BASE(mat33::IDENTITY) {}
	FFTL_FORCEINLINE mat3Store(const mat33& in) : _BASE(in) {}
	FFTL_FORCEINLINE mat3Store& operator=(const mat33& in)	{ this->Store(in); return *this; }
private:
	f32 m_v[9];
};

template<bool TRANSPOSED>
class mat3Store<true, TRANSPOSED> : public mat3StoreBase<true, TRANSPOSED>
{
public:
	typedef mat3StoreBase<true, TRANSPOSED> _BASE;
	FFTL_FORCEINLINE mat3Store() {}
	FFTL_FORCEINLINE mat3Store(mat33::enIndentityType) : _BASE(mat33::IDENTITY) {}
	FFTL_FORCEINLINE mat3Store(const mat33& in) : _BASE(in) {}
	FFTL_FORCEINLINE mat3Store& operator=(const mat33& in)	{ this->Store(in); return *this; }
private:
	alignas(16) f32 m_v[9];
};



typedef mat3Store<false, false> mat3st;




} // namespace FFTL

#include "Matrix33.inl"
