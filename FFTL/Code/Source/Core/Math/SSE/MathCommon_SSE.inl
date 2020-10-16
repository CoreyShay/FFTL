/*

Original author:
Corey Shay
corey@signalflowtechnologies.com

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef _FFTL_MATH_SSE2_INL
#define _FFTL_MATH_SSE2_INL
#else
#error "Already included"
#endif // !_FFTL_MATH_SSE2_INL

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) //warning 4127: conditional expression is constant
#pragma warning(disable : 4702) //warning C4702: unreachable code
#endif


namespace FFTL
{




template<>
FFTL_FORCEINLINE f32 Min(f32 a, f32 b)
{
	return _mm_cvtss_f32( _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE f64 Min(f64 a, f64 b)
{
	return _mm_cvtsd_f64( _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}
template<>
FFTL_FORCEINLINE f64 Min(f64 a, f32 b)
{
	return _mm_cvtsd_f64( _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}
template<>
FFTL_FORCEINLINE f64 Min(f32 a, f64 b)
{
	return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)));
}

template<>
FFTL_FORCEINLINE f32 Max(f32 a, f32 b)
{
	return _mm_cvtss_f32( _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE f64 Max(f64 a, f64 b)
{
	return _mm_cvtsd_f64( _mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}
template<>
FFTL_FORCEINLINE f64 Max(f64 a, f32 b)
{
	return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)));
}
template<>
FFTL_FORCEINLINE f64 Max(f32 a, f64 b)
{
	return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)));
}

template<>
FFTL_FORCEINLINE bool IsNan(f32 y)
{
	const __m128 v = _mm_set_ss(y);
#if defined(FFTL_SSE) && __has_include(<immintrin.h>) && defined(_MSC_VER)
	return _mm_comi_ss(v, v, _CMP_NEQ_UQ) != 0;
//	return _mm_comieq_ss(v, v) == 0;
#else
	return _mm_movemask_ps(_mm_cmpneq_ps(v, v)) != 0;
#endif
}
template<>
FFTL_FORCEINLINE bool IsNan(f64 y)
{
	const __m128d v = _mm_set_sd(y);
#if defined(FFTL_SSE2) && __has_include(<immintrin.h>) && defined(_MSC_VER)
	return _mm_comi_sd(v, v, _CMP_NEQ_UQ) != 0;
#else
	return _mm_movemask_pd(_mm_cmpneq_pd(v, v)) != 0;
#endif
}






FFTL_FORCEINLINE Vec4f V4fZero()
{
	return _mm_setzero_ps();
}
FFTL_FORCEINLINE Vec4f V4fLoadA(const f32* pf)
{
	// LoadA / StoreA is explicitly aligned for performance reasons.
	// If the assert fires, be sure to align your data accordingly,
	// or use LoadU / StoreU if necessary.
	FFTL_ASSERT(((size_t)pf & 15) == 0);
	return _mm_load_ps(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadU(const f32* pf)
{
	return _mm_loadu_ps(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadAR(const f32* pf)
{
	const Vec4f v = _mm_load_ps(pf);
	return _mm_shuffle_ps(v, v, FFTL_MM_SHUFFLE_XYZW(3,2,1,0));
}
FFTL_FORCEINLINE Vec4f V4fLoadUR(const f32* pf)
{
	const Vec4f v = _mm_loadu_ps(pf);
	return _mm_shuffle_ps(v, v, FFTL_MM_SHUFFLE_XYZW(3,2,1,0));
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	return _mm_load_ss(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	return _mm_castpd_ps( _mm_load_sd(reinterpret_cast<const f64*>(pf)) );
}
FFTL_FORCEINLINE Vec4f V4fLoad3(const f32* pf)
{
	__m128 v = _mm_load_ss(pf+2);
	v = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,0,3,3));
	v = _mm_loadl_pi(v, (const __m64*)pf);
	return v;
}
FFTL_FORCEINLINE void V4fStoreA(f32* pf, Vec4f_In v)
{
	// LoadA / StoreA is explicitly aligned for performance reasons.
	// If the assert fires, be sure to align your data accordingly,
	// or use LoadU / StoreU if necessary.
	FFTL_ASSERT(((size_t)pf & 15) == 0);
	_mm_store_ps(pf, v);
}
FFTL_FORCEINLINE void V4fStoreU(f32* pf, Vec4f_In v)
{
	_mm_storeu_ps(pf, v);
}
FFTL_FORCEINLINE void V4fStore1(f32* pf, Vec4f_In v)
{
	_mm_store_ss(pf, v);
}
FFTL_FORCEINLINE void V4fStore2(f32* pf, Vec4f_In v)
{
	_mm_storel_pi((__m64*)pf, v);
}
FFTL_FORCEINLINE void V4fStore3(f32* pf, Vec4f_In v)
{
	_mm_storel_pi((__m64*)pf, v);
	_mm_store_ss(pf+2, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)));
}
FFTL_FORCEINLINE void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD)
{
#if defined(FFTL_SSE4)
	reinterpret_cast<s32*>(pf)[iA] = _mm_extract_ps(v, 0);
	reinterpret_cast<s32*>(pf)[iB] = _mm_extract_ps(v, 1);
	reinterpret_cast<s32*>(pf)[iC] = _mm_extract_ps(v, 2);
	reinterpret_cast<s32*>(pf)[iD] = _mm_extract_ps(v, 3);
#else
	_mm_store_ss(pf+iA, v );
	_mm_store_ss(pf+iB, _mm_shuffle_ps(v, v, _MM_SHUFFLE(1,1,1,1)) );
	_mm_store_ss(pf+iC, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)) );
	_mm_store_ss(pf+iD, _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,3,3,3)) );
#endif
}
FFTL_FORCEINLINE Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w)
{
	return _mm_setr_ps(x, y, z, w);
}
FFTL_FORCEINLINE Vec4f V4fSet1(f32 x)
{
	return _mm_set_ss(x);
}
FFTL_FORCEINLINE Vec4f V4fSplat(f32 f)
{
#if defined(FFTL_AVX2)
	return _mm_broadcastss_ps(_mm_set_ss(f));
#else
	return _mm_set1_ps(f);
#endif
}
FFTL_FORCEINLINE Vec4f V4fSplat(const f32* pf)
{
#if defined(FFTL_AVX)
	return _mm_broadcast_ss(pf);
#else
	return _mm_load1_ps(pf);
#endif
}
FFTL_FORCEINLINE Vec4f V4fSplatXY(const f32* pf)
{
	return _mm_castpd_ps(_mm_load1_pd(reinterpret_cast<const f64*>(pf)));
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	return _mm_and_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	return _mm_andnot_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	return _mm_or_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	return _mm_xor_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	return _mm_add_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	return _mm_sub_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	return _mm_mul_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
	return _mm_div_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMin(Vec4f_In a, Vec4f_In b)
{
	return _mm_min_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMax(Vec4f_In a, Vec4f_In b)
{
	return _mm_max_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	return sse_MulAdd_ps(a, b, c);
}
FFTL_FORCEINLINE Vec4f V4fNMulAdd(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	return sse_NMulAdd_ps(a, b, c);
}
FFTL_FORCEINLINE Vec4f V4fMulSub(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	return sse_MulSub_ps(a, b, c);
}
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	return sse_AddMul_ps(a, b, c);
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	return sse_SubMul_ps(a, b, c);
}
FFTL_FORCEINLINE Vec4f V4fNegate(Vec4f_In v)
{
	return _mm_sub_ps(_mm_setzero_ps(), v);
}
FFTL_FORCEINLINE Vec4f V4fAbs(Vec4f_In v)
{
	return _mm_and_ps(v, _mm_sub_ps(_mm_setzero_ps(), v) );
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	return _mm_sqrt_ps(v);
}
FFTL_FORCEINLINE Vec4f V4fRcpSqrt(Vec4f_In v)
{
	return _mm_rsqrt_ps(v);
}
FFTL_FORCEINLINE Vec4f V4fRcp(Vec4f_In v)
{
	return _mm_rcp_ps(v);
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fHSumV(v) );
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	Vec4f r;
#if defined(FFTL_SSE4)
	r = _mm_hadd_ps(v, v);
	r = _mm_hadd_ps(r, r);
#else
	r = _mm_add_ps( v, _mm_shuffle_ps(v, v, FFTL_MM_SHUFFLE_XYZW(1,0,3,2)) );
	r = _mm_add_ps( r, _mm_shuffle_ps(r, r, FFTL_MM_SHUFFLE_XYZW(2,2,0,0)) );
#endif
	return r;
}
FFTL_FORCEINLINE Vec4f V4fCompareEq(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpeq_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fCompareNq(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpneq_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fCompareGt(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpgt_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fCompareLt(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmplt_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fCompareGe(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpge_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fCompareLe(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmple_ps(a, b);
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	return _mm_movemask_ps(v);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	return _mm_movemask_ps( _mm_cmpeq_ps(a, b) ) == 15;
}
FFTL_FORCEINLINE bool V4fIsAllZero(Vec4f_In v)
{
	return _mm_movemask_ps( _mm_cmpeq_ps(v, _mm_setzero_ps()) ) == 15;
}

FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpacklo_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpackhi_ps(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b)
{
	return _mm_shuffle_ps(a, b, FFTL_MM_SHUFFLE_XYZW(0,2,0,2));
}
FFTL_FORCEINLINE Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b)
{
	return _mm_shuffle_ps(a, b, FFTL_MM_SHUFFLE_XYZW(1,3,1,3));
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	return _mm_shuffle_ps(v, v, FFTL_MM_SHUFFLE_XYZW(3,2,1,0));
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return _mm_cvtss_f32(v);
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<1,1,1,1>(v) );
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<2,2,2,2>(v) );
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<3,3,3,3>(v) );
}




template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute(Vec4f_In v)
{
	static_assert(
		(T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3),
		"Template parameters need to be in range");

	return _mm_shuffle_ps( v, v, FFTL_MM_SHUFFLE_XYZW( T0, T1, T2, T3 ) );
}

template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 1, 2, 3>(Vec4f_In v)
{
	return v;
}

//	While this function may look scary, all the branches and switches actually compile away in release builds, leaving only 1 or 2 SSE instructions.
template <int T0, int T1, int T2, int T3>
FFTL_FORCEINLINE Vec4f V4fPermute(Vec4f_In a, Vec4f_In b)
{
	static_assert(
		(T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3 || T0 == 4 || T0 == 5 || T0 == 6 || T0 == 7) &&
		(T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3 || T1 == 4 || T1 == 5 || T1 == 6 || T1 == 7) &&
		(T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3 || T2 == 4 || T2 == 5 || T2 == 6 || T2 == 7) &&
		(T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3 || T3 == 4 || T3 == 5 || T3 == 6 || T3 == 7),
		"Template parameters need to be in range");

#if !defined(FFTL_ALLOW_SINGLE_COMPONENT_PERMUTE2)
	static_assert(
		!((T0 < 4) && (T1 < 4) && (T2 < 4) && (T3 < 4)) &&
		!((T0 > 3) && (T1 > 3) && (T2 > 3) && (T3 > 3)),
		"Use the single parameter V4fPermute function.");
#endif

	constexpr uint shufX = (T0 == 0 || T0 == 1 || T0 == 2 || T0 == 3) ? 0 : 1;
	constexpr uint shufY = (T1 == 0 || T1 == 1 || T1 == 2 || T1 == 3) ? 0 : 1;
	constexpr uint shufZ = (T2 == 0 || T2 == 1 || T2 == 2 || T2 == 3) ? 0 : 1;
	constexpr uint shufW = (T3 == 0 || T3 == 1 || T3 == 2 || T3 == 3) ? 0 : 1;

	//	Catch when we only need values from either a or b.
	FFTL_IF_CONSTEXPR ((T0 < 4) && (T1 < 4) && (T2 < 4) && (T3 < 4))
	{
		return _mm_shuffle_ps(a, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)));
	}
	else FFTL_IF_CONSTEXPR ((T0 > 3) && (T1 > 3) && (T2 > 3) && (T3 > 3))
	{
		return _mm_shuffle_ps(b, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)));
	}
	//	Catch the easy SSE condition when _X and _Y come from a different vector than _Z and _W.
	// Uses only 1 SSE shuffle instruction.
	else FFTL_IF_CONSTEXPR (shufX == shufY && shufZ == shufW)
	{
		FFTL_IF_CONSTEXPR (shufX == 0)	//	_X and _Y from v1. _Z and _W from v2.
			return _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
		else			//	_X and _Y from v2. _Z and _W from v1.
			return _mm_shuffle_ps( b, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
	}
#if defined(FFTL_SSE4)
	//	If we have SSE4, we can use the blend instruction as long as we don't need reordering
	else FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T2) == 2 && FFTL_PERMUTEMASK(T3) == 3)
	{
		return _mm_blend_ps(a, b, (shufX<<0) | (shufY<<1) | (shufZ<<2) | (shufW<<3));
	}
#endif
	//	Next 4 cases, when we need 2 32 bit words from each vector, but it can't be handled with a single SSE instruction, so we use 2.
	else FFTL_IF_CONSTEXPR (shufX==0 && shufY==1 && shufZ==0 && shufW==1)
	{
		const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T3)) );
		return _mm_shuffle_ps( t, t, FFTL_MM_SHUFFLE_XYZW(0,2,1,3) );
	}
	else FFTL_IF_CONSTEXPR (shufX==0 && shufY==1 && shufZ==1 && shufW==0)
	{
		const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0),FFTL_PERMUTEMASK( T3), FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2)) );
		return _mm_shuffle_ps( t, t, FFTL_MM_SHUFFLE_XYZW(0,2,3,1) );
	}
	else FFTL_IF_CONSTEXPR (shufX==1 && shufY==0 && shufZ==0 && shufW==1)
	{
		const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T3)) );
		return _mm_shuffle_ps( t, t, FFTL_MM_SHUFFLE_XYZW(2,0,1,3) );
	}
	else FFTL_IF_CONSTEXPR (shufX==1 && shufY==0 && shufZ==1 && shufW==0)
	{
		const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T1), FFTL_PERMUTEMASK(T3), FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T2)) );
		return _mm_shuffle_ps( t, t, FFTL_MM_SHUFFLE_XYZW(2,0,3,1) );
	}


	//	Next 2 cases, when we need 3 32 bit words from one vector, and 1 word from the other, we use 2 instructions.
	else FFTL_IF_CONSTEXPR (
		(shufX==0 && shufY==1 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==0 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==0 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==1 && shufW==0)	)
	{
		//	When we need one word from a
		FFTL_IF_CONSTEXPR (shufX==0)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T2) == 2 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(b, a, FFTL_MM_INSERTPS_MASK_HELPER(0, FFTL_PERMUTEMASK(T0), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), 3, FFTL_PERMUTEMASK(T1), 3) );
			return _mm_shuffle_ps( t, b, FFTL_MM_SHUFFLE_XYZW(0, 2, FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
		}
		else FFTL_IF_CONSTEXPR (shufY == 0)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T2) == 2 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(b, a, FFTL_MM_INSERTPS_MASK_HELPER(1, FFTL_PERMUTEMASK(T1), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( b, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), 3, FFTL_PERMUTEMASK(T1), 3) );
			return _mm_shuffle_ps( t, b, FFTL_MM_SHUFFLE_XYZW(0, 2, FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
		}
		else FFTL_IF_CONSTEXPR (shufZ == 0)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(b, a, FFTL_MM_INSERTPS_MASK_HELPER(2, FFTL_PERMUTEMASK(T2), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T2), 3, FFTL_PERMUTEMASK(T3), 3) );
			return _mm_shuffle_ps( b, t, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), 0, 2) );
		}
		else // shufW==0
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T2) == 2)
				return _mm_insert_ps(b, a, FFTL_MM_INSERTPS_MASK_HELPER(3, FFTL_PERMUTEMASK(T3), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( b, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T2), 3, FFTL_PERMUTEMASK(T3), 3) );
			return _mm_shuffle_ps( b, t, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), 0, 2) );
		}
	}
	else FFTL_IF_CONSTEXPR (
		(shufX==1 && shufY==0 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==1 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==1 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==0 && shufW==1)	)
	{
		//	When we need one word from b
		FFTL_IF_CONSTEXPR (shufX == 1)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T2) == 2 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(a, b, FFTL_MM_INSERTPS_MASK_HELPER(0, FFTL_PERMUTEMASK(T0), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( b, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), 3, FFTL_PERMUTEMASK(T1), 3) );
			return _mm_shuffle_ps( t, a, FFTL_MM_SHUFFLE_XYZW(0, 2, FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
		}
		else FFTL_IF_CONSTEXPR (shufY == 1)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T2) == 2 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(a, b, FFTL_MM_INSERTPS_MASK_HELPER(1, FFTL_PERMUTEMASK(T1), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), 3, FFTL_PERMUTEMASK(T1), 3) );
			return _mm_shuffle_ps( t, a, FFTL_MM_SHUFFLE_XYZW(0, 2, FFTL_PERMUTEMASK(T2), FFTL_PERMUTEMASK(T3)) );
		}
		else FFTL_IF_CONSTEXPR (shufZ == 1)
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T3) == 3)
				return _mm_insert_ps(a, b, FFTL_MM_INSERTPS_MASK_HELPER(2, FFTL_PERMUTEMASK(T2), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( b, a, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T2), 3, FFTL_PERMUTEMASK(T3), 3) );
			return _mm_shuffle_ps( a, t, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), 0, 2) );
		}
		else // shufW==1
		{
#if defined(FFTL_SSE4)
			FFTL_IF_CONSTEXPR (FFTL_PERMUTEMASK(T0) == 0 && FFTL_PERMUTEMASK(T1) == 1 && FFTL_PERMUTEMASK(T2) == 2)
				return _mm_insert_ps(a, b, FFTL_MM_INSERTPS_MASK_HELPER(3, FFTL_PERMUTEMASK(T3), 0, 0, 0, 0));
#endif
			const __m128 t = _mm_shuffle_ps( a, b, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T2), 3, FFTL_PERMUTEMASK(T3), 3) );
			return _mm_shuffle_ps( a, t, FFTL_MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T0), FFTL_PERMUTEMASK(T1), 0, 2) );
		}
	}
	else
	{
		FFTL_ASSERT_MSG(0, "Did we miss a case? Uh oh.");
		return _mm_setzero_ps();
	}

}

//	Compiler catch naive permutes
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 1, 2, 3>(Vec4f_In a, Vec4f_In b)
{
	(void)b;
	return a;
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 5, 6, 7>(Vec4f_In a, Vec4f_In b)
{
	(void)a;
	return b;
}

//	We can have the compiler catch some special cases where we can use a single unpack instruction rather than 2 shuffle instructions, or one with better throughput.
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 4, 1, 5>(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpacklo_ps(a, b);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 0, 5, 1>(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpacklo_ps(b, a);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<2, 6, 3, 7>(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpackhi_ps(a, b);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<6, 2, 7, 3>(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpackhi_ps(b, a);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 1, 2, 3>(Vec4f_In a, Vec4f_In b)
{
	return _mm_move_ss(a, b);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 5, 6, 7>(Vec4f_In a, Vec4f_In b)
{
	return _mm_move_ss(b, a);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<0, 1, 4, 5>(Vec4f_In a, Vec4f_In b)
{
	return _mm_movelh_ps(a, b);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<4, 5, 0, 1>(Vec4f_In a, Vec4f_In b)
{
	return _mm_movelh_ps(b, a);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<2, 3, 6, 7>(Vec4f_In a, Vec4f_In b)
{
	return _mm_movehl_ps(b, a);
}
template<>
FFTL_FORCEINLINE Vec4f V4fPermute<6, 7, 2, 3>(Vec4f_In a, Vec4f_In b)
{
	return _mm_movehl_ps(a, b);
}








FFTL_FORCEINLINE Vec2d V2dZero()
{
	return _mm_setzero_pd();
}
FFTL_FORCEINLINE Vec2d V2dLoadA(const f64* pf)
{
	return _mm_load_pd(pf);
}
FFTL_FORCEINLINE Vec2d V2dLoadU(const f64* pf)
{
	return _mm_loadu_pd(pf);
}
FFTL_FORCEINLINE void V2dStoreA(f64* pf, Vec2d v)
{
	_mm_store_pd(pf, v);
}
FFTL_FORCEINLINE void V2dStoreU(f64* pf, Vec2d_In v)
{
	_mm_storeu_pd(pf, v);
}
FFTL_FORCEINLINE Vec2d V2dSet(f64 x, f64 y)
{
	return _mm_setr_pd(x, y);
}
FFTL_FORCEINLINE Vec2d V2dAnd(Vec2d_In a, Vec2d_In b)
{
	return _mm_and_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAndNot(Vec2d_In a, Vec2d_In b)
{
	return _mm_andnot_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_or_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dXOr(Vec2d_In a, Vec2d_In b)
{
	return _mm_xor_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dAdd(Vec2d_In a, Vec2d_In b)
{
	return _mm_add_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSub(Vec2d_In a, Vec2d_In b)
{
	return _mm_sub_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dMul(Vec2d_In a, Vec2d_In b)
{
	return _mm_mul_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dDiv(Vec2d_In a, Vec2d_In b)
{
	return _mm_div_pd(a, b);
}
FFTL_FORCEINLINE Vec2d V2dSqrt(Vec2d_In v)
{
	return _mm_sqrt_pd(v);
}
FFTL_FORCEINLINE bool V2dIsEqual(Vec2d_In a, Vec2d_In b)
{
	return _mm_movemask_pd( _mm_cmpeq_pd(a, b) ) == 3;
}
FFTL_FORCEINLINE bool V2dIsAllZero(Vec2d_In v)
{
	return _mm_movemask_pd( _mm_cmpeq_pd(v, _mm_setzero_pd()) ) == 3;
}

FFTL_FORCEINLINE Vec4u V4uAdd(Vec4u_In a, Vec4u_In b)
{
	return _mm_add_epi32(a, b);
}
FFTL_FORCEINLINE Vec4u V4uSub(Vec4u_In a, Vec4u_In b)
{
	return _mm_sub_epi32(a, b);
}
FFTL_FORCEINLINE Vec4u V4uMul(Vec4u_In a, Vec4u_In b)
{
	return sse_MulInt32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iAdd(Vec4i_In a, Vec4i_In b)
{
	return _mm_add_epi32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iSub(Vec4i_In a, Vec4i_In b)
{
	return _mm_sub_epi32(a, b);
}
FFTL_FORCEINLINE Vec4i V4iMul(Vec4i_In a, Vec4i_In b)
{
	return sse_MulInt32(a, b);
}

FFTL_FORCEINLINE Vec4i V4fRoundToVfi(Vec4f_In a)
{
	return _mm_cvtps_epi32(a);
}


inline constexpr mask32x4::mask32x4(const __m128& v)
	: m_v(v)
{
}

FFTL_FORCEINLINE mask32x4& mask32x4::operator=(const __m128& v)
{
	m_v = v;
	return *this;
}

inline constexpr mask32x4::operator const __m128&() const
{
	return m_v;
}

FFTL_FORCEINLINE mask32x4::operator __m128&()
{
	return m_v;
}

FFTL_FORCEINLINE int mask32x4::ToIntMask() const
{
	return _mm_movemask_ps(m_v);
}

template<s32 x, s32 y, s32 z, s32 w>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromInts()
{
	return mask32x4(_mm_castsi128_ps(_mm_setr_epi32(x, y, z, w)));
}

template<bool x, bool y, bool z, bool w>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools()
{
	constexpr int ix = x ? 0 : 1;
	constexpr int iy = y ? 0 : 1;
	constexpr int iz = z ? 0 : 2;
	constexpr int iw = w ? 0 : 3;

	__m128 a = _mm_setzero_ps();
	a = _mm_cmpeq_ss(a, a);
	a = V4fPermute<ix, iy, iz, iw>(a);
	return mask32x4(a);
}

template<>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools<0, 0, 0, 0>()
{
	return mask32x4(_mm_setzero_ps());
}

template<>
FFTL_FORCEINLINE mask32x4 mask32x4::GenMaskFromBools<1, 1, 1, 1>()
{
	__m128 a = _mm_setzero_ps();
	a = _mm_cmpeq_ps(a, a);
	return mask32x4(a);
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt(int i)
{
	constexpr int ix = bX ? 0 : 2;
	constexpr int iy = bY ? 0 : 2;
	constexpr int iz = bZ ? 0 : 2;
	constexpr int iw = bW ? 0 : 2;

	__m128i a = _mm_cvtsi32_si128(i);
	a = _mm_shuffle_epi32(a, FFTL_MM_SHUFFLE_XYZW(ix, iy, iz, iw));
	return mask32x4(_mm_castsi128_ps(a));
}
template<>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt<1, 0, 0, 0>(int i)
{
	__m128i a = _mm_cvtsi32_si128(i);
	return mask32x4(_mm_castsi128_ps(a));
}
template<>
FFTL_FORCEINLINE mask32x4 mask32x4::PropagateInt<0, 0, 0, 0>(int)
{
	return mask32x4(_mm_setzero_ps());
}


FFTL_FORCEINLINE mask32x4 mask32x4::operator|(const mask32x4& b) const
{
	return mask32x4(_mm_or_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 mask32x4::operator&(const mask32x4& b) const
{
	return mask32x4(_mm_and_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 mask32x4::operator^(const mask32x4& b) const
{
	return mask32x4(_mm_xor_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator|(const T& b) const
{
	return T(_mm_or_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator&(const T& b) const
{
	return T(_mm_and_ps(m_v, b.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T mask32x4::operator^(const T& b) const
{
	return T(_mm_xor_ps(m_v, b.m_v));
}

template<typename T, typename> FFTL_FORCEINLINE T AndNot(const mask32x4& a, const T& b)
{
	return T(_mm_andnot_ps(b.m_v, a.m_v));
}
template<typename T, typename> FFTL_FORCEINLINE T AndNot(const T& a, const mask32x4& b)
{
	return T(_mm_andnot_ps(b.m_v, a.m_v));
}
FFTL_FORCEINLINE mask32x4 AndNot(const mask32x4& a, const mask32x4& b)
{
	return mask32x4(_mm_andnot_ps(b.m_v, a.m_v));
}

FFTL_FORCEINLINE f32_4 f32_4::operator|(const mask32x4& b) const
{
	return f32_4(_mm_or_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE f32_4 f32_4::operator&(const mask32x4& b) const
{
	return f32_4(_mm_and_ps(m_v, b.m_v));
}
FFTL_FORCEINLINE f32_4 f32_4::operator^(const mask32x4& b) const
{
	return f32_4(_mm_xor_ps(m_v, b.m_v));
}

FFTL_FORCEINLINE mask32x4 CmpEq(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmpeq_ps(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpNe(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmpneq_ps(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpLt(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmplt_ps(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpLe(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmple_ps(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpGt(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmpgt_ps(a.m_v, b.m_v));
}
FFTL_FORCEINLINE mask32x4 CmpGe(f32_4_In a, f32_4_In b)
{
	return mask32x4(_mm_cmpge_ps(a.m_v, b.m_v));
}

template<typename T, bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const T& a, const T& b)
{
	return T{ sse_blend<bX, bY, bZ, bW>(a.GetNative(), b.GetNative()) };
}
template<typename T>
FFTL_FORCEINLINE typename std::enable_if<std::is_base_of<f32_4, T>::value, T>::type Blend(const mask32x4& msk, const T& a, const T& b)
{
	return T{ sse_blend(msk, a.GetNative(), b.GetNative()) };
}




FFTL_FORCEINLINE bool GetCpuFlushDenormalMode()
{
	const u32 x = _MM_GET_FLUSH_ZERO_MODE();

	//	Return the flush-to-zero bit.
	return (x & _MM_FLUSH_ZERO_ON) != 0;
}
FFTL_FORCEINLINE void SetCpuFlushDenormalMode(bool bEnable)
{
	//	Enable or disable the flush to zero bit
	const u32 x = bEnable ? _MM_FLUSH_ZERO_ON : _MM_FLUSH_ZERO_OFF;

	//	Store the new x into the floating-point status and control register
	_MM_SET_FLUSH_ZERO_MODE(x);
}


} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif
