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
class vecmask;

#if FFTL_SSE
typedef __m128 vecNative;
#elif FFTL_ARM_NEON
typedef float32x4_t vecNative;
#else
typedef f32 vecNative[4];
#endif


template<uint N>
class vecT : public f32_4
{
public:

	enum enZeroType { ZERO };

	//
	// Constructors
	//

	/// WARNING: The default constructor does not initialize. In debug it initializes with sNaNs.
	vecT();

	vecT(enZeroType);
	constexpr vecT(const vecT& v);
	constexpr vecT(const f32_4& v);
	vecT& operator=(const vecT& v);

#if defined(FFTL_SIMD_F32x4)
	constexpr vecT(const Vec4f& v);
	vecT& operator=(const Vec4f& v);
	FFTL_NODISCARD constexpr operator const Vec4f&() const;
	FFTL_NODISCARD operator Vec4f&();
#else
	FFTL_NODISCARD f32				Get(uint n) const { return m_v[n]; }
	FFTL_NODISCARD const f32*		Ptr() const { return m_v; }
	FFTL_NODISCARD f32*				Ptr() { return m_v; }
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
	FFTL_NODISCARD constexpr const vecNative&	AsIntrinsic() const { return m_v; }

	//	reinterprets as vecmask
	FFTL_NODISCARD FFTL_FORCEINLINE const vecmask&	AsVecfcmp() const { return *reinterpret_cast<const vecmask*>(this); }
	FFTL_NODISCARD FFTL_FORCEINLINE vecmask&		AsVecfcmp() { return *reinterpret_cast<vecmask*>(this); }

	//	Casting (reinterpreting) between N types (all data is compatible)
	template<uint M>
	FFTL_NODISCARD const vecT<M>&	CastTo() const { return *reinterpret_cast<const vecT<M>*>(this); }
	template<uint M>
	FFTL_NODISCARD vecT<M>&			CastTo() { return *reinterpret_cast<vecT<M>*>(this); }
	
	FFTL_NODISCARD FFTL_FORCEINLINE const vecT<4>& CastTo4() const { return CastTo<4>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE const vecT<3>& CastTo3() const { return CastTo<3>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE const vecT<2>& CastTo2() const { return CastTo<2>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE const vecT<1>& CastTo1() const { return CastTo<1>(); }
	
	FFTL_NODISCARD FFTL_FORCEINLINE vecT<4>& CastTo4() { return CastTo<4>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE vecT<3>& CastTo3() { return CastTo<3>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE vecT<2>& CastTo2() { return CastTo<2>(); }
	FFTL_NODISCARD FFTL_FORCEINLINE vecT<1>& CastTo1() { return CastTo<1>(); }

	//
	// Getters
	//

	FFTL_NODISCARD f32	GetX() const;
	FFTL_NODISCARD f32	GetY() const;
	FFTL_NODISCARD f32	GetZ() const;
	FFTL_NODISCARD f32	GetW() const;

	///	Extracts and reinterprets the binary representation of the f32 to an int
	int					GetAsIntX() const;
	int					GetAsIntY() const;
	int					GetAsIntZ() const;
	int					GetAsIntW() const;
	int					GetAsIntR() const { return GetAsIntX(); }
	int					GetAsIntG() const { return GetAsIntY(); }
	int					GetAsIntB() const { return GetAsIntZ(); }
	int					GetAsIntA() const { return GetAsIntW(); }

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
	FFTL_NODISCARD vecT				operator-() const;
	FFTL_NODISCARD vecT				operator+(const vecT& b) const;
	FFTL_NODISCARD vecT				operator-(const vecT& b) const;
	FFTL_NODISCARD vecT				operator*(const vecT& b) const;
	FFTL_NODISCARD vecT				operator/(const vecT& b) const;
	FFTL_NODISCARD bool				operator==(const vecT& b) const;
	FFTL_NODISCARD bool				operator!=(const vecT& b) const;

	FFTL_NODISCARD vecT				operator|(const vecmask& b) const;
	FFTL_NODISCARD vecT				operator&(const vecmask& b) const;
	FFTL_NODISCARD vecT				operator^(const vecmask& b) const;
	template<uint M> friend vecT<M>	AndNot(const vecT<M>& a, const vecmask& b);
	template<uint M> friend vecT<M>	AndNot(const vecmask& a, const vecT<M>& b);

	vecT&				operator|=(const vecmask& b);
	vecT&				operator&=(const vecmask& b);
	vecT&				operator^=(const vecmask& b);


	//
	// Comparisons that return a vecmask compare mask
	//

	template<uint M> friend vecmask		CmpEq(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecmask		CmpNe(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecmask		CmpLt(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecmask		CmpLe(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecmask		CmpGt(const vecT<M>& a, const vecT<M>& b);
	template<uint M> friend vecmask		CmpGe(const vecT<M>& a, const vecT<M>& b);


	//
	// Surgical arithmetic affecting only the selected elements
	//

	template<bool bX, bool bY, bool bZ, bool bW> FFTL_NODISCARD vecT	Add(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> FFTL_NODISCARD vecT	Sub(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> FFTL_NODISCARD vecT	Mul(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> FFTL_NODISCARD vecT	Div(f32 f) const;
	template<bool bX, bool bY, bool bZ, bool bW> FFTL_NODISCARD vecT	Negate() const;

	//
	// Surgical arithmetic affecting only one element
	//

	FFTL_NODISCARD vecT		AddX(f32 f) const;
	FFTL_NODISCARD vecT		AddY(f32 f) const;
	FFTL_NODISCARD vecT		AddZ(f32 f) const;
	FFTL_NODISCARD vecT		AddW(f32 f) const;
	FFTL_NODISCARD vecT		SubX(f32 f) const;
	FFTL_NODISCARD vecT		SubY(f32 f) const;
	FFTL_NODISCARD vecT		SubZ(f32 f) const;
	FFTL_NODISCARD vecT		SubW(f32 f) const;
	FFTL_NODISCARD vecT		MulX(f32 f) const;
	FFTL_NODISCARD vecT		MulY(f32 f) const;
	FFTL_NODISCARD vecT		MulZ(f32 f) const;
	FFTL_NODISCARD vecT		MulW(f32 f) const;
	FFTL_NODISCARD vecT		DivX(f32 f) const;
	FFTL_NODISCARD vecT		DivY(f32 f) const;
	FFTL_NODISCARD vecT		DivZ(f32 f) const;
	FFTL_NODISCARD vecT		DivW(f32 f) const;
	FFTL_NODISCARD vecT		NegateX() const;
	FFTL_NODISCARD vecT		NegateY() const;
	FFTL_NODISCARD vecT		NegateZ() const;
	FFTL_NODISCARD vecT		NegateW() const;


	FFTL_NODISCARD vecT		Negate() const;
	FFTL_NODISCARD vecT		ZeroNonFinite() const;
	FFTL_NODISCARD vecT		ReplaceNonFinite(const vecT& a) const;



	template<bool ALIGNED>
	FFTL_NODISCARD static vecT	Load(const f32* p);

	FFTL_NODISCARD static vecT			LoadA(const f32* p);
	FFTL_NODISCARD static vecT			LoadU(const f32* p);
	FFTL_NODISCARD static vecT			Splat(f32 f);
	FFTL_NODISCARD static vecT			X000(f32 f); //(f, 0, 0, 0)
	
	template<bool ALIGNED, uint M>
	friend  void						Store(f32* p, const vecT<M>& v);
	template<uint M> friend void		StoreA(f32* p, const vecT<M>& v);
	template<uint M> friend void		StoreU(f32* p, const vecT<M>& v);

	template<uint M> friend bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, f32 tol);
	template<uint M> friend vecmask		IsNearEqualV(const vecT<M>& a, const vecT<M>& b, f32 tol);
	template<uint M> friend bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
	template<uint M> friend vecmask		IsNearEqualV(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
	template<uint M> friend bool		IsNearZero(const vecT<M>& v, f32 tol);
	template<uint M> friend bool		IsNearZero(const vecT<M>& v, const vecT<M>& tol);
	template<uint M> friend vecmask		IsNearZeroV(const vecT<M>& v, f32 tol);
	template<uint M> friend vecmask		IsNearZeroV(const vecT<M>& v, const vecT<M>& tol);

	template<uint M> friend bool		IsNormal(const vecT<M>& v);
	template<uint M> friend bool		IsNan(const vecT<M>& v);			// Returns true if any component is NaN
	template<uint M> friend vecmask		IsNanV(const vecT<M>& v);			// Sets the return mask for any NaN component
	template<uint M> friend bool		IsInf(const vecT<M>& v);			// Returns true if any component is infinite
	template<uint M> friend vecmask		IsInfV(const vecT<M>& v);			// Sets the return mask for any infinite component
	template<uint M> friend bool		IsFinite(const vecT<M>& v);			// Returns true only if all components are finite
	template<uint M> friend vecmask		IsFiniteV(const vecT<M>& v);		// Sets the return mask for any finite component
	template<uint M> friend bool		IsOutrageous(const vecT<M>& v);		// Returns true if any component is not finite
	template<uint M> friend vecmask		IsOutrageousV(const vecT<M>& v);	// Sets the return mask for any NaN or infinite component
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
	template<u32 x, u32 y, u32 z, u32 w>	FFTL_NODISCARD static vecT	GenConstRtp();
	template<u32 val>						FFTL_NODISCARD static vecT	GenConstRtp();
	
	FFTL_NODISCARD static vecT					Zero();
	FFTL_NODISCARD static vecT					Infinity();



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



	//	Geometry functions
	template<uint M> friend vecT<M>	PointOnRay(const vecT<M>& start, const vecT<M>& dir, f32 dist);
	template<uint M> friend vecT<M>	ClosestPointOnLineSegment(const vecT<M>& a, const vecT<M>& b, const vecT<M>& p);

	//	Utility functions
	template<bool bX, bool bY, bool bZ, bool bW, uint M>
	friend vecT<M> Blend(const vecT<M>& a, const vecT<M>& b);
	template<uint M>
	friend vecT<M> Blend(const vecT<M>& a, const vecT<M>& b, const vecmask& msk);

	//	Geometry
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








// Purpose: vecmask - Holds a vector comparison result, or used to mask vectors using bitwise logical operators.
class vecmask
{
	template<uint N> friend class vecT;

public:
	vecmask();
	constexpr vecmask(const vecmask& v);
	vecmask& operator=(const vecmask& v);

#if defined(FFTL_SSE)
	constexpr vecmask(const __m128& v);
	vecmask& operator=(const __m128& v);
	FFTL_NODISCARD constexpr operator const __m128&() const;
	operator __m128&();
#elif defined(FFTL_ARM_NEON)
	constexpr vecmask(const uint32x4_t& v);
	vecmask& operator=(const uint32x4_t& v);
	FFTL_NODISCARD constexpr operator const uint32x4_t&() const;
	FFTL_NODISCARD operator uint32x4_t&();
#else
	u32								Get(uint n) const { return m_v[n]; }
	const u32*						Ptr() const { return m_v; }
	u32*							Ptr() { return m_v; }
#endif

	FFTL_NODISCARD vecmask			operator|(const vecmask& b) const;
	FFTL_NODISCARD vecmask			operator&(const vecmask& b) const;
	FFTL_NODISCARD vecmask			operator^(const vecmask& b) const;
	friend vecmask					AndNot	 (const vecmask& a, const vecmask& b);

	vecmask&						operator|=(const vecmask& b);
	vecmask&						operator&=(const vecmask& b);
	vecmask&						operator^=(const vecmask& b);

	template<uint N> FFTL_NODISCARD vecT<N>	operator|(const vecT<N>& b) const;
	template<uint N> FFTL_NODISCARD vecT<N>	operator&(const vecT<N>& b) const;
	template<uint N> FFTL_NODISCARD vecT<N>	operator^(const vecT<N>& b) const;
	template<uint N> friend vecmask	AndNot(const vecT<N>& a, const vecmask& b);
	template<uint N> friend vecmask	AndNot(const vecmask& a, const vecT<N>& b);

	FFTL_NODISCARD int							ToIntMask() const; // returns a 4 bit int containing the high bit from each element

	template<bool bX, bool bY, bool bZ, bool bW>	FFTL_NODISCARD static vecmask	GenMaskFromBools();
	template<s32 x, s32 y, s32 z, s32 w>			FFTL_NODISCARD static vecmask	GenMaskFromInts();
	template<bool bX, bool bY, bool bZ, bool bW>	FFTL_NODISCARD static vecmask	PropagateInt(int i);

private:
#if defined(FFTL_SSE)
	__m128	m_v;
#elif defined(FFTL_ARM_NEON)
	uint32x4_t m_v;
#else
	alignas(16) u32 m_v[4];
#endif
};





template<uint M> FFTL_NODISCARD vecT<M>	InsertX(const vecT<M>& v, f32 f);
template<uint M> FFTL_NODISCARD vecT<M>	InsertY(const vecT<M>& v, f32 f);
template<uint M> FFTL_NODISCARD vecT<M>	InsertZ(const vecT<M>& v, f32 f);
template<uint M> FFTL_NODISCARD vecT<M>	InsertW(const vecT<M>& v, f32 f);

template<bool bX, bool bY, bool bZ, bool bW, uint M>
FFTL_NODISCARD vecT<M> ZeroElements(const vecT<M>& v);












template<uint M> FFTL_NODISCARD vecT<M>	AndNot(const vecT<M>& a, const vecmask& b);
template<uint M> FFTL_NODISCARD vecT<M>	AndNot(const vecmask& a, const vecT<M>& b);


//
// Comparisons that return a vecmask compare mask
//

template<uint M> FFTL_NODISCARD vecmask		CmpEq(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecmask		CmpNe(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecmask		CmpLt(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecmask		CmpLe(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecmask		CmpGt(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecmask		CmpGe(const vecT<M>& a, const vecT<M>& b);



template<bool ALIGNED, uint M> void		Store(f32* p, const vecT<M>& v);
template<uint M> void					StoreA(f32* p, const vecT<M>& v);
template<uint M> void					StoreU(f32* p, const vecT<M>& v);

template<uint M> FFTL_NODISCARD bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, f32 tol);
template<uint M> FFTL_NODISCARD vecmask		IsNearEqualV(const vecT<M>& a, const vecT<M>& b, f32 tol);
template<uint M> FFTL_NODISCARD bool		IsNearEqual(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
template<uint M> FFTL_NODISCARD vecmask		IsNearEqualV(const vecT<M>& a, const vecT<M>& b, const vecT<M>& tol);
template<uint M> FFTL_NODISCARD bool		IsNearZero(const vecT<M>& v, f32 tol);
template<uint M> FFTL_NODISCARD bool		IsNearZero(const vecT<M>& v, const vecT<M>& tol);
template<uint M> FFTL_NODISCARD vecmask		IsNearZeroV(const vecT<M>& v, f32 tol);
template<uint M> FFTL_NODISCARD vecmask		IsNearZeroV(const vecT<M>& v, const vecT<M>& tol);

template<uint M> FFTL_NODISCARD bool		IsNormal(const vecT<M>& v);
template<uint M> FFTL_NODISCARD bool		IsNan(const vecT<M>& v);			// Returns true if any component is NaN
template<uint M> FFTL_NODISCARD vecmask		IsNanV(const vecT<M>& v);			// Sets the return mask for any NaN component
template<uint M> FFTL_NODISCARD bool		IsInf(const vecT<M>& v);			// Returns true if any component is infinite
template<uint M> FFTL_NODISCARD vecmask		IsInfV(const vecT<M>& v);			// Sets the return mask for any infinite component
template<uint M> FFTL_NODISCARD bool		IsFinite(const vecT<M>& v);			// Returns true only if all components are finite
template<uint M> FFTL_NODISCARD vecmask		IsFiniteV(const vecT<M>& v);		// Sets the return mask for any finite component
template<uint M> FFTL_NODISCARD bool		IsOutrageous(const vecT<M>& v);		// Returns true if any component is not finite
template<uint M> FFTL_NODISCARD vecmask		IsOutrageousV(const vecT<M>& v);	// Sets the return mask for any NaN or infinite component
template<uint M> FFTL_NODISCARD f32			Length_sq(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>		Length_sqV(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32			Length(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>		LengthV(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32			Length_fast(const vecT<M>& v);

template<uint M> FFTL_NODISCARD vecT<M>		Normalize(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
template<uint M> FFTL_NODISCARD vecT<M>		NormalizeSafe(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
template<uint M> FFTL_NODISCARD vecT<M>		NormalizeFast(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>		NormalizeFastSafe(const vecT<M>& v);

template<uint M> FFTL_NODISCARD vecT<M>		ZeroNegative(const vecT<M>& v);

template<uint M> FFTL_NODISCARD vecT<M>		Reflect(const vecT<M>& a, const vecT<M>& n);
template<uint M> FFTL_NODISCARD vecT<M>		Project(const vecT<M>& a, const vecT<M>& p); /// Projects p onto a (NOT VICE VERSA!)



//
//	vecT functions outside the class.
//

template<uint M> FFTL_NODISCARD const vecT<2>&	AsVec2(const vecT<M>& a);
template<uint M> FFTL_NODISCARD const vecT<3>&	AsVec3(const vecT<M>& a);
template<uint M> FFTL_NODISCARD const vecT<4>&	AsVec4(const vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<2>&		AsVec2(vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<3>&		AsVec3(vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<4>&		AsVec4(vecT<M>& a);

template<uint M> FFTL_NODISCARD vecT<M>	Min(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecT<M>	Max(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD f32		Min(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32		Max(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	MinV(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	MaxV(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32		MinAbs(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32		MaxAbs(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	Clamp(const vecT<M>& val, const vecT<M>& lhs, const vecT<M>& b);
template<uint M> FFTL_NODISCARD u32		MinIndex(const vecT<M>& v); // Returns the index of min lowest value
template<uint M> FFTL_NODISCARD u32		MaxIndex(const vecT<M>& v); // Returns the index of max lowest value
template<uint M> FFTL_NODISCARD vecT<M>	Sqrt(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	RSqrt(const vecT<M>& v); // Newton-Rhapson fast inverse square root
template<uint M> FFTL_NODISCARD vecT<M>	Lerp(const vecT<M>& lhs, const vecT<M>& b, f32 amt);
template<uint M> FFTL_NODISCARD vecT<M>	Abs(const vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<M>	AbsDiff(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecT<M>	Pow4(const vecT<M>& a, f32 exp);
template<uint M> FFTL_NODISCARD vecT<M>	Deg2rad(const vecT<M>& degrees);
template<uint M> FFTL_NODISCARD vecT<M>	Rad2deg(const vecT<M>& radians);
template<uint M> FFTL_NODISCARD vecT<M>	Sign(const vecT<M>& a);
template<uint M> FFTL_NODISCARD f32		HSum(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	HSumV(const vecT<M>& v);
template<uint M> FFTL_NODISCARD f32		Dot(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecT<M>	DotV(const vecT<M>& a, const vecT<M>& b);
template<uint M> FFTL_NODISCARD vecT<M>	Cross(const vecT<M>& a, const vecT<M>& b, const vecT<M>& c);
template<uint M> FFTL_NODISCARD f32		Distance(const vecT<M>& lhs, const vecT<M>& b);
template<uint M> FFTL_NODISCARD f32		Distance_sq(const vecT<M>& lhs, const vecT<M>& b);
template<uint M> FFTL_NODISCARD f32		Mean(const vecT<M>& v);
template<uint M> FFTL_NODISCARD vecT<M>	XXXX(const vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<M>	YYYY(const vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<M>	ZZZZ(const vecT<M>& a);
template<uint M> FFTL_NODISCARD vecT<M>	WWWW(const vecT<M>& a);

//	R specifies the number of non-zero return elements desired, for example:
// dotV<2>(a, b) { f32 r = dot(a, b); return vecT<N>(r, r, 0, 0); }
template<uint R, uint M> FFTL_NODISCARD vecT<M>		DotV(const vecT<M>& a, const vecT<M>& b);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		Length_sqV(const vecT<M>& v);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		LengthV(const vecT<M>& v);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		Normalize(const vecT<M>& v);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		Normalize(const vecT<M>& v, f32& out_fLen);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		NormalizeSafe(const vecT<M>& v);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		NormalizeSafe(const vecT<M>& v, f32& out_fLen);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		NormalizeFast(const vecT<M>& v);
template<uint R, uint M> FFTL_NODISCARD vecT<M>		NormalizeFastSafe(const vecT<M>& v);

///	Rearranges the elements of a single vector, where each template parameter indexes one element in the source vector.
/// permute<3,1,0,2>(v) = v.w, v.y, v.x, v.z
template<int T0, int T1, int T2, int T3, uint M>
FFTL_NODISCARD vecT<M> Permute(const vecT<M>& v);

///	Rearranges the elements of a 2 vectors, where each template parameter indexes one element in one of the source vectors.
/// permute<b3,a1,a0,b2>(a, b) = b.w, a.y, a.x, b.z
template<int T0, int T1, int T2, int T3, uint M>
FFTL_NODISCARD vecT<M> Permute(const vecT<M>& a, const vecT<M>& b);



//	Geometry functions
template<uint M> FFTL_NODISCARD vecT<M>	PointOnRay(const vecT<M>& start, const vecT<M>& dir, f32 dist);
template<uint M> FFTL_NODISCARD vecT<M>	ClosestPointOnLineSegment(const vecT<M>& a, const vecT<M>& b, const vecT<M>& p);

//	Utility functions
template<bool bX, bool bY, bool bZ, bool bW, uint M>
FFTL_NODISCARD vecT<M> Blend(const vecT<M>& a, const vecT<M>& b);
template<uint M>
FFTL_NODISCARD vecT<M> Blend(const vecT<M>& a, const vecT<M>& b, const vecmask& msk);

//	Geometry
template<uint M> void SinCos(const vecT<M>& a, vecT<M>& s, vecT<M>& c);
template<uint M> FFTL_NODISCARD vecT<M> Sin(const vecT<M>& y);
template<uint M> FFTL_NODISCARD vecT<M> Cos(const vecT<M>& y);




FFTL_NODISCARD vecmask					AndNot(const vecmask& a, const vecmask& b);
template<uint N> FFTL_NODISCARD vecmask	AndNot(const vecT<N>& a, const vecmask& b);
template<uint N> FFTL_NODISCARD vecmask	AndNot(const vecmask& a, const vecT<N>& b);

} // namespace FFTL

#include "Vector.inl"

