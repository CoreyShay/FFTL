/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Matrix43 - Matrix class with 4 x 3 components, 16 byte aligned and using SIMD

#pragma once

#include "Vector3.h"

namespace FFTL
{

class quat;
class mat33;
class mat44;

template<bool ALIGNED, bool TRANSPOSED>
class mat43Store;

template<bool ALIGNED, bool TRANSPOSED>
class mat3Store;


class mat43
{
public:

	static constexpr uint N_ROWS = 4;
	static constexpr uint N_COLS = 3;
	static constexpr uint VEC_WIDTH = 4;

	enum enIndentityType { IDENTITY };

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
	mat43();
	mat43(enIndentityType);
	mat43(const mat43& mat);
	mat43(const vec3& row0, const vec3& row1, const vec3& row2, const vec3& row3);

	mat43(const mat33& axis, const vec3& pos);
	mat43(const vec3& pos, const mat33& axis);

	template<bool ALIGNED, bool TRANSPOSED>
	mat43(const mat3Store<ALIGNED, TRANSPOSED>& axis, const vec3& pos);
	template<bool ALIGNED, bool TRANSPOSED>
	mat43(const vec3& pos, const mat3Store<ALIGNED, TRANSPOSED>& axis);

//	explicit mat43(const f32 vals[12] );
//	explicit mat43(const f32 vals[4][3] );

	//	Create from condensed data
	template<bool ALIGNED, bool TRANSPOSED>
	mat43(const mat43Store<ALIGNED, TRANSPOSED>& mat);

	template<bool ALIGNED, bool TRANSPOSED>
	mat43& operator=(const mat43Store<ALIGNED, TRANSPOSED>& mat);

	//
	// Setters
	//

	/// Set matrix to identity
	void			Identity();
	/// Set matrix to zero
	void			zero();

	//
	// Operators
	//

	mat43&			operator=(const mat43& rhs);
	mat43			operator*(f32 rhs) const;
	mat43&			operator*=(f32 rhs);
	mat43			operator/(f32 rhs) const;
	mat43&			operator/=(f32 rhs);
	mat43 			operator+(const mat43& rhs) const;
	mat43&			operator+=(const mat43& rhs);
	mat43 			operator-(const mat43& rhs) const;
	mat43&			operator-=(const mat43& rhs);
	bool			operator==(const mat43& rhs) const;
	bool			operator!=(const mat43& rhs) const;

	//
	// Conversion
	//

//	void			store(f32* m) const;
//	void			toArray(f32* m) const;
	const f32*		Ptr() const;
	f32*			Ptr();

	//
	// Functions of the matrix
	//

	bool			IsNearEqual(const mat43& rhs, f32 tol = 0.001f) const;
	bool			IsNan() const;
	bool			IsInf() const;
	bool			IsFinite() const;
	bool			IsOutrageous() const;
	/// Checks whether any element is larger than the given threshold
	bool			IsOrthogonal3x3() const;
	/// Checks if the 3x3 sub-matrix is orthonormal
	bool			IsOrthonormal3x3() const;
	/// Checks if the 3x3 sub-matrix is orthonormal and the last row is (0,0,0,1)
	bool			IsOrthonormal() const;
	/// Computes the determinant of the 3x3 part of the matrix
	f32				Determinant3x3() const;
	/// Returns a vec3 of the 1st row
	const vec3&		Right() const;
	/// Returns a vec3 of the 2nd row
	const vec3&		Forward() const;
	/// Returns a vec3 of the 3rd row
	const vec3&		Up() const;
	/// Returns a vec3 of the 4th row
	const vec3&		Position() const;
	/// Returns a vec3 of the 1st row
	vec3&			Right();
	/// Returns a vec3 of the 2nd row
	vec3&			Forward();
	/// Returns a vec3 of the 3rd row
	vec3&			Up();
	/// Returns a vec3 of the 4th row
	vec3&			Position();
	template<uint ROW> const vec3& Row() const;
	template<uint ROW> vec3& Row();
	/// Returns Euler angles (in degrees) that describe the orientation of the axes
	vec3			GetAngles() const;
	/// Returns the size of the right, forward and up vectors
	vec3			GetScale() const;
	/// Returns the squared size of the right, forward and up vectors
	vec3			GetScale_sq() const;
	/// Reinterprets as 3x3 matrix. Data is compatible.
	const mat33&	AsMat33() const;
	mat33&			AsMat33();
	/// Reinterprets as 4x4 matrix. Data is compatible.
	const mat44&	AsMat44() const;
	mat44&			AsMat44();
	
	//
	// Transforms
	//
	
	/// Computes p * (*this)
	vec3			Transform(const vec3& p) const;
	/// Computes p * (3x3 part of *this).
	/// Does NOT assume the matrix is 3x3.
	vec3			Transform3x3(const vec3& p) const;

	//
	// In-place operations (do not return *this - this is inefficient and error-prone)
	//

	/// Changes the orientation of this matrix so that forward is parallel to target - position.
	void			Lookat(const vec3& target);
	/// Sets this->forward to fwd, this->right orthogonal to fwd and up and this->up orthogonal to fwd and this->right.
	/// Fwd and up must be unit length.
	void			Orient(const vec3& fwd, const vec3& up);
	/// Sets this->forward to fwd and creates right and up to be an orthonormal system. Tries to match up = (0,0,1). 
	void			Orient(const vec3& fwd);
	/// Computes *this = mul(mat44::translation(x,y,z), *this).
	void			TranslatePre(f32 x, f32 y, f32 z);
	/// Computes *this = mul(mat44::translation(v), *this).
	void			TranslatePre(const vec3& v);
	/// Computes *this = mul(mat44::translation(x,y,z), *this).
	/// Assumes last column is (0,0,0,1).
	void			TranslatePost(const vec3& v);
	/// Computes *this = mul(mat44::scaling(sx, sy, sz), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(f32 sx, f32 sy, f32 sz);
	/// Computes *this = mul(mat44::scaling(s.x, s.y, s.z), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(const vec3& s);
	/// Computes *this = mul(mat44::scaling(s), *this)
	/// Assumes last column is (0,0,0,1).
	void			ScalePre(f32 s);
	/// Computes *this = mul(*this, mat44::scaling(sx, sy, sz))
//	void			ScalePost(f32 sx, f32 sy, f32 sz);
	/// Computes *this = mul(*this, mat44::scaling(s.x, s.y, s.z))
	void			ScalePost(const vec3& s);
	/// Computes *this = mul(*this, mat44::scaling(s))
	void			ScalePost(f32 s);
	/// Computes *this = inverse of *this
	/// The inverse of a 4x3 matrix is still 4x3.
	void			Invert();
	/// Computes 3x3 part of *this = inverse of 3x3 part of *this
	void			Invert3x3();
	/// Computes *this = inverse of *this
	/// Assumes that the matrix is orthonormal.
	void			InvertOrthonormal();
	/// Transposes of the 3x3 part
	void			Transpose3x3();
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
	void			CopyFrom(const mat43& rhs);
	/// Copies this matrix from rhs
	void			NormalizeAxis();
	/// Normalizes the 3x3 part fast
	void			NormalizeAxisFast();
	/// Normalizes the 3x3 part fast
	void			NormalizeAxisSafe();

	//
	// Out-of-place operations
	//

	/// Returns a copy of this matrix where the orientation is changed so that forward is parallel to target - position.
	FFTL_FORCEINLINE mat43	GetLookat(const vec3& target) const					{ mat43 m; m.Position() = Position(); m.Lookat(target); return m; }
	/// Returns a matrix where forward is fwd and right is orthogonal to fwd and up and up is orthogonal to fwd and this->right.
	/// Fwd and up must be unit length.
	FFTL_FORCEINLINE mat43	GetOrient(const vec3& fwd, const vec3& up) const		{ mat43 m; m.Position() = Position(); m.Orient(fwd, up); return m; }
	/// Returns a matrix where forward to is fwd and right and up are chosen to complete an orthonormal system. Tries to match up = (0,0,1). 
	FFTL_FORCEINLINE mat43	GetOrient(const vec3& fwd) const						{ mat43 m; m.Position() = Position(); m.Orient(fwd); return m; }
	FFTL_FORCEINLINE mat43	GetTranslatedPre(f32 x, f32 y, f32 z) const				{ mat43 m = *this; m.TranslatePre(x, y, z); return m; }
	FFTL_FORCEINLINE mat43	GetTranslatedPre(const vec3& v) const					{ mat43 m = *this; m.TranslatePre(v); return m; }
	FFTL_FORCEINLINE mat43	GetTranslatedPost(const vec3& v) const					{ mat43 m = *this; m.TranslatePost(v); return m; }
	FFTL_FORCEINLINE mat43	GetScaledPre(f32 sx, f32 sy, f32 sz) const				{ mat43 m = *this; m.ScalePre(sx, sy, sz); return m; }
	FFTL_FORCEINLINE mat43	GetScaledPre(const vec3& s) const						{ mat43 m = *this; m.ScalePre(s); return m; }
	FFTL_FORCEINLINE mat43	GetScaledPre(f32 s) const								{ mat43 m = *this; m.ScalePre(s); return m; }
	FFTL_FORCEINLINE mat43	GetScaledPost(const vec3& s) const						{ mat43 m = *this; m.ScalePost(s); return m; }
	FFTL_FORCEINLINE mat43	GetScaledPost(f32 s) const								{ mat43 m = *this; m.ScalePost(s); return m; }
	FFTL_FORCEINLINE mat43	GetInverse() const										{ mat43 m = *this; m.Invert(); return m; }
	FFTL_FORCEINLINE mat43	GetInverse3x3() const									{ mat43 m = *this; m.Invert3x3(); return m; }
	FFTL_FORCEINLINE mat43	GetInverseOrthonormal() const							{ mat43 m = *this; m.InvertOrthonormal(); return m; }
	FFTL_FORCEINLINE mat43	GetTranspose3x3() const									{ mat43 m = *this; m.Transpose3x3(); return m; }
	FFTL_FORCEINLINE mat43	GetOrthogonalized() const								{ mat43 m = *this; m.Orthogonalize(); return m; }
	FFTL_FORCEINLINE mat43	GetOrthonormalized() const								{ mat43 m = *this; m.Orthonormalize(); return m; }
	FFTL_FORCEINLINE mat43	GetNormalizedAxis() const								{ mat43 m = *this; m.NormalizeAxis(); return m; }

	// 
	// Out-of-place builders
	//

	FFTL_FORCEINLINE static mat43	CreateLookAt(const vec3& lookAt, const vec3& position);

private:

	vec3 m_v[4];
};

/// Assumes that the first matrix is 4x3 (4th column (0,0,0,1)) and the other is 3x3 (position == 0 and 4th column = (0,0,0,1))
void			Mul(const mat43& a, const mat33& b, mat43& out);
/// Assumes that the first matrix is 4x3 (4th column (0,0,0,1)) and the other is 3x3 (position == 0 and 4th column = (0,0,0,1))
mat43			Mul(const mat43& a, const mat33& b);

/// Assumes that both matrices have normal 4th columns, i.e. (0,0,0,1)
void			Mul(const mat43& a, const mat43& b, mat43& out);
/// Assumes that both matrices have normal 4th columns, i.e. (0,0,0,1)
mat43			Mul(const mat43& a, const mat43& b);

/// Assumes that both matrices are 3x3 (position == 0 and 4th column = (0,0,0,1))
void			Mul3x3(const mat43& a, const mat43& b, mat43& out);
/// Assumes that both matrices are 3x3 (position == 0 and 4th column = (0,0,0,1))
mat43			Mul3x3(const mat43& a, const mat43& b);

/// Computes m * v. m does NOT have to be 3x3.
vec3			Mul3x3(const mat43& m, const vec3& v);
/// Computes v * m. m does NOT have to be 3x3.
vec3			Mul3x3(const vec3& v, const mat43& m);
// FOR OTHER MATRIX-VECTOR TRANSFORMS, USE mat43::transformX

mat43			Add(const mat43& a, const mat43& b);
mat43			Sub(const mat43& a, const mat43& b);

mat43			Abs(const mat43& a);






//	This is a more compact way to store a mat43 in that it doesn't have the unnecessary w components (12 floats rather than 16)
template<bool ALIGNED, bool TRANSPOSED>
class mat43StoreBase
{
protected:
	FFTL_FORCEINLINE mat43StoreBase();
	FFTL_FORCEINLINE mat43StoreBase(mat43::enIndentityType);
	FFTL_FORCEINLINE mat43StoreBase(const mat43& in) { Store(in); }
public:
	FFTL_FORCEINLINE void Identity();
	FFTL_FORCEINLINE void Store(const mat43& in);
	FFTL_FORCEINLINE void Load(mat43& out) const;
	FFTL_FORCEINLINE void Load3x3(mat33& out) const;
	FFTL_FORCEINLINE void operator=(const mat43& in)	{ Store(in); }
	FFTL_FORCEINLINE const f32* Ptr() const				{ return reinterpret_cast<const f32*>(this); }
	FFTL_FORCEINLINE f32* Ptr()							{ return reinterpret_cast<f32*>(this); }

	template<uint ROW> vec3 LoadRow() const;
	FFTL_FORCEINLINE vec3 LoadRight() const				{ return LoadRow<0>(); }
	FFTL_FORCEINLINE vec3 LoadForward() const			{ return LoadRow<1>(); }
	FFTL_FORCEINLINE vec3 LoadUp() const				{ return LoadRow<2>(); }
	FFTL_FORCEINLINE vec3 LoadPosition() const			{ return LoadRow<3>(); }
};

template<bool ALIGNED=false, bool TRANSPOSED=false>
class mat43Store : public mat43StoreBase<ALIGNED, TRANSPOSED>
{
public:
	typedef mat43StoreBase<ALIGNED, TRANSPOSED> _BASE;
	FFTL_FORCEINLINE mat43Store() {}
	FFTL_FORCEINLINE mat43Store(mat43::enIndentityType) : _BASE(mat43::IDENTITY) {}
	FFTL_FORCEINLINE mat43Store(const mat43& in) : _BASE(in) {}
private:
	f32 m_v[12];
};

template<bool TRANSPOSED>
class mat43Store<true, TRANSPOSED> : public mat43StoreBase<true, TRANSPOSED>
{
public:
	typedef mat43StoreBase<true, TRANSPOSED> _BASE;
	FFTL_FORCEINLINE mat43Store() {}
	FFTL_FORCEINLINE mat43Store(mat43::enIndentityType) : _BASE(mat43::IDENTITY) {}
	FFTL_FORCEINLINE mat43Store(const mat43& in) : _BASE(in) {}
private:
	alignas(16) f32 m_v[12];
};



typedef mat43Store<false, false> mat43st;




} // namespace FFTL

#include "Matrix43.inl"

