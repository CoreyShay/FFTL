/*

Original author:
Corey Shay
cshay892@gmail.com

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

#ifndef _FFTL_MATH_AVX_INL
#define _FFTL_MATH_AVX_INL


namespace FFTL
{






FFTL_FORCEINLINE Vec8f V8fZero()
{
	return _mm256_setzero_ps();
}
FFTL_FORCEINLINE Vec8f V8fLoadA(const f32* pf)
{
	return _mm256_load_ps(pf);
}
FFTL_FORCEINLINE Vec8f V8fLoadU(const f32* pf)
{
	return _mm256_loadu_ps(pf);
}
FFTL_FORCEINLINE Vec8f V8fLoadAR(const f32* pf)
{
	return V8fReverse( V8fLoadA(pf) );
}
FFTL_FORCEINLINE Vec8f V8fLoadUR(const f32* pf)
{
	return V8fReverse( V8fLoadU(pf) );
}
FFTL_FORCEINLINE void V8fStoreA(f32* pf, Vec8f_In v)
{
	_mm256_store_ps(pf, v);
}
FFTL_FORCEINLINE void V8fStoreU(f32* pf, Vec8f_In v)
{
	_mm256_storeu_ps(pf, v);
}
FFTL_FORCEINLINE void V8fStore1(f32* pf, Vec8f_In v)
{
	_mm_store_ss(pf, _mm256_castps256_ps128(v));
}
FFTL_FORCEINLINE Vec8f V8fSet(f32 x, f32 y, f32 z, f32 w, f32 a, f32 b, f32 c, f32 d)
{
	return _mm256_setr_ps(x, y, z, w, a, b, c, d);
}
FFTL_FORCEINLINE Vec8f V8fSplat8(f32 f)
{
	return _mm256_set1_ps(f);
}
FFTL_FORCEINLINE Vec8f V8fSplat8(const f32* pf)
{
	return _mm256_broadcast_ss(pf);
}
FFTL_FORCEINLINE Vec8f V8fSplat(Vec4f_In v)
{
	return _mm256_permute2f128_ps(_mm256_castps128_ps256(v), _mm256_castps128_ps256(v), 0);
}
FFTL_FORCEINLINE Vec8f V8fAnd(Vec8f_In a, Vec8f_In b)
{
	return _mm256_and_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fAndNot(Vec8f_In a, Vec8f_In b)
{
	return _mm256_andnot_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fOr(Vec8f_In a, Vec8f_In b)
{
	return _mm256_or_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fXOr(Vec8f_In a, Vec8f_In b)
{
	return _mm256_xor_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fAdd(Vec8f_In a, Vec8f_In b)
{
	return _mm256_add_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fSub(Vec8f_In a, Vec8f_In b)
{
	return _mm256_sub_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fMul(Vec8f_In a, Vec8f_In b)
{
	return _mm256_mul_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fDiv(Vec8f_In a, Vec8f_In b)
{
	return _mm256_div_ps(a, b);
}
FFTL_FORCEINLINE Vec8f V8fSqrt(Vec8f_In v)
{
	return _mm256_sqrt_ps(v);
}
FFTL_FORCEINLINE bool V8fIsEqual(Vec8f_In a, Vec8f_In b)
{
	return _mm256_movemask_ps( _mm256_cmp_ps(a, b, _CMP_EQ_OQ) ) == 255;
}
FFTL_FORCEINLINE Vec8f V8fReverse(Vec8f_In v)
{
	Vec8f r;
	r = _mm256_shuffle_ps(v, v, _MM_SHUFFLE(0,1,2,3));
	r = _mm256_permute2f128_ps(r, r, 1);
	return r;
}


} // namespace FFTL

#endif //_FFTL_MATH_AVX_INL