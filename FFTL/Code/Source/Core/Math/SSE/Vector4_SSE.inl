#ifndef __VECTOR4_SSE_INL__
#define __VECTOR4_SSE_INL__
#else
#error "Already included"
#endif // !__VECTOR4_SSE_INL__


#if defined(FFTL_SSE)

#include "Utils_SSE.h"

namespace FFTL
{



//
//	Functions unique to vec4
//

FFTL_FORCEINLINE vec4 Cross(const vec4& a, const vec4& b, const vec4& c)
{
	//	Ripped from DirectX SDK's xnamathvector.inl

	const __m128 V1 = a.AsIntrinsic();
	const __m128 V2 = b.AsIntrinsic();
	const __m128 V3 = c.AsIntrinsic();

	// V2zwyz * V3wzwy
	__m128 vResult = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(2, 1, 3, 2));
	__m128 vTemp3 = _mm_shuffle_ps(V3, V3, _MM_SHUFFLE(1, 3, 2, 3));
	vResult = _mm_mul_ps(vResult, vTemp3);
	// - V2wzwy * V3zwyz
	__m128 vTemp2 = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(1, 3, 2, 3));
	vTemp3 = _mm_shuffle_ps(vTemp3, vTemp3, _MM_SHUFFLE(1, 3, 0, 1));
	vTemp2 = _mm_mul_ps(vTemp2, vTemp3);
	vResult = _mm_sub_ps(vResult, vTemp2);
	// term1 * V1yxxx
	__m128 vTemp1 = _mm_shuffle_ps(V1, V1, _MM_SHUFFLE(0, 0, 0, 1));
	vResult = _mm_mul_ps(vResult, vTemp1);

	// V2ywxz * V3wxwx
	vTemp2 = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(2, 0, 3, 1));
	vTemp3 = _mm_shuffle_ps(V3, V3, _MM_SHUFFLE(0, 3, 0, 3));
	vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
	// - V2wxwx * V3ywxz
	vTemp2 = _mm_shuffle_ps(vTemp2, vTemp2, _MM_SHUFFLE(2, 1, 2, 1));
	vTemp1 = _mm_shuffle_ps(V3, V3, _MM_SHUFFLE(2, 0, 3, 1));
	vTemp2 = _mm_mul_ps(vTemp2, vTemp1);
	vTemp3 = _mm_sub_ps(vTemp3, vTemp2);
	// vResult - temp * V1zzyy
	vTemp1 = _mm_shuffle_ps(V1, V1, _MM_SHUFFLE(1, 1, 2, 2));
	vTemp1 = _mm_mul_ps(vTemp1, vTemp3);
	vResult = _mm_sub_ps(vResult, vTemp1);

	// V2yzxy * V3zxyx
	vTemp2 = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(1, 0, 2, 1));
	vTemp3 = _mm_shuffle_ps(V3, V3, _MM_SHUFFLE(0, 1, 0, 2));
	vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
	// - V2zxyx * V3yzxy
	vTemp2 = _mm_shuffle_ps(vTemp2, vTemp2, _MM_SHUFFLE(2, 0, 2, 1));
	vTemp1 = _mm_shuffle_ps(V3, V3, _MM_SHUFFLE(1, 0, 2, 1));
	vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
	vTemp3 = _mm_sub_ps(vTemp3, vTemp1);
	// vResult + term * V1wwwz
	vTemp1 = _mm_shuffle_ps(V1, V1, _MM_SHUFFLE(2, 3, 3, 3));
	vTemp3 = _mm_mul_ps(vTemp3, vTemp1);
	vResult = _mm_add_ps(vResult, vTemp3);

	return vResult;
}

FFTL_FORCEINLINE void Transpose4x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& io_d)
{
#if 1
	const __m128 tmp0 = V4fPermute<A0, A1, B0, B1>(io_a, io_b);
	const __m128 tmp2 = V4fPermute<A2, A3, B2, B3>(io_a, io_b);
	const __m128 tmp1 = V4fPermute<A0, A1, B0, B1>(io_c, io_d);
	const __m128 tmp3 = V4fPermute<A2, A3, B2, B3>(io_c, io_d);

	io_a = V4fPermute<A0, A2, B0, B2>(tmp0, tmp1);
	io_b = V4fPermute<A1, A3, B1, B3>(tmp0, tmp1);
	io_c = V4fPermute<A0, A2, B0, B2>(tmp2, tmp3);
	io_d = V4fPermute<A1, A3, B1, B3>(tmp2, tmp3);
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





} // namespace FFTL

#endif // defined(SL_MATH_USE_SSE_VEC4)