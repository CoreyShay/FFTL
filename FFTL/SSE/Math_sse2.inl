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

#ifndef _FFTL_MATH_SSE2_INL
#define _FFTL_MATH_SSE2_INL


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4127 ) //conditional expression is constant
#endif

#define _MM_SHUFFLE_XYZW(x,y,z,w) _MM_SHUFFLE(w,z,y,x)
#define _MM_INSERTPS_MASK_HELPER(_to_a, _from_b, _zeroX_, _zeroY_, _zeroZ_, _zeroW_) ( (_zeroX_&1) | ((_zeroY_&1)<<1) | ((_zeroZ_&1)<<2) | ((_zeroW_&1)<<3) | ((_to_a&3)<<4) | ((_from_b&3)<<6) )

namespace FFTL
{




template<>
FFTL_FORCEINLINE float Min(float a, float b)
{
	return _mm_cvtss_f32( _mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE double Min(double a, double b)
{
	return _mm_cvtsd_f64( _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)) );
}

template<>
FFTL_FORCEINLINE float Max(float a, float b)
{
	return _mm_cvtss_f32( _mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)) );
}
template<>
FFTL_FORCEINLINE double Max(double a, double b)
{
	return _mm_cvtsd_f64( _mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)) );
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
	return _mm_shuffle_ps( v, v, _MM_SHUFFLE_XYZW(3,2,1,0));
}
FFTL_FORCEINLINE Vec4f V4fLoadUR(const f32* pf)
{
	const Vec4f v = _mm_loadu_ps(pf);
	return _mm_shuffle_ps( v, v, _MM_SHUFFLE_XYZW(3,2,1,0));
}
FFTL_FORCEINLINE Vec4f V4fLoad1(const f32* pf)
{
	return _mm_load_ss(pf);
}
FFTL_FORCEINLINE Vec4f V4fLoad2(const f32* pf)
{
	return _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)pf);
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
#if FFTL_SSE4
	reinterpret_cast<int32*>(pf)[iA] = _mm_extract_ps(v, 0);
	reinterpret_cast<int32*>(pf)[iB] = _mm_extract_ps(v, 1);
	reinterpret_cast<int32*>(pf)[iC] = _mm_extract_ps(v, 2);
	reinterpret_cast<int32*>(pf)[iD] = _mm_extract_ps(v, 3);
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
FFTL_FORCEINLINE Vec4f V4fSplat4(f32 f)
{
	return _mm_set1_ps(f);
}
FFTL_FORCEINLINE Vec4f V4fSplat4(const f32* pf)
{
#if FFTL_AVX
	return _mm_broadcast_ss(pf);
#else
	return _mm_load1_ps(pf);
#endif
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
FFTL_FORCEINLINE Vec4f V4fAddMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//	TODO: Look into performing fused-multiply-add when the compiler switch is force enabled.
	Vec4f r = _mm_mul_ps(b, c);
	r = _mm_add_ps(a, r);
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSubMul(Vec4f_In a, Vec4f_In b, Vec4f_In c)
{
	//	TODO: Look into performing fused-multiply-subtract when the compiler switch is force enabled.
	Vec4f r = _mm_mul_ps(b, c);
	r = _mm_sub_ps(a, r);
	return r;
}
FFTL_FORCEINLINE Vec4f V4fSqrt(Vec4f_In v)
{
	return _mm_sqrt_ps(v);
}
FFTL_FORCEINLINE f32 V4fHSumF(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fHSumV(v) );
}
FFTL_FORCEINLINE Vec4f V4fHSumV(Vec4f_In v)
{
	Vec4f r;
#if FFTL_SSE4
	r = _mm_hadd_ps(v, v);
	r = _mm_hadd_ps(r, r);
#else
	r = _mm_add_ps( v, _mm_shuffle_ps(v, v, _MM_SHUFFLE_XYZW(1,0,3,2)) );
	r = _mm_add_ps( r, _mm_shuffle_ps(r, r, _MM_SHUFFLE_XYZW(2,2,0,0)) );
#endif
	return r;
}
FFTL_FORCEINLINE Vec4f V4fCompareEqual(Vec4f_In a, Vec4f_In b)
{
	return _mm_cmpeq_ps(a, b);
}
FFTL_FORCEINLINE int V4fToIntMask(Vec4f_In v)
{
	return _mm_movemask_ps(v);
}
FFTL_FORCEINLINE bool V4fIsEqual(Vec4f_In a, Vec4f_In b)
{
	return _mm_movemask_ps( _mm_cmpeq_ps(a, b) ) == 15;
}
FFTL_FORCEINLINE Vec4f V4fMergeXY(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpacklo_ps( a, b );
}
FFTL_FORCEINLINE Vec4f V4fMergeZW(Vec4f_In a, Vec4f_In b)
{
	return _mm_unpackhi_ps( a, b );
}
FFTL_FORCEINLINE Vec4f V4fSplitXZ(Vec4f_In a, Vec4f_In b)
{
	return _mm_shuffle_ps(a, b, _MM_SHUFFLE_XYZW(0,2,0,2));
}
FFTL_FORCEINLINE Vec4f V4fSplitYW(Vec4f_In a, Vec4f_In b)
{
	return _mm_shuffle_ps(a, b, _MM_SHUFFLE_XYZW(1,3,1,3));
}

FFTL_FORCEINLINE Vec4f V4fReverse(Vec4f_In v)
{
	return _mm_shuffle_ps(v, v, _MM_SHUFFLE_XYZW(3,2,1,0));
}

FFTL_FORCEINLINE f32 V4fGetX(Vec4f_In v)
{
	return _mm_cvtss_f32(v);
}
FFTL_FORCEINLINE f32 V4fGetY(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_Y,sh_X,sh_X,sh_X>(v) );
}
FFTL_FORCEINLINE f32 V4fGetZ(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_Z,sh_X,sh_X,sh_X>(v) );
}
FFTL_FORCEINLINE f32 V4fGetW(Vec4f_In v)
{
	return _mm_cvtss_f32( V4fPermute<sh_W,sh_X,sh_X,sh_X>(v) );
}




template <enShuf T_X, enShuf T_Y, enShuf T_Z, enShuf T_W>
FFTL_FORCEINLINE Vec4f V4fPermute( Vec4f_In v )
{
	FFTL_StaticAssert(	(T_X==sh_X || T_X==sh_Y || T_X==sh_Z || T_X==sh_W) &&
		(T_Y==sh_X || T_Y==sh_Y || T_Y==sh_Z || T_Y==sh_W) &&
		(T_Z==sh_X || T_Z==sh_Y || T_Z==sh_Z || T_Z==sh_W) &&
		(T_W==sh_X || T_W==sh_Y || T_W==sh_Z || T_W==sh_W)  );

	FFTL_StaticAssert( !(T_X==sh_X && T_Y==sh_Y && T_Z==sh_Z && T_W==sh_W) ); // This permute does nothing meaningful!

#if defined(FFTL_AVX)
	return _mm_permute_ps( v, _MM_SHUFFLE_XYZW( T_X, T_Y, T_Z, T_W ) );
#else
	return _mm_shuffle_ps( v, v, _MM_SHUFFLE_XYZW( T_X, T_Y, T_Z, T_W ) );
#endif
}

//	While this function may look scary, all the branches and switches actually compile away in release builds, leaving only 1 or 2 SSE instructions.
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


	const uint shufX = (T_X==sh_X1||T_X==sh_Y1||T_X==sh_Z1||T_X==sh_W1) ? 0 : 1;
	const uint shufY = (T_Y==sh_X1||T_Y==sh_Y1||T_Y==sh_Z1||T_Y==sh_W1) ? 0 : 1;
	const uint shufZ = (T_Z==sh_X1||T_Z==sh_Y1||T_Z==sh_Z1||T_Z==sh_W1) ? 0 : 1;
	const uint shufW = (T_W==sh_X1||T_W==sh_Y1||T_W==sh_Z1||T_W==sh_W1) ? 0 : 1;


	//	Catch the easy SSE condition when _X and _Y come from a different vector than _Z and _W.
	// Uses only 1 SSE shuffle instruction.
	if (shufX == shufY && shufZ == shufW)
	{
		if (shufX == 0)	//	_X and _Y from v1. _Z and _W from v2.
			return _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
		else			//	_X and _Y from v2. _Z and _W from v1.
			return _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
	}
#if defined(FFTL_SSE4)
	//	If we have SSE4, we can use the blend instruction as long as we don't need reordering
	else if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_Z) == 2 && FFTL_PERMUTEMASK(T_W) == 3)
	{
		return _mm_blend_ps(a, b, (shufX<<0) | (shufY<<1) | (shufZ<<2) | (shufW<<3));
	}
#endif
	//	Next 4 cases, when we need 2 32 bit words from each vector, but it can't be handled with a single SSE instruction,
	// we use one _mm_shuffle_ps instruction, then one _mm_shuffle_epi32 instruction.
	else if (shufX==0 && shufY==1 && shufZ==0 && shufW==1)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_W)) );
		return V4fPermute<sh_X,sh_Z,sh_Y,sh_W>(vShuffleTemp);
	}
	else if (shufX==0 && shufY==1 && shufZ==1 && shufW==0)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X),FFTL_PERMUTEMASK( T_W), FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_Z)) );
		return V4fPermute<sh_X,sh_Z,sh_W,sh_Y>(vShuffleTemp);
	}
	else if (shufX==1 && shufY==0 && shufZ==0 && shufW==1)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_W)) );
		return V4fPermute<sh_Z,sh_X,sh_Y,sh_W>(vShuffleTemp);
	}
	else if (shufX==1 && shufY==0 && shufZ==1 && shufW==0)
	{
		const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Y), FFTL_PERMUTEMASK(T_W), FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Z)) );
		return V4fPermute<sh_Z,sh_X,sh_W,sh_Y>(vShuffleTemp);
	}


	//	Next 2 cases, when we need 3 32 bit words from one vector, and 1 word from the other, we use 2 _mm_shuffle_ps instructions.
	else if (
		(shufX==0 && shufY==1 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==0 && shufZ==1 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==0 && shufW==1)	||
		(shufX==1 && shufY==1 && shufZ==1 && shufW==0)	)
	{
		//	When we need one word from V1
		if (shufX == 0)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_Z) == 2 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(b, a, _MM_INSERTPS_MASK_HELPER(0, FFTL_PERMUTEMASK(T_X), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), sh_W, FFTL_PERMUTEMASK(T_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, b, _MM_SHUFFLE_XYZW(sh_X, sh_Z, FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
		}
		else if (shufY == 0)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Z) == 2 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(b, a, _MM_INSERTPS_MASK_HELPER(1, FFTL_PERMUTEMASK(T_Y), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), sh_W, FFTL_PERMUTEMASK(T_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, b, _MM_SHUFFLE_XYZW(sh_X, sh_Z, FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
		}
		else if (shufZ == 0)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(b, a, _MM_INSERTPS_MASK_HELPER(2, FFTL_PERMUTEMASK(T_Z), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Z), sh_W, FFTL_PERMUTEMASK(T_W), sh_W) );
			return _mm_shuffle_ps( b, vShuffleTemp, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), sh_X, sh_Z) );
		}
		else // shufW == 0
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_Z) == 2)
				return _mm_insert_ps(b, a, _MM_INSERTPS_MASK_HELPER(3, FFTL_PERMUTEMASK(T_W), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Z), sh_W, FFTL_PERMUTEMASK(T_W), sh_W) );
			return _mm_shuffle_ps( b, vShuffleTemp, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), sh_X, sh_Z) );
		}
	}
	else if (
		(shufX==1 && shufY==0 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==1 && shufZ==0 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==1 && shufW==0)	||
		(shufX==0 && shufY==0 && shufZ==0 && shufW==1)	)
	{
		//	When we need one word from V2

		if (shufX == 1)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_Z) == 2 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(a, b, _MM_INSERTPS_MASK_HELPER(0, FFTL_PERMUTEMASK(T_X), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), sh_W, FFTL_PERMUTEMASK(T_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, a, _MM_SHUFFLE_XYZW(sh_X, sh_Z, FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
		}
		else if (shufY == 1)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Z) == 2 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(a, b, _MM_INSERTPS_MASK_HELPER(1, FFTL_PERMUTEMASK(T_Y), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), sh_W, FFTL_PERMUTEMASK(T_Y), sh_W) );
			return _mm_shuffle_ps( vShuffleTemp, a, _MM_SHUFFLE_XYZW(sh_X, sh_Z, FFTL_PERMUTEMASK(T_Z), FFTL_PERMUTEMASK(T_W)) );
		}
		else if (shufZ == 1)
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_W) == 3)
				return _mm_insert_ps(a, b, _MM_INSERTPS_MASK_HELPER(2, FFTL_PERMUTEMASK(T_Z), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( b, a, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Z), sh_W, FFTL_PERMUTEMASK(T_W), sh_W) );
			return _mm_shuffle_ps( a, vShuffleTemp, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), sh_X, sh_Z) );
		}
		else // shufW==1
		{
#if defined(FFTL_SSE4)
			if (FFTL_PERMUTEMASK(T_X) == 0 && FFTL_PERMUTEMASK(T_Y) == 1 && FFTL_PERMUTEMASK(T_Z) == 2)
				return _mm_insert_ps(a, b, _MM_INSERTPS_MASK_HELPER(3, FFTL_PERMUTEMASK(T_W), 0, 0, 0, 0));
#endif
			const Vec4f vShuffleTemp = _mm_shuffle_ps( a, b, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_Z), sh_W, FFTL_PERMUTEMASK(T_W), sh_W) );
			return _mm_shuffle_ps( a, vShuffleTemp, _MM_SHUFFLE_XYZW(FFTL_PERMUTEMASK(T_X), FFTL_PERMUTEMASK(T_Y), sh_X, sh_Z) );
		}
	}

	else
	{
		//	I didn't think of this case. I guess nobody's perfect.
		FFTL_ASSERT(0);
		return _mm_setzero_ps();
	}
}

//	We can have the compiler catch some special cases where we can use a single unpack instruction rather than 2 shuffle instructions.
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X1,sh_X2,sh_Y1,sh_Y2>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeXY( a, b );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_X2,sh_X1,sh_Y2,sh_Y1>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeXY( b, a );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z1,sh_Z2,sh_W1,sh_W2>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeZW( a, b );
}
template <>
FFTL_FORCEINLINE Vec4f V4fPermute<sh_Z2,sh_Z1,sh_W2,sh_W1>( Vec4f_In a, Vec4f_In b )
{
	return V4fMergeZW( b, a );
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
FFTL_FORCEINLINE bool V4fIsEqual(Vec2d_In a, Vec2d_In b)
{
	return _mm_movemask_pd( _mm_cmpeq_pd(a, b) ) == 3;
}


FFTL_FORCEINLINE ScopedFlushDenormals::ScopedFlushDenormals()
{
	m_prevMode = _MM_GET_FLUSH_ZERO_MODE();
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}
FFTL_FORCEINLINE ScopedFlushDenormals::~ScopedFlushDenormals()
{
	_MM_SET_FLUSH_ZERO_MODE(m_prevMode);
}


} // namespace FFTL

#undef _MM_SHUFFLE_XYZW
#undef _MM_INSERTPS_MASK_HELPER


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //_FFTL_MATH_SSE2_INL