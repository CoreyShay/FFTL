#ifndef __VECTOR4_INL__
#define __VECTOR4_INL__
#else
#error "Already included"
#endif // !__VECTOR4_INL__

#if defined(FFTL_SSE)
#	include "SSE/Vector4_SSE.inl"
#elif defined(FFTL_ARM_NEON)
#	include "NEON/Vector4_NEON.inl"
#else
#	include "Default/Vector4_Default.inl"
#endif


namespace FFTL
{

FFTL_FORCEINLINE vec4 TransformXandY(const vec2& v, const vec4& col0, const vec4& col1)
{
	const vec4& v4 = v.CastTo4();
	vec4 r;
	r =					XXXX(v4) * col0;
	r = V4fAddMul(r,	YYYY(v4),  col1);

	return r;
}

FFTL_FORCEINLINE void Transpose3x4(vec4& io_a, vec4& io_b, vec4& io_c, vec4& ot_d)
{
	const Vec4f tmp0 = V4fPermute<A0, A1, B0, B1>(io_a, io_b);
	const Vec4f tmp2 = V4fPermute<A2, A3, B2, B3>(io_a, io_b);
	const Vec4f tmp1 = V4fPermute<0, 1, 0, 1>(io_c);//2nd half will be garbage but not needed
	const Vec4f tmp3 = V4fPermute<2, 3, 2, 3>(io_c);//2nd half will be garbage but not needed

	io_a = V4fPermute<A0, A2, B0, B2>(tmp0, tmp1);
	io_b = V4fPermute<A1, A3, B1, B3>(tmp0, tmp1);
	io_c = V4fPermute<A0, A2, B0, B2>(tmp2, tmp3);
	ot_d = V4fPermute<A1, A3, B1, B3>(tmp2, tmp3);
}

FFTL_FORCEINLINE void Transpose3x4_3x3(vec4& io_a, vec4& io_b, vec4& io_c)
{
	const Vec4f tmp0 = V4fPermute<A0, A1, B0, B1>(io_a, io_b);
	const Vec4f tmp2 = V4fPermute<A2, A3, B2, B3>(io_a, io_b);
	const Vec4f tmp1 = V4fPermute< 0, 1, 0, 1>(io_c);//2nd half will be garbage but not needed
	const Vec4f tmp3 = V4fPermute< 2, 3, 2, 3>(io_c);//2nd half will be garbage but not needed

	io_a = V4fPermute<A0, A2, B0, B2>(tmp0, tmp1);
	io_b = V4fPermute<A1, A3, B1, B3>(tmp0, tmp1);
	io_c = V4fPermute<A0, A2, B0, B2>(tmp2, tmp3);
}


} // namespace FFTL
