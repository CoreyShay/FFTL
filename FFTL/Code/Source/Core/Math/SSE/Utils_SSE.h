/*======================================================================*

Signal Flow Technologies comment

*======================================================================*/
// Purpose: Some useful SSE specific inline functions to be shared.

#pragma once


#include <emmintrin.h>
#if defined(FFTL_SSE4)
#	include <smmintrin.h>
#	if defined(FFTL_VECTOR_USE_AVX)
#		include <immintrin.h>
#	endif
#endif


#define _MM_SHUFFLE_XYZW(_x_,_y_,_z_,_w_) _MM_SHUFFLE(_w_,_z_,_y_,_x_)
#define _MM_DPPS_MASK_HELPER(_in_mask_, _out_mask_) ( (_out_mask_&15) | ((_in_mask_&15)<<4) )
#define _MM_INSERTPS_MASK_HELPER(_to_a, _from_b, _zeroX_, _zeroY_, _zeroZ_, _zeroW_) ( (_zeroX_&1) | ((_zeroY_&1)<<1) | ((_zeroZ_&1)<<2) | ((_zeroW_&1)<<3) | ((_to_a&3)<<4) | ((_from_b&3)<<6) )


namespace FFTL
{

// r = mask ? b : a;
FFTL_NODISCARD __m128 sse_blend(const __m128& a, const __m128& b, const __m128& mask);

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_NODISCARD __m128 sse_blend(const __m128& a, const __m128& b);

template<>
FFTL_NODISCARD __m128 sse_blend<0, 0, 0, 0>(const __m128& a, const __m128& b);

template<>
FFTL_NODISCARD __m128 sse_blend<1, 1, 1, 1>(const __m128& a, const __m128& b);

template<bool bX, bool bY, bool bZ, bool bW>
FFTL_NODISCARD __m128 sse_zero_elements(const __m128& v);

template<>
FFTL_NODISCARD __m128 sse_zero_elements<1, 1, 1, 1>(const __m128&);

FFTL_NODISCARD __m128 sse_negate_ps(const __m128& v);
FFTL_NODISCARD __m128 sse_negate_ss(const __m128& v);
FFTL_NODISCARD __m128d sse_negate_pd(const __m128d& v);
FFTL_NODISCARD __m128d sse_negate_sd(const __m128d& v);

FFTL_NODISCARD __m128 sse_abs_ss(const __m128& a);
FFTL_NODISCARD __m128d sse_abs_sd(const __m128d& a);
FFTL_NODISCARD __m128 sse_abs_ps(const __m128& a);
FFTL_NODISCARD __m128d sse_abs_pd(const __m128d& a);

//	Remove the fractional part and round toward zero
FFTL_NODISCARD __m128 sse_truncate_ss(const __m128& v);
FFTL_NODISCARD __m128 sse_truncate_ps(const __m128& v);
FFTL_NODISCARD __m128d sse_truncate_sd(const __m128d& v);

FFTL_NODISCARD __m128i sse_iFloor_ss(const __m128& v);
FFTL_NODISCARD __m128i sse_iFloor_sd(const __m128d& v);
FFTL_NODISCARD __m128i sse_iFloor_ps(const __m128& v);
FFTL_NODISCARD __m128 sse_fFloor_ss(const __m128& v);
FFTL_NODISCARD __m128d sse_fFloor_sd(const __m128d& v);
FFTL_NODISCARD __m128 sse_fFloor_ps(const __m128& v);
FFTL_NODISCARD __m128i sse_iCeil_ss(const __m128& v);
FFTL_NODISCARD __m128i sse_iCeil_ps(const __m128& v);
FFTL_NODISCARD __m128 sse_fCeil_ss(const __m128& v);
FFTL_NODISCARD __m128 sse_fCeil_ps(const __m128& v);

FFTL_NODISCARD __m128 sse_MulAdd_ps(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_MulAdd_ss(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_NMulAdd_ps(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_NMulAdd_ss(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_MulSub_ps(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_MulSub_ss(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_AddMul_ps(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_AddMul_ss(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_SubMul_ps(const __m128& a, const __m128& b, const __m128& c);
FFTL_NODISCARD __m128 sse_SubMul_ss(const __m128& a, const __m128& b, const __m128& c);

FFTL_NODISCARD __m128i sse_MulInt32(const __m128i &a, const __m128i &b);

FFTL_NODISCARD __m128 sse_genMaskXYZ();

template<u32 val>
FFTL_NODISCARD __m128 sse_constRtp();

template<u32 x, u32 y, u32 z, u32 w>
FFTL_NODISCARD __m128 sse_constRtp();

template<>
FFTL_NODISCARD __m128 sse_constRtp<0>();

template<>
FFTL_NODISCARD __m128 sse_constRtp<0x7F800000>(); // infinity

template<>
FFTL_NODISCARD __m128 sse_constRtp<0x7F800000, 0, 0, 0>(); // infinity

FFTL_NODISCARD __m128 sse_load2(const f32* p);
FFTL_NODISCARD __m128 sse_load3(const f32* p);

void sse_store2(f32* p, const __m128& v);
void sse_store3(f32* p, const __m128& v);

void sse_transpose4x4(__m128& io_a, __m128& io_b, __m128& io_c, __m128& io_d);
void sse_transpose4x3(__m128& io_a, __m128& io_b, __m128& io_c, const __m128& in_d);
void sse_transpose3x4(__m128& io_a, __m128& io_b, __m128& io_c, __m128& ot_d);

void sse_SinCos(const __m128& rads, __m128& s, __m128& c);



} // namespace FFTL

