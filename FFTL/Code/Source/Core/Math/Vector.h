/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/

// Purpose: vecT - Vector class with 4 components, with f32 specialization. SSE used where possible.

#pragma once
#define __VECTOR_H__


#include "MathCommon.h"


namespace FFTL
{

//	Forward declarations
class mask32x4;

typedef Vec4f vecNative;


template<uint N>
class vecT : public f32_4
{
public:

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
	vecT();

	vecT(enZeroType);
	constexpr vecT(const vecT& v) = default;
	constexpr vecT(const f32_4& v);
	constexpr vecT(Vec4f_In v);
	vecT& operator=(const vecT& v) = default;
	vecT& operator=(Vec4f_In v);

#if defined(FFTL_SIMD_F32x4)
#else
	[[nodiscard]] f32				Get(uint n) const { return Ptr()[n]; }
	[[nodiscard]] const f32*		Ptr() const { return reinterpret_cast<const f32*>(&m_v); }
	[[nodiscard]] f32*				Ptr() { return reinterpret_cast<f32*>(&m_v); }
#endif

	constexpr vecT(f32 x, f32 y, f32 z, f32 w) : f32_4(x, y, z, w) {}	//(x, y, z, w)
	vecT(f32 x, f32 y, f32 z);			//(x, y, z, 0)
	vecT(f32 x, f32 y);					//(x, y, 0, 0)
	vecT(f32 x);						//(x, 0, 0, 0)

	FFTL_FORCEINLINE vecT(const vecT<3>& xyz, f32 w) { (void)xyz; (void)w; static_assert(N == 4, "Not implemented"); Set(xyz, w); }
	FFTL_FORCEINLINE vecT(const vecT<2>& xy, f32 z) { (void)xy; (void)z; static_assert(N == 3, "Not implemented"); Set(xy, z); }

	///	Use these template sets when we know the input values at compile time to eliminate unnecessary code generation.
	/// Input integer bits will be reinterpreted as floats.
	///	If less than 4 parameters, remaining components will be zeroed.
	/// { x = *(f32*)(&x); y = *(f32*)(&y); z = *(f32*)(&z); w = *(f32*)(&w);
	template<s32 x, s32 y, s32 z, s32 w>	void Set();	//(x, y, z, w)
	template<s32 x, s32 y, s32 z>			void Set();	//(x, y, z, 0)
	template<s32 x, s32 y>					void Set();	//(x, y, 0, 0)
	template<s32 x>							void Set();	//(x, 0, 0, 0)

	///	Use these sets only when we can't know the input values at compile time.
	///	If less than 4 parameters, remaining components will be zeroed.
	/// { x = x; y = y; z = z; w = w; }
	void				Set(f32 x, f32 y, f32 z, f32 w);	//(x, y, z, w)
	void				Set(f32 x, f32 y, f32 z);			//(x, y, z, 0)
	void				Set(f32 x, f32 y);					//(x, y, 0, 0)
	void				Set(f32 x);							//(x, 0, 0, 0)

	void				Set(const vecT<3>& xyz, f32 w) { (void)xyz; (void)w; static_assert(N == 4, "Not implemented"); }
	void				Set(const vecT<2>& xy, f32 z) { (void)xy;	(void)z; static_assert(N == 3, "Not implemented"); }
	void				SetAll(f32 f);


	//	Returns the native type.
	[[nodiscard]] constexpr const vecNative&		AsIntrinsic() const { return m_v; }

	//	reinterprets as mask32x4
	[[nodiscard]] FFTL_FORCEINLINE const mask32x4&	AsVecfcmp() const { return *reinterpret_cast<const mask32x4*>(this); }
	[[nodiscard]] FFTL_FORCEINLINE mask32x4&		AsVecfcmp() { return *reinterpret_cast<mask32x4*>(this); }

	//	Casting (reinterpreting) between N types (all data is compatible)
	template<uint M>
	[[nodiscard]] const vecT<M>&	CastTo() const { return *reinterpret_cast<const vecT<M>*>(this); }
	template<uint M>
	[[nodiscard]] vecT<M>&			CastTo() { return *reinterpret_cast<vecT<M>*>(this); }
	
	[[nodiscard]] FFTL_FORCEINLINE const vecT<4>& CastTo4() const { return CastTo<4>(); }
	[[nodiscard]] FFTL_FORCEINLINE const vecT<3>& CastTo3() const { return CastTo<3>(); }
	[[nodiscard]] FFTL_FORCEINLINE const vecT<2>& CastTo2() const { return CastTo<2>(); }
	[[nodiscard]] FFTL_FORCEINLINE const vecT<1>& CastTo1() const { return CastTo<1>(); }
	
	[[nodiscard]] FFTL_FORCEINLINE vecT<4>& CastTo4() { return CastTo<4>(); }
	[[nodiscard]] FFTL_FORCEINLINE vecT<3>& CastTo3() { return CastTo<3>(); }
	[[nodiscard]] FFTL_FORCEINLINE vecT<2>& CastTo2() { return CastTo<2>(); }
	[[nodiscard]] FFTL_FORCEINLINE vecT<1>& CastTo1() { return CastTo<1>(); }

	//
	// Getters
	//

	[[nodiscard]] f32	GetX() const;
	[[nodiscard]] f32	GetY() const;
	[[nodiscard]] f32	GetZ() const;
	[[nodiscard]] f32	GetW() const;

	///	Extracts and reinterprets the binary representation of the f32 to an int
	[[nodiscard]] int					GetAsIntX() const;
	[[nodiscard]] int					GetAsIntY() const;
	[[nodiscard]] int					GetAsIntZ() const;
	[[nodiscard]] int					GetAsIntW() const;
	[[nodiscard]] int					GetAsIntR() const { return GetAsIntX(); }
	[[nodiscard]] int					GetAsIntG() const { return GetAsIntY(); }
	[[nodiscard]] int					GetAsIntB() const { return GetAsIntZ(); }
	[[nodiscard]] int					GetAsIntA() const { return GetAsIntW(); }

	//
	// Element insertion (preserves the remaining elements in this)
	//

	template<uint M> friend vecT<M>	InsertX(const vecT<M>& v, f32 f);
	template<uint M> friend vecT<M>	InsertY(const vecT<M>& v, f32 f);
	template<uint M> friend vecT<M>	InsertZ(const vecT<M>& v, f32 f);
	template<uint M> friend vecT<M>	InsertW(const vecT<M>& v, f32 f);

	template<bool bX, bool bY, bool bZ, bool bW, uint M>
	friend vecT<M> ZeroElements(const vecT<M>& v);

	//
	// Operators
	//

	vecT							operator+() const;
	vecT&							operator+=(const vecT& b);
	vecT&							operator-=(const vecT& b);
	vecT&							operator*=(const vecT& b);
	vecT&							operator/=(const vecT& b);
	[[nodiscard]] vecT				operator-() const;
	[[nodiscard]] vecT				operator+(const vecT& b) const;
	[[nodiscard]] vecT				operator-(const vecT& b) const;
	[[nodiscard]] vecT				operator*(const vecT& b) const;
	[[nodiscard]] vecT				operator/(const vecT& b) const;
	[[nodiscard]] bool				operator==(const vecT& b) const;
	[[nodiscard]] bool				operator!=(const vecT& b) const;

	[[nodiscard]] vecT				operator|(const mask32x4& b) const { return vecT{ f32_4::operator|(b) }; }
	[[nodiscard]] vecT				operator&(const mask32x4& b) const { return vecT{ f32_4::operator&(b) }; }
	[[nodiscard]] vecT				operator^(const mask32x4& b) const { return vecT{ f32_4::operator^(b) }; }
	template<uint M> friend vecT<M>	AndNot(const vecT<M>& a, const mask32x4& b);
	template<uint M> friend vecT<M>	AndNot(const mask32x4& a, const vecT<M>& b);

	vecT&				operator|=(const mask32x4& b) { return *this = *this | b; }
	vecT&				operator&=(const mask32x4& b) { return *this = *this & b; }
	vecT&				operator^=(const mask32x4& b) { return *this = *this ^ b; }


	//
	// Surgical arithmetic affecting only the selected elements
	//

	template<bool bX, bool bY, bool bZ, bool bW> [[nodiscard]] vecT	Add(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> [[nodiscard]] vecT	Sub(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> [[nodiscard]] vecT	Mul(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> [[nodiscard]] vecT	Div(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> [[nodiscard]] vecT	Negate() const;

	//
	// Surgical arithmetic affecting only one element
	//

	[[nodiscard]] vecT		AddX(f32 f) const;
	[[nodiscard]] vecT		AddY(f32 f) const;
	[[nodiscard]] vecT		AddZ(f32 f) const;
	[[nodiscard]] vecT		AddW(f32 f) const;
	[[nodiscard]] vecT		SubX(f32 f) const;
	[[nodiscard]] vecT		SubY(f32 f) const;
	[[nodiscard]] vecT		SubZ(f32 f) const;
	[[nodiscard]] vecT		SubW(f32 f) const;
	[[nodiscard]] vecT		MulX(f32 f) const;
	[[nodiscard]] vecT		MulY(f32 f) const;
	[[nodiscard]] vecT		MulZ(f32 f) const;
	[[nodiscard]] vecT		MulW(f32 f) const;
	[[nodiscard]] vecT		DivX(f32 f) const;
	[[nodiscard]] vecT		DivY(f32 f) const;
	[[nodiscard]] vecT		DivZ(f32 f) const;
	[[nodiscard]] vecT		DivW(f32 f) const;
	[[nodiscard]] vecT		NegateX() const;
	[[nodiscard]] vecT		NegateY() const;
	[[nodiscard]] vecT		NegateZ() const;
	[[nodiscard]] vecT		NegateW() const;


	[[nodiscard]] vecT		Negate() const;
	[[nodiscard]] vecT		ZeroNonFinite() const;
	[[nodiscard]] vecT		ReplaceNonFinite(const vecT& a) const;



	template<bool ALIGNED>
	[[nodiscard]] static vecT	Load(const f32* p);

	[[nodiscard]] static vecT			LoadA(const f32* p);
	[[nodiscard]] static vecT			LoadU(const f32* p);
	[[nodiscard]] static vecT			Splat(f32 f);
	[[nodiscard]] static vecT			X000(f32 f); //(f, 0, 0, 0)
	
	template<bool ALIGNED, uint M>
	friend  void						Store(f32* p, const vecT<M>& v);
	template<uint M> friend void		StoreA(f32* p, const vecT<M>& v);
	template<uint M> friend void		StoreU(f32* p, const vecT<M>& v);

	template<uint M> friend bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, f32 tol);
	template<uint M> friend mask32x4	IsNearEqualV(const vecT<M>& a, const vecT<M>& b, f32 tol);
	template<uint M> friend bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
	template<uint M> friend mask32x4	IsNearEqualV(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
	template<uint M> friend bool		IsNearZero(const vecT<M>& v, f32 tol);
	template<uint M> friend bool		IsNearZero(const vecT<M>& v, const vecT<M>& tol);
	template<uint M> friend mask32x4	IsNearZeroV(const vecT<M>& v, f32 tol);
	template<uint M> friend mask32x4	IsNearZeroV(const vecT<M>& v, const vecT<M>& tol);

	template<uint M> friend bool		IsNormal(const vecT<M>& v);
	template<uint M> friend bool		IsNan(const vecT<M>& v);			// Returns true if any component is NaN
	template<uint M> friend mask32x4	IsNanV(const vecT<M>& v);			// Sets the return mask for any NaN component
	template<uint M> friend bool		IsInf(const vecT<M>& v);			// Returns true if any component is infinite
	template<uint M> friend mask32x4	IsInfV(const vecT<M>& v);			// Sets the return mask for any infinite component
	template<uint M> friend bool		IsFinite(const vecT<M>& v);			// Returns true only if all components are finite
	template<uint M> friend mask32x4	IsFiniteV(const vecT<M>& v);		// Sets the return mask for any finite component
	template<uint M> friend bool		IsNonFinite(const vecT<M>& v);		// Returns true if any component is not finite
	template<uint M> friend mask32x4	IsNonFiniteV(const vecT<M>& v);		// Sets the return mask for any NaN or infinite component
	template<uint M> friend f32			Length_sq(const vecT<M>& v);
	template<uint M> friend vecT<M>		Length_sqV(const vecT<M>& v);
	template<uint M> friend f32			Length(const vecT<M>& v);
	template<uint M> friend vecT<M>		LengthV(const vecT<M>& v);
	template<uint M> friend f32			Length_fast(const vecT<M>& v);

	template<uint M> friend vecT<M>		Normalize(const vecT<M>& v);
	template<uint M> friend vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
	template<uint M> friend vecT<M>		NormalizeSafe(const vecT<M>& v);
	template<uint M> friend vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
	template<uint M> friend vecT<M>		NormalizeFast(const vecT<M>& v);
	template<uint M> friend vecT<M>		NormalizeFastSafe(const vecT<M>& v);

	template<uint M> friend vecT<M>		ZeroNegative(const vecT<M>& v);

	template<uint M> friend vecT<M>		Reflect(const vecT& a, const vecT& n);
	template<uint M> friend vecT<M>		Project(const vecT& a, const vecT& p); /// Projects p onto a (NOT VICE VERSA!)

	//
	//	Constant generators
	//

	///	Generates vecT((f32&)x, (f32&)y, (f32&)z, (f32&)w).
	///	Using integers to reinterpret as floats because template parameters can't be floats.
	template<u32 x, u32 y, u32 z, u32 w>	[[nodiscard]] static vecT	GenConstRtp();
	template<u32 val>						[[nodiscard]] static vecT	GenConstRtp();
	
	[[nodiscard]] static vecT					Zero();
	[[nodiscard]] static vecT					Infinity();



	//
	//	vecT functions outside the class.
	//

	template<uint M> friend const vecT<2>&	AsVec2(const vecT<M>& a);
	template<uint M> friend const vecT<3>&	AsVec3(const vecT<M>& a);
	template<uint M> friend const vecT<4>&	AsVec4(const vecT<M>& a);
	template<uint M> friend vecT<2>&		AsVec2(vecT<M>& a);
	template<uint M> friend vecT<3>&		AsVec3(vecT<M>& a);
	template<uint M> friend vecT<4>&		AsVec4(vecT<M>& a);

	template<uint M> friend vecT<M>	Min(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecT<M>	Max(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend f32		Min(const vecT<M>& v);
	template<uint M> friend f32		Max(const vecT<M>& v);
	template<uint M> friend vecT<M>	MinV(const vecT<M>& v);
	template<uint M> friend vecT<M>	MaxV(const vecT<M>& v);
	template<uint M> friend f32		MinAbs(const vecT<M>& v);
	template<uint M> friend f32		MaxAbs(const vecT<M>& v);
	template<uint M> friend vecT<M>	Clamp(const vecT<M>& val, const vecT<M>& lhs, const vecT<M>& b);
	template<uint M> friend u32		MinIndex(const vecT<M>& v); // Returns the index of min lowest value
	template<uint M> friend u32		MaxIndex(const vecT<M>& v); // Returns the index of max lowest value
	template<uint M> friend vecT<M>	Sqrt(const vecT<M>& v);
	template<uint M> friend vecT<M>	RSqrt(const vecT<M>& v); // Newton-Rhapson fast inverse square root
	template<uint M> friend vecT<M>	Lerp(const vecT<M>& lhs, const vecT<M>& b, f32 amt);
	template<uint M> friend vecT<M>	Abs(const vecT<M>& a);
	template<uint M> friend vecT<M>	AbsDiff(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecT<M>	Pow4(const vecT<M>& a, f32 exp);
	template<uint M> friend vecT<M>	Deg2rad(const vecT<M>& degrees);
	template<uint M> friend vecT<M>	Rad2deg(const vecT<M>& radians);
	template<uint M> friend vecT<M>	Sign(const vecT<M>& a);
	template<uint M> friend f32		HSum(const vecT<M>& v);
	template<uint M> friend vecT<M>	HSumV(const vecT<M>& v);
	template<uint M> friend f32		Dot(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecT<M>	DotV(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecT<M>	Cross(const vecT<M>& a, const vecT<M>& b, const vecT<M>& c);
	template<uint M> friend f32		Distance(const vecT<M>& lhs, const vecT<M>& b);
	template<uint M> friend f32		Distance_sq(const vecT<M>& lhs, const vecT<M>& b);
	template<uint M> friend f32		Mean(const vecT<M>& v);
	template<uint M> friend vecT<M>	XXXX(const vecT<M>& a);
	template<uint M> friend vecT<M>	YYYY(const vecT<M>& a);
	template<uint M> friend vecT<M>	ZZZZ(const vecT<M>& a);
	template<uint M> friend vecT<M>	WWWW(const vecT<M>& a);

	//	R specifies the number of non-zero return elements desired, for example:
	// dotV<2>(a, b) { f32 r = dot(a, b); return vecT<N>(r, r, 0, 0); }
	template<uint R, uint M> friend vecT<M>		DotV(const vecT<M>& a, const vecT<M>& b);
	template<uint R, uint M> friend vecT<M>		Length_sqV(const vecT<M>& v);
	template<uint R, uint M> friend vecT<M>		LengthV(const vecT<M>& v);
	template<uint R, uint M> friend vecT<M>		Normalize(const vecT<M>& v);
	template<uint R, uint M> friend vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
	template<uint R, uint M> friend vecT<M>		NormalizeSafe(const vecT<M>& v);
	template<uint R, uint M> friend vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
	template<uint R, uint M> friend vecT<M>		NormalizeFast(const vecT<M>& v);
	template<uint R, uint M> friend vecT<M>		NormalizeFastSafe(const vecT<M>& v);

	///	Rearranges the elements of a single vector, where each template parameter indexes one element in the source vector.
	/// permute<3,1,0,2>(v) = v.w, v.y, v.x, v.z
	template<int T0, int T1, int T2, int T3, uint M>
	friend vecT<M> Permute(const vecT<M>& v);

	///	Rearranges the elements of a 2 vectors, where each template parameter indexes one element in one of the source vectors.
	/// permute<b3,a1,a0,b2>(a, b) = b.w, a.y, a.x, b.z
	template<int T0, int T1, int T2, int T3, uint M>
	friend vecT<M> Permute(const vecT<M>& a, const vecT<M>& b);

	//	Geometry
	template<uint M> friend vecT<M>	PointOnRay(const vecT<M>& start, const vecT<M>& dir, f32 dist);
	template<uint M> friend vecT<M>	ClosestPointOnLineSegment(const vecT<M>& a, const vecT<M>& b, const vecT<M>& p);
	template<uint M> friend void SinCos(const vecT<M>& a, vecT<M>& s, vecT<M>& c);
	template<uint M> friend vecT<M> Sin(const vecT<M>& y);
	template<uint M> friend vecT<M> Cos(const vecT<M>& y);
};

enum enVecPermute
{
	A0 = 0, A1 = 1, A2 = 2, A3 = 3,
	B0 = 4, B1 = 5, B2 = 6, B3 = 7,
};




//	vecNf type aliases
typedef vecT<4> vec4;
typedef vecT<3> vec3;
typedef vecT<2> vec2;
typedef vecT<1> vec1;




constexpr vec4 vec4_PI( PI_32, PI_32, PI_32, PI_32 );
constexpr vec4 vec4_PI_180{ static_cast<f32>(PI_64 / 180), static_cast<f32>(PI_64 / 180), static_cast<f32>(PI_64 / 180), static_cast<f32>(PI_64 / 180) };
constexpr vec4 vec4_180_PI{ static_cast<f32>(180 / PI_64), static_cast<f32>(180 / PI_64), static_cast<f32>(180 / PI_64), static_cast<f32>(180 / PI_64) };

constexpr vec3 vec3_xaxis{ 1, 0, 0, 0 };
constexpr vec3 vec3_yaxis{ 0, 1, 0, 0 };
constexpr vec3 vec3_zaxis{ 0, 0, 1, 0 };




template<uint M> [[nodiscard]] vecT<M>	InsertX(const vecT<M>& v, f32 f);
template<uint M> [[nodiscard]] vecT<M>	InsertY(const vecT<M>& v, f32 f);
template<uint M> [[nodiscard]] vecT<M>	InsertZ(const vecT<M>& v, f32 f);
template<uint M> [[nodiscard]] vecT<M>	InsertW(const vecT<M>& v, f32 f);

template<bool bX, bool bY, bool bZ, bool bW, uint M>
[[nodiscard]] vecT<M> ZeroElements(const vecT<M>& v);








template<bool ALIGNED, uint M> void			Store(f32* p, const vecT<M>& v);
template<uint M> void						StoreA(f32* p, const vecT<M>& v);
template<uint M> void						StoreU(f32* p, const vecT<M>& v);

template<uint M> [[nodiscard]] bool		IsNormal( const vecT<M>& v );
template<uint M> [[nodiscard]] f32			Length_sq( const vecT<M>& v );
template<uint M> [[nodiscard]] vecT<M>		Length_sqV(const vecT<M>& v);
template<uint M> [[nodiscard]] f32			Length(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>		LengthV(const vecT<M>& v);
template<uint M> [[nodiscard]] f32			Length_fast(const vecT<M>& v);

template<uint M> [[nodiscard]] vecT<M>		Normalize(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
template<uint M> [[nodiscard]] vecT<M>		NormalizeSafe(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
template<uint M> [[nodiscard]] vecT<M>		NormalizeFast(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>		NormalizeFastSafe(const vecT<M>& v);

template<uint M> [[nodiscard]] vecT<M>		ZeroNegative(const vecT<M>& v);

template<uint M> [[nodiscard]] vecT<M>		Reflect(const vecT<M>& a, const vecT<M>& n);
template<uint M> [[nodiscard]] vecT<M>		Project(const vecT<M>& a, const vecT<M>& p); /// Projects p onto a (NOT VICE VERSA!)



//
//	vecT functions outside the class.
//

template<uint M> [[nodiscard]] const vecT<2>&	AsVec2(const vecT<M>& a);
template<uint M> [[nodiscard]] const vecT<3>&	AsVec3(const vecT<M>& a);
template<uint M> [[nodiscard]] const vecT<4>&	AsVec4(const vecT<M>& a);
template<uint M> [[nodiscard]] vecT<2>&		AsVec2(vecT<M>& a);
template<uint M> [[nodiscard]] vecT<3>&		AsVec3(vecT<M>& a);
template<uint M> [[nodiscard]] vecT<4>&		AsVec4(vecT<M>& a);

template<uint M> [[nodiscard]] vecT<M>	Min(const vecT<M>& a, const vecT<M>& b);
template<uint M> [[nodiscard]] vecT<M>	Max(const vecT<M>& a, const vecT<M>& b);
template<uint M> [[nodiscard]] f32		Min(const vecT<M>& v);
template<uint M> [[nodiscard]] f32		Max(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	MinV(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	MaxV(const vecT<M>& v);
template<uint M> [[nodiscard]] f32		MinAbs(const vecT<M>& v);
template<uint M> [[nodiscard]] f32		MaxAbs(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	Clamp(const vecT<M>& val, const vecT<M>& lhs, const vecT<M>& b);
template<uint M> [[nodiscard]] u32		MinIndex(const vecT<M>& v); // Returns the index of min lowest value
template<uint M> [[nodiscard]] u32		MaxIndex(const vecT<M>& v); // Returns the index of max lowest value
template<uint M> [[nodiscard]] vecT<M>	Sqrt(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	RSqrt(const vecT<M>& v); // Newton-Rhapson fast inverse square root
template<uint M> [[nodiscard]] vecT<M>	Lerp(f32 mu, const vecT<M>& from, const vecT<M>& to);
template<uint M> [[nodiscard]] vecT<M>	Abs(const vecT<M>& a);
template<uint M> [[nodiscard]] vecT<M>	AbsDiff(const vecT<M>& a, const vecT<M>& b);
template<uint M> [[nodiscard]] vecT<M>	Pow4(const vecT<M>& a, f32 exp);
template<uint M> [[nodiscard]] vecT<M>	Deg2rad(const vecT<M>& degrees);
template<uint M> [[nodiscard]] vecT<M>	Rad2deg(const vecT<M>& radians);
template<uint M> [[nodiscard]] vecT<M>	Sign(const vecT<M>& a);
template<uint M> [[nodiscard]] f32		HSum(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	HSumV(const vecT<M>& v);
template<uint M> [[nodiscard]] f32		Dot(const vecT<M>& a, const vecT<M>& b);
template<uint M> [[nodiscard]] vecT<M>	DotV(const vecT<M>& a, const vecT<M>& b);
template<uint M> [[nodiscard]] vecT<M>	Cross(const vecT<M>& a, const vecT<M>& b, const vecT<M>& c);
template<uint M> [[nodiscard]] f32		Distance(const vecT<M>& lhs, const vecT<M>& b);
template<uint M> [[nodiscard]] f32		Distance_sq(const vecT<M>& lhs, const vecT<M>& b);
template<uint M> [[nodiscard]] f32		Mean(const vecT<M>& v);
template<uint M> [[nodiscard]] vecT<M>	XXXX(const vecT<M>& a);
template<uint M> [[nodiscard]] vecT<M>	YYYY(const vecT<M>& a);
template<uint M> [[nodiscard]] vecT<M>	ZZZZ(const vecT<M>& a);
template<uint M> [[nodiscard]] vecT<M>	WWWW(const vecT<M>& a);

//	R specifies the number of non-zero return elements desired, for example:
// dotV<2>(a, b) { f32 r = dot(a, b); return vecT<N>(r, r, 0, 0); }
template<uint R, uint M> [[nodiscard]] vecT<M>		DotV(const vecT<M>& a, const vecT<M>& b);
template<uint R, uint M> [[nodiscard]] vecT<M>		Length_sqV(const vecT<M>& v);
template<uint R, uint M> [[nodiscard]] vecT<M>		LengthV(const vecT<M>& v);
template<uint R, uint M> [[nodiscard]] vecT<M>		Normalize(const vecT<M>& v);
template<uint R, uint M> [[nodiscard]] vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
template<uint R, uint M> [[nodiscard]] vecT<M>		NormalizeSafe(const vecT<M>& v);
template<uint R, uint M> [[nodiscard]] vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
template<uint R, uint M> [[nodiscard]] vecT<M>		NormalizeFast(const vecT<M>& v);
template<uint R, uint M> [[nodiscard]] vecT<M>		NormalizeFastSafe(const vecT<M>& v);

///	Rearranges the elements of a single vector, where each template parameter indexes one element in the source vector.
/// permute<3,1,0,2>(v) = v.w, v.y, v.x, v.z
template<int T0, int T1, int T2, int T3, uint M>
[[nodiscard]] vecT<M> Permute(const vecT<M>& v);

///	Rearranges the elements of a 2 vectors, where each template parameter indexes one element in one of the source vectors.
/// permute<b3,a1,a0,b2>(a, b) = b.w, a.y, a.x, b.z
template<int T0, int T1, int T2, int T3, uint M>
[[nodiscard]] vecT<M> Permute(const vecT<M>& a, const vecT<M>& b);


//	Blend: return b(N) ? a[N] : b[N]
template<bool bX, bool bY, bool bZ, bool bW, uint M>
[[nodiscard]] vecT<M> Blend(const vecT<M>& a, const vecT<M>& b);

//	Blend: return msk ? a : b
template<uint M>
[[nodiscard]] vecT<M> Blend(const mask32x4& msk, const vecT<M>& a, const vecT<M>& b);


//	Geometry
template<uint M> [[nodiscard]] vecT<M>	PointOnRay(const vecT<M>& start, const vecT<M>& dir, f32 dist);
template<uint M> [[nodiscard]] vecT<M>	ClosestPointOnLineSegment(const vecT<M>& a, const vecT<M>& b, const vecT<M>& p);
template<uint M> void SinCos(const vecT<M>& a, vecT<M>& s, vecT<M>& c);
template<uint M> [[nodiscard]] vecT<M> Sin(const vecT<M>& y);
template<uint M> [[nodiscard]] vecT<M> Cos(const vecT<M>& y);




} // namespace FFTL

#include "Vector.inl"

