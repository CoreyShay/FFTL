#ifndef __VECTOR2_INL__
#define __VECTOR2_INL__
#else
#error "Already included"
#endif // !__VECTOR2_INL__


namespace FFTL
{


FFTL_FORCEINLINE vec2 Cross(const vec2& a)
{
	vec2 r = Permute<1, 0, 1, 0>(a);
	return r.Negate<1, 0, 1, 0>();
}



} // namespace FFTL
