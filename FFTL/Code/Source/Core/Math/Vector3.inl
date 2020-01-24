#ifndef __VECTOR3_INL__
#define __VECTOR3_INL__
#else
#error "Already included"
#endif // !__VECTOR3_INL__

#if defined(FFTL_SSE)
#	include "SSE/Vector3_SSE.inl"
#elif defined(FFTL_ARM_NEON)
#	include "NEON/Vector3_NEON.inl"
#else
#	include "Default/Vector3_Default.inl"
#endif

namespace FFTL
{


//
//	Functions unique to vec3
//

template<>
FFTL_FORCEINLINE vecT<3>::vecT(const vecT<2>& xy, f32 z)
{
	Set(xy, z);
}

FFTL_FORCEINLINE void Transpose4x3(vec3& io_a, vec3& io_b, vec3& io_c, const vec3& in_d)
{
	const Vec4f tmp0 = V4fPermute<0, 1, 4, 5>(io_a, io_b);
	const Vec4f tmp2 = V4fPermute<2, 3, 6, 7>(io_a, io_b);
	const Vec4f tmp1 = V4fPermute<0, 1, 4, 5>(io_c, in_d);
	const Vec4f tmp3 = V4fPermute<2, 3, 6, 7>(io_c, in_d);

	io_a = V4fPermute<0, 2, 4, 6>(tmp0, tmp1);
	io_b = V4fPermute<1, 3, 5, 7>(tmp0, tmp1);
	io_c = V4fPermute<0, 2, 4, 6>(tmp2, tmp3);
}



} // namespace FFTL
