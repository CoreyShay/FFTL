/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Some useful SSE specific inline functions to be shared.

#ifndef UTILS_SSE_INL
#define UTILS_SSE_INL
#else
#error "Already included"
#endif // !UTILS_SSE_INL


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4127)// conditional expression is constant
#endif

namespace FFTL
{


template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE __m128 sse_blend(const __m128& a, const __m128& b)
{
#if defined(FFTL_SSE4)
	constexpr int i0 = bX ? 0 : 1;
	constexpr int i1 = bY ? 0 : 1;
	constexpr int i2 = bZ ? 0 : 1;
	constexpr int i3 = bW ? 0 : 1;
	constexpr int mask = (i0) | (i1<<1) | (i2<<2) | (i3<<3);
	return _mm_blend_ps(a, b, mask);
#else
	constexpr int i0 = bX ? 4 : 0;
	constexpr int i1 = bY ? 5 : 1;
	constexpr int i2 = bZ ? 6 : 2;
	constexpr int i3 = bW ? 7 : 3;
	return V4fPermute<i0, i1, i2, i3>(a, b);
#endif
}

template<>
FFTL_FORCEINLINE __m128 sse_blend<0, 0, 0, 0>(const __m128& a, const __m128& b)
{
	(void)a;
	return b;
}
template<>
FFTL_FORCEINLINE __m128 sse_blend<1, 1, 1, 1>(const __m128& a, const __m128& b)
{
	(void)b;
	return a;
}
template<>
FFTL_FORCEINLINE __m128 sse_blend<0, 1, 1, 1>(const __m128& a, const __m128& b)
{
	return _mm_move_ss(a, b);
}
template<>
FFTL_FORCEINLINE __m128 sse_blend<1, 0, 0, 0>(const __m128& a, const __m128& b)
{
	return _mm_move_ss(b, a);
}

// r = mask ? b : a;
FFTL_FORCEINLINE __m128 sse_blend(const __m128& mask, const __m128& a, const __m128& b)
{
#if defined(FFTL_SSE4)
	return _mm_blendv_ps(b, a, mask);
#else
	return _mm_xor_ps(b, _mm_and_ps(_mm_xor_ps(b, a), mask));
#endif
}

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_FORCEINLINE __m128 sse_zero_elements(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_insert_ps(v, v, FFTL_MM_INSERTPS_MASK_HELPER(0, 0, bX, bY, bZ, bW));
#else
	return sse_blend<bX, bY, bZ, bW>(_mm_setzero_ps(), v);
#endif
}
template<>
FFTL_FORCEINLINE __m128 sse_zero_elements<1, 1, 1, 1>(const __m128&)
{
	return _mm_setzero_ps();
}

FFTL_FORCEINLINE __m128 sse_negate_ps(const __m128& v)
{
	return _mm_sub_ps(_mm_setzero_ps(), v);
}
FFTL_FORCEINLINE __m128 sse_negate_ss(const __m128& v)
{
	return _mm_sub_ss(_mm_setzero_ps(), v);
}
FFTL_FORCEINLINE __m128d sse_negate_pd(const __m128d& v)
{
	return _mm_sub_pd(_mm_setzero_pd(), v);
}
FFTL_FORCEINLINE __m128d sse_negate_sd(const __m128d& v)
{
	return _mm_sub_sd(_mm_setzero_pd(), v);
}

FFTL_FORCEINLINE __m128 sse_abs_ss(const __m128& a)
{
#if 0	// Fastest only if mask is in a register
	return _mm_andnot_ps(_mm_set_ss(-0.f), a);
#elif !defined(FFTL_64BIT)	// Could be faster than below if no bypass delay, and maybe ideal for non x64 because it only uses 1 register
	return _mm_castsi128_ps(_mm_srli_epi32(_mm_slli_epi32(_mm_castps_si128(a), 1), 1));
#elif 1 // Results in one and instruction if in a loop, and apparently as fast as the negate and trick otherwise
	const __m128i vZ = _mm_setzero_si128();
	__m128i vMsk = _mm_cmpeq_epi32(vZ, vZ);
	vMsk = _mm_srli_epi32(vMsk, 1);
	return _mm_and_ps(a, _mm_castsi128_ps(vMsk));
#else
	const __m128 vNeg = sse_negate_ss(a);
//	return _mm_max_ps(a, vNeg);
	return _mm_and_ps(a, vNeg); // NOTE: andps is usually faster than maxps, but watch for unintended behavior when NaN's are involved
#endif
}
FFTL_FORCEINLINE __m128d sse_abs_sd(const __m128d& a)
{
#if 0	// Fastest only if mask is in a register
	return _mm_andnot_pd(_mm_set_sd(-0.0), a);
#elif !defined(FFTL_64BIT)	// Could be faster than below if no bypass delay, and maybe ideal for non x64 because it only uses 1 register
	return _mm_castsi128_pd(_mm_srli_epi64(_mm_slli_epi64(_mm_castpd_si128(a), 1), 1));
#elif defined(FFTL_SSE4) // Results in one and instruction if in a loop, and apparently as fast as the negate and trick otherwise
	const __m128i vZ = _mm_setzero_si128();
	__m128i vMsk = _mm_cmpeq_epi64(vZ, vZ);
	vMsk = _mm_srli_epi64(vMsk, 1);
	return _mm_and_pd(a, _mm_castsi128_pd(vMsk));
#else
	const __m128d vNeg = sse_negate_sd(a);
//	return _mm_max_ps(a, vNeg);
	return _mm_and_pd(a, vNeg); // NOTE: andps is usually faster than maxps, but watch for unintended behavior when NaN's are involved
#endif
}

FFTL_FORCEINLINE __m128 sse_abs_ps(const __m128& a)
{
#if 0	// Fastest only if mask is in a register
	return _mm_andnot_ps(_mm_set1_ps(-0.f), a);
#elif !defined(FFTL_64BIT)	// Could be faster than below if no bypass delay, and maybe ideal for non x64 because it only uses 1 register
	return _mm_castsi128_ps(_mm_srli_epi32(_mm_slli_epi32(_mm_castps_si128(a), 1), 1));
#elif 1 // Results in one and instruction if in a loop, and apparently as fast as the negate and trick otherwise
	const __m128i vZ = _mm_setzero_si128();
	__m128i vMsk = _mm_cmpeq_epi32(vZ, vZ);
	vMsk = _mm_srli_epi32(vMsk, 1);
	return _mm_and_ps(a, _mm_castsi128_ps(vMsk));
#else
	const __m128 vNeg = sse_negate_ps(a);
//	return _mm_max_ps(a, vNeg);
	return _mm_and_ps(a, vNeg); // NOTE: andps is usually faster than maxps, but watch for unintended behavior when NaN's are involved
#endif
}
FFTL_FORCEINLINE __m128d sse_abs_pd(const __m128d& a)
{
#if 0	// Fastest only if mask is in a register
	return _mm_andnot_pd(_mm_set1_pd(-0.0), a);
#elif !defined(FFTL_64BIT)	// Could be faster than below if no bypass delay, and maybe ideal for non x64 because it only uses 1 register
	return _mm_castsi128_pd(_mm_srli_epi64(_mm_slli_epi64(_mm_castpd_si128(a), 1), 1));
#elif defined(FFTL_SSE4) // Results in one and instruction if in a loop, and apparently as fast as the negate and trick otherwise
	const __m128i vZ = _mm_setzero_si128();
	__m128i vMsk = _mm_cmpeq_epi64(vZ, vZ);
	vMsk = _mm_srli_epi64(vMsk, 1);
	return _mm_and_pd(a, _mm_castsi128_pd(vMsk));
#else
	const __m128d vNeg = sse_negate_pd(a);
//	return _mm_max_pd(a, vNeg);
	return _mm_and_pd(a, vNeg); // NOTE: andps is usually faster than maxps, but watch for unintended behavior when NaN's are involved
#endif
}

//	Remove the fractional part and round toward zero
FFTL_FORCEINLINE __m128 sse_truncate_ss(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ss(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	const __m128 fi = _mm_cvtepi32_ps(vi);						// Convert back to float
	return fi;
#endif
}
FFTL_FORCEINLINE __m128 sse_truncate_ps(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ps(v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	return _mm_cvtepi32_ps(vi);									// Convert back to f32
#endif
}
FFTL_FORCEINLINE __m128d sse_truncate_sd(const __m128d& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_sd(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#elif defined(FFTL_64BIT)
	const s64 vi = _mm_cvttsd_si64(v);							// Convert to integer with truncation
	return _mm_cvtsi64_sd(v, vi);								// Convert back to f32
#elif 1
	FFTL_MATH_ASSERT(fabs(_mm_cvtsd_f64(v)) <= static_cast<double>(0x7fffffff));	// We're limited to 32 bit integer precision for this to work.
	const __m128i vi = _mm_cvttpd_epi32(v);						// Convert to integer with truncation
	return _mm_cvtepi32_pd(vi);									// Convert back to float
#else
	const s64 vi = _mm_cvtsd_f64(v);							// Extract float from XMM
	return _mm_set_sd(vi);										// Convert back to f32
#endif
}

FFTL_FORCEINLINE __m128i sse_iFloor_ss(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_cvttps_epi32(_mm_round_ss(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	const __m128 fi = _mm_cvtepi32_ps(vi);						// Convert back to f32
	const __m128 cmp = _mm_cmpgt_ps(fi, v);						// Compare the truncated f32 to the original. If it's greater, this becomes 0xffffffff, else 0
	const __m128i r = _mm_add_epi32(vi, _mm_castps_si128(cmp));	// Add either 0 or -1 to the truncated value to correct input values below 0
	return r;
#endif
}
FFTL_FORCEINLINE __m128i sse_iFloor_sd(const __m128d& v)
{
#if defined(FFTL_SSE4)
	return _mm_cvttpd_epi32(_mm_round_sd(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));
#else
	const __m128i vi = _mm_cvttpd_epi32(v);						// Convert to integer with truncation
	const __m128d fi = _mm_cvtepi32_pd(vi);						// Convert back to f32
	const __m128d cmp = _mm_cmpgt_pd(fi, v);					// Compare the truncated f32 to the original. If it's greater, this becomes 0xffffffff, else 0
	const __m128i r = _mm_add_epi32(vi, _mm_castpd_si128(cmp));	// Add either 0 or -1 to the truncated value to correct input values below 0
	return r;
#endif
}
FFTL_FORCEINLINE __m128i sse_iFloor_ps(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_cvttps_epi32(_mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	const __m128 fi = _mm_cvtepi32_ps(vi);						// Convert back to f32
	const __m128 cmp = _mm_cmpgt_ps(fi, v);						// Compare the truncated f32 to the original. If it's greater, this becomes 0xffffffff, else 0
	const __m128i r = _mm_add_epi32(vi, _mm_castps_si128(cmp));	// Add either 0 or -1 to the truncated value to correct input values below 0
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_fFloor_ss(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ss(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
#else
	return _mm_cvtepi32_ps(sse_iFloor_ss(v));
#endif
}
FFTL_FORCEINLINE __m128d sse_fFloor_sd(const __m128d& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_sd(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
#else
	return _mm_cvtepi32_pd(sse_iFloor_sd(v));
#endif
}
FFTL_FORCEINLINE __m128 sse_fFloor_ps(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
#else
	return _mm_cvtepi32_ps(sse_iFloor_ps(v));
#endif
}
FFTL_FORCEINLINE __m128i sse_iCeil_ss(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_cvttps_epi32(_mm_round_ss(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	const __m128 fi = _mm_cvtepi32_ps(vi);						// Convert back to f32
	const __m128 cmp = _mm_cmplt_ps(fi, v);						// Compare the truncated f32 to the original. If it's less, this becomes 0xffffffff, else 0
	const __m128i r = _mm_sub_epi32(vi, _mm_castps_si128(cmp));	// Subtract either 0 or -1 from the truncated value to correct input values above 0
	return r;
#endif
}
FFTL_FORCEINLINE __m128i sse_iCeil_ps(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_cvttps_epi32(_mm_round_ps(v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));
#else
	const __m128i vi = _mm_cvttps_epi32(v);						// Convert to integer with truncation
	const __m128 fi = _mm_cvtepi32_ps(vi);						// Convert back to f32
	const __m128 cmp = _mm_cmplt_ps(fi, v);						// Compare the truncated f32 to the original. If it's less, this becomes 0xffffffff, else 0
	const __m128i r = _mm_sub_epi32(vi, _mm_castps_si128(cmp));	// Subtract either 0 or -1 from the truncated value to correct input values above 0
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_fCeil_ss(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ss(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
#else
	return _mm_cvtepi32_ps(sse_iCeil_ss(v));
#endif
}
FFTL_FORCEINLINE __m128 sse_fCeil_ps(const __m128& v)
{
#if defined(FFTL_SSE4)
	return _mm_round_ps(v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
#else
	return _mm_cvtepi32_ps(sse_iCeil_ps(v));
#endif
}

FFTL_FORCEINLINE __m128 sse_MulAdd_ps(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_macc_ps(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fmadd_ps(a, b, c);
#else
	__m128 r = _mm_mul_ps(a, b);
	r = _mm_add_ps(r, c);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_MulAdd_ss(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_macc_ss(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fmadd_ss(a, b, c);
#else
	__m128 r = _mm_mul_ss(a, b);
	r = _mm_add_ss(r, c);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_NMulAdd_ps(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_nmacc_ps(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fnmadd_ps(a, b, c);
#else
	__m128 r = _mm_mul_ps(a, b);
	r = _mm_sub_ps(c, r);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_NMulAdd_ss(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_nmacc_ss(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fnmadd_ss(a, b, c);
#else
	__m128 r = _mm_mul_ss(a, b);
	r = _mm_sub_ss(c, r);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_MulSub_ps(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_msub_ps(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fmsub_ps(a, b, c);
#else
	__m128 r = _mm_mul_ps(a, b);
	r = _mm_sub_ps(r, c);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_MulSub_ss(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_msub_ss(a, b, c);
#elif defined(FFTL_FMA3)
	return _mm_fmsub_ss(a, b, c);
#else
	__m128 r = _mm_mul_ss(a, b);
	r = _mm_sub_ps(r, c);
	return r;
#endif
}

FFTL_FORCEINLINE __m128 sse_AddMul_ps(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_macc_ps(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm_fmadd_ps(b, c, a);
#else
	__m128 r = _mm_mul_ps(b, c);
	r = _mm_add_ps(a, r);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_AddMul_ss(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_macc_ss(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm_fmadd_ss(b, c, a);
#else
	__m128 r = _mm_mul_ss(b, c);
	r = _mm_add_ss(a, r);
	return r;
#endif
}

FFTL_FORCEINLINE __m128 sse_SubMul_ps(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_nmacc_ps(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm_fnmadd_ps(b, c, a);
#else
	__m128 r = _mm_mul_ps(b, c);
	r = _mm_sub_ps(a, r);
	return r;
#endif
}
FFTL_FORCEINLINE __m128 sse_SubMul_ss(const __m128& a, const __m128& b, const __m128& c)
{
#if defined(FFTL_FMA4)
	return _mm_nmacc_ss(b, c, a);
#elif defined(FFTL_FMA3)
	return _mm_fnmadd_ss(b, c, a);
#else
	__m128 r = _mm_mul_ss(b, c);
	r = _mm_sub_ps(a, r);
	return r;
#endif
}

FFTL_FORCEINLINE __m128i sse_MulInt32(const __m128i &a, const __m128i &b)
{
#if defined(FFTL_SSE4) || defined(FFTL_ASSUME_SSE4)	// modern CPU - use SSE 4.1
	return _mm_mullo_epi32(a, b);
#else				// old CPU - use SSE 2
	__m128i tmp1 = _mm_mul_epu32(a, b); /* mul 2,0*/
	__m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a, 4), _mm_srli_si128(b, 4) ); /* mul 3,1 */
	return _mm_unpacklo_epi32( _mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0,0,2,0)) ); /* shuffle results to [63..0] and pack */
#endif
}

FFTL_FORCEINLINE __m128 sse_genMaskXYZ()
{
	__m128i a = _mm_setzero_si128();
	a = _mm_cmpeq_epi32(a, a);
	a = _mm_srli_si128(a, 4);
	return _mm_castsi128_ps(a);
}

template<u32 val>
FFTL_FORCEINLINE __m128 sse_constRtp()
{
	const __m128i r = _mm_set1_epi32(val);
//	const __m128i r = _mm_shuffle_epi32(_mm_cvtsi32_si128(val), 0);
	return _mm_castsi128_ps(r);
}
template<u32 x, u32 y, u32 z, u32 w>
FFTL_FORCEINLINE __m128 sse_constRtp()
{
	if constexpr (x == y && x == z && x == w)
	{
		return sse_constRtp<x>();
	}
	else if constexpr (y == 0 && z == 0 && w == 0)
	{
		return _mm_castsi128_ps( _mm_cvtsi32_si128(x) );
	}
	else
	{
		if constexpr (FFTL_HAS_CONSTEXPR_BIT_CAST)
		{
			FFTL_BIT_CAST_CONSTEXPR const f32 fx = bit_cast<f32>(x);
			FFTL_BIT_CAST_CONSTEXPR const f32 fy = bit_cast<f32>(y);
			FFTL_BIT_CAST_CONSTEXPR const f32 fz = bit_cast<f32>(z);
			FFTL_BIT_CAST_CONSTEXPR const f32 fw = bit_cast<f32>(w);

			return _mm_setr_ps(fx, fy, fz, fw);
		}
		else
		{
			alignas(16) constexpr u32 i[4] = { x, y, z, w };
			return bit_cast<const __m128>(i);
		}
	}
}

template<>
FFTL_FORCEINLINE __m128 sse_constRtp<0>()
{
	return _mm_setzero_ps();
}

template<>
FFTL_FORCEINLINE __m128 sse_constRtp<0x7F800000>() // infinity
{
	return _mm_rcp_ps(_mm_setzero_ps());
}

template<>
FFTL_FORCEINLINE __m128 sse_constRtp<0x7F800000, 0, 0, 0>() // infinity
{
	return _mm_rcp_ss(_mm_setzero_ps());
}

FFTL_FORCEINLINE __m128 sse_load2(const f32* p)
{
	return _mm_castpd_ps(_mm_load_sd((const double*)p));
}
FFTL_FORCEINLINE __m128 sse_load3(const f32* p)
{
#if 1
	const __m128 v01 = _mm_castpd_ps(_mm_load_sd((const double*)p));
	const __m128 v2 = _mm_load_ss(p + 2);
	return V4fPermute<0, 1, 4, 5>(v01, v2);
#else
	const __m128 v2 = _mm_load_ss(p + 2);
	return _mm_loadl_pi(V4fPermute<1,1,0,1>(v2), (const __m64*)p);
#endif
}

FFTL_FORCEINLINE void sse_store2(f32* p, const __m128& v)
{
	_mm_store_sd((double*)p, _mm_castps_pd(v));
}
FFTL_FORCEINLINE void sse_store3(f32* p, const __m128& v)
{
	const __m128 z = _mm_shuffle_ps(v, v, FFTL_MM_SHUFFLE_XYZW(2, 2, 2, 2));
	_mm_store_sd((double*)p, _mm_castps_pd(v));
	_mm_store_ss(p + 2, z);
}




FFTL_FORCEINLINE void sse_transpose4x4(__m128& io_a, __m128& io_b, __m128& io_c, __m128& io_d)
{
#if 1
	const __m128 tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const __m128 tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const __m128 tmp1 = V4fPermute<0, 1, 4, 5>(io_c, io_d);
	const __m128 tmp3 = V4fPermute<2, 3, 6, 7>(io_c, io_d);

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
	io_d = V4fPermute<1, 3, 5, 7>(tmp2, tmp3);
#else
	const __m128 tmp0 = _mm_shuffle_ps(io_a, io_b, 0x44);//xyxy
	const __m128 tmp2 = _mm_shuffle_ps(io_a, io_b, 0xEE);//zwzw
	const __m128 tmp1 = _mm_shuffle_ps(io_c, io_d, 0x44);
	const __m128 tmp3 = _mm_shuffle_ps(io_c, io_d, 0xEE);

	io_a = _mm_shuffle_ps(tmp0, tmp1, 0x88);//xzxz
	io_b = _mm_shuffle_ps(tmp0, tmp1, 0xDD);//ywyw
	io_c = _mm_shuffle_ps(tmp2, tmp3, 0x88);
	io_d = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
#endif
}

FFTL_FORCEINLINE void sse_transpose4x3(__m128& io_a, __m128& io_b, __m128& io_c, const __m128& in_d)
{
#if 1
	const __m128 tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const __m128 tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const __m128 tmp1 = V4fPermute<0, 1, 4, 5>(io_c, in_d);
	const __m128 tmp3 = V4fPermute<2, 3, 6, 7>(io_c, in_d);

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
#else
	const __m128 tmp0 = _mm_shuffle_ps(io_a, io_b, 0x44);
	const __m128 tmp2 = _mm_shuffle_ps(io_a, io_b, 0xEE);
	const __m128 tmp1 = _mm_shuffle_ps(io_c, in_d, 0x44);
	const __m128 tmp3 = _mm_shuffle_ps(io_c, in_d, 0xEE);

	io_a = _mm_shuffle_ps(tmp0, tmp1, 0x88);
	io_b = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
	io_c = _mm_shuffle_ps(tmp2, tmp3, 0x88);
#endif
}

FFTL_FORCEINLINE void sse_transpose3x4(__m128& io_a, __m128& io_b, __m128& io_c, __m128& ot_d)
{
	const __m128 tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const __m128 tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const __m128 tmp1 = V4fPermute<0, 1, 0, 1>(io_c);//2nd half will be garbage but not needed
	const __m128 tmp3 = V4fPermute<2, 3, 2, 3>(io_c);//2nd half will be garbage but not needed

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
	ot_d = V4fPermute<1, 3, 5, 7>(tmp2, tmp3);
}



} // namespace FFTL


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

