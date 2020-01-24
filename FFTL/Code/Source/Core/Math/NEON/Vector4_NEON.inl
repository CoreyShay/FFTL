#ifndef __VECTOR4_NEON_INL__
#define __VECTOR4_NEON_INL__
#else
#error "Already included"
#endif // !__VECTOR4_NEON_INL__


#if defined(FFTL_ARM_NEON)

#include "Utils_NEON.h"

namespace FFTL
{



//
//	Functions unique to vec4
//

FFTL_FORCEINLINE vec4 Cross(const vec4& a, const vec4& b, const vec4& c)
{
	//	Ripped from DirectX SDK's xnamathvector.inl

	const float32x4_t V1 = a.AsIntrinsic();
	const float32x4_t V2 = b.AsIntrinsic();
	const float32x4_t V3 = c.AsIntrinsic();

	// V2zwyz * V3wzwy
	float32x4_t vResult = V4fPermute<2, 3, 5, 6>(V2, V2);
	float32x4_t vTemp3 = V4fPermute<3, 2, 7, 5>(V3, V3);
	vResult = vmulq_f32(vResult, vTemp3);
	// - V2wzwy * V3zwyz
	float32x4_t vTemp2 = V4fPermute<3, 2, 7, 5>(V2, V2);
	vTemp3 = V4fPermute<1, 0, 7, 5>(vTemp3, vTemp3);
	vTemp2 = vmulq_f32(vTemp2, vTemp3);
	vResult = vsubq_f32(vResult, vTemp2);
	// term1 * V1yxxx
	float32x4_t vTemp1 = V4fPermute<1, 0, 4, 4>(V1, V1);
	vResult = vmulq_f32(vResult, vTemp1);

	// V2ywxz * V3wxwx
	vTemp2 = V4fPermute<1, 3, 4, 6>(V2, V2);
	vTemp3 = V4fPermute<3, 0, 7, 4>(V3, V3);
	vTemp3 = vmulq_f32(vTemp3, vTemp2);
	// - V2wxwx * V3ywxz
	vTemp2 = V4fPermute<1, 2, 5, 6>(vTemp2, vTemp2);
	vTemp1 = V4fPermute<1, 3, 4, 6>(V3, V3);
	vTemp2 = vmulq_f32(vTemp2, vTemp1);
	vTemp3 = vsubq_f32(vTemp3, vTemp2);
	// vResult - temp * V1zzyy
	vTemp1 = V4fPermute<2, 2, 5, 5>(V1, V1);
	vTemp1 = vmulq_f32(vTemp1, vTemp3);
	vResult = vsubq_f32(vResult, vTemp1);

	// V2yzxy * V3zxyx
	vTemp2 = V4fPermute<1, 2, 4, 5>(V2, V2);
	vTemp3 = V4fPermute<2, 0, 5, 4>(V3, V3);
	vTemp3 = vmulq_f32(vTemp3, vTemp2);
	// - V2zxyx * V3yzxy
	vTemp2 = V4fPermute<1, 2, 4, 6>(vTemp2, vTemp2);
	vTemp1 = V4fPermute<1, 2, 4, 5>(V3, V3);
	vTemp1 = vmulq_f32(vTemp1, vTemp2);
	vTemp3 = vsubq_f32(vTemp3, vTemp1);
	// vResult + term * V1wwwz
	vTemp1 = V4fPermute<3, 3, 7, 6>(V1, V1);
	vTemp3 = vmulq_f32(vTemp3, vTemp1);
	vResult = vaddq_f32(vResult, vTemp3);

	return vResult;
}

FFTL_FORCEINLINE void Transpose4x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& io_d)
{
#if 1
	const float32x4_t tmp0 = V4fPermute<A0, A1, B0, B1>(io_a, io_b);
	const float32x4_t tmp2 = V4fPermute<A2, A3, B2, B3>(io_a, io_b);
	const float32x4_t tmp1 = V4fPermute<A0, A1, B0, B1>(io_c, io_d);
	const float32x4_t tmp3 = V4fPermute<A2, A3, B2, B3>(io_c, io_d);

	io_a = V4fPermute<A0, A2, B0, B2>(tmp0, tmp1);
	io_b = V4fPermute<A1, A3, B1, B3>(tmp0, tmp1);
	io_c = V4fPermute<A0, A2, B0, B2>(tmp2, tmp3);
	io_d = V4fPermute<A1, A3, B1, B3>(tmp2, tmp3);
#else
	const float32x4_t tmp0 = V4fPermute<>(io_a, io_b, 0x44);//xyxy
	const float32x4_t tmp2 = V4fPermute<>(io_a, io_b, 0xEE);//zwzw
	const float32x4_t tmp1 = V4fPermute<>(io_c, io_d, 0x44);
	const float32x4_t tmp3 = V4fPermute<>(io_c, io_d, 0xEE);

	io_a = V4fPermute<>(tmp0, tmp1, 0x88);//xzxz
	io_b = V4fPermute<>(tmp0, tmp1, 0xDD);//ywyw
	io_c = V4fPermute<>(tmp2, tmp3, 0x88);
	io_d = V4fPermute<>(tmp2, tmp3, 0xDD);
#endif
}





} // namespace FFTL

#endif // defined(SL_MATH_USE_SSE_VEC4)