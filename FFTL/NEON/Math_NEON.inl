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

#ifndef _FFTL_MATH_DEFAULT_INL
#define _FFTL_MATH_DEFAULT_INL


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4127 ) //conditional expression is constant
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#endif

#define FFTL_SIMD_DIVIDE_APPROXIMATE 1


namespace FFTL
{




FFTL_FORCEINLINE Vec4f V4fZero()
{
	return vdupq_n_f32(0);
}
FFTL_FORCEINLINE Vec4f V4fLoadA(const f32* pf)
{
	return vld1q_f32(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadU(const f32* pf)
{
	return vld1q_f32(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoadAR(const f32* pf)
{
	const Vec4f v = vld1q_f32(pf);
	return V4fReverse(v);
}
FFTL_FORCEINLINE Vec4f V4fLoadUR(const f32* pf)
{
	const Vec4f v = vld1q_f32(pf);
	return V4fReverse(v);
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	const Vec4f v = vdupq_n_f32(0);
	return vld1q_lane_f32(pf, v, 0);
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	return vcombine_f32( vld1_f32(pf), vdup_n_f32(0) );
}
FFTL_FORCEINLINE Vec4f V4fLoad3(const f32* pf)
{
	float32x2_t v0 = vld1_f32(pf);
	float32x2_t v1 = vdup_n_f32(0);
	v1 = vld1_lane_f32(pf+2, v1, 0);
	return vcombine_f32(v0, v1);
}
FFTL_FORCEINLINE void V4fStoreA(f32* pf, Vec4f_In v)
{
	vst1q_f32(pf, v);
}
FFTL_FORCEINLINE void V4fStoreU(f32* pf, Vec4f_In v)
{
	vst1q_f32(pf, v);
}
FFTL_FORCEINLINE void V4fStore1(f32* pf, Vec4f_In v)
{
	vst1q_lane_f32(pf+0, v, 0);
}
FFTL_FORCEINLINE void V4fStore2(f32* pf, Vec4f_In v)
{
	vst1_f32(pf, vget_low_f32(v));
}
FFTL_FORCEINLINE void V4fStore3(f32* pf, Vec4f_In v)
{
	vst1_f32(pf, vget_low_f32(v));
	vst1q_lane_f32(pf+2, v, 2);
}
FFTL_FORCEINLINE void V4fScatter(f32* pf, Vec4f_In v, int iA, int iB, int iC, int iD)
{
	vst1q_lane_f32(pf+iA, v, 0);
	vst1q_lane_f32(pf+iB, v, 1);
	vst1q_lane_f32(pf+iC, v, 2);
	vst1q_lane_f32(pf+iD, v, 3);
}
FFTL_FORCEINLINE Vec4f V4fSet(f32 x, f32 y, f32 z, f32 w)
{
	Vec4f v;
	v = vsetq_lane_f32(x, v, 0);
	v = vsetq_lane_f32(y, v, 1);
	v = vsetq_lane_f32(z, v, 2);
	v = vsetq_lane_f32(w, v, 3);
	return v;
}
FFTL_FORCEINLINE Vec4f V4fSplat4(f32 f)
{
	return vdupq_n_f32(f);
}
FFTL_FORCEINLINE Vec4f V4fSplat4(const f32* pf)
{
	return vdupq_n_f32(*pf);
}
FFTL_FORCEINLINE Vec4f V4fAnd(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vandq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fAndNot(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vbicq_u32(vreinterpretq_u32_f32(b), vreinterpretq_u32_f32(a)) );
}
FFTL_FORCEINLINE Vec4f V4fOr(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( vorrq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fXOr(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32( veorq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)) );
}
FFTL_FORCEINLINE Vec4f V4fAdd(Vec4f_In a, Vec4f_In b)
{
	return vaddq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fSub(Vec4f_In a, Vec4f_In b)
{
	return vsubq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fMul(Vec4f_In a, Vec4f_In b)
{
	return vmulq_f32(a, b);
}
FFTL_FORCEINLINE Vec4f V4fDiv(Vec4f_In a, Vec4f_In b)
{
	// get an initial estimate of 1/b.
	Vec4f r = vrecpeq_f32(b);

	// use a couple Newton-Raphson steps to refine the estimate.  Depending on your
	// application's accuracy requirements, you may be able to get away with only
	// one refinement (instead of the two used here).  Be sure to test!
	r = vmulq_f32(vrecpsq_f32(b, r), r);
	r = vmulq_f32(vrecpsq_f32(b, r), r);

	// and finally, compute a/b = a*(1/b)
	return vmulq_f32(a, r);
}
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a+b*c;
#if defined(__clang__)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	Vec4f ret = a;
	__asm__("vmla.f32 %0, %1, %2" : "+w" (ret) : "w" (b), "w" (c));
	return ret;
#else
	return vmlaq_f32(a, b, c); // Fused-multiply-add
#endif
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//return a-b*c;
#if defined(__clang__)
	//	The advertised vmlaw_32 intrinsic doesn't actually perform fused-multiply-add like it's supposed to in some cases.
	// In some headers you'll see it simply inlined to a+b*c. The inline assembly below fixes this issue so it's a single instruction.
	Vec4f ret = a;
	__asm__("vmls.f32 %0, %1, %2" : "+w" (ret) : "w" (b), "w" (c));
	return ret;
#else
	return vmlsq_f32(a, b, c); // Fused-multiply-subtract
#endif
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	Vec4f r = vrsqrteq_f32(v);
	return vmulq_f32(r, v);
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	float32x2_t r;
	float32x2_t v0 = vget_low_f32(v);
	float32x2_t v1 = vget_high_f32(v);
	r = vpadd_f32(v0, v1);
	r = vpadd_f32(r, r);
	return vget_lane_f32(r, 0);
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	float32x2_t r;
	float32x2_t v0 = vget_low_f32(v);
	float32x2_t v1 = vget_high_f32(v);
	r = vpadd_f32(v0, v1);
	r = vpadd_f32(r, r);
	return vcombine_f32(r, r);
}
FFTL_FORCEINLINE Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b)
{
	return vreinterpretq_f32_u32(vceqq_f32(a, b));
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	const uint32x4_t n = vreinterpretq_u32_f32(v);
	const uint32_t n0 = vgetq_lane_u32(n, 0) >> 31;
	const uint32_t n1 = vgetq_lane_u32(n, 1) >> 31;
	const uint32_t n2 = vgetq_lane_u32(n, 2) >> 31;
	const uint32_t n3 = vgetq_lane_u32(n, 3) >> 31;
	const uint32_t r = n0 | (n1 << 1) | (n2 << 2) | (n3 << 3);
	return (int)r;
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	const uint32x4_t n = vceqq_f32(a, b);
	const uint32_t n0 = vgetq_lane_u32(n, 0);
	const uint32_t n1 = vgetq_lane_u32(n, 1);
	const uint32_t n2 = vgetq_lane_u32(n, 2);
	const uint32_t n3 = vgetq_lane_u32(n, 3);
	return (n0 & n1 & n2 & n3) != 0;
}
FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	const float32x2x2_t r = vzip_f32(vget_low_f32(a), vget_low_f32(b) );
	return vcombine_f32(r.val[0], r.val[1]);
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	const float32x2x2_t r = vzip_f32(vget_high_f32(a), vget_high_f32(b));
	return vcombine_f32(r.val[0], r.val[1]);
}
FFTL_FORCEINLINE Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b)
{
	float32x2_t a0 = vget_low_f32(a);
	float32x2_t a1 = vget_high_f32(a);
	float32x2_t b0 = vget_low_f32(b);
	float32x2_t b1 = vget_high_f32(b);
	float32x2x2_t f0213 = vuzp_f32(a0, a1);
	float32x2x2_t f4657 = vuzp_f32(b0, b1);
	return vcombine_f32( f0213.val[0], f4657.val[0] );
}
FFTL_FORCEINLINE Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b)
{
	float32x2_t a0 = vget_low_f32(a);
	float32x2_t a1 = vget_high_f32(a);
	float32x2_t b0 = vget_low_f32(b);
	float32x2_t b1 = vget_high_f32(b);
	float32x2x2_t f0213 = vuzp_f32(a0, a1);
	float32x2x2_t f4657 = vuzp_f32(b0, b1);
	return vcombine_f32( f0213.val[1], f4657.val[1] );
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	const Vec4f r = vrev64q_f32(v);
	return vcombine_f32( vget_high_f32(r), vget_low_f32(r) );
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return vgetq_lane_f32(v, 0);
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return vgetq_lane_f32(v, 1);
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return vgetq_lane_f32(v, 2);
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return vgetq_lane_f32(v, 3);
}


template <enShuf T_X, enShuf T_Y, enShuf T_Z, enShuf T_W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In v )
{
	FFTL_StaticAssert(	(T_X==sh_X || T_X==sh_Y || T_X==sh_Z || T_X==sh_W) &&
		(T_Y==sh_X || T_Y==sh_Y || T_Y==sh_Z || T_Y==sh_W) &&
		(T_Z==sh_X || T_Z==sh_Y || T_Z==sh_Z || T_Z==sh_W) &&
		(T_W==sh_X || T_W==sh_Y || T_W==sh_Z || T_W==sh_W)  );

	FFTL_StaticAssert( !(T_X==sh_X && T_Y==sh_Y && T_Z==sh_Z && T_W==sh_W) ); // This permute does nothing meaningful!
#if defined(__clang__)
	return __builtin_shufflevector(v, v, T_X, T_Y, T_Z, T_W);
#elif 1
	Vec4f r;
	r = vsetq_lane_f32(vgetq_lane_f32(v, T_X), r, 0);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T_Y), r, 1);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T_Z), r, 2);
	r = vsetq_lane_f32(vgetq_lane_f32(v, T_W), r, 3);
	return r;
#else
	const f32* p = reinterpret_cast<const f32*>(&v);
	const Vec4f r = { p[T_X], p[T_Y], p[T_Z], p[T_W] };
	return r;
#endif
}

template <enShuf2 T_X, enShuf2 T_Y, enShuf2 T_Z, enShuf2 T_W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In a, Vec4f_In b )
{
	//	Template inputs need to be shuffle enumerations sh_(X/Y/Z/W)(1/2)
	FFTL_StaticAssert(
		(T_X==sh_X1 || T_X==sh_Y1 || T_X==sh_Z1 || T_X==sh_W1	||
		T_X==sh_X2 || T_X==sh_Y2 || T_X==sh_Z2 || T_X==sh_W2 )	&&
		(T_Y==sh_X1 || T_Y==sh_Y1 || T_Y==sh_Z1 || T_Y==sh_W1	||
		T_Y==sh_X2 || T_Y==sh_Y2 || T_Y==sh_Z2 || T_Y==sh_W2 )	&&
		(T_Z==sh_X1 || T_Z==sh_Y1 || T_Z==sh_Z1 || T_Z==sh_W1	||
		T_Z==sh_X2 || T_Z==sh_Y2 || T_Z==sh_Z2 || T_Z==sh_W2 )	&&
		(T_W==sh_X1 || T_W==sh_Y1 || T_W==sh_Z1 || T_W==sh_W1	||
		T_W==sh_X2 || T_W==sh_Y2 || T_W==sh_Z2 || T_W==sh_W2 )	 );

	//	If you try to shuffle this way, you'll just end up with one of the 2 input arguments. Don't do that.
	FFTL_StaticAssert(
		!(T_X==sh_X1 && T_Y==sh_Y1 && T_Z==sh_Z1 && T_W==sh_W1) &&
		!(T_X==sh_X2 && T_Y==sh_Y2 && T_Z==sh_Z2 && T_W==sh_W2) );

	//	If this error fires, it means you should use the single parameter V4fShuffle function.
	FFTL_StaticAssert(
		!( (T_X < 4) && (T_Y < 4) && (T_Z < 4) && (T_W < 4) ) &&
		!( (T_X > 3) && (T_Y > 3) && (T_Z > 3) && (T_W > 3) ) );

#if defined(__clang__)
	return __builtin_shufflevector(a, b, T_X, T_Y, T_Z, T_W);
#else
	const uint shufX = (T_X==sh_X1||T_X==sh_Y1||T_X==sh_Z1||T_X==sh_W1) ? 0 : 1;
	const uint shufY = (T_Y==sh_X1||T_Y==sh_Y1||T_Y==sh_Z1||T_Y==sh_W1) ? 0 : 1;
	const uint shufZ = (T_Z==sh_X1||T_Z==sh_Y1||T_Z==sh_Z1||T_Z==sh_W1) ? 0 : 1;
	const uint shufW = (T_W==sh_X1||T_W==sh_Y1||T_W==sh_Z1||T_W==sh_W1) ? 0 : 1;

	const f32* arr[2] = { reinterpret_cast<const f32*>(&a), reinterpret_cast<const f32*>(&b) };
	const Vec4f r = { arr[shufX][FFTL_PERMUTEMASK(T_X)], arr[shufY][FFTL_PERMUTEMASK(T_Y)], arr[shufZ][FFTL_PERMUTEMASK(T_Z)], arr[shufW][FFTL_PERMUTEMASK(T_W)] };
	return r;
#endif
}

//	We can have the compiler catch some special cases where we can minimize instructions
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X1, sh_X2, sh_Y1, sh_Y2>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeXY(a, b);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X2, sh_X1, sh_Y2, sh_Y1>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeXY(b, a);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z1, sh_Z2, sh_W1, sh_W2>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeZW(a, b);
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z2, sh_Z1, sh_W2, sh_W1>(Vec4f_In a, Vec4f_In b)
{
	return V4fMergeZW(b, a);
}










FFTL_FORCEINLINE ScopedFlushDenormals::ScopedFlushDenormals()
{
	uint32 x;

	//	Get the floating-point status and control register and store it into x.
	asm(
		"vmrs %[result],FPSCR \r\n"
		:[result] "=r" (x) : :
		);

	//	Store the flush-to-zero bit.
	m_prevMode = x & (1<<24);
	
	//	Enable the flush-to-zero bit.
	x |= (1<<24);
	
	//	Store the new x with the flush-to-zero bit enabled into the floating-point status and control register
	asm(
		"vmsr FPSCR,%[value]"
		:
		: [value] "r" (x)
		:
		);
}
FFTL_FORCEINLINE ScopedFlushDenormals::~ScopedFlushDenormals()
{
	uint32 x;

	//	Get the floating-point status and control register and store it into x.
	asm(
		"vmrs %[result],FPSCR \r\n"
		:[result] "=r" (x) : :
		);

	//	If the flush-to-zero bit was enabled before, it will be disabled here.
	x &= ~(1<<24) | m_prevMode;

	//	Restore the previous value of the flush-to-zero bit into the the floating-point status and control register.
	asm(
		"vmsr FPSCR,%[value]"
		:
		: [value] "r" (x)
		:
		);
}

} // namespace FFTL


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif //_FFTL_MATH_DEFAULT_INL